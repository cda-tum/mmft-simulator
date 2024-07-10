/**
 * @file NodalAnalysis.h
 */

#pragma once

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

    Eigen::MatrixXd A;      // matrix A = [G, B; C, D]
    Eigen::VectorXd z;      // vector z = [i; e]
    Eigen::VectorXd x;      // vector x = [v; j]

    void readConductance();         // loop through channels and build matrix G
    void updateReferenceP();        // update the reference pressure for each group
    void readPressurePumps();       // loop through pressure pumps and build matrix B, C and vector e
    void readFlowRatePumps();       // loop through flowRate pumps and build vector i
    void solve();                   // solve equation x = A^(-1) * z
    void setResults();              // set pressure of nodes to v and flow rate at pressure pumps to j
    void initGroundNodes();         // initialize the ground nodes of the groups

    // For hybrid simulations
    void readCfdSimulators(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);
    void writeCfdSimulators(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

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
    void conductNodalAnalysis(std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

};

/**
 * @brief Conducts the Modifed Nodal Analysis (e.g., http://qucs.sourceforge.net/tech/node14.html) and computes the pressure levels for each node.
 * Hence, the passed nodes contain the final pressure levels when the function is finished.
 *
 * @param[in,out] nodes List of nodes, which contain the result (pressure values) of the modified nodal analysis.
 * @param[in] channels List of channels.
 * @param[in,out] pressurePumps List of pressure pumps.
 * @param[in] flowRatePumps List of flowrate pumps.
 */
template<typename T>
void conductNodalAnalysis( const arch::Network<T>* network);

/**
 * @brief Conducts the Modifed Nodal Analysis (e.g., http://qucs.sourceforge.net/tech/node14.html) and computes the pressure levels for each node.
 * Hence, the passed nodes contain the final pressure levels when the function is finished.
 *
 * @param[in,out] nodes List of nodes, which contain the result (pressure values) of the modified nodal analysis.
 * @param[in] channels List of channels.
 * @param[in,out] pressurePumps List of pressure pumps.
 * @param[in] flowRatePumps List of flowrate pumps.
 */
template<typename T>
bool conductNodalAnalysis( const arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

bool contains( const std::unordered_set<int>& set, int key);
bool contains( const std::unordered_map<int,int>& map, int key);

}   // namespace nodal