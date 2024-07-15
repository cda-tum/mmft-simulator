/**
 * @file CFDSim.h
 */

#pragma once

#include <memory>
#include <unordered_map>

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

    /**
     * @brief Conduct theta iterations of the CFD simulation on the network.
     * @param[in] network The network on which the CFD simulations are conducted.
     */
    template<typename T>
    bool conductCFDSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators, int iteration, T alpha);

}   // namespace sim