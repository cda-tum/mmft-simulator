/**
 * @file Simulation.h
 */

#pragma once

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
class ChannelPosition;

template<typename T>
class Network;

}

namespace result {

// Forward declared dependencies
template<typename T>
class SimulationResult;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

template<typename T>
class DropletInjection;

template<typename T>
class Event;

template<typename T>
class Fluid;

template<typename T>
class ResistanceModel;

enum class Type {
    Abstract,       ///< A simulation in the 1D abstraction level
    Hybrid,         ///< A simulation combining the 1D and CFD abstraction levels
    CFD             ///< A simulation in the CFD abstraction level
};

enum class Platform {
    Continuous,     ///< A simulation with a single continuous fluid.
    BigDroplet,     ///< A simulation with droplets filling a channel cross-section
    Mixing          ///< A simulation wit multiple miscible fluids.
};

/**
 * @brief Class that conducts the simulation and owns all parameters necessary for it.
 */
template<typename T>
class Simulation {
private:
    int fixtureId = 0;
    Type simType = Type::Abstract;                                                      ///< The type of simulation that is being done.                                      
    Platform platform = Platform::Continuous;                                           ///< The microfluidic platform that is simulated in this simulation.
    arch::Network<T>* network;                                                          ///< Network for which the simulation should be conducted.
    std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids;                          ///< Fluids specified for the simulation.
    std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets;                      ///< Droplets which are simulated in droplet simulation.
    std::unordered_map<int, std::unique_ptr<DropletInjection<T>>> dropletInjections;    ///< Injections of droplets that should take place during a droplet simulation.
    ResistanceModel<T>* resistanceModel;                                                ///< The resistance model used for te simulation.
    int continuousPhase = 0;                                                            ///< Fluid of the continuous phase.
    int iteration = 0;
    int maxIterations = 1e5;
    T maximalAdaptiveTimeStep = 0;                                                      ///< Maximal adaptive time step that is applied when droplets change the channel.
    T time = 0.0;                                                                       ///< Current time of the simulation.
    T dt = 0.01;
    T writeInterval = 0.1;
    T tMax = 100;
    bool eventBasedWriting = false;
    bool dropletsAtBifurcation = false;                                  ///< If one or more droplets are currently at a bifurcation. Triggers the usage of the maximal adaptive time step.
    std::unique_ptr<result::SimulationResult<T>> simulationResult = nullptr;

    /**
     * @brief Initializes the resistance model and the channel resistances of the empty channels.
     */
    void initialize();

    /**
     * @brief Update the droplet resistances of the channels based on the current positions of the droplets.
     */
    void updateDropletResistances();

    /**
     * @brief Compute all possible next events.
     */
    std::vector<std::unique_ptr<Event<T>>> computeEvents();

    /**
     * @brief Moves all droplets according to the given time step.
     * @param[in] timeStep to which the droplets should be moved to.
     */
    void moveDroplets(T timeStep);

    /**
     * @brief Store simulation parameters to the result.
     * @param[in, out] result Reference to the simulation result in which all current parameters of the simulation should be stored.
     */
    void storeSimulationParameters(result::SimulationResult<T>& result);

    /**
     * @brief Store all simulation results to the result.
     * @param[in, out] result Reference to the simulation result in which all current parameters of the simulation should be stored.
     */
    void storeSimulationResults(result::SimulationResult<T>& result);

    /**
     * @brief Store the current simulation state in simulationResult.
    */
    void saveState();

public:
    /**
     * @brief Creates simulation.
     */
    Simulation();

    /**
     * @brief Create fluid.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] concentration Concentration of the fluid in % (between 0.0 and 1.0).
     * @return Pointer to created fluid.
     */
    Fluid<T>* addFluid(T viscosity, T density, T concentration);

    /**
     * @brief Create droplet.
     * @param[in] fluidId Unique identifier of the fluid the droplet consists of.
     * @param[in] volume Volume of the fluid in m^3.
     * @return Pointer to created droplet.
     */
    Droplet<T>* addDroplet(int fluidId, T volume);

    /**
     * @brief Create injection.
     * @param[in] dropletId Id of the droplet that should be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s.
     * @param[in] channelId Id of the channel, where droplet should be injected.
     * @param[in] injectionPosition Position inside the channel at which the droplet should be injected (relative to the channel length between 0.0 and 1.0).
     * @return Pointer to created injection.
     */
    DropletInjection<T>* addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition);

    /**
     * @brief Set the platform of the simulation.
     * @param[in] platform
     */
    void setPlatform(Platform platform);

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    void setType(Type type);

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    void setFixtureId(int fixtureId);

    /**
     * @brief Set the network for which the simulation should be conducted.
     * @param[in] network Network on which the simulation will be conducted.
     */
    void setNetwork(arch::Network<T>* network);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setFluids(std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setDroplets(std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluidId The ID of the fluid the continuous phase consists of.
     */
    void setContinuousPhase(int fluidId);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setContinuousPhase(Fluid<T>* fluid);

    /**
     * @brief Define which resistance model should be used for the channel and droplet resistance calculations.
     * @param[in] model The resistance model to be used.
     */
    void setResistanceModel(ResistanceModel<T>* model);

    /**
     * @brief Get the platform of the simulation.
     * @return platform of the simulation
     */
    Platform getPlatform();

    /**
     * @brief Get the type of the simulation.
     * @return type of the simulation
     */
    Type getType();

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    int getFixtureId();

    /**
     * @brief Get the network.
     * @return Network or nullptr if no network is specified.
     */
    arch::Network<T>* getNetwork();

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    Fluid<T>* getFluid(int fluidId);

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    std::unordered_map<int, std::unique_ptr<Fluid<T>>>& getFluids();

    /**
     * @brief Get droplet
     * @param dropletId Id of the droplet
     * @return Pointer to droplet with the corresponding id
     */
    Droplet<T>* getDroplet(int dropletId);

    /**
     * @brief Gets droplet that is present at the corresponding node (i.e., the droplet spans over this node).
     * @param nodeId The id of the node
     * @return Pointer to droplet or nullptr if no droplet was found
     */
    Droplet<T>* getDropletAtNode(int nodeId);

    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    DropletInjection<T>* getDropletInjection(int injectionId);

    /**
     * @brief Get the continuous phase.
     * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
     */
    Fluid<T>* getContinuousPhase();

    /**
     * @brief Get the results of the simulation.
     * @return Pointer to the result of the simulation.
     */
    result::SimulationResult<T>* getSimulationResults();

    /**
     * @brief Creates a new fluid out of two existing fluids.
     * @param fluid0Id Id of the first fluid.
     * @param volume0 The volume of the first fluid.
     * @param fluid1Id Id of the second fluid.
     * @param volume1 The volume of the second fluid.
     * @return Pointer to new fluid.
     */
    Fluid<T>* mixFluids(int fluid0Id, T volume0, int fluid1Id, T volume1);

    /**
     * @brief Creates a new droplet from two existing droplets.
     Please note that this only creates a new droplet inside the simulation, but the actual boundaries have to be set separately, which is usually done inside the corresponding merge events.
        * @param droplet0Id Id of the first droplet.
        * @param droplet1Id Id of the second droplet.
        * @return Pointer to new droplet.
        */
    Droplet<T>* mergeDroplets(int droplet0Id, int droplet1Id);

    /**
     * @brief Conduct the simulation.
     * @return The result of the simulation containing all intermediate simulation steps and calculated parameters.
     */
    void simulate();

    /**
     * @brief Print the results as pressure at the nodes and flow rates at the channels
     */
    void printResults();
};

}   // namespace sim