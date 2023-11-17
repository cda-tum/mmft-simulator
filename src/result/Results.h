/**
 * @file Results.h
 */

#pragma once

#include <unordered_map>

#include "../architecture/Network.h"
#include "../simulation/Droplet.h"
#include "../simulation/Fluid.h"
#include "../simulation/Injection.h"

namespace result {

/**
 * @brief Struct to contain a state specified by time, an unordered map of pressures, an unordered map of flow rates, a vector of clogged channel ids, an unordered map of droplet positions.
 */
template<typename T>
struct State {
    int id;                                                             ///< Sequential id of the state
    T time;                                                             ///< Simulation time at which the following values were calculated.
    std::unordered_map<int, T> pressures;                               ///< Keys are the nodeIds.
    std::unordered_map<int, T> flowRates;                               ///< Keys are the edgeIds (channels and pumps).
    std::unordered_map<int, sim::DropletPosition<T>> dropletPositions;  ///< Only contains the position of droplets that are currently inside the network (key is the droplet id).
    
    /**
     * @brief Constructs a state, which represent a time step during a simulation.
     * @param[in] id Id of the state
     * @param[in] time Value of the current time step.
     */
    State(int id, T time);

    /**
     * @brief Constructs a state, which represent a time step during a simulation.
     * @param[in] id Id of the state
     * @param[in] time Value of the current time step.
     * @param[in] pressures The pressure values at the nodes at the current time step.
     * @param[in] flowRates The flowRate values at the nodes at the current time step.
     */
    State(int id, T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates);

    /**
     * @brief Constructs a state, which represent a time step during a simulation.
     * @param[in] id Id of the state
     * @param[in] time Value of the current time step.
     * @param[in] pressures The pressure values at the nodes at the current time step.
     * @param[in] flowRates The flowRate values at the nodes at the current time step.
     */
    State(int id, T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions);

    /**
     * @brief Function to get pressure at a specific node.
     * @return Pressures of this state in Pa.
     */
    std::unordered_map<int, T>& getPressures() const;

    /**
     * @brief Function to get flow rate at a specific channel.
     * @return Flowrates of this state in m^3/s.
     */
    std::unordered_map<int, T>& getFlowRates() const;

    /**
     * @brief Function to get flow rate at a specific channel.
     * @return Flowrates of this state in m^3/s.
     */
    std::unordered_map<int, T>& getDropletPositions() const;
};

/**
 * @brief Struct to contain the simulation result specified by a chip, an unordered map of fluids, an unordered map of droplets, an unordered map of injections, a vector of states, a continuous fluid id, the maximal adaptive time step and the id of a resistance model.
 */
template<typename T>
struct SimulationResult {
    arch::Network<T>* network;                                      /// Contains the chip, with all the channels and pumps.
    std::unordered_map<int, sim::Fluid<T>>* fluids;                 /// Contains all fluids which were defined (i.e., also the fluids which were created when droplets merged).
    std::unordered_map<int, sim::Droplet<T>>* droplets;             /// Contains all droplets that occurred during the simulation not only the once that were injected (i.e., also merged and splitted droplets)/
    std::unordered_map<int, sim::DropletInjection<T>>* injections;  /// Contains all injections that happened during the simulation.
    std::vector<std::unique_ptr<State<T>>> states;                  /// Contains all states ordered according to their simulation time (beginning at the start of the simulation).    

    int continuousPhaseId;              /// Fluid id which served as the continuous phase.
    T maximalAdaptiveTimeStep;     /// Value for the maximal adaptive time step that was used.
    int resistanceModel;                /// Id of the used resistance model.

    SimulationResult(   arch::Network<T>* network, 
                        std::unordered_map<int, sim::Fluid<T>>* fluids, 
                        std::unordered_map<int, sim::Droplet<T>>* droplets, 
                        std::unordered_map<int, sim::DropletInjection<T>>* injections);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions);

    /**
     * @brief Get the simulated pressures at the nodes.
     * @return Vector of pressure values
     */
    std::unordered_map<int, T>& getFinalPressures() const;

    /**
     * @brief Get the simulated flowrates in the channels.
     * @return Vector of flowrate values
     */
    std::unordered_map<int, T>& getFinalFlowRates() const;

    /**
     * @brief Get the simulated flowrates in the channels.
     * @return Vector of flowrate values
     */
    std::unordered_map<int, T>& getFinalDropletPositions() const;

    /**
     * @brief Get the simulated states that were stored during simulation.
     * @return Vector of states
     */
    std::vector<std::unique_ptr<State<T>>>& getStates() const;
};

}   // namespace results