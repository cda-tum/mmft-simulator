#include "NodalAnalysis.h"

namespace nodal {

template<typename T>
void conductNodalAnalysis( const arch::Network<T>* network)
    {
    const int nNodes = network->getNodes().size() - 1;    // -1 due to ground node
    std::unordered_set<int> conductingNodeIds;
    std::unordered_map<int, int> groundNodeIds;

    // Sort nodes into conducting nodes and ground nodes.
    // First loop, all nodes with id > 0 are conducting nodes.
    int iPump = nNodes;
    for (const auto& [key, group] : network->getGroups()) {
        for (const auto& nodeId : group->nodeIds) {
            if(!network->getNodes().at(nodeId)->getGround() && nodeId != group->groundNodeId) {
                conductingNodeIds.emplace(nodeId);
            } else if (!network->getNodes().at(nodeId)->getGround() && nodeId == group->groundNodeId) {
                groundNodeIds.emplace(nodeId, iPump);
                iPump++;
            }
        }
    }

    const int nPressurePumps = network->getPressurePumps().size() + groundNodeIds.size();
    const int nNodesAndPressurePumps = nNodes + nPressurePumps;

    // Generate empty matrix A and vector z
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);  // matrix A = [G, B; C, D]
    Eigen::VectorXd z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);                          // vector z = [i; e]

    // loop through channels and build matrix G
    for (const auto& channel : network->getChannels()) {
        auto nodeAMatrixId = channel.second->getNodeA();
        auto nodeBMatrixId = channel.second->getNodeB();
        const T conductance = 1. / channel.second->getResistance();

        // main diagonal elements of G
        if (!network->getNodes().at(nodeAMatrixId)->getGround()) {
            A(nodeAMatrixId, nodeAMatrixId) += conductance;
        }

        if (!network->getNodes().at(nodeBMatrixId)->getGround()) {
            A(nodeBMatrixId, nodeBMatrixId) += conductance;
        }

        // minor diagonal elements of G (if no ground node was present)
        if (!network->getNodes().at(nodeAMatrixId)->getGround() && !network->getNodes().at(nodeBMatrixId)->getGround()) {
            A(nodeAMatrixId, nodeBMatrixId) -= conductance;
            A(nodeBMatrixId, nodeAMatrixId) -= conductance;
        }
    }

    // Update the reference pressure for each group
    for (const auto& [key, group] : network->getGroups()) {
        if (group->initialized) {
            auto& node = network->getNodes().at(group->groundNodeId);
            group->pRef = node->getPressure();
            int pumpId = groundNodeIds.at(group->groundNodeId);

            A(group->groundNodeId, pumpId) = 1;   // matrix B
            A(pumpId, group->groundNodeId) = 1;   // matrix C

            z(pumpId) = node->getPressure();
        }
    }

    // loop through pressurePumps and build matrix B, C and vector e
    for (const auto& pressurePump : network->getPressurePumps()) {
        auto nodeAMatrixId = pressurePump.second->getNodeA();
        auto nodeBMatrixId = pressurePump.second->getNodeB();

        if (contains(conductingNodeIds, nodeAMatrixId)) {
            A(nodeAMatrixId, iPump) = -1;   // matrix B
            A(iPump, nodeAMatrixId) = -1;   // matrix C
        }

        if (contains(conductingNodeIds, nodeBMatrixId)) {
            A(nodeBMatrixId, iPump) = 1;   // matrix B
            A(iPump, nodeBMatrixId) = 1;   // matrix C
        }

        z(iPump) = pressurePump.second->getPressure();

        iPump++;
    }

    // loop through flowRatePumps and build vector i
    for (const auto& flowRatePump : network->getFlowRatePumps()) {
        auto nodeAMatrixId = flowRatePump.second->getNodeA();
        auto nodeBMatrixId = flowRatePump.second->getNodeB();
        const T flowRate = flowRatePump.second->getFlowRate();

        if (contains(conductingNodeIds, nodeAMatrixId)){
            z(nodeAMatrixId) = -flowRate;
        }
        if (contains(conductingNodeIds, nodeBMatrixId)){
            z(nodeBMatrixId) = flowRate;
        }
    }

    // solve equation x = A^(-1) * z
    VectorXd x = A.colPivHouseholderQr().solve(z);

    // set pressure of nodes to result value
    for (const auto& [key, group] : network->getGroups()) {
        for (auto nodeMatrixId : group->nodeIds) {
            auto& node = network->getNodes().at(nodeMatrixId);
            if (contains(conductingNodeIds, nodeMatrixId)) {
                node->setPressure(x(nodeMatrixId));
            } else if (node->getGround()) {
                node->setPressure(0.0);
            }
        }
    }

    for (auto& channel : network->getChannels()) {
        auto& nodeA = network->getNodes().at(channel.second->getNodeA());
        auto& nodeB = network->getNodes().at(channel.second->getNodeB());
        channel.second->setPressure(nodeA->getPressure() - nodeB->getPressure());
    }

    // set flow rate at pressure pumps
    iPump = nNodes;
    for (auto& pressurePump : network->getPressurePumps()){
        pressurePump.second->setFlowRate(x(iPump));
        iPump++;
    }

    // Initialize the ground nodes of the groups
    for (const auto& [key, group] : network->getGroups()) {
        if (!group->initialized && !group->grounded) {
            T pMin = -1.0;
            for (auto nodeId : group->nodeIds) {
                if (pMin < 0.0) {
                    pMin = x(nodeId);
                    group->groundNodeId = nodeId;
                }
                if (x(nodeId) < pMin) {
                    pMin = x(nodeId);
                    group->groundNodeId = nodeId;
                }
            }
            for (auto channelId : group->channelIds) {
                if (network->getChannels().at(channelId)->getNodeA() == group->groundNodeId) {
                    group->groundChannelId = channelId;
                } 
                else if (network->getChannels().at(channelId)->getNodeB() == group->groundNodeId) {
                    group->groundChannelId = channelId;
                }
            }
            groundNodeIds.emplace(group->groundNodeId, 0);
            conductingNodeIds.erase(group->groundNodeId);
            group->initialized = true;
        }
    }
}

template<typename T>
bool conductNodalAnalysis( const arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators)
    {
    const int nNodes = network->getNodes().size() - 1;    // -1 due to ground node
    std::unordered_set<int> conductingNodeIds;
    std::unordered_map<int, int> groundNodeIds;

    // Sort nodes into conducting nodes and ground nodes.
    // First loop, all nodes with id > 0 are conducting nodes.
    int iPump = nNodes;
    for (const auto& [key, group] : network->getGroups()) {
        for (const auto& nodeId : group->nodeIds) {
            if(!network->getNodes().at(nodeId)->getGround() && nodeId != group->groundNodeId) {
                conductingNodeIds.emplace(nodeId);
            } else if (!network->getNodes().at(nodeId)->getGround() && nodeId == group->groundNodeId) {
                groundNodeIds.emplace(nodeId, iPump);
                iPump++;
            }
        }
    }

    const int nPressurePumps = network->getPressurePumps().size() + groundNodeIds.size();
    const int nNodesAndPressurePumps = nNodes + nPressurePumps;

    // Generate empty matrix A and vector z
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);  // matrix A = [G, B; C, D]
    Eigen::VectorXd z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);                          // vector z = [i; e]

    // loop through channels and build matrix G
    for (const auto& channel : network->getChannels()) {
        auto nodeAMatrixId = channel.second->getNodeA();
        auto nodeBMatrixId = channel.second->getNodeB();
        const T conductance = 1. / channel.second->getResistance();

        // main diagonal elements of G
        if (!network->getNodes().at(nodeAMatrixId)->getGround()) {
            A(nodeAMatrixId, nodeAMatrixId) += conductance;
        }

        if (!network->getNodes().at(nodeBMatrixId)->getGround()) {
            A(nodeBMatrixId, nodeBMatrixId) += conductance;
        }

        // minor diagonal elements of G (if no ground node was present)
        if (!network->getNodes().at(nodeAMatrixId)->getGround() && !network->getNodes().at(nodeBMatrixId)->getGround()) {
            A(nodeAMatrixId, nodeBMatrixId) -= conductance;
            A(nodeBMatrixId, nodeAMatrixId) -= conductance;
        }
    }

    // loop through modules
    for (const auto& [key, module] : network->getModules()) {
        // For now only LBM modules implemented.
        #ifndef USE_ESSLBM
        assert(module->getModuleType() == arch::ModuleType::LBM);
        #elif USE_ESSLBM
        assert(module->getModuleType() == arch::ModuleType::ESS_LBM);
        #endif
    }

    for (const auto& [key, cfdSimulator] : cfdSimulators) {
        // If module is not initialized (1st loop), loop over channels of fully connected graph
        if ( ! cfdSimulator->getInitialized() ) {
            for (const auto& [key, channel] : cfdSimulator->getNetwork()->getChannels()) {
                auto nodeAMatrixId = channel->getNodeA();
                auto nodeBMatrixId = channel->getNodeB();
                const T conductance = 1. / channel->getResistance();

                // main diagonal elements of G
                if (contains(conductingNodeIds, nodeAMatrixId)) {
                    A(nodeAMatrixId, nodeAMatrixId) += conductance;
                }

                if (contains(conductingNodeIds, nodeBMatrixId)) {
                    A(nodeBMatrixId, nodeBMatrixId) += conductance;
                }

                // minor diagonal elements of G (if no ground node was present)
                if (contains(conductingNodeIds, nodeAMatrixId) && contains(conductingNodeIds, nodeBMatrixId)) {
                    A(nodeAMatrixId, nodeBMatrixId) -= conductance;
                    A(nodeBMatrixId, nodeAMatrixId) -= conductance;
                }
            }
        }
        /* If module is initialized, loop over boundary nodes and:
            *  - read pressure for ground nodes
            *  - read flow rate for conducting nodes
            */
        else if ( cfdSimulator->getInitialized() ) {
            for (const auto& [key, node] : cfdSimulator->getModule()->getNodes()) {
                // Write the module's flowrates into vector i if the node is not a group's ground node
                if (contains(conductingNodeIds, key)) {
                    T flowRate = cfdSimulator->getFlowRates().at(key) * cfdSimulator->getOpenings().at(key).height;
                    z(key) = -flowRate;
                } 
                // Write module's pressure into matrix B, C and vector e
                else if (contains(groundNodeIds, key)) {
                    T pressure = cfdSimulator->getPressures().at(key);
                    node->setPressure(pressure);
                }
            }
        }
    }

    // Update the reference pressure for each group
    for (const auto& [key, group] : network->getGroups()) {
        if (group->initialized) {
            auto& node = network->getNodes().at(group->groundNodeId);
            group->pRef = node->getPressure();
            int pumpId = groundNodeIds.at(group->groundNodeId);

            A(group->groundNodeId, pumpId) = 1;   // matrix B
            A(pumpId, group->groundNodeId) = 1;   // matrix C

            z(pumpId) = node->getPressure();
        }
    }

    // loop through pressurePumps and build matrix B, C and vector e
    for (const auto& pressurePump : network->getPressurePumps()) {
        auto nodeAMatrixId = pressurePump.second->getNodeA();
        auto nodeBMatrixId = pressurePump.second->getNodeB();

        if (contains(conductingNodeIds, nodeAMatrixId)) {
            A(nodeAMatrixId, iPump) = -1;   // matrix B
            A(iPump, nodeAMatrixId) = -1;   // matrix C
        }

        if (contains(conductingNodeIds, nodeBMatrixId)) {
            A(nodeBMatrixId, iPump) = 1;   // matrix B
            A(iPump, nodeBMatrixId) = 1;   // matrix C
        }

        z(iPump) = pressurePump.second->getPressure();

        iPump++;
    }

    // loop through flowRatePumps and build vector i
    for (const auto& flowRatePump : network->getFlowRatePumps()) {
        auto nodeAMatrixId = flowRatePump.second->getNodeA();
        auto nodeBMatrixId = flowRatePump.second->getNodeB();
        const T flowRate = flowRatePump.second->getFlowRate();

        if (contains(conductingNodeIds, nodeAMatrixId)){
            z(nodeAMatrixId) = -flowRate;
        }
        if (contains(conductingNodeIds, nodeBMatrixId)){
            z(nodeBMatrixId) = flowRate;
        }
    }

    // solve equation x = A^(-1) * z
    VectorXd x = A.colPivHouseholderQr().solve(z);

    // set pressure of nodes to result value
    for (const auto& [key, group] : network->getGroups()) {
        for (auto nodeMatrixId : group->nodeIds) {
            auto& node = network->getNodes().at(nodeMatrixId);
            if (contains(conductingNodeIds, nodeMatrixId)) {
                node->setPressure(x(nodeMatrixId));
            } else if (node->getGround()) {
                node->setPressure(0.0);
            }
        }
    }

    for (auto& channel : network->getChannels()) {
        auto& nodeA = network->getNodes().at(channel.second->getNodeA());
        auto& nodeB = network->getNodes().at(channel.second->getNodeB());
        channel.second->setPressure(nodeA->getPressure() - nodeB->getPressure());
    }

    bool pressureConvergence = true;

    // Set the pressures and flow rates on the boundary nodes of the modules
    for (auto& cfdSimulator : cfdSimulators) {
        std::unordered_map<int, T> old_pressures = cfdSimulator.second->getPressures();
        std::unordered_map<int, T> old_flowrates = cfdSimulator.second->getFlowRates();
        std::unordered_map<int, T> pressures_ = cfdSimulator.second->getPressures();
        std::unordered_map<int, T> flowRates_ = cfdSimulator.second->getFlowRates();
        for (auto& [key, node] : cfdSimulator.second->getModule()->getNodes()){
            // Communicate pressure to the module
            if (contains(conductingNodeIds, key)) {
                T old_pressure = old_pressures.at(key);
                T new_pressure = node->getPressure();
                T set_pressure = 0.0;
                if (old_pressure > 0 ) {
                    set_pressure = old_pressure + cfdSimulator.second->getAlpha() * ( new_pressure - old_pressure );
                } else {
                    set_pressure = new_pressure;
                }
                pressures_.at(key) = set_pressure;

                if (abs(old_pressure - new_pressure) > 1e-2) {
                    pressureConvergence = false;
                }
            }
            // Communicate the flow rate to the module
            else if (contains(groundNodeIds, key)) {
                T old_flowRate = old_flowrates.at(key) ;
                T new_flowRate = x(groundNodeIds.at(key)) / cfdSimulator.second->getOpenings().at(key).width;
                T set_flowRate = 0.0;
                if (old_flowRate > 0 ) {
                    set_flowRate = old_flowRate + 5 * cfdSimulator.second->getAlpha() *  ( new_flowRate - old_flowRate );
                } else {
                    set_flowRate = new_flowRate;
                }
                flowRates_.at(key) = set_flowRate;

                if (abs(old_flowRate - new_flowRate) > 1e-2) {
                    pressureConvergence = false;
                }
            }
        }
        cfdSimulator.second->setPressures(pressures_);
        cfdSimulator.second->setFlowRates(flowRates_);
    }

    // set flow rate at pressure pumps
    iPump = nNodes;
    for (auto& pressurePump : network->getPressurePumps()){
        pressurePump.second->setFlowRate(x(iPump));
        iPump++;
    }

    // Initialize the ground nodes of the groups
    for (const auto& [key, group] : network->getGroups()) {
        if (!group->initialized && !group->grounded) {
            T pMin = -1.0;
            for (auto nodeId : group->nodeIds) {
                if (pMin < 0.0) {
                    pMin = x(nodeId);
                    group->groundNodeId = nodeId;
                }
                if (x(nodeId) < pMin) {
                    pMin = x(nodeId);
                    group->groundNodeId = nodeId;
                }
            }
            for (auto channelId : group->channelIds) {
                if (network->getChannels().at(channelId)->getNodeA() == group->groundNodeId) {
                    group->groundChannelId = channelId;
                } 
                else if (network->getChannels().at(channelId)->getNodeB() == group->groundNodeId) {
                    group->groundChannelId = channelId;
                }
            }
            groundNodeIds.emplace(group->groundNodeId, 0);
            conductingNodeIds.erase(group->groundNodeId);
            group->initialized = true;
        }
    }
    
    // Loop over modules to set the ground nodes
    for (const auto& [key, cfdSimulator] : cfdSimulators) {
        if ( ! cfdSimulator->getInitialized() ) {
            std::unordered_map<int, T> flowRates_ = cfdSimulator->getFlowRates();
            std::unordered_map<int, bool> groundNodes;
            for (const auto& [nodeId, node] : cfdSimulator->getModule()->getNodes()) {
                T flowRate = 0.0;
                if (contains(groundNodeIds, nodeId)) {
                    groundNodes.try_emplace(nodeId, true);
                    for (auto& [key, group] : network->getGroups()) {
                        if (nodeId == group->groundNodeId) {
                            T height = network->getChannels().at(group->groundChannelId)->getHeight();
                            flowRate = network->getChannels().at(group->groundChannelId)->getFlowRate()/height;
                        }
                    }
                } else {
                    groundNodes.try_emplace(nodeId, false);
                }
                flowRates_.at(nodeId) = flowRate;
            }
            cfdSimulator->setGroundNodes(groundNodes);
            cfdSimulator->setFlowRates(flowRates_);
            cfdSimulator->setInitialized(true);
        }
    }

    return pressureConvergence;
}

bool contains( const std::unordered_set<int>& set, int key) {
    bool contain = false;
    for (auto& nodeId : set) {
        if (key == nodeId) {
            contain = true;
        }
    }
    return contain;
}

bool contains( const std::unordered_map<int, int>& map, int key) {
    bool contain = false;
    for (auto& [nodeId, pumpId] : map) {
        if (key == nodeId) {
            contain = true;
        }
    }
    return contain;
}

}   // namespace nodal
