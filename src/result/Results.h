/**
 * @file Results.h
 */

#pragma once

#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace arch {
    
// Forward declared dependencies
template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

template<typename T>
class DropletPosition;

template<typename T>
class Fluid;
}

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
    std::unordered_map<int, std::string> vtkFiles;
    std::unordered_map<int, sim::DropletPosition<T>> dropletPositions;  ///< Only contains the position of droplets that are currently inside the network (key is the droplet id).
    std::unordered_map<int, std::deque<sim::MixturePosition<T>>> mixturePositions;  ///< Only contains the position of mixtures that are currently inside the network (key is the channel id).
    std::unordered_map<int, int> filledEdges;                           ///< Contains the mixture ids that fill the edges of the network <EdgeID, MixtureID>

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
    State(int id, T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::string> vtkFiles);

    /**
     * @brief Constructs a state, which represent a time step during a simulation.
     * @param[in] id Id of the state
     * @param[in] time Value of the current time step.
     * @param[in] pressures The pressure values at the nodes at the current time step.
     * @param[in] flowRates The flowRate values at the nodes at the current time step.
     * @param[in] dropletPositions The positions of the droplets at the current time step.
     */
    State(int id, T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions);

    /**
     * @brief Constructs a state, which represent a time step during a simulation.
     * @param[in] id Id of the state
     * @param[in] time Value of the current time step.
     * @param[in] pressures The pressure values at the nodes at the current time step.
     * @param[in] flowRates The flowRate values at the nodes at the current time step.
     * @param[in] mixturePositions The positions of the mixtures at the current time step.
     */
    State(int id, T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::deque<sim::MixturePosition<T>>> mixturePositions, std::unordered_map<int, int> filledEdges);

    /**
     * @brief Function to get pressure at a specific node.
     * @return Pressures of this state in Pa.
     */
    const std::unordered_map<int, T>& getPressures() const;

    /**
     * @brief Function to get flow rate at a specific channel.
     * @return Flowrates of this state in m^3/s.
     */
    const std::unordered_map<int, T>& getFlowRates() const;

    /**
     * @brief Function to get flow rate at a specific channel.
     * @return Flowrates of this state in m^3/s.
     */
    const std::unordered_map<int, std::string>& getVtkFiles() const;

    /**
     * @brief Function to get the droplet positions of this state.
     * @return DropletPositions.
     */
    std::unordered_map<int, sim::DropletPosition<T>>& getDropletPositions();

    /**
     * @brief Function to get the mixture positions of this state.
     * @return MixturePositions.
     */
    std::unordered_map<int, std::deque<sim::MixturePosition<T>>>& getMixturePositions();

    /**
     * @brief Function to get the time of a state.
     * @return Time in s.
     */
    T getTime() const;

    /**
     * @brief Print the state
    */
    const void printState();
};

/**
 * @brief Struct to contain the simulation result specified by a chip, an unordered map of fluids, an unordered map of droplets, an unordered map of injections, a vector of states, a continuous fluid id, the maximal adaptive time step and the id of a resistance model.
 */
template<typename T>
struct SimulationResult {
    arch::Network<T>* network;                                      /// Contains the chip, with all the channels and pumps.
    std::unordered_map<int, sim::Fluid<T>>* fluids;                 /// Contains all fluids which were defined (i.e., also the fluids which were created when droplets merged).
    std::unordered_map<int, sim::Droplet<T>>* droplets;             /// Contains all droplets that occurred during the simulation not only the once that were injected (i.e., also merged and splitted droplets)
    std::unordered_map<int, sim::Mixture<T>*> mixtures;
    std::unordered_map<int, sim::Specie<T>>* species;
    std::unordered_map<int, int> filledEdges;
    std::vector<std::unique_ptr<State<T>>> states;                  /// Contains all states ordered according to their simulation time (beginning at the start of the simulation).    

    int continuousPhaseId;              /// Fluid id which served as the continuous phase.
    T maximalAdaptiveTimeStep;     /// Value for the maximal adaptive time step that was used.
    int resistanceModel;                /// Id of the used resistance model.

    /**
     * @brief Constructs an uninitialized simulationResult object, which stores all results of a simulation.
     */
    SimulationResult();

    /**
     * @brief Constructs a simulationResult object, which stores all results of a simulation.
     * @param[in] network pointer to the network on which the simulation acts
     * @param[in] fluids a pointer to the unordered map of fluids in the simulation
     * @param[in] droplets a pointer to the unordered map of droplets in the simulation
     */
    SimulationResult(   arch::Network<T>* network, 
                        std::unordered_map<int, sim::Fluid<T>>* fluids, 
                        std::unordered_map<int, sim::Droplet<T>>* droplets);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::string> vtkFiles);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions);

    /**
     * @brief Adds a state to the simulation results.
     * @param[in] state
    */
    void addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::deque<sim::MixturePosition<T>>> mixturePositions);

    /**
     * @brief Get the simulated flowrates in the channels.
     * @return Vector of flowrate values
     */
    const std::unordered_map<int, T>& getFinalMixturePositions() const;

    /**
     * @brief Get the simulated states that were stored during simulation.
     * @return Vector of states
     */
    const std::vector<std::unique_ptr<State<T>>>& getStates() const;

    /**
     * @brief Print all the states that were stored during simulation.
    */
    const void printStates() const;

    /**
     * @brief Print last state that was stored during simulation.
    */
    const void printLastState() const;

    /**
     * @brief Print state key
     * @param[in] key The key of the state that should be printed.
    */
    const void printState(int key) const;

    const void setMixtures(std::unordered_map<int, sim::Mixture<T>*> mixtures);

    const std::unordered_map<int, sim::Mixture<T>*>& getMixtures() const;

    const void printMixtures();

    const void writeMixture(int mixtureId);
};

}   // namespace results