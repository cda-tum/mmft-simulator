/**
 * @file Simulation.h
 */

#pragma once

#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Simulation;

/**
 * @brief Class that conducts a abstract continuous simulation
 */
template<typename T>
class AbstractContinuous final : public Simulation<T> {
private:

    void saveState() override;

public:

    /**
     * @brief Constructor of the abstract continuous simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractContinuous(arch::Network<T>* network);

    /**
     * @brief Abstract continuous simulation
     * - Conduct nodal analysis
     * - Save state
     */
    void simulate() override;

};

}   // namespace sim
