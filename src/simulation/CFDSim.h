/**
 * @file CFDSim.h
 */

#pragma once

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}   // namespace arch

namespace sim {

    /**
     * @brief Conduct theta iterations of the CFD simulation on the network.
     * @param[in] network The network on which the CFD simulations are conducted.
     */
    template<typename T>
    bool conductCFDSimulation(const arch::Network<T>* network);

}   // namespace sim