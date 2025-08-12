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
 * @brief Class that conducts an abstract droplet simulation. On top of solving the flow rates and pressures of the continuous phase, droplets of
 * immiscible fluids can be simulated in the channel system.
 */
template<typename T>
class AbstractDroplet final : public Simulation<T> {
private:
    std::unordered_map<int, std::shared_ptr<DropletImplementation<T>>> droplets;        ///< Droplets which are simulated in droplet simulation.
    std::unordered_map<int, std::shared_ptr<DropletInjection<T>>> dropletInjections;    ///< Injections of droplets that should take place during a droplet simulation.
    std::unordered_map<int, std::set<int>> injectionMap;                                ///< Mapping of injections to droplets stored as <dropletId, <injectionId1, injectionId2, ...>>.
    bool dropletsAtBifurcation = false;                                                 ///< If one or more droplets are currently at a bifurcation. Triggers the usage of the maximal adaptive time step.

    void saveState() override;

    /**
     * @brief Update the droplet resistances of the channels based on the current positions of the droplets.
     */
    void updateDropletResistances();

    /**
     * @brief Adds a dropletId and injectionId coupling to the injectionMap. A droplet can be mapped to multiple
     * injections, but an injection is coupled to a single droplet only.
     */
    void injectionMapInsertion(int dropletId, int injectionId);

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

    /**
     * @brief Set the droplets of this simulation to a pre-defined map that is passed.
     * @param[in] droplets an unordered map with unique identifiers and shared_ptr to Droplet<T> objects.
     */
    inline void setDroplets(std::unordered_map<int, std::shared_ptr<Droplet<T>>> droplets) { this->droplets = std::move(droplets); }

    /**
     * @brief Removes the droplet from the simulator. If one or multiple dropletInjections are coupled
     * to the removed droplet, these are also removed from the simulator.
     * @param[in] dropletId the id of the droplet to be removed.
     * @throws logic_error if the dropletId is not listed in the simulator.
    */
    void removeDroplet(int dropletId);

    /**
     * @brief Removes the droplet injection from the simulation.
     * @param[in] dropletInjectionId Id of the droplet injection that is to be removed.
     * @throws logic_error if the droplet injection is not listed in the simulation.
    */
    void removeDropletInjection(int dropletInjectionId);

public:

    /**
     * @brief Constructor of the abstract droplet simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractDroplet(std::shared_ptr<arch::Network<T>> network);

    /**
     * @brief Create and add droplet to the simulator.
     * @param[in] fluidId Unique identifier of the fluid the droplet consists of.
     * @param[in] volume Volume of the fluid in m^3.
     * @return Pointer to created droplet.
     */
    [[maybe_unused]] std::shared_ptr<Droplet<T>> addDroplet(int fluidId, T volume);

    /**
     * @brief Create and add droplet to the simulator.
     * @param[in] fluid Pointer to the fluid the droplet consists of.
     * @param[in] volume Volume of the fluid in m^3.
     * @return Pointer to created droplet.
     */
    [[maybe_unused]] std::shared_ptr<Droplet<T>> addDroplet(const std::shared_ptr<Fluid<T>>& fluid, T volume);

    /**
     * @brief Creates a new droplet from two existing droplets. Please note that this only creates a new 
     * droplet inside the simulation, but the actual boundaries have to be set separately, which is usually 
     * done inside the corresponding merge events. Unlike the addMergedDroplet() function, this function returns
     * a pointer to a DropletImplementation<T> object, and should be hidden from the end-user.
     * @param droplet0 Pointer to the first droplet.
     * @param droplet1 Pointer to the second droplet.
     * @return Pointer to new droplet.
     * @note Since the arguments of this function are DropletImplementation references, which the end-user cannot create or obtain otherwise;
     * this function is de-facto hidden for the user, even though it is set as public.
     */
    std::shared_ptr<DropletImplementation<T>> addMergedDropletImplementation(const DropletImplementation<T>& droplet0, const DropletImplementation<T>& droplet1);

    /**
     * @brief Creates a new droplet from two existing droplets. Please note that this only creates a new 
     * droplet inside the simulation, but the actual boundaries have to be set separately, which is usually 
     * done inside the corresponding merge events.
     * @param droplet0Id Id of the first droplet.
     * @param droplet1Id Id of the second droplet.
     * @return Pointer to new droplet.
     */
    std::shared_ptr<Droplet<T>> addMergedDroplet(int droplet0Id, int droplet1Id);

    /**
     * @brief Creates a new droplet from two existing droplets. Please note that this only creates a new 
     * droplet inside the simulation, but the actual boundaries have to be set separately, which is usually 
     * done inside the corresponding merge events.
     * @param droplet0 Pointer to the first droplet.
     * @param droplet1 Pointer to the second droplet.
     * @return Pointer to new droplet.
     */
    std::shared_ptr<Droplet<T>> addMergedDroplet(const std::shared_ptr<Droplet<T>>& droplet0, const std::shared_ptr<Droplet<T>>& droplet1);

    /**
     * @brief Get droplet
     * @param dropletId Id of the droplet
     * @return Pointer to droplet with the corresponding id
     */
    [[nodiscard]] inline std::shared_ptr<Droplet<T>> getDroplet(int dropletId) const { return droplets.at(dropletId); }

    /**
     * @brief Checks whether a droplet is present at the corresponding node (i.e., the droplet spans over this node).
     * If a droplet is found it returns a tuple of a bool (true) and a pointer to the droplet. Otherwise <0, nullptr>
     * is returned. 
     * @param nodeId The id of the node
     * @return A tuple with a bool and a pointer to the Droplet.
     */
    [[nodiscard]] std::tuple<bool, std::shared_ptr<Droplet<T>>> getDropletAtNode(int nodeId) const;

    /**
     * @brief Checks whether a droplet is present at the corresponding node (i.e., the droplet spans over this node).
     * If a droplet is found it returns a tuple of a bool (true) and a pointer to the droplet. Otherwise <0, nullptr>
     * is returned. 
     * @param node Pointer to the node
     * @return A tuple with a bool and a pointer to the Droplet.
     */
    [[nodiscard]] std::tuple<bool, std::shared_ptr<Droplet<T>>> getDropletAtNode(const std::shared_ptr<arch::Node<T>>& node) const;

    /**
     * @brief Removes the droplet from the simulator. If one or multiple dropletInjections are coupled
     * to the removed droplet, these are also removed from the simulator.
     * @param[in] droplet pointer to the droplet that should be removed.
     * @throws logic_error if the droplet is not listed in the simulator.
    */
    void removeDroplet(const std::shared_ptr<Droplet<T>>& droplet);

    /**
     * @brief Create injection.
     * @param[in] dropletId Id of the droplet that should be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s.
     * @param[in] channelId Id of the channel, where droplet should be injected.
     * @param[in] injectionPosition Position inside the channel at which the droplet should be injected (relative to the channel length between 0.0 and 1.0).
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<DropletInjection<T>> addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition);

    /**
     * @brief Create injection.
     * @param[in] droplet Pointer to the droplet that should be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s.
     * @param[in] channel Pointer to the channel, where droplet should be injected.
     * @param[in] injectionPosition Position inside the channel at which the droplet should be injected (relative to the channel length between 0.0 and 1.0).
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<DropletInjection<T>> addDropletInjection(const std::shared_ptr<Droplet<T>>& droplet, T injectionTime, const std::shared_ptr<arch::RectangularChannel<T>>& channel, T injectionPosition); 

    /**
     * @brief Get injection
     * @param[in] injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    [[nodiscard]] inline std::shared_ptr<DropletInjection<T>> getDropletInjection(int injectionId) const { return dropletInjections.at(injectionId); }

    /**
     * @brief Removes the droplet injection from the simulation.
     * @param[in] dropletInjection Pointer to the droplet injection that is to be removed.
     * @throws logic_error if the droplet injection is not listed in the simulation.
    */
    void removeDropletInjection(const std::shared_ptr<DropletInjection<T>>& dropletInjection);

    /**
     * @brief In addition to removing the fluid, the droplets that point to the fluid that is to be
     * removed, will be reset to point to the default fluid, i.e., the continuous phase.
     * The continuous phase can, per definition, not be removed from the simulation.
     * @param[in] fluid A pointer to the fluid that is to be removed.
    */
    void removeFluid(const std::shared_ptr<Fluid<T>>& fluid) override;

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
