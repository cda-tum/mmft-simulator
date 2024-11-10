/**
 * @file NodalAnalysis.h
 */

#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    const std::vector<T> baseline1 = {0.0, 1000.0, 1000.0, 1000.0, 859.216,
                                    791.962, 859.216, 753.628, 753.628, 422.270, 0.0};

    const std::vector<T> baseline2 = {0.0, 1000.0, 577.014, 729.934, 491.525,
                                    469.468, 641.131, 607.210, 427.408, 270.394, 0.0};

    const std::vector<T> baseline3 = {0.0, 1000.0, 687.204, 801.008, 625.223,
                                    601.422, 428.435, 428.007, 331.775, 733.690, 
                                    703.349, 578.736, 525.094, 529.993, 326.021, 
                                    198.163, 0.0};

    const std::vector<T> baseline4 = {0.0, 1000.0, 781.127, 616.813, 
                                    604.130, 535.202, 447.967, 371.026, 
                                    698.425, 695.905, 552.151, 540.371, 
                                    522.332, 454.959, 355.563, 217.833, 0.0};
                                    
    std::vector<std::vector<T>> errors;

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

    int iteration = 0;

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


    void writeSystem();

};


}   // namespace nodal