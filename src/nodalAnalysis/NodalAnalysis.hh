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

        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(nNodesAndPressurePumps, nNodesAndPressurePumps);  // matrix A = [G, B; C, D]
        Eigen::VectorXd z = Eigen::VectorXd::Zero(nNodesAndPressurePumps);                          // vector z = [i; e]

        // loop through resistances and build matrix G
        for (const auto& channel : network->getChannels()) {
            auto nodeAMatrixId = channel.second->getNodeA();
            auto nodeBMatrixId = channel.second->getNodeB();
            const T conductance = 1. / channel.second->getResistance();

            // main diagonal elements of G
            if (nodeAMatrixId > groundNodeValue) {
                A(nodeAMatrixId, nodeAMatrixId) += conductance;
            }

            if (nodeBMatrixId > groundNodeValue) {
                A(nodeBMatrixId, nodeBMatrixId) += conductance;
            }

            // minor diagonal elements of G (if no ground node was present)
            if (nodeAMatrixId > groundNodeValue && nodeBMatrixId > groundNodeValue) {
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
                    //std::cout << "[NodalAnalysis] channel " << key << " with nodes " << nodeAMatrixId <<
                    //" and " << nodeBMatrixId << " has resistance of " << channel->getResistance() << std::endl;

                    // main diagonal elements of G
                    if (nodeAMatrixId > groundNodeValue) {
                        A(nodeAMatrixId, nodeAMatrixId) += conductance;
                    }

                    if (nodeBMatrixId > groundNodeValue) {
                        A(nodeBMatrixId, nodeBMatrixId) += conductance;
                    }

                    // minor diagonal elements of G (if no ground node was present)
                    if (nodeAMatrixId > groundNodeValue && nodeBMatrixId > groundNodeValue) {
                        A(nodeAMatrixId, nodeBMatrixId) -= conductance;
                        A(nodeBMatrixId, nodeAMatrixId) -= conductance;
                    }
                }
                module->setInitialized(true);
            }

            else if ( module->getInitialized() ) {
                // Write the module's resistances into matrix G
                for (const auto& [key, node] : module->getNodes()) {
                    T flowRate = module->getFlowRates().at(key) * module->getOpenings().at(key).height;
                    std::cout << "[NodalAnalysis] at node " << key << " the flowrate is set at " << flowRate << " [m^3/s] " << std::endl;
                    z(key) = -flowRate;
                }
            }
        }

        // loop through pressurePumps and build matrix B, C and vector e
        int iPump = nNodes;
        for (const auto& pressurePump : network->getPressurePumps()) {
            auto nodeAMatrixId = pressurePump.second->getNodeA();
            auto nodeBMatrixId = pressurePump.second->getNodeB();

            if (nodeAMatrixId > groundNodeValue) {
                A(nodeAMatrixId, iPump) = -1;   // matrix B
                A(iPump, nodeAMatrixId) = -1;   // matrix C
            }

            if (nodeBMatrixId > groundNodeValue) {
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

            if (nodeAMatrixId > groundNodeValue){
                z(nodeAMatrixId) = -flowRate;
            }
            if (nodeBMatrixId > groundNodeValue){
                z(nodeBMatrixId) = flowRate;
            }
        }

        // solve equation x = A^(-1) * z
        VectorXd x = A.colPivHouseholderQr().solve(z);

        // set pressure of nodes to result value
        for (const auto& [key,node] : network->getNodes()) {
            auto nodeMatrixId = node->getId();
            if (nodeMatrixId > groundNodeValue) {
                node->setPressure(x(nodeMatrixId));
            } else {
                node->setPressure(0.0);
            }
        }

        for (auto& channel : network->getChannels()) {
            auto& nodeA = network->getNodes().at(channel.second->getNodeA());
            auto& nodeB = network->getNodes().at(channel.second->getNodeB());
            channel.second->setPressure(nodeA->getPressure() - nodeB->getPressure());
        }

        bool pressureConvergence = true;

        // Set the pressures on the boundary nodes of the modules
        for (auto& module : network->getModules()) {
            std::unordered_map<int, T> old_pressures = module.second->getPressures();
            std::unordered_map<int, T> pressures_;
            for (auto& [key, node] : module.second->getNodes()){
                T old_pressure = old_pressures.at(key);
                T new_pressure = node->getPressure();
                T set_pressure = 0.0;
                if (old_pressure > 0 ) {
                    set_pressure = old_pressure + module.second->getAlpha() * ( new_pressure - old_pressure );
                } else {
                    set_pressure = new_pressure;
                }
                pressures_.try_emplace(key, set_pressure);
                /*
                std::cout << "[NodalAnalysis] at node " << key << " we set pressure " << set_pressure << 
                    " from: \n old pressure: " << old_pressure << " and new pressure: " << new_pressure << std::endl;
                */
                std::cout << "[NodalAnalysis] at node " << key << " the pressure is set at " << set_pressure << " [Pa] " << std::endl;

                if (abs(old_pressure - new_pressure) > module.second->getEpsilon()) {
                    pressureConvergence = false;
                }
            }
            module.second->setPressures(pressures_);
        }

        // set flow rate at pressure pumps
        iPump = nNodes;
        for (auto& pressurePump : network->getPressurePumps()){
            pressurePump.second->setFlowRate(x(iPump));
            iPump++;
        }

        return pressureConvergence;
    }

}   // namespace nodal