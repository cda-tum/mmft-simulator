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
class Module;

template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Event;

template<typename T>
class MixingModel;

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
class AbstractMixing : public Simulation<T> {
private:
    std::unordered_map<int, std::unique_ptr<Specie<T>>> species;                        ///< Species specified for the simulation.
    std::unordered_map<int, std::unique_ptr<Mixture<T>>> mixtures;                      ///< Mixtures present in the simulation.
    std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>> mixtureInjections;    ///< Injections of fluids that should take place during the simulation.
    std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>> permanentMixtureInjections; ///< Permanent injections of fluids that should take place during the simulation. Used to simulate a fluid change or include an exposure of the system to a specific mixture/concentration.
    MixingModel<T>* mixingModel = nullptr;                                              ///< The mixing model used for a mixing simulation.

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
    AbstractMixing(Type simType, Platform platform, arch::Network<T>* network);

    void saveState() override;

public:
    /**
     * @brief Constructor of the abstract mixing simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractMixing(arch::Network<T>* network);

    /**
     * @brief Create specie.
     * @param[in] diffusivity Diffusion coefficient of the specie in the carrier medium in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in the carrier medium in g/m^3.
     * @return Pointer to created specie.
     */
    Specie<T>* addSpecie(T diffusivity, T satConc);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] species Unordered map of specie ids and pointer to that specie.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);
    
    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);

    /**
     * @brief Create mixture injection. The injection is always performed at the beginning (position 0.0) of the channel.
     * @param[in] mixtureId Id of the mixture that should be injected.
     * @param[in] channelId Id of the channel, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @return Pointer to created injection.
     */
    MixtureInjection<T>* addMixtureInjection(int mixtureId, int channelId, T injectionTime);

    /**
     * @brief Create permanent mixture injection which will continuously inject the mixture.
     *        The injection is always performed at the beginning (position 0.0) of the channel.
     * @param[in] mixtureId Id of the mixture that should be injected.
     * @param[in] channelId Id of the channel, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @return Pointer to created injection.
     */
    MixtureInjection<T>* addPermanentMixtureInjection(int mixtureId, int channelId, T injectionTime);

    /**
     * @brief Define which mixing model should be used for the concentrations.
     * @param[in] model The mixing model to be used.
     */
    void setMixingModel(MixingModel<T>* model);

    /**
     * @brief Calculate and set new state of the continuous fluid simulation. Move mixture positions and create new mixtures if necessary.
     * @param[in] timeStep Time step in s for which the new mixtures state should be calculated.
     */
    void calculateNewMixtures(double timeStep);

    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    MixtureInjection<T>* getMixtureInjection(int injectionId) const;

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    const std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>>& getMixtureInjections() const;

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    const std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>>& getPermanentMixtureInjections() const;

    /**
     * @brief Get the mixing model that is used in the simulation.
     * @return The mixing model of the simulation.
     */
    MixingModel<T>* getMixingModel() const;

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture
     * @return Pointer to mixture with the correspondig id
     */
    Mixture<T>* getMixture(int mixtureId) const;

    /**
     * @brief Get mixtures.
     * @return Reference to the unordered map of mixtures
     */
    const std::unordered_map<int, std::unique_ptr<Mixture<T>>>& getMixtures() const;

    /**
     * @brief Get specie.
     * @param specieId Id of the specie.
     * @return Pointer to specie with the correspondig id
     */
    Specie<T>* getSpecie(int specieId) const;

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture
     * @return Pointer to mixture with the correspondig id
     */
    const std::unordered_map<int, std::unique_ptr<Specie<T>>>& getSpecies() const;

    /**
     * @brief Abstract mixing simulation for flow with species concentrations
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
