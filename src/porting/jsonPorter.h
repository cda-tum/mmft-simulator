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
arch::Network<T> networkFromJSON(std::string jsonFile);

/**
 * @brief Define an existing Network from a JSON file
 * @param[in] jsonFile location of the json file
 * @param[in] network network object that needs to be defined
 * @returns Network network
*/
template<typename T>
void networkFromJSON(std::string jsonFile, arch::Network<T>& network);

/**
 * @brief Constructor of the Network from a JSON string
 * @param[in] json json string
 * @returns Network network
*/
template<typename T>
arch::Network<T> networkFromJSON(nlohmann::json jsonString);

/**
 * @brief Constructor of the Simulation from a JSON file
 * @param[in] jsonFile json file
 * @param[in] network pointer to the network on which the simulation acts
 * @returns Simulation simulation
*/
template<typename T>
sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network);

/**
 * @brief Define an existing Simulation from a JSON file
 * @param[in] jsonFile json file
 * @param[in] network pointer to the network on which the simulation acts
 * @param[in] simulation simulation object that needs to be defined
 * @returns Simulation simulation
*/
template<typename T>
void simulationFromJSON(std::string jsonFile, arch::Network<T>* network, sim::Simulation<T>& simulation);

/**
 * @brief Define an existing Simulation from a JSON file
 * @param[in] jsonFile json file
 * @param[in] network pointer to the network on which the simulation acts
 * @param[in] simulation simulation object that needs to be defined
 * @returns Simulation simulation
*/
template<typename T>
void simulationFromJSON(nlohmann::json jsonString, arch::Network<T>* network, sim::Simulation<T>& simulation);


/**
 * @brief Constructor of the Simulation from a JSON string
 * @param[in] json json string
 * @param[in] network pointer to the network on which the simulation acts
 * @returns Simulation simulation
*/
template<typename T>
sim::Simulation<T> simulationFromJSON(nlohmann::json jsonString, arch::Network<T>* network);

/**
 * @brief Generates a json string of the simulation results and writes it in the provided location
 * @param[in] jsonFile location at which the json string should be written
 * @param[in] simulation pointer to the simulation of which the results must be stored
*/
template<typename T>
void resultToJSON(std::string jsonFile, sim::Simulation<T>* simulation);

/**
 * @brief Constructor of a json string of the simulation results
 * @param[in] simulation pointer to the simulation of which the results must be stored
 * @returns ordered_json json string
*/
template<typename T>
nlohmann::ordered_json resultToJSON(sim::Simulation<T>* simulation);

}   // namespace porting