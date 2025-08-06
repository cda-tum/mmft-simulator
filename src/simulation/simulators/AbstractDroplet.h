/**
 * @file AbstractDroplet.h
 */

#pragma once

#include <functional>
#include <memory>
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
class Droplet;

template<typename T>
class DropletInjection;

template<typename T>
class Event;

/**
 * @brief Class that conducts a abstract continuous simulation
 */
template<typename T>
class AbstractDroplet final : public Simulation<T> {
private:
    std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets;                      ///< Droplets which are simulated in droplet simulation.
    std::unordered_map<int, std::unique_ptr<DropletInjection<T>>> dropletInjections;    ///< Injections of droplets that should take place during a droplet simulation.
    bool dropletsAtBifurcation = false;                                                 ///< If one or more droplets are currently at a bifurcation. Triggers the usage of the maximal adaptive time step.

    void saveState() override;

    /**
     * @brief Update the droplet resistances of the channels based on the current positions of the droplets.
     */
    void updateDropletResistances();

    /**
     * @brief Moves all droplets according to the given time step.
     * @param[in] timeStep to which the droplets should be moved to.
     */
    void moveDroplets(T timeStep);

    /**
     * @brief Compute all possible next events.
     * @return Vector of pointers containing unique pointers to the Events.
     */
    std::vector<std::unique_ptr<Event<T>>> computeEvents();

public:

    /**
     * @brief Constructor of the abstract droplet simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractDroplet(std::shared_ptr<arch::Network<T>> network);

    /**
     * TODO:
     * Overload the deleteFluid function, to account for droplets that contain that fluid
     * -> To be deleted? 
     * -> Throw logic::error?
     * -> Default to continuous phase?
    */
    void removeFluid(const std::shared_ptr<Fluid<T>>& fluid) override { };

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
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setDroplets(std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets);

    /**
     * @brief Get droplet
     * @param dropletId Id of the droplet
     * @return Pointer to droplet with the corresponding id
     */
    Droplet<T>* getDroplet(int dropletId) const;

    /**
     * @brief Gets droplet that is present at the corresponding node (i.e., the droplet spans over this node).
     * @param nodeId The id of the node
     * @return Pointer to droplet or nullptr if no droplet was found
     */
    Droplet<T>* getDropletAtNode(int nodeId) const;

    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    DropletInjection<T>* getDropletInjection(int injectionId) const;

    /**
     * @brief Creates a new droplet from two existing droplets. Please note that this only creates a new 
     * droplet inside the simulation, but the actual boundaries have to be set separately, which is usually 
     * done inside the corresponding merge events.
     * @param droplet0Id Id of the first droplet.
     * @param droplet1Id Id of the second droplet.
     * @return Pointer to new droplet.
     */
    Droplet<T>* mergeDroplets(int droplet0Id, int droplet1Id);

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
     * @brief Abstract droplet simulation for droplets that 'fill' a channel's cross-section. (i.e., no 'free, floating' droplets, but 'squeezed' droplets)
     * Simulation loop:
     * - update droplet resistances
     * - conduct nodal analysis
     * - update droplets (flow rates of boundaries)
     * - compute events
     * - search for next event (break if no event is left)
     * - move droplets
     * - perform event
     */
    void simulate() override;

};

}   // namespace sim
