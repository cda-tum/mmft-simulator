/**
 * @file Simulation.h
 */

#pragma once

#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <string>
#include <tuple>
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
class MembraneModel;

/**
 * @brief Class that conducts an abstract continuous simulation
 */
template<typename T>
class AbstractMembrane final : public AbstractMixing<T> {
private:
    MembraneModel<T>* membraneModel = nullptr;                                          ///< The membrane model used for an OoC simulation.

    void assertInitialized() const override;

public:

    /**
     * @brief Constructor of the abstract membrane simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractMembrane(arch::Network<T>* network);

    /**
     * @brief Define which membrane model should be used for the membrane resistance calculations.
     * @param[in] model The membrane model to be used.
     */
    void setMembraneModel(MembraneModel<T>* model);

    /**
     * @brief Get the membrane model that is used in the simulation.
     * @return The membrane model of the simulation
     */
    MembraneModel<T>* getMembraneModel() const;
    
    /**
     * @brief Abstract simulation with membranes/tanks
     * Simulation loop:
     * - conduct nodal analysis
     * - compute events
     * - sort events
     * - search for next event (break if no event is left)
     * - perform next event
     */
    void simulate() override;

};

}   // namespace sim
