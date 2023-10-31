#pragma once

#include <olb2D.h>
#include <olb2D.hh>

#include <tuple>

#include "../architecture/Network.h"
#include "../architecture/modules/Module.h"
#include "../architecture/modules/CFDModule.h"

namespace sim {

    /**
     * @brief Conduct theta iterations of the CFD simulation on the network.
     * @param[in] network The network on which the CFD simulations are conducted.
     */
    template<typename T>
    std::tuple<bool, T> conductCFDSimulation(const arch::Network<T>* network);

}   // namespace sim