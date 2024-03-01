/**
 * @file jsonPorter.h
 */

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}   // namespace arch

namespace sim {

// Forward declared dependencies
template<typename T>
class Simulation;

}   // namespace sim

namespace porting {

/**
 * @brief Constructor of the Network from a JSON string
 * @param json json string
 * @return Network network
*/
template<typename T>
arch::Network<T> networkFromJSON(std::string jsonFile);

template<typename T>
arch::Network<T> networkFromJSON(nlohmann::json jsonString);

template<typename T>
sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_);

template<typename T>
sim::Simulation<T> simulationFromJSON(nlohmann::json jsonString, arch::Network<T>* network_);

template<typename T>
void resultToJSON(std::string jsonFile, sim::Simulation<T>* simulation);

template<typename T>
nlohmann::json resultToJSON(sim::Simulation<T>* simulation);

}   // namespace porting