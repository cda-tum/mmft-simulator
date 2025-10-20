#include "NodalAnalysis.h"

namespace nodal {

template<typename T>
NodalAnalysis<T>::NodalAnalysis(const arch::Network<T>* network_) {
    network = network_;
    nNodes = network->getNodes().size() + network->getVirtualNodes();

    // loop through modules
    for (const auto& [key, module] : network->getCfdModules()) {
        // For now only LBM modules implemented.
        #ifndef USE_ESSLBM
        assert(module->getModuleType() == arch::ModuleType::LBM);
        #elif USE_ESSLBM
        assert(module->getModuleType() == arch::ModuleType::ESS_LBM);
        #endif
    }

    // Sort nodes into conducting nodes and ground nodes.
    // First loop, all nodes with id > 0 are conducting nodes.
    int iPump = nNodes;
    for (const auto& [key, group] : network->getGroups()) {
        group->checkGroundValue();
        for (const auto& nodeId : group->nodeIds) {
            // The node is a conducting node
            if(!network->getNodes().at(nodeId)->getGround() && nodeId != size_t(group->groundNodeId)) {
                conductingNodeIds.emplace(nodeId);
            } 
            // The node is an overall ground node, or counts as ground to a group
            else if (!network->getNodes().at(nodeId)->getGround() && nodeId == size_t(group->groundNodeId)) {
                groundNodeIds.emplace(nodeId, iPump);
                iPump++;
            }
        }
    }
    
    nPressurePumps = network->getPressurePumps().size() + groundNodeIds.size();
    int nNodesAndPressurePumps = nNodes + nPressurePumps + network->getCfdModules().size();

    A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);
    z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);
    x = Eigen::VectorXd::Zero(nNodesAndPressurePumps);

}

template<typename T>
void NodalAnalysis<T>::clear() {

    pressureConvergence = true;

    conductingNodeIds.clear();
    groundNodeIds.clear();

    int iPump = network->getNodes().size() + network->getVirtualNodes() + network->getPressurePumps().size();

    for (const auto& [key, group] : network->getGroups()) {
        group->checkGroundValue();
        // Sort nodes into conducting nodes and ground nodes.
        for (const auto& nodeId : group->nodeIds) {
            // The node is a conducting node
            if(!network->getNodes().at(nodeId)->getGround() && nodeId != size_t(group->groundNodeId)) {
                conductingNodeIds.emplace(nodeId);
            } 
            // The node is an overall ground node, or counts as ground to a group
            else if (!network->getNodes().at(nodeId)->getGround() && nodeId == size_t(group->groundNodeId)) {
                groundNodeIds.emplace(nodeId, iPump);
                iPump++;
            }
        }
    }

    size_t nNodesAndPressurePumps = nNodes + nPressurePumps + groundNodeIds.size();

    A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);
    z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);
    x = Eigen::VectorXd::Zero(nNodesAndPressurePumps);

    A.setZero();
    z.setZero();
}

template<typename T>
void NodalAnalysis<T>::conductNodalAnalysis() {
    clear();
    readConductance();
    readPressurePumps();
    readFlowRatePumps();
    solve();
    setResults();
    initGroundNodes();
}

template<typename T>
bool NodalAnalysis<T>::conductNodalAnalysis(const std::unordered_map<int, std::shared_ptr<sim::CFDSimulator<T>>>& cfdSimulators) {
    clear();
    readConductance();
    readCfdSimulators(cfdSimulators);
    updateReferenceP();
    readPressurePumps();
    readFlowRatePumps();
    solve();
    setResults();
    writeCfdSimulators(cfdSimulators);
    initGroundNodes(cfdSimulators);
    return pressureConvergence;
}

template<typename T>
void NodalAnalysis<T>::readConductance() {
    // loop through channels and build matrix G
    for (const auto& channel : network->getChannels()) {
        auto nodeAMatrixId = channel.second->getNodeAId();
        auto nodeBMatrixId = channel.second->getNodeBId();
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
}

template<typename T>
void NodalAnalysis<T>::updateReferenceP() {
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
}

template<typename T>
void NodalAnalysis<T>::readPressurePumps() {
    int iPump = nNodes;
    // loop through pressurePumps and build matrix B, C and vector e
    for (const auto& pressurePump : network->getPressurePumps()) {
        auto nodeAMatrixId = pressurePump.second->getNodeAId();
        auto nodeBMatrixId = pressurePump.second->getNodeBId();

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
}

template<typename T>
void NodalAnalysis<T>::readFlowRatePumps() {
    // loop through flowRatePumps and build vector i
    for (const auto& flowRatePump : network->getFlowRatePumps()) {
        auto nodeAMatrixId = flowRatePump.second->getNodeAId();
        auto nodeBMatrixId = flowRatePump.second->getNodeBId();
        const T flowRate = flowRatePump.second->getFlowRate();

        if (contains(conductingNodeIds, nodeAMatrixId)){
            z(nodeAMatrixId) = -flowRate;
        }
        if (contains(conductingNodeIds, nodeBMatrixId)){
            z(nodeBMatrixId) = flowRate;
        }
    }
}

template<typename T>
void NodalAnalysis<T>::solve() {
    // solve equation x = A^(-1) * z
    x = A.colPivHouseholderQr().solve(z);
}

template<typename T>
void NodalAnalysis<T>::setResults() {
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
        auto& nodeA = network->getNodes().at(channel.second->getNodeAId());
        auto& nodeB = network->getNodes().at(channel.second->getNodeBId());
        channel.second->setPressure(nodeA->getPressure() - nodeB->getPressure());
    }

    // set flow rate at pressure pumps
    int iPump = nNodes;
    for (auto& pressurePump : network->getPressurePumps()){
        pressurePump.second->setFlowRate(x(iPump));
        iPump++;
    }
}

template<typename T>
void NodalAnalysis<T>::initGroundNodes() {
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
            group->checkGroundValue();
            for (auto channelId : group->channelIds) {
                if (network->getChannels().at(channelId)->getNodeAId() == size_t(group->groundNodeId)) {
                    group->groundChannelId = channelId;
                } 
                else if (network->getChannels().at(channelId)->getNodeBId() == size_t(group->groundNodeId)) {
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
void NodalAnalysis<T>::initGroundNodes(const std::unordered_map<int, std::shared_ptr<sim::CFDSimulator<T>>>& cfdSimulators) {
    // Initialize the ground nodes of the groups
    for (const auto& [key, group] : network->getGroups()) {
        if (!group->initialized && !group->grounded) {
            T pMin = -1.0;
            for (auto nodeId : group->nodeIds) {
                if (nodeId > std::numeric_limits<int>::max()) {
                    throw std::runtime_error("Node ID " + std::to_string(nodeId) + " is too large to be converted to int.");
                }
                if (pMin < 0.0) {
                    pMin = x(nodeId);
                    group->groundNodeId = int(nodeId);  // This is fine becasue we checked the range 
                }
                if (x(nodeId) < pMin) {
                    pMin = x(nodeId);
                    group->groundNodeId = int(nodeId);  // This is fine becasue we checked the range
                }
            }
            group->checkGroundSign();
            for (auto channelId : group->channelIds) {
                if (network->getChannels().at(channelId)->getNodeAId() == size_t(group->groundNodeId)) {
                    group->groundChannelId = channelId;
                } 
                else if (network->getChannels().at(channelId)->getNodeBId() == size_t(group->groundNodeId)) {
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
            std::unordered_map<size_t, T> flowRates_ = cfdSimulator->getFlowRates();
            std::unordered_map<size_t, bool> groundNodes;
            for (const auto& [nodeId, node] : cfdSimulator->getModule()->getNodes()) {
                T flowRate = 0.0;
                if (contains(groundNodeIds, nodeId)) {
                    groundNodes.try_emplace(nodeId, true);
                    for (auto& [key, group] : network->getGroups()) {
                        group->checkGroundValue();
                        if (nodeId == size_t(group->groundNodeId)) {
                            // Get the 2-dimensional flow rate for 2D CFD simulator (m^2/s)
                            flowRate = network->getRectangularChannel(group->groundChannelId)->get2DFlowRate();
                        }
                    }
                } else {
                    groundNodes.try_emplace(nodeId, false);
                }
                flowRates_.at(nodeId) = flowRate;
            }
            cfdSimulator->setGroundNodes(groundNodes);
            cfdSimulator->storeFlowRates(flowRates_);
            cfdSimulator->setInitialized(true);
        }
    }
}

template<typename T>
void NodalAnalysis<T>::readCfdSimulators(const std::unordered_map<int, std::shared_ptr<sim::CFDSimulator<T>>>& cfdSimulators) {
    for (const auto& [key, cfdSimulator] : cfdSimulators) {
        // If module is not initialized (1st loop), loop over channels of fully connected graph
        if ( ! cfdSimulator->getInitialized() ) {
            for (const auto& [key, channel] : cfdSimulator->getModule()->getNetwork()->getChannels()) {
                auto nodeAMatrixId = channel->getNodeAId();
                auto nodeBMatrixId = channel->getNodeBId();
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
                    T flowRate = cfdSimulator->getFlowRates().at(key) * cfdSimulator->getModule()->getOpenings().at(key).height;
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
}

template<typename T>
void NodalAnalysis<T>::writeCfdSimulators(const std::unordered_map<int, std::shared_ptr<sim::CFDSimulator<T>>>& cfdSimulators) {

    // Set the pressures and flow rates on the boundary nodes of the modules
    for (auto& cfdSimulator : cfdSimulators) {
        std::unordered_map<size_t, T> old_pressures = cfdSimulator.second->getPressures();
        std::unordered_map<size_t, T> old_flowrates = cfdSimulator.second->getFlowRates();
        std::unordered_map<size_t, T> pressures_ = cfdSimulator.second->getPressures();
        std::unordered_map<size_t, T> flowRates_ = cfdSimulator.second->getFlowRates();
        for (auto& [key, node] : cfdSimulator.second->getModule()->getNodes()){
            // Communicate pressure to the module
            if (contains(conductingNodeIds, key)) {
                T old_pressure = old_pressures.at(key);
                T new_pressure = node->getPressure();
                T set_pressure = 0.0;
                if (old_pressure > 0 ) {
                    set_pressure = old_pressure + cfdSimulator.second->getAlpha(key) * ( new_pressure - old_pressure );
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
                T new_flowRate = x(groundNodeIds.at(key)) / cfdSimulator.second->getModule()->getOpenings().at(key).width;
                T set_flowRate = 0.0;
                if (old_flowRate > 0 ) {
                    set_flowRate = old_flowRate + 5 * cfdSimulator.second->getAlpha(key) *  ( new_flowRate - old_flowRate );
                } else {
                    set_flowRate = new_flowRate;
                }
                flowRates_.at(key) = set_flowRate;

                if (abs(old_flowRate - new_flowRate) > 1e-2) {
                    pressureConvergence = false;
                }
            }
        }
        cfdSimulator.second->storePressures(pressures_);
        cfdSimulator.second->storeFlowRates(flowRates_);
    }
}

template<typename T>
bool NodalAnalysis<T>::contains( const std::unordered_set<int>& set, int key) {
    bool contain = false;
    for (auto& nodeId : set) {
        if (key == nodeId) {
            contain = true;
        }
    }
    return contain;
}

template<typename T>
bool NodalAnalysis<T>::contains( const std::unordered_map<int, int>& map, int key) {
    bool contain = false;
    for (auto& [nodeId, pumpId] : map) {
        if (key == nodeId) {
            contain = true;
        }
    }
    return contain;
}

template<typename T>
void NodalAnalysis<T>::printSystem() {
    std::cout << "Matrix A:\n" << A  << "\n\n" << std::endl;
    std::cout << "Vector z:\n" << z  << "\n\n" << std::endl;
    std::cout << "Vector x:\n" << x  << "\n\n" << std::endl;
}

}   // namespace nodal