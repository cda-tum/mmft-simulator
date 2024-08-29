/**
 * @file jsonReaders.h
 */

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}   // namespace arch

namespace sim {

// Forward declared dependencies
enum class Platform;

template<typename T>
class lbmSimulator;

template<typename T>
class essLbmSimulator;

template<typename T>
class Simulation;

enum class Type;

}   // namespace sim
namespace porting {

/**
 * @brief Construct and store the nodes in the network as defined by the json string
 * @param[in] jsonString json string
 * @param[in] network network object
*/
template<typename T>
void readNodes (json jsonString, arch::Network<T>& network);

/**
 * @brief Construct and store the channels in the network as defined by the json string
 * @param[in] jsonString json string
 * @param[in] network network object
*/
template<typename T>
void readChannels (json jsonString, arch::Network<T>& network);

/**
 * @brief Construct and store the modules in the network as defined by the json string
 * @param[in] jsonString json string
 * @param[in] network network object
*/
template<typename T>
void readModules (json jsonString, arch::Network<T>& network);

/**
 * @brief Set the platform of the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @return Platform platform
*/
template<typename T>
sim::Platform readPlatform (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Set the simulation type of the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @return Type simulation type
*/
template<typename T>
sim::Type readType (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Construct and store the fluids in the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
*/
template<typename T>
void readFluids (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Construct and store the droplets in the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
*/
template<typename T>
void readDroplets (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readSpecies (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readTissues (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readMixtures (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Construct and store the droplet injections in the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @param[in] activeFixture active fixture
*/
template<typename T>
void readDropletInjections (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

template<typename T>
void readMixtureInjections (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

/**
 * @brief Set the boundary conditions of the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @param[in] activeFixture active fixture
*/
template<typename T>
void readBoundaryConditions (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

/**
 * @brief Set the continuous phase of the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @param[in] activeFixture active fixture
*/
template<typename T>
void readContinuousPhase (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

/**
 * @brief Construct and stores the CFD modules simulators that are included in the network as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
 * @param[in] network pointer to the network
*/
template<typename T>
void readSimulators (json jsonString, sim::Simulation<T>& simulation, arch::Network<T>* network);

/**
 * @brief Construct and stores the update scheme that is used for the Abstract-CFD coupling.
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
*/
template<typename T>
void readUpdateScheme (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Sets channels in the network to pressure or flow rate pump, as defined by the json string
 * @param[in] jsonString json string
 * @param[in] network pointer to the network
 * @param[in] activeFixture active fixture
*/
template<typename T>
void readPumps (json jsonString, arch::Network<T>* network);

/**
 * @brief Construct and store the resistance model of the simulation as defined by the json string
 * @param[in] jsonString json string
 * @param[in] simulation simulation object
*/
template<typename T>
void readResistanceModel (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readMixingModel (json jsonString, sim::Simulation<T>& simulation);

/**
 * @brief Returns the id of the active fixture as defined in the json string
 * @returns The id of the active fixture
*/
template<typename T>
int readActiveFixture (json jsonString);

}   // namespace porting
