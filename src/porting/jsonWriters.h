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
void writePressures (json& jsonString, result::State<T>* state);

template<typename T>
void writeFlowRates (json& jsonString, result::State<T>* state);

template<typename T>
void writeDroplets (json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation);

}   // namespace porting