/**
 * @file AbstractMixing.h
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
class ConcentrationSemantics;

template<typename T>
class Event;

template<typename T>
class MixingModel;

template<typename T>
class DiffusionMixingModel;

template<typename T>
class InstantaneousMixingModel;

template<typename T>
class Mixture;

template<typename T>
class MixtureInjection;

template<typename T>
class Specie;

/**
 * @brief Class that conducts a abstract continuous simulation
 */
template<typename T>
class AbstractMixing : public Simulation<T>, public ConcentrationSemantics<T> {

protected:

    void assertInitialized() const override;

    /**
     * @brief Store the mixtures in this simulation in simulationResult.
    */
    void saveMixtures();

    /**
     * @brief Compute all possible next events for mixing simulation.
     */
    std::vector<std::unique_ptr<Event<T>>> computeMixingEvents();

    /**
     * @brief Protected constructor of the abstract mixing simulator object, used by derived objects
     * @param[in] simType Simulation type of the derived simulation object.
     * @param[in] platform Simulation platform of the derived simulation object.
     * @param[in] network Pointer to the network object, in which the simulation takes place.
     */
    AbstractMixing(Type simType, Platform platform, std::shared_ptr<arch::Network<T>> network);
    
    void saveState() override;

    /**
     * @brief Calculate and set new state of the continuous fluid simulation. Move mixture positions and create new mixtures if necessary.
     * @param[in] timeStep Time step in s for which the new mixtures state should be calculated.
     */
    void calculateNewMixtures(double timeStep);

public:

    /**
     * @brief Constructor of the abstract mixing simulator object.
     * @param[in] network Pointer to the network object, in which the simulation takes place.
     * @throws std::logic_error if the network is null.
     * @note The mixing model is defaulted to an InstantaneousMixingModel.
     */
    AbstractMixing(std::shared_ptr<arch::Network<T>> network);

    /**
     * @brief Abstract mixing simulation for flow with species concentrations.
     * Simulation loop:
     * - Update pressure and flowrates
     * - Calculate next mixture event
     * - Sort events
     * - Propagate mixtures to next event time
     * - Perform next event
     */
    void simulate() override;

};

}   // namespace sim
