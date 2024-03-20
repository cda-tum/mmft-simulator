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
class Simulation;

enum class Type;

}   // namespace sim
namespace porting {

template<typename T>
void readNodes (json jsonString, arch::Network<T>& network);

template<typename T>
void readChannels (json jsonString, arch::Network<T>& network);

template<typename T>
sim::Platform readPlatform (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
sim::Type readType (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readFluids (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readDroplets (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
void readDropletInjections (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

template<typename T>
void readBoundaryConditions (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

template<typename T>
void readContinuousPhase (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

template<typename T>
void readSimulators (json jsonString, arch::Network<T>* network);

template<typename T>
void readPumps (json jsonString, arch::Network<T>* network);

template<typename T>
void readResistanceModel (json jsonString, sim::Simulation<T>& simulation);

template<typename T>
int readActiveFixture (json jsonString);

}   // namespace porting
