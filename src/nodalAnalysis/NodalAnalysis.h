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
void conductNodalAnalysis( const std::shared_ptr<arch::Network<T>> network);

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
bool conductNodalAnalysis( const std::shared_ptr<arch::Network<T>> network, std::unordered_map<int, std::shared_ptr<sim::CFDSimulator<T>>>& cfdSimulators);

bool contains( const std::unordered_set<int>& set, int key);
bool contains( const std::unordered_map<int,int>& map, int key);

}   // namespace nodal
