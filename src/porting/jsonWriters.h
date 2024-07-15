/**
 * @file jsonWriters.h
 */

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

using ordered_json = nlohmann::ordered_json;

namespace sim {

// Forward declared dependencies
template<typename T>
class Simulation;

}   // namespace sim

namespace result {

// Forward declared dependencies
template<typename T>
class State;

}   // namespace result

namespace porting {

/**
 * @brief Write the pressures at the nodes in a network at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @return The json string containing the result
*/
template<typename T>
auto writePressures (result::State<T>* state);

/**
 * @brief Write the flow rates in the channels of a network at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @return The json string containing the result
*/
template<typename T>
auto writeChannels (result::State<T>* state);

/**
 * @brief Write the location of the vtk results of a module at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @return The json string containing the result
*/
template<typename T>
auto writeModules (result::State<T>* state);

/**
 * @brief Write the droplet positions at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return The json string containing the result
*/
template<typename T>
auto writeDroplets (result::State<T>* state, sim::Simulation<T>* simulation);

/**
 * @brief Write set of fluids of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return The json string containing the result
*/
template<typename T>
auto writeFluids (sim::Simulation<T>* simulation);

template<typename T>
auto writeMixtures (sim::Simulation<T>* simulation);

/**
 * @brief Return the simulation type of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return String of the simulation type
*/
template<typename T>
std::string writeSimType (sim::Simulation<T>* simulation);

/**
 * @brief Return the platform of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return String of the platform
*/
template<typename T>
std::string writeSimPlatform (sim::Simulation<T>* simulation);

}   // namespace porting