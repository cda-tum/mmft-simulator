/**
 * @file NodalAnalysis.h
 */

#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Eigen/Dense"

using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}   // namespace arch

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

}   // namespace sim

namespace nodal {

template<typename T>
class NodalAnalysis {
private:
    const arch::Network<T>* network;

    int nNodes;             // Number of nodes
    int nPressurePumps;     // Number of pressurePumps

    bool pressureConvergence;

    Eigen::MatrixXd A;      // matrix A = [G, B; C, D]
    Eigen::VectorXd z;      // vector z = [i; e]
    Eigen::VectorXd x;      // vector x = [v; j]

    std::unordered_set<int> conductingNodeIds;
    std::unordered_map<int, int> groundNodeIds;

    void readConductance();         // loop through channels and build matrix G
    void updateReferenceP();        // update the reference pressure for each group
    void readPressurePumps();       // loop through pressure pumps and build matrix B, C and vector e
    void readFlowRatePumps();       // loop through flowRate pumps and build vector i
    void solve();                   // solve equation x = A^(-1) * z
    void setResults();              // set pressure of nodes to v and flow rate at pressure pumps to j
    void initGroundNodes();         // initialize the ground nodes of the groups
    void clear();

    // For hybrid simulations
    void readCfdSimulators(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);
    void writeCfdSimulators(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);
    void initGroundNodes(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

    // Helper functions
    bool contains( const std::unordered_set<int>& set, int key);
    bool contains( const std::unordered_map<int,int>& map, int key);
    void printSystem();

public:
    /**
     * @brief Creates a NodalAnalysis object
     */
    NodalAnalysis(const arch::Network<T>* network);

    /**
     * @brief Conducts the Modifed Nodal Analysis (e.g., http://qucs.sourceforge.net/tech/node14.html) and computes the pressure levels for each node.
     * Hence, the passed nodes contain the final pressure levels when the function is finished.
     * 
     * @param[in] network Pointer to the network on which the nodal analysis is conducted. Results are stored in the network's nodes.
     */
    void conductNodalAnalysis();

    /**
     * @brief Conducts the Modifed Nodal Analysis (e.g., http://qucs.sourceforge.net/tech/node14.html) and computes the pressure levels for each node.
     * Hence, the passed nodes contain the final pressure levels when the function is finished.
     * 
     * @param[in] network Pointer to the network on which the nodal analysis is conducted. Results are stored in the network's nodes.
     * @param[in] cfdSimulators The cfd simulators for a hybrid simulation
     */
    bool conductNodalAnalysis(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

};


}   // namespace nodal