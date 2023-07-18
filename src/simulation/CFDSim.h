#pragma once

#include <olb2D.h>
#include <olb2D.hh>

#include "../architecture/Network.h"
#include "../architecture/Module.h"
#include "../architecture/lbmModule.h"

namespace sim {

    /**
     * @brief Conduct theta iterations of the CFD simulation on the network.
     * @param[in] network The network on which the CFD simulations are conducted.
     */
    template<typename T>
    bool conductCFDSimulation(const arch::Network<T>* network);

}   // namespace sim