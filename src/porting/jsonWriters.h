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

namespace arch {

template<typename T>
class Network;

}   // namespace arch

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
auto writePressures (const result::State<T>* state);

/**
 * @brief Write the flow rates in the channels of a network at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @return The json string containing the result
*/
template<typename T>
auto writeChannels (const arch::Network<T>* network, const result::State<T>* state);

/**
 * @brief Write the location of the vtk results of a module at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @return The json string containing the result
*/
template<typename T>
auto writeModules (const result::State<T>* state);

/**
 * @brief Write the droplet positions at a state (timestamp) of the simulation
 * @param[in] state the state (timestamp) of the simulation that should be written
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return The json string containing the result
*/
template<typename T>
auto writeDroplets (const result::State<T>* state, const sim::AbstractDroplet<T>* simulation);

/**
 * @brief Write set of fluids of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return The json string containing the result
*/
template<typename T>
auto writeFluids (const sim::Simulation<T>* simulation);

template<typename T>
auto writeMixtures (const sim::AbstractMixing<T>* simulation);

template<typename T>
auto writeMixtures (json& jsonString, const result::State<T>* state, const sim::AbstractMixing<T>* simulation);

/**
 * @brief Return the simulation type of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return String of the simulation type
*/
template<typename T>
std::string writeSimType (const sim::Simulation<T>* simulation);

/**
 * @brief Return the platform of the simulation
 * @param[in] simulation pointer to the simulation of which the results are written
 * @return String of the platform
*/
template<typename T>
std::string writeSimPlatform (const sim::Simulation<T>* simulation);

}   // namespace porting