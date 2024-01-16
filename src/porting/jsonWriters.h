/**
 * @file jsonWriters.h
 */

#pragma once

#include <memory>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "../architecture/Channel.h"
#include "../architecture/FlowRatePump.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"
#include "../architecture/PressurePump.h"

#include "../simulation/Simulation.h"

#include "../result/Results.h"

#include "nlohmann/json.hpp"

namespace porting {

    template<typename T>
    void writePressures (json& jsonString, result::State<T>* state);

    template<typename T>
    void writeFlowRates (json& jsonString, result::State<T>* state);

    template<typename T>
    void writeDroplets (json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation);

    template<typename T>
    void writeConcentrations (json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation);

}   // namespace porting