/**
 * @file Droplet.h
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace arch {
  
// Forward declared dependencies
template<typename T>
class ChannelPosition;

template<typename T>
class Network;

template<typename T>
class Node;

template<typename T>
class Channel;

}

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class AbstractDroplet;

template<typename T>
class Fluid;

template<typename T>
class DropletImplementation;

template<typename T>
class ResistanceModel;

/**
 * @brief Enum to specify in which state the droplet currently is in.
 */
enum class DropletState {
    INJECTION,  ///< Droplet planned to be injected but currently not yet in the network.
    NETWORK,    ///< Droplet currently flows through the network.
    TRAPPED,    ///< Droplet is trapped in the network.
    SINK        ///< Droplet has left the network (is in the sink).
};

/**
 * @brief Enum to specify in which state a droplet boundary currently is.
 */
enum class BoundaryState { NORMAL, WAIT_INFLOW, WAIT_OUTFLOW };

/**
 * @brief Class to specify a boundary of a droplet.
 */
template<typename T>
class DropletBoundary {
  private:
    arch::ChannelPosition<T> channelPosition;   ///< Channel position of the boundary.
    bool volumeTowardsNodeA;                    ///< Direction in which the volume of the boundary is located (true if it is towards node0).
    T flowRate;                                 ///< Flow rate of the boundary (if <0 the boundary moves towards the droplet center, >0 otherwise).
    BoundaryState state;                        ///< Current status of the boundary

    /**
     * @brief Construct a new droplet boundary.
     * @param channel Channel of the boundary.
     * @param position Position of the boundary within the channel.
     * @param volumeTowardsNodeA Direction in which the volume of the boundary is located (true if it is towards node0).
     * @param state State in which the boundary is in.
     * @note The constructor is private for reasons of encapsulation. DropletBoundary<T> objects can 
     * only be created by friend classes.
     */
    DropletBoundary(arch::Channel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state);

  public:
    /**
     * @brief Get the channel position of the boundary.
     * @return The channel position.
     */
    [[nodiscard]] inline arch::ChannelPosition<T>& getChannelPosition() { return channelPosition; }

    /**
     * @brief Get the channel position of the boundary.
     * @return The channel position.
     */
    [[nodiscard]] inline const arch::ChannelPosition<T>& readChannelPosition() const { return channelPosition; }

    /**
     * @brief Get the flow rate of the boundary.
     * @return The flow rate of the boundary.
     */
    [[nodiscard]] inline T getFlowRate() const { return flowRate; }

    /**
     * @brief Get the direction in which the volume of the boundary is located.
     * @return true if the volume of the droplet lies between the boundary and node0 and false if it lies in the direction of node1.
     */
    [[nodiscard]] inline bool isVolumeTowardsNodeA() const { return volumeTowardsNodeA; }

    /**
     * @brief Get the state in which the boundary is currently in.
     * @return Current state of the boundary.
     */
    [[nodiscard]] inline BoundaryState getState() const { return state; }

    /**
     * @brief Set the flow rate of the boundary.
     * @param flowRate Flow rate of the boundary.
     */
    inline void setFlowRate(T flowRate) { this->flowRate = flowRate; }

    /**
     * @brief Set the direction in which the volume of the boundary is located.
     * @param volumeTowardsNodeA Set to true if the droplet volume lies between the boundary and node0, otherwise set to false.
     */
    inline void setVolumeTowardsNodeA(bool volumeTowardsNodeA) { this->volumeTowardsNodeA = volumeTowardsNodeA; }

    /**
     * @brief Set the state of the boundary.
     * @param state State to which the boundary should be set.
     */
    inline void setState(BoundaryState state) { this->state = state; }

    /**
     * @brief Get the reference node of the boundary, which is the node that "touches" the droplet volume (i.e., if volumeTowardsNode0==true, then node0, otherwise node1)
     * @return Reference node of the boundary.
     */
    [[nodiscard]] arch::Node<T>* getReferenceNode(arch::Network<T>* network);

    /**
     * @brief Get the reference node of the boundary, which is the node that "touches" the droplet volume (i.e., if volumeTowardsNode0==true, then node0, otherwise node1)
     * @return Reference node of the boundary.
     */
    [[nodiscard]] size_t getReferenceNode();

    /**
     * @brief Get the opposite reference node of the boundary, which is the node that does not "touch" the droplet volume (i.e., if volumeTowardsNode0==true, then node1, otherwise node0)
     * @return Opposite reference node of the boundary.
     */
    [[nodiscard]] arch::Node<T>* getOppositeReferenceNode(arch::Network<T>* network);

    /**
     * @brief Get the opposite reference node of the boundary, which is the node that does not "touch" the droplet volume (i.e., if volumeTowardsNode0==true, then node1, otherwise node0)
     * @return Opposite reference node of the boundary.
     */
    [[nodiscard]] size_t getOppositeReferenceNode();

    /**
     * @brief Get the remaining volume between the boundary and the destination node, towards which the boundary is currently flowing.
     Hence, inflow boundaries return the volume between the boundary and the reference node, while outflow boundaries return the volume between the boundary and the opposite reference node.
     * @return The remaining volume of the boundary.
     */
    [[nodiscard]] T getRemainingVolume();

    /**
     * @brief Get the volume between the boundary and the reference node.
     * @return The volume between the boundary and the reference node.
     */
    [[nodiscard]] T getVolume();

    /**
     * @brief Get the flow rate of the channel the boundary is currently in. Does not necessarily have to match with the actual boundary flow rate.
     * @return The flow rate in the channel.
     */
    [[nodiscard]] T getChannelFlowRate();

    /**
     * @brief Get the time at which the boundary will reach the end of the channel.
     * @return Time in s until the boundary will reach the end of the channel.
     */
    [[nodiscard]] T getTime();

    /**
     * @brief Move boundary by the given timestep considering the given slip factor.
     * @param timeStep Timestep to which the boundary should be updated.
     */
    void moveBoundary(T timeStep);

    /**
     * @brief Update boundary wait state.
     * @param network The simulated network.
     */
    void updateWaitState(arch::Network<T> const& network);

    /**
     * @brief Get if boundary is in wait state.
     * @return true if it is in wait state, false if not.
     */
    bool isInWaitState();

    // Friend class definition
    friend class AbstractDroplet<T>;
    friend class DropletImplementation<T>;
};

/**
 * @brief Struct to contain the current position of a droplet during a particular time step.
 */
template<typename T>
struct DropletPosition {
    std::vector<DropletBoundary<T>> boundaries;  ///< Contain all boundaries which are present during a particular time step.
    std::vector<int> channelIds;              ///< Contains the ids of the channels that are fully occupied by the droplet.

    /**
     * @brief Constructs a droplet position.
     */
    DropletPosition();
};

/**
 * @brief Class to specify a droplet.
 */
template<typename T>
class Droplet {
  private:
    const size_t id;                                                ///< Unique identifier of the droplet.
    std::string name = "";                                          ///< Name of the droplet.
    T volume = 1e-11;                                               ///< Volume of the droplet in m^3.
    Fluid<T>* fluid = nullptr;                                      ///< Pointer to fluid of which the droplet consists of.

  protected:
    std::vector<std::unique_ptr<DropletBoundary<T>>> boundaries;    ///< Boundaries of the droplet.
    std::vector<arch::Channel<T>*> channels;                        ///< Contains the channels, that are completely occupied by the droplet (can happen in short channels or with large droplets).

    /**
     * @brief Specify a droplet.
     * @param[in] id Unique identifier of the droplet.
     * @param[in] volume Volume of the droplet in m^3.
     * @param[in] fluid Pointer to fluid the droplet consists of.
     */
    Droplet(size_t id, T volume, Fluid<T>* fluid);

  public:
    /**
     * @brief Returns unique identifier of the droplet.
     * @return Unique identifier of the droplet.
     */
    [[nodiscard]] inline size_t getId() const { return id; }

    /**
     * @brief Retrieve the name of the droplet.
     * @return The name of the droplet.
     */
    [[nodiscard]] inline std::string getName() const { return name; }

    /**
     * @brief Set name of droplet.
     * @param[in] name New name of droplet.
     */
    inline void setName(std::string name) { this->name = std::move(name); }

    /**
     * @brief Retrieve the volume of droplet.
     * @return The volume of the droplet in m^3.
     */
    [[nodiscard]] inline T getVolume() const { return volume; }

    /**
     * @brief Change volume of droplet.
     * @param[in] volume New volume of the droplet.
     */
    inline void setVolume(T volume) { this->volume = volume; }

    /**
     * @brief Retrieve the fluid of the droplet.
     * @return A read-only pointer to the fluid this droplet consists of.
     */
    [[nodiscard]] inline const Fluid<T>* readFluid() const { return fluid; }

    /** TODO: Miscellaneous */
    /**
     * @brief Set the fluid of this droplet.
     * TODO add SimHash to confirm this fluid belongs to same simulator
     * @param[in] fluid A pointer to the fluid that composes the droplet.
     * @throws a logic_error is the fluid pointer is a nullptr.
     */
    void setFluid(Fluid<T>* fluid);

    /**
     * @brief A vector to read-only references of the droplet boundaries.
     * @return const pointers to the boundaries
     */
    const std::vector<const DropletBoundary<T>*> readBoundaries() const;

    /**
     * @brief Get read-only references to all fully occupied channels.
     * @return const pointers to the fully occupied channels
     */
    const std::vector<const arch::Channel<T>*> readFullyOccupiedChannels() const;

    virtual ~Droplet() = default;

};

/**
 * @brief Class to specify a droplet.
 */
template<typename T>
class DropletImplementation : public Droplet<T> {
  private:
    inline static int dropletCounter = 0;                           ///< Global counter for amount of created droplet objects.
    T const slipFactor = 1.28;                                      ///< Slip factor of droplets.
    DropletState dropletState = DropletState::INJECTION;            ///< Current state of the droplet

    /**
     * @brief A static member function that resets the droplet object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetDropletCounter() noexcept { dropletCounter = 0; }

    /**
     * @brief A static member function that returns the amount of droplet objects that have been created.
     * Is used in (friend) AbstractDroplet<T>::addDroplet() to create a droplet object and add it to the simulation.
     * @returns The number of created droplet objects: dropletCounter.
     */
    static size_t getDropletCounter() noexcept { return dropletCounter; }

    /**
     * @brief Constructor of a droplet implementation objcet. This is a derived class of Droplet<T> and 
     * contains all the functionalities that are important for the implementation, yet should be hidden 
     * from the end-user. Helper definition for improved encapsulation.
     * @param[in] id Unique identifier of the droplet.
     * @param[in] volume Volume of the droplet in m^3.
     * @param[in] fluid Pointer to fluid the droplet consists of.
     */
    DropletImplementation(size_t id, T volume, Fluid<T>* fluid);

  public:

    /**
     * @brief Change state in which the droplet currently is in.
     * @param[in] dropletState The new state in which the droplet is in.
     */
    void setDropletState(DropletState dropletState);

    /**
     * @brief Retrieve the state in which the droplet is currently in.
     * @return The droplet state the droplet has at the moment.
     */
    DropletState getDropletState() const;

    /**
     * @brief Add the resistance the droplet causes to the channels it currently occupies.
     * @param[in] model The resistance model on which basis the resistance caused by the droplet is calculated.
     */
    void addDropletResistance(const ResistanceModel<T>& model);

    /**
     * @brief If the droplet currently is at a bifurcation
     * @return boolean
     */
    [[nodiscard]] bool isAtBifurcation();

    /**
     * @brief If the droplet currently is inside a single channel
     * @return boolean
     */
    [[nodiscard]] bool isInsideSingleChannel();

    /**
     * @brief Get the Boundaries object
     * @return all boundaries
     */
    [[nodiscard]] inline const std::vector<std::unique_ptr<DropletBoundary<T>>>& getBoundaries() { return this->boundaries; }

    /**
     * @brief Get all fully occupied channels
     * @return all fully occupied channels
     */
    [[nodiscard]] inline const std::vector<arch::Channel<T>*>& getFullyOccupiedChannels() { return this->channels; }

    /**
     * @brief Add a boundary to the boundary list of the droplet
     * @param channel Channel
     * @param position Position within channel
     * @param volumeTowardsNodeA Direction in which the droplet lies within the channel (in regards to node0)
     * @param state State the boundary is in
     */
    void addBoundary(arch::Channel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state);

    /**
     * @brief Add fully occupied channel to the fully occupied channel list.
     * @param channel New fully occupied channel.
     */
    void addFullyOccupiedChannel(arch::Channel<T>* channel);

    /**
     * @brief Remove boundary from the boundary list.
     * @param boundaryReference Reference to the boundary that should be removed.
     */
    void removeBoundary(DropletBoundary<T>& boundaryReference);

    /**
     * @brief Remove fully occupied channel from the fully occupied channel list.
     * @param channelId Id of the channel that should be removed.
     */
    void removeFullyOccupiedChannel(size_t channelId);

    /**
     * @brief Get a list of boundaries that are "connected" to the corresponding node
     * @param nodeId Id of the node
     * @param doNotConsider boundary that should not be included in the list (helpful, during request for droplet merging)
     * @return List of connected boundaries
     */
    [[nodiscard]] std::vector<DropletBoundary<T>*> getConnectedBoundaries(size_t nodeId, DropletBoundary<T>* doNotConsider = nullptr);

    /**
     * @brief Get a list of fully occupied channels that are "connected" to the corresponding node 
     * @param nodeId Id of the node
     * @return List of connected fully occupied channels
     */
    [[nodiscard]] std::vector<arch::Channel<T>*> getConnectedFullyOccupiedChannels(size_t nodeId);

    /**
     * @brief Update the flow-rates of the droplet boundaries according to the flowRates inside the channels
     * @param network Network
     */
    void updateBoundaries(const arch::Network<T>& network);

    // Friend class definition
    friend class AbstractDroplet<T>;
    friend class test::definitions::GlobalTest<T>;
};

}  // namespace sim
