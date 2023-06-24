#include "NodalAnalysis.h"

#include <tuple>
#include <unordered_map>
#include <vector>

#include "Eigen/Dense"
#include <Node.h>
#include <Channel.h>
#include <PressurePump.h>
#include <FlowRatePump.h>
#include <Module.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace nodal {

    template<typename T>
    bool conductNodalAnalysis( const arch::Network<T>* network)
        {
        const int nNodes = network->getNodes().size() - 1;    // -1 due to ground node
        const int nPressurePumps = network->getPressurePumps().size();
        const int nFlowRatePumps = network->getFlowRatePumps().size();
        const int nNodesAndPressurePumps = nNodes + nPressurePumps;
        const int groundNodeValue = 0;

        std::unordered_set<int> conductingNodeIds;
        std::unordered_set<int> groundNodeIds;

        for (const auto& [key, group] : network->getGroups()) {
            for (const auto& nodeId : group->nodeIds) {
                if(nodeId > groundNodeValue && nodeId != group->groundNodeId) {
                    conductingNodeIds.emplace(nodeId);
                }
                groundNodeIds.emplace(group->groundNodeId);
            }
        }

        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);  // matrix A = [G, B; C, D]
        Eigen::VectorXd z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);                          // vector z = [i; e]

        // loop through resistances and build matrix G
        for (const auto& channel : network->getChannels()) {
            auto nodeAMatrixId = channel.second->getNodeA();
            auto nodeBMatrixId = channel.second->getNodeB();
            const T conductance = 1. / channel.second->getResistance();

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

        // loop through modules and include resistances
        for (const auto& [key, module] : network->getModules()) {
            // TODO: remove assertion and adapt code for other module types.
            // For now only LBM modules implemented.
            assert(module->getModuleType() == arch::ModuleType::LBM);

            if ( ! module->getInitialized() ) {
                //std::cout << "[NodalAnalysis] initializing the module based on its network" << std::endl;
                for (const auto& [key, channel] : module->getNetwork()->getChannels()) {
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

            else if ( module->getInitialized() ) {
                for (const auto& [key, node] : module->getNodes()) {
                // Write the module's flowrates into vector i if the node is not a group's ground node
                    if (contains(groundNodeIds, key)) {
                        T flowRate = module->getFlowRates().at(key) * module->getOpenings().at(key).height;
                        std::cout << "[NodalAnalysis] at node " << key << " the flowrate is set at " << flowRate << " [m^3/s] " << std::endl;
                        z(key) = -flowRate;
                    }
                }
            }
        }

        // loop through pressurePumps and build matrix B, C and vector e
        int iPump = nNodes;
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

        for (const auto& [key, group] : network->getGroups()) {
            if (!group->initialized) {
                T pMin = -1.0;
                int ground;
                for (auto nodeId : group->nodeIds) {
                    if (pMin > 0.0 && x(nodeId) < pMin) {
                        pMin = x(nodeId);
                        ground = nodeId;
                    } else if (pMin <= 0.0) {
                        pMin = x(nodeId);
                        ground = nodeId;
                    }
                }
                groundNodeIds.emplace(ground);
                conductingNodeIds.erase(ground);
                group->initialized = true;
            }
        }
        
        for (const auto& [key, module] : network->getModules()) {
            for (const auto& [key, node] : module->getNodes()) {
                if (contains(groundNodeIds, key)) {
                    module->getOpenings().at(key).ground = true;
                } else {
                    module->getOpenings().at(key).ground = false;
                }
            }
            module->setInitialized(true);
        }

        // set pressure of nodes to result value
        for (const auto& [key, group] : network->getGroups()) {
            for (auto nodeMatrixId : group->nodeIds) {
                auto& node = network->getNodes().at(nodeMatrixId);
                if (contains(conductingNodeIds, nodeMatrixId)) {
                    node->setPressure(x(nodeMatrixId) + group->pRef);
                } else if (nodeMatrixId <= groundNodeValue) {
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
        for (auto& module : network->getModules()) {
            std::unordered_map<int, T> old_pressures = module.second->getPressures();
            std::unordered_map<int, T> old_flowrates = module.second->getFlowRates();
            std::unordered_map<int, T> pressures_;
            std::unordered_map<int, T> flowRates_;
            for (auto& [key, node] : module.second->getNodes()){
                // Communicate pressure to the module
                if (contains(conductingNodeIds, key)) {
                    T old_pressure = old_pressures.at(key);
                    T new_pressure = node->getPressure();
                    T set_pressure = 0.0;
                    if (old_pressure > 0 ) {
                        set_pressure = old_pressure + module.second->getAlpha() * ( new_pressure - old_pressure );
                    } else {
                        set_pressure = new_pressure;
                    }
                    pressures_.try_emplace(key, set_pressure);

                    std::cout << "[NodalAnalysis] at node " << key << " the pressure is set at " << set_pressure << " [Pa] " << std::endl;

                    if (abs(old_pressure - new_pressure) > module.second->getEpsilon()) {
                        pressureConvergence = false;
                    }
                }
                // Communicate the flow rate to the module 
                else if (contains(groundNodeIds, key)) {
                    T old_flowRate = old_flowrates.at(key);
                    T new_flowRate = x(key);
                    T set_flowRate = 0.0;
                    if (old_flowRate > 0 ) {
                        set_flowRate = old_flowRate + module.second->getAlpha() * ( new_flowRate - old_flowRate );
                    } else {
                        set_flowRate = new_flowRate;
                    }
                }

            }
            module.second->setPressures(pressures_);
            //module.second->setFlowRates(flowRates_);
        }

        // set flow rate at pressure pumps
        iPump = nNodes;
        for (auto& pressurePump : network->getPressurePumps()){
            pressurePump.second->setFlowRate(x(iPump));
            iPump++;
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

}   // namespace nodal