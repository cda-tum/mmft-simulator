/**
 * @file jsonPorter.h
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

    /**
     * @brief Constructor of the Network from a JSON string
     * @param json json string
     * @return Network network
    */
    template<typename T>
    arch::Network<T> networkFromJSON(std::string jsonFile);

    template<typename T>
    sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_);

    template<typename T>
    void resultToJSON(std::string jsonFile);

}   // namespace porting