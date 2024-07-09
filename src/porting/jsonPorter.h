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
 * @brief Constructor of the Network from a JSON file
 * @param[in] jsonFile Location of the json file
 * @returns Network network
*/
template<typename T>
std::shared_ptr<arch::Network<T>> networkFromJSON(std::string jsonFile);

/**
 * @brief Constructor of the Simulation from a JSON file
 * @param[in] jsonFile json file
 * @param[in] network pointer to the network on which the simulation acts
 * @returns Simulation simulation
*/
template<typename T>
std::shared_ptr<sim::Simulation<T>> simulationFromJSON(std::string jsonFile, std::shared_ptr<arch::Network<T>> network);

/**
 * @brief Generates a json string of the simulation results and writes it in the provided location
 * @param[in] jsonFile location at which the json string should be written
 * @param[in] simulation pointer to the simulation of which the results must be stored
*/
template<typename T>
void resultToJSON(std::string jsonFile, std::shared_ptr<sim::Simulation<T>> simulation);

/**
 * @brief Constructor of a json string of the simulation results
 * @param[in] simulation pointer to the simulation of which the results must be stored
 * @returns ordered_json json string
*/
template<typename T>
nlohmann::ordered_json resultToJSON(std::shared_ptr<sim::Simulation<T>> simulation);

}   // namespace porting
