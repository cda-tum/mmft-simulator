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
class RectangularChannel;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Fluid;

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

  public:
    /**
     * @brief Construct a new droplet boundary
     * @param channel Channel of the boundary.
     * @param position Position of the boundary within the channel.
     * @param volumeTowardsNodeA Direction in which the volume of the boundary is located (true if it is towards node0).
     * @param state State in which the boundary is in.
     */
    DropletBoundary(arch::RectangularChannel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state);

    /**
     * @brief Get the channel position of the boundary.
     * @return The channel position.
     */
    arch::ChannelPosition<T>& getChannelPosition();

    /**
     * @brief Get the flow rate of the boundary.
     * @return The flow rate of the boundary.
     */
    T getFlowRate() const;

    /**
     * @brief Get the direction in which the volume of the boundary is located.
     * @return true if the volume of the droplet lies between the boundary and node0 and false if it lies in the direction of node1.
     */
    bool isVolumeTowardsNodeA() const;

    /**
     * @brief Get the state in which the boundary is currently in.
     * @return Current state of the boundary.
     */
    BoundaryState getState() const;

    /**
     * @brief Set the flow rate of the boundary.
     * @param flowRate Flow rate of the boundary.
     */
    void setFlowRate(T flowRate);

    /**
     * @brief Set the direction in which the volume of the boundary is located.
     * @param volumeTowardsNodeA Set to true if the droplet volume lies between the boundary and node0, otherwise set to false.
     */
    void setVolumeTowardsNodeA(bool volumeTowardsNodeA);

    /**
     * @brief Set the state of the boundary.
     * @param state State to which the boundary should be set.
     */
    void setState(BoundaryState state);

    /**
     * @brief Get the reference node of the boundary, which is the node that "touches" the droplet volume (i.e., if volumeTowardsNode0==true, then node0, otherwise node1)
     * @return Reference node of the boundary.
     */
    arch::Node<T>* getReferenceNode(arch::Network<T>* network);

    /**
     * @brief Get the reference node of the boundary, which is the node that "touches" the droplet volume (i.e., if volumeTowardsNode0==true, then node0, otherwise node1)
     * @return Reference node of the boundary.
     */
    int getReferenceNode();

    /**
     * @brief Get the opposite reference node of the boundary, which is the node that does not "touch" the droplet volume (i.e., if volumeTowardsNode0==true, then node1, otherwise node0)
     * @return Opposite reference node of the boundary.
     */
    arch::Node<T>* getOppositeReferenceNode(arch::Network<T>* network);

    /**
     * @brief Get the opposite reference node of the boundary, which is the node that does not "touch" the droplet volume (i.e., if volumeTowardsNode0==true, then node1, otherwise node0)
     * @return Opposite reference node of the boundary.
     */
    int getOppositeReferenceNode();

    /**
     * @brief Get the remaining volume between the boundary and the destination node, towards which the boundary is currently flowing.
     Hence, inflow boundaries return the volume between the boundary and the reference node, while outflow boundaries return the volume between the boundary and the opposite reference node.
     * @return The remaining volume of the boundary.
     */
    T getRemainingVolume();

    /**
     * @brief Get the volume between the boundary and the reference node.
     * @return The volume between the boundary and the reference node.
     */
    T getVolume();

    /**
     * @brief Get the flow rate of the channel the boundary is currently in. Does not necessarily have to match with the actual boundary flow rate.
     * @return The flow rate in the channel.
     */
    T getChannelFlowRate();

    /**
     * @brief Get the time at which the boundary will reach the end of the channel.
     * @return Time in s until the boundary will reach the end of the channel.
     */
    T getTime();

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
    T const slipFactor = 1.28;                            ///< Slip factor of droplets.
    int const id;                                         ///< Unique identifier of the droplet.
    std::string name = "";                                ///< Name of the droplet.
    T volume;                                             ///< Volume of the droplet in m^3.
    Fluid<T>* fluid;                                      ///< Pointer to fluid of which the droplet consists of.
    std::vector<Droplet<T>*> mergedDroplets;              ///< List of previous droplets, if this droplet got merged.
    DropletState dropletState = DropletState::INJECTION;  ///< Current state of the droplet
    std::vector<std::unique_ptr<DropletBoundary<T>>> boundaries;
    std::vector<arch::RectangularChannel<T>*> channels;              ///< Contains the channels, that are completely occupied by the droplet (can happen in short channels or with large droplets).

  public:
    /**
     * @brief Specify a droplet.
     * @param[in] id Unique identifier of the droplet.
     * @param[in] volume Volume of the droplet in m^3.
     * @param[in] fluid Pointer to fluid the droplet consists of.
     */
    Droplet(int id, T volume, Fluid<T>* fluid);

    /**
     * @brief Change volume of droplet.
     * @param[in] volume New volume of the droplet.
     */
    void setVolume(T volume);

    /**
     * @brief Set name of droplet.
     * @param[in] name New name of droplet.
     */
    void setName(std::string name);

    /**
     * @brief Change state in which the droplet currently is in.
     * @param[in] dropletState The new state in which the droplet is in.
     */
    void setDropletState(DropletState dropletState);

    /**
     * @brief Returns unique identifier of the droplet.
     * @return Unique identifier of the droplet.
     */
    int getId() const;

    /**
     * @brief Retrieve the name of the droplet.
     * @return The name of the droplet.
     */
    std::string getName() const;

    /**
     * @brief Retrieve the volume of droplet.
     * @return The volume of the droplet in m^3.
     */
    T getVolume() const;

    /**
     * @brief Retrieve the state in which the droplet is currently in.
     * @return The droplet state the droplet has at the moment.
     */
    DropletState getDropletState() const;

    /**
     * @brief Retrieve the fluid of the droplet.
     * @return The fluid the droplet consists of in m^3.
     */
    const Fluid<T>* getFluid() const;

    /**
     * @brief Add the resistance the droplet causes to the channels it currently occupies.
     * @param[in] model The resistance model on which basis the resistance caused by the droplet is calculated.
     */
    void addDropletResistance(const ResistanceModel<T>& model);

    /**
     * @brief Get the Boundaries object
     * @return all boundaries
     */
    const std::vector<std::unique_ptr<DropletBoundary<T>>>& getBoundaries();

    /**
     * @brief Get all fully occupied channels
     * @return all fully occupied channels
     */
    std::vector<arch::RectangularChannel<T>*>& getFullyOccupiedChannels();

    /**
     * @brief If the droplet currently is at a bifurcation
     * @return true
     * @return false
     */
    bool isAtBifurcation();

    /**
     * @brief If the droplet currently is inside a single channel
     * @return true
     * @return false
     */
    bool isInsideSingleChannel();

    /**
     * @brief Add a boundary to the boundary list of the droplet
     * @param channel Channel
     * @param position Position within channel
     * @param volumeTowardsNodeA Direction in which the droplet lies within the channel (in regards to node0)
     * @param state State the boundary is in
     */
    void addBoundary(arch::RectangularChannel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state);

    /**
     * @brief Add fully occupied channel to the fully occupied channel list.
     * @param channel New fully occupied channel.
     */
    void addFullyOccupiedChannel(arch::RectangularChannel<T>* channel);

    /**
     * @brief Remove boundary from the boundary list.
     * @param boundaryReference Reference to the boundary that should be removed.
     */
    void removeBoundary(DropletBoundary<T>& boundaryReference);

    /**
     * @brief Remove fully occupied channel from the fully occupied channel list.
     * @param channelId Id of the channel that should be removed.
     */
    void removeFullyOccupiedChannel(int channelId);

    /**
     * @brief Get a list of boundaries that are "connected" to the corresponding node
     * @param nodeId Id of the node
     * @param doNotConsider boundary that should not be included in the list (helpful, during request for droplet merging)
     * @return List of connected boundaries
     */
    std::vector<DropletBoundary<T>*> getConnectedBoundaries(int nodeId, DropletBoundary<T>* doNotConsider = nullptr);

    /**
     * @brief Get a list of fully occupied channels that are "connected" to the corresponding node 
     * @param nodeId Id of the node
     * @return List of connected fully occupied channels
     */
    std::vector<arch::RectangularChannel<T>*> getConnectedFullyOccupiedChannels(int nodeId);

    /**
     * @brief Update the flow-rates of the droplet boundaries according to the flowRates inside the channels
     * @param chip Chip
     */
    void updateBoundaries(const arch::Network<T>& network);

    /**
     * @brief Adds a droplet from which the current droplet was created by merging.
     * @param droplet Pointer to droplet.
     */
    void addMergedDroplet(Droplet<T>* droplet);

    /**
     * @brief Gets the list of droplets, that created the actual droplet due to merging.
     * @return List of merged droplets.
     */
    const std::vector<Droplet<T>*>& getMergedDroplets() const;
};

}  // namespace sim
