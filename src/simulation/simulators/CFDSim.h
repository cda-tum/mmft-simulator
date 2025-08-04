/**
 * @file CFDSim.h
 */

#pragma once

#include <memory>
#include <unordered_map>

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

/**
 * @brief Conduct the NS simulation step (of theta iterations) for all CFD simulators on the simulation. 
 * The amount of collide and stream iterations, theta, is obtained from the update scheme of the simulator.
 * @returns A boolean for whether all simulators have converged, or not.
 */
template<typename T>
bool conductCFDSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators);

/**
 * @brief Conduct the coupling step between the AD lattice and the NS lattice for all simulators.
 */
template<typename T>
void coupleNsAdLattices(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators);

/**
 * @brief Conduct the AD simulation step (of theta iterations) for all CFD simulators on the simulation. 
 * The amount of collide and stream iterations, theta, is obtained from the update scheme of the simulator.
 * @returns A boolean for whether all simulators have converged, or not.
 */
template<typename T>
bool conductADSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators);

}   // namespace sim