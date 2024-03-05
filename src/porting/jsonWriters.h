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

using json = nlohmann::json;

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

template<typename T>
auto writePressures (result::State<T>* state);

template<typename T>
auto writeFlowRates (result::State<T>* state);

template<typename T>
auto writeDroplets (result::State<T>* state, sim::Simulation<T>* simulation);

template<typename T>
auto writeFluids (sim::Simulation<T>* simulation);

template<typename T>
std::string writeSimType (sim::Simulation<T>* simulation);

template<typename T>
std::string writeSimPlatform (sim::Simulation<T>* simulation);

}   // namespace porting