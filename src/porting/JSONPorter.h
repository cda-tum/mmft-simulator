#pragma once

#include <memory>
#include <fstream>
#include <unordered_map>

#include "../architecture/Channel.h"
#include "../architecture/FlowRatePump.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"
#include "../architecture/Platform.h"
#include "../architecture/PressurePump.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace porting {

    /**
     * @brief Constructor of the Network from a JSON string
     * @param json json string
     * @return Network network
    */
    template<typename T>
    arch::Network<T> networkFromJSON(std::string jsonFile);

    template<typename T>
    arch::Network<T> networkToJSON(std::string jsonFile);

}   // namespace porting