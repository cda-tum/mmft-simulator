/**
 * @file Network.h
 */

#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace porting { 

// Forward declared dependencies
template<typename T>
void readNodes (json jsonString, arch::Network<T>& network);
template<typename T>
void readChannels (json jsonString, arch::Network<T>& network);

}

namespace nodal {

// Forward declared dependencies
template<typename T>
class NodalAnalysis;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Simulation;

}

namespace arch {

// Forward declared dependencies
enum class ChannelType;
template<typename T>
class FlowRatePump;
template<typename T>
class Membrane;
template<typename T>
class CfdModule;
template<typename T>
class essLbmModule;
template<typename T>
class Network;
template<typename T>
class Node;
template<typename T>
class Opening;
template<typename T>
class PressurePump;
template<typename T>
class RectangularChannel;
template<typename T>
class Tank;

/**
 * @brief A struct that defines an group, which is a detached abstract network, neighbouring the ground node(s) and/or CFD domains.
*/
template<typename T>
struct Group {

    size_t groupId;                 ///< Id of the group.
    bool initialized = false;       ///< Initialization of the group.
    bool grounded = false;          ///< Is this group connected to ground node(s)?
    int groundNodeId = -1;          ///< The node with pressure = pMin at the initial timestep.
    int groundChannelId = -1;       ///< The channel that contains the ground node as node.
    std::unordered_set<size_t> nodeIds;            ///< Ids of nodes in this group.
    std::unordered_set<size_t> channelIds;         ///< Ids of channels in this group.
    std::unordered_set<size_t> flowRatePumpIds;    ///< Ids of flow rate pumps in this group.
    std::unordered_set<size_t> pressurePumpIds;    ///< Ids of pressure pumps in this group.

    // In-/Outlets nodes of the group that are not ground nodes
    std::unordered_map<size_t, std::unique_ptr<FlowRatePump<T>>> Openings; 

    // The reference pressure of the group
    T pRef = 0.;

    /**
     * @brief Constructor of a group.
     * @param[in] groupId Id of the group.
     * @param[in] nodeIds Ids of the nodes that constitute this group.
     * @param[in] channelIds Ids of the channels that constitute this group.
    */
    Group(size_t groupId_, std::unordered_set<size_t> nodeIds_, std::unordered_set<size_t> channelIds_, Network<T>* network_) :
        groupId(groupId_), nodeIds(nodeIds_), channelIds(channelIds_) {
        for (auto& nodeId : nodeIds) {
            if (network_->getNode(nodeId)->getGround()) {
                grounded = true;
            }
        }
    }

    void checkGroundSign() {
        if (groundNodeId < 0) {
            throw std::runtime_error("Group " + std::to_string(groupId) + " has no ground node defined.");
        }
    }

    void checkGroundValue() {
        if (groundNodeId > std::numeric_limits<int>::max()) {
            throw std::runtime_error("Group " + std::to_string(groupId) + " has a ground node ID that is too large to be converted to int.");
        }
    }
};

/**
 * @brief Class to specify a Network of Nodes, Channels, and Models for a Platform on a Chip.
*/
template<typename T>
class Network {
private:
    std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes;                     ///< Nodes the network consists of.
    std::set<std::shared_ptr<Node<T>>> sinks;                                       ///< Pointers to nodes that are sinks.
    std::set<std::shared_ptr<Node<T>>> groundNodes;                                 ///< Pointers to nodes that are ground nodes.
    std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels;               ///< Map of ids and channel pointers to channels in the network.
    std::unordered_map<size_t, std::shared_ptr<FlowRatePump<T>>> flowRatePumps;     ///< Map of ids and channel pointers to flow rate pumps in the network.
    std::unordered_map<size_t, std::shared_ptr<PressurePump<T>>> pressurePumps;     ///< Map of ids and channel pointers to pressure pumps in the network.
    std::unordered_map<size_t, std::shared_ptr<Membrane<T>>> membranes;             ///< Map of ids and membrane pointer of all membranes in the network.
    std::unordered_map<size_t, std::shared_ptr<Tank<T>>> tanks;                     ///< Map of ids and tank pointer of all tanks in the network.
    std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>> modules;              ///< Map of ids and module pointers to modules in the network.
    std::unordered_map<size_t, std::unique_ptr<Group<T>>> groups;                   ///< Map of ids and pointers to groups that form the (unconnected) 1D parts of the network
    std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<Channel<T>>>> reach; ///< Set of nodes and corresponding channels (reach) at these nodes in the network.
    std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>> modularReach;         ///< Set of nodes with corresponding module (or none) at these nodes in the network.

    int virtualNodes = 0;

protected:
    /**
     * @brief Constructor of the Network
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
     * @param[in] flowRatePump Flow rate pumps of the network.
     * @param[in] pressurePump Pressure pumps of the network.
     * @param[in] modules Modules of the network.
    */
    Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels,
            std::unordered_map<size_t, std::shared_ptr<FlowRatePump<T>>> flowRatePump,
            std::unordered_map<size_t, std::shared_ptr<PressurePump<T>>> pressurePump,
            std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>> modules);

    /**
     * @brief Constructor of the Network
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
    */
    Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels);

    /**
     * @brief Constructor of the Network that generates a fully connected graph between the nodes.
     * @param[in] nodes Nodes of the network.
    */
    Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes);

    /**
     * @brief Constructor of a Network object.
    */
    Network() { };

    /**
     * @brief Goes through network and sets all nodes and channels that are visited to true.
     * @param[in] id Id of the node that is visited.
     * @param[in, out] visitedNodes Reference to a map that stores which nodes have already been visited.
     * @param[in, out] visitedChannels Reference to a map that stores which channels have already been visited.
     */
    void visitNodes(size_t id, std::unordered_map<size_t, bool>& visitedNodes, std::unordered_map<size_t, bool>& visitedChannels, std::unordered_map<size_t, bool>& visitedModules);
    
    /**
     * @brief Calculate total count across all edge types (channels, flowRatePumps, pressurePumps, membranes, tanks).
     * @note Can be used to calculate the next free ID for an edge.
     */
    [[nodiscard]] size_t edgeCount() const;

    /**
     * @brief Removes all edges from the network, that are connected to a specific node, but not channels and. hence, not in the reach of the node.
     * @param[in] nodeId Id of the node for which the edges should be removed.
     */
    void removeEdgesFromNodeReach(size_t nodeId);

    /**
     * @brief Removes all flow rate pumps from the network, that are connected to a specific node.
     * @param[in] nodeId Id of the node for which the flow rate pumps should be removed.
     */ 
    void removeFlowRatePumpsFromNodeReach(size_t nodeId);

    /**
     * @brief Removes all pressure pumps from the network, that are connected to a specific node.
     * @param[in] nodeId Id of the node for which the pressure pumps should be removed.
     */
    void removePressurePumpsFromNodeReach(size_t nodeId);

    /**
     * @brief Removes all membranes from the network, that are connected to a specific node.
     * @param[in] nodeId Id of the node for which the membranes should be removed.
     */
    void removeMembranesFromNodeReach(size_t nodeId);

    /**
     * @brief Removes all tanks from the network, that are connected to a specific node.
     * @param[in] nodeId Id of the node for which the tanks should be removed.
     */
    void removeTanksFromNodeReach(size_t nodeId);

    /**
     * @brief Sorts the nodes and channels into detached abstract domain groups
    */
    void sortGroups();

    /**
     * @brief Get the groups in the network.
     * @returns Groups
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::unique_ptr<Group<T>>>& getGroups() const { return groups; }

public:

    //=====================================================================================
    //======================================  Network =====================================
    //=====================================================================================

    /**
     * @brief Factory function to create a Network object and returns a shared_ptr.
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
     * @param[in] flowRatePump Flow rate pumps of the network.
     * @param[in] pressurePump Pressure pumps of the network.
     * @param[in] modules Modules of the network.
    */
    static std::shared_ptr<Network<T>> createNetwork(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes, 
                                                    std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels,
                                                    std::unordered_map<size_t, std::shared_ptr<FlowRatePump<T>>> flowRatePumps,
                                                    std::unordered_map<size_t, std::shared_ptr<PressurePump<T>>> pressurePumps,
                                                    std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>> modules) 
    {
        return std::shared_ptr<Network<T>>(new Network<T>(nodes, channels, flowRatePumps, pressurePumps, modules));
    }

    /**
     * @brief Factory function to create a Network object and returns a shared_ptr.
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
    */
    static std::shared_ptr<Network<T>> createNetwork(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes, 
                                                    std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels)
    {
        return std::shared_ptr<Network<T>>(new Network<T>(nodes, channels));
    }

    /**
     * @brief Factory function to create a Network object that generates a fully connected graph 
     * between the nodes and returns a shared_ptr.
     * @param[in] nodes Nodes of the network.
    */
    static std::shared_ptr<Network<T>> createNetwork(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes) 
    {
        return std::shared_ptr<Network<T>>(new Network<T>(nodes));
    }

    /**
     * @brief Factory function to create a Network object and returns a shared_ptr.
     * @param[in] nodes Nodes of the network.
    */
    static std::shared_ptr<Network<T>> createNetwork() 
    {
        return std::shared_ptr<Network<T>>(new Network<T>());
    }

    /**
     * @brief Get all the nodes in the network that are dangling. I.e., that are connected to 1 ege.
     * @return A vector of all dangling nodes.
     */
    std::set<std::shared_ptr<Node<T>>> getDanglingNodes();

    /**
     * @brief Checks if chip network is valid.
     * @return If the network is valid.
     */
    bool isNetworkValid();

    /**
     * TODO: Implement function to write network to JSON
     */
    /**
     * @brief Store the network object in a JSON file.
    */
    // void toJson(std::string jsonString) const;

    /**
     * @brief Prints the contents of this network
     */
    void print();

    //=====================================================================================
    //======================================  Nodes =======================================
    //=====================================================================================

protected:
    /**
     * @brief Adds a new node to the network.
    */
    [[maybe_unused]] std::shared_ptr<Node<T>> addNode(size_t nodeId, T x, T y, bool ground=false);

public:
    /**
     * @brief Adds a new node to the network.
    */
    [[maybe_unused]] std::shared_ptr<Node<T>> addNode(T x, T y, bool ground=false);

    /**
     * @brief Adds a new node to the network.
    */
    [[maybe_unused]] std::shared_ptr<Node<T>> addNode(T x, T y, bool ground, bool sink);

    /**
     * @brief Get a pointer to the node with the specific id.
    */
    [[nodiscard]] std::shared_ptr<Node<T>> getNode(size_t nodeId) const;

    /**
     * @brief Get the nodes of the network.
     * @returns Nodes.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Node<T>>>& getNodes() const { return nodes; }

    /**
     * @brief Returns a pointer to the ground node.
     * @return Pointer to the ground node.
     */
    [[nodiscard]] inline const std::set<std::shared_ptr<Node<T>>>& getGroundNodes() const { return groundNodes; }

    /**
     * @brief Returns the id of the ground node.
     * @return Id of the ground node.
     */
    [[nodiscard]] std::set<size_t> getGroundNodeIds() const;

    /**
     * @brief Returns the amount of virtual nodes given by the GUI.
     * @return Amount of virtual nodes in the original network.
     */
    [[nodiscard]] int getVirtualNodes() const { return virtualNodes; }

    /**
     * @brief Sets the amount of virtual nodes read from the GUI.
     * @param[in] virtualNodes Amount of virtual nodes.
     */
    inline void setVirtualNodes(int virtualNodes) { this->virtualNodes = virtualNodes; }

    /**
     * @brief Checks if a node with the specified id exists in the network.
     * @param[in] nodeId Id of the node to check.
     * @returns true if such a node exists.
     */
    [[nodiscard]] inline bool hasNode(size_t nodeId) const { return nodes.find(nodeId) != nodes.end(); }

    /**
     * @brief Checks if a specific node exists in the network.
     * @param[in] node Pointer to the node to check.
     * @returns true if such a node exists.
     */
    [[nodiscard]] inline bool hasNode(std::shared_ptr<Node<T>> node) const { return hasNode(node->getId()); }

    /**
     * @brief Specifies a node as sink.
     * @param[in] nodeId Id of the node that is a sink.
     */
    void setSink(size_t nodeId);

    /**
     * @brief Specifies a node as sink.
     * @param[in] node Pointer to the node that is a sink.
     */
    void setSink(std::shared_ptr<Node<T>> node) { setSink(node->getId()); }

    /**
     * @brief Checks and returns if a node is a sink.
     * @param[in] nodeId Id of the node that should be checked.
     * @return true if the node with the specified id is a sink.
     */
    [[nodiscard]] inline bool isSink(size_t nodeId) const { return nodes.at(nodeId)->getSink(); }

    /**
     * @brief Checks and returns if a node is a sink.
     * @param[in] node Pointer to the node that should be checked.
     * @return true if the passed node is a sink.
     */
    [[nodiscard]] inline bool isSink(std::shared_ptr<Node<T>> node) const { return isSink(node->getId()); }

    /**
     * @brief Sets a node as the ground node, i.e., this node has a pressure value of 0 and acts as a reference node for all other nodes.
     * @param[in] nodeId Id of the node that should be the ground node of the network.
     */
    void setGround(size_t nodeId);   

    /**
     * @brief Sets a node as the ground node, i.e., this node has a pressure value of 0 and acts as a reference node for all other nodes.
     * @param[in] node Pointer to the node that should be the ground node of the network.
     */
    void setGround(std::shared_ptr<Node<T>> node) { setGround(node->getId()); }

    /**
     * @brief Checks and returns if a node is a ground node.
     * @param[in] nodeId Id of the node that should be checked.
     * @return true if the node with the specified id is a ground node.
     */
    [[nodiscard]] inline bool isGround(size_t nodeId) const { return nodes.at(nodeId)->getGround(); }

    /**
     * @brief Checks and returns if a node is a ground node.
     * @param[in] node Pointer to the node that should be checked.
     * @return true if the specified node is a ground node.
     */
    [[nodiscard]] inline bool isGround(std::shared_ptr<Node<T>> node) const { return isGround(node->getId()); }

    /**
     * @brief Calculate the distance between the two given nodes
     */
    [[nodiscard]] T calculateNodeDistance(size_t nodeAId, size_t nodeBId) const;

    /**
     * @brief Calculate the distance between the two given nodes
     */
    [[nodiscard]] T calculateNodeDistance(std::shared_ptr<Node<T>> nodeAId, std::shared_ptr<Node<T>> nodeBId) const { return calculateNodeDistance(nodeAId->getId(), nodeBId->getId()); }

    /**
     * @brief Removes a node from the network.
     * @param[in] node Pointer to the node that should be removed.
     * @throws logic_error if the node is not found in the network.
     */
    void removeNode(const std::shared_ptr<Node<T>>& node);

    //=====================================================================================
    //======================================  Channels ====================================
    //=====================================================================================
protected:
    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] length Length of the channel in m.
     * @param[in] type What kind of channel it is.
     * @param[in] channelId Id of the channel.
     * @return pointer to the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, T length, ChannelType type, size_t channelId);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, ChannelType type, size_t channelId);

public:
    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] length Length of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, T length, ChannelType type);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeA Pointer to of the node at one end of the channel.
     * @param[in] nodeB Pointer to the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] length Length of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(const std::shared_ptr<Node<T>>& nodeA, const std::shared_ptr<Node<T>>& nodeB, T height, T width, T length, ChannelType type) 
    { 
        return addRectangularChannel(nodeA->getId(), nodeB->getId(), height, width, length, type); 
    }

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, ChannelType type);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeA Pointer to the node at one end of the channel.
     * @param[in] nodeB Pointer to the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(const std::shared_ptr<Node<T>>& nodeA, const std::shared_ptr<Node<T>>& nodeB, T height, T width, ChannelType type) 
    { 
        return addRectangularChannel(nodeA->getId(), nodeB->getId(), height, width, type); 
    }

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] resistance Resistance of the channel in Pas/L.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(size_t nodeAId, size_t nodeBId, T resistance, ChannelType type);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeA Pointer to the node at one end of the channel.
     * @param[in] nodeB Pointer to the node at the other end of the channel.
     * @param[in] resistance Resistance of the channel in Pas/L.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    [[maybe_unused]] std::shared_ptr<RectangularChannel<T>> addRectangularChannel(const std::shared_ptr<Node<T>>& nodeA, const std::shared_ptr<Node<T>>& nodeB, T resistance, ChannelType type) 
    { 
        return addRectangularChannel(nodeA->getId(), nodeB->getId(), resistance, type); 
    }

    /**
     * @brief Get a pointer to the channel with the specific id.
     * @param[in] channelId Id of the channel.
    */
    [[nodiscard]] inline std::shared_ptr<Channel<T>> getChannel(size_t channelId) const { return channels.at(channelId); }

    /**
     * @brief Get a pointer to the channel with the specific id, if the channel is rectangular.
     * @param[in] channelId Id of the channel.
     * @throws std::bad_cast if the channel with the specified id is not rectangular.
    */
    [[nodiscard]] std::shared_ptr<RectangularChannel<T>> getRectangularChannel(size_t channelId) const;

    /**
     * @brief Get the channels of the network.
     * @returns Channels.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Channel<T>>>& getChannels() const { return channels; }

    /**
     * @brief Get a map of all channels at a specific node.
     * @param[in] nodeId Id of the node at which the adherent channels should be returned.
     * @return Vector of pointers to channels adherent to this node.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<Channel<T>>> getChannelsAtNode(size_t nodeId) const;

    /**
     * @brief Get a map of all channels at a specific node.
     * @param[in] node Pointer to the node at which the adherent channels should be returned.
     * @return Vector of pointers to channels adherent to this node.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<Channel<T>>> getChannelsAtNode(std::shared_ptr<Node<T>> node) const 
    {
        return getChannelsAtNode(node->getId());
    }

    /**
     * @brief Checks and returns if an edge is a channel
    */
    [[nodiscard]] bool isChannel(int edgeId) const { return channels.find(edgeId) != channels.end(); }

    /**
     * @brief Checks and returns if an edge is a channel
    */
    [[nodiscard]] bool isChannel(const std::shared_ptr<Edge<T>>& edge) const { return isChannel(edge->getId()); }
    
    /**
     * @brief removes a channel from the network.
     * @param[in] channel Pointer to the channel that should be removed.
     * @throws logic_error if the channel is not found in the network.
     */
    void removeChannel(const std::shared_ptr<Channel<T>>& channel);

    //=====================================================================================
    //=======================================  Pumps ======================================
    //=====================================================================================

    /**
     * @brief Adds a new flow rate pump to the network.
     * @param[in] nodeAId Id of the node at one end of the flow rate pump.
     * @param[in] nodeBId Id of the node at the other end of the flow rate pump.
     * @param[in] flowRate Volumetric flow rate of the pump in m^3/s.
     * @return Pointer to the newly created flow rate pump.
     */
    [[maybe_unused]] std::shared_ptr<FlowRatePump<T>> addFlowRatePump(size_t nodeAId, size_t nodeBId, T flowRate);

    /**
     * @brief Adds a new flow rate pump to the network.
     * @param[in] nodeA Pointer to the node at one end of the flow rate pump.
     * @param[in] nodeB Pointer to the node at the other end of the flow rate pump.
     * @param[in] flowRate Volumetric flow rate of the pump in m^3/s.
     * @return Pointer to the newly created flow rate pump.
     */
    [[maybe_unused]] inline std::shared_ptr<FlowRatePump<T>> addFlowRatePump(std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T flowRate) {
        return addFlowRatePump(nodeA->getId(), nodeB->getId(), flowRate);   
    }

    /**
     * @brief Get a pointer to the flowrate pump with the specific id.
     * @param[in] pumpId Id of the flowrate pump.
     * @return Pointer to the flowrate pump with this id.
    */
    [[nodiscard]] inline std::shared_ptr<FlowRatePump<T>> getFlowRatePump(size_t pumpId) const { return flowRatePumps.at(pumpId); }

    /**
     * @brief Turns a channel with the specific id into a flow rate pump with given flow rate.
     * @param channelID id of the channel.
     * @param flowRate flow rate value of the flow rate pump.
    */
    void setFlowRatePump(size_t channelId, T flowRate);

    /**
     * @brief Checks and returns if an edge is a flowRate pump
    */
    [[nodiscard]] inline bool isFlowRatePump(size_t edgeId) const { return flowRatePumps.find(edgeId) != flowRatePumps.end(); }

    /**
     * @brief Get the flow rate pumps of the network.
     * @returns Flow rate pumps.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<FlowRatePump<T>>>& getFlowRatePumps() const { return flowRatePumps; }

    /**
     * @brief Removes a flow rate pump from the network.
     * @param[in] pump Pointer to the flow rate pump that should be removed.
     * @throws logic_error if the flow rate pump is not found in the network.
     */
    void removeFlowRatePump(const std::shared_ptr<FlowRatePump<T>>& pump);

    /**
     * @brief Adds a new pressure pump to the chip.
     * @param[in] nodeAId Id of the node at one end of the pressure pump.
     * @param[in] nodeBId Id of the node at the other end of the pressure pump.
     * @param[in] pressure Pressure of the pump in Pas/L.
     * @return Pointer to the newly created pressure pump.
     */
    [[maybe_unused]] std::shared_ptr<PressurePump<T>> addPressurePump(size_t nodeAId, size_t nodeBId, T pressure);

    /**
     * @brief Adds a new pressure pump to the chip.
     * @param[in] nodeA Pointer to the node at one end of the pressure pump.
     * @param[in] nodeB Pointer to the node at the other end of the pressure pump.
     * @param[in] pressure Pressure of the pump in Pas/L.
     * @return Pointer to the newly created pressure pump.
     */
    [[maybe_unused]] inline std::shared_ptr<PressurePump<T>> addPressurePump(std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T pressure) {
        return addPressurePump(nodeA->getId(), nodeB->getId(), pressure);    
    }

    /**
     * @brief Get a pointer to the pressure pump with the specific id.
    */
    [[nodiscard]] inline std::shared_ptr<PressurePump<T>> getPressurePump(size_t pumpId) const {  return pressurePumps.at(pumpId); }

    /**
     * @brief Turns a channel with the specific id into a pressurepump with given pressure.
     * @param channelID id of the channel.
     * @param pressure pressure value of the pressure pump.
    */
    void setPressurePump(size_t channelId, T pressure);

    /**
     * @brief Checks and returns if an edge is a pressure pump
    */
    [[nodiscard]] inline bool isPressurePump(size_t edgeId) const { return pressurePumps.find(edgeId) != pressurePumps.end(); }

    /**
     * @brief Get the pressure pumps of the network.
     * @returns Pressure pumps.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<PressurePump<T>>>& getPressurePumps() const { return pressurePumps; }

    /**
     * @brief Removes a pressure pump from the network.
     * @param[in] pump Pointer to the pressure pump that should be removed. 
     * @throws logic_error if the pressure pump is not found in the network.
     */
    void removePressurePump(const std::shared_ptr<PressurePump<T>>& pump);

    //=====================================================================================
    //======================================  Modules =====================================
    //=====================================================================================

    /**
     * @brief Adds a new module to the network.
     * @param[in] position Absolute position of the module in the network w.r.t. bottom left corner.
     * @param[in] size Absolute size of the module in m.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @return Pointer to the newly created module.
    */
    [[maybe_unused]] std::shared_ptr<CfdModule<T>> addCfdModule(std::vector<T> position,
                                                                std::vector<T> size,
                                                                std::string stlFile,
                                                                std::unordered_map<size_t, Opening<T>> openings);

    /**
     * @brief Get a pointer to the module with the specidic id.
    */
    [[nodiscard]] inline std::shared_ptr<CfdModule<T>> getCfdModule(size_t moduleId) const { return modules.at(moduleId); }

    /**
     * @brief Get the modules of the network.
     * @returns Modules.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>>& getCfdModules() const { return modules; }

    /**
     * @brief Removes a module from the network.
     * @param[in] module Pointer to the module that should be removed.
     * @throws logic_error if the module is not found in the network.
     */
    void removeModule(const std::shared_ptr<Module<T>>& module);

    //=====================================================================================
    //====================================== Membrane =====================================
    //=====================================================================================

    /**
     * @brief Creates and adds a membrane to a channel in the simulator.
     * @param[in] channelId Id of the channel. Channel defines nodes, length and width.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] poreSize Size of the pores in m.
     * @param[in] porosity Porosity of the membrane in % (between 0 and 1).
     * @return Id of the membrane.
     */
    [[maybe_unused]] std::shared_ptr<Membrane<T>> addMembraneToChannel(size_t channelId, T height, T width, T poreRadius, T porosity);

    /**
     * @brief Creates and adds a membrane to a channel in the simulator.
     * @param[in] channel Pointer to the channel. Channel defines nodes, length and width.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] poreSize Size of the pores in m.
     * @param[in] porosity Porosity of the membrane in % (between 0 and 1).
     * @return Id of the membrane.
     */
    [[maybe_unused]] inline std::shared_ptr<Membrane<T>> addMembraneToChannel(const std::shared_ptr<Channel<T>>& channel, T height, T width, T poreRadius, T porosity) 
    {
        return addMembraneToChannel(channel->getId(), height, width, poreRadius, porosity);
    }

    /**
     * @brief Get pointer to a membrane with the specified id.
     * @param membraneId Id of the membrane.
     * @return Pointer to the membrane with this id.
     */
    [[nodiscard]] std::shared_ptr<Membrane<T>> getMembrane(size_t membraneId) const;
    
    /**
     * @brief Get the membrane that is connected to both specified nodes.
     * @param nodeAId Id of nodeA.
     * @param nodeBId Id of nodeB.
     * @return Pointer to the membrane that lies between these nodes.
     */
    [[nodiscard]] std::shared_ptr<Membrane<T>> getMembraneBetweenNodes(size_t nodeAId, size_t nodeBId) const;

    /**
     * @brief Get the membrane that is connected to both specified nodes.
     * @param nodeA Pointer to nodeA.
     * @param nodeB Pointer to nodeB.
     * @return Pointer to the membrane that lies between these nodes.
     */
    [[nodiscard]] std::shared_ptr<Membrane<T>> getMembraneBetweenNodes(const std::shared_ptr<Node<T>>& nodeA, const std::shared_ptr<Node<T>>& nodeB) const
    {
        return getMembraneBetweenNodes(nodeA->getId(), nodeB->getId());
    }

    /**
     * @brief Get a map of all membranes of the chip.
     * @return Map that consists of the membrane ids and pointers to the corresponding membranes.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Membrane<T>>>& getMembranes() const { return membranes; }

    /**
     * @brief Get vector of all membranes that are connected to the specified node.
     * @param nodeId Id of the node.
     * @return Vector containing pointers to all membranes that are connected to this node.
     */
    [[nodiscard]] std::vector<std::shared_ptr<Membrane<T>>> getMembranesAtNode(size_t nodeId) const;

    /**
     * @brief Get vector of all membranes that are connected to the specified node.
     * @param node Pointer to the node.
     * @return Vector containing pointers to all membranes that are connected to this node.
     */
    [[nodiscard]] inline std::vector<std::shared_ptr<Membrane<T>>> getMembranesAtNode(std::shared_ptr<Node<T>> node) const
    { 
        return getMembranesAtNode(node->getId()); 
    }

    /**
     * @brief Checks and returns if an edge is a membrane
    */
    [[nodiscard]] bool isMembrane(size_t edgeId) const { return membranes.find(edgeId) != membranes.end(); }

    //=====================================================================================
    //======================================== Tank =======================================
    //=====================================================================================

    /**
     * @brief Creates and adds a tank to a membrane in the simulator.
     * @param[in] membraneId Id of the membrane. Membrane defines nodes, length and width.
     * @param[in] height Height of the tank in m.
     * @param[in] width Width of the channel in m.
     */
    [[maybe_unused]] std::shared_ptr<Tank<T>> addTankToMembrane(size_t membraneId, T height, T width);

    /**
     * @brief Creates and adds a tank to a membrane in the simulator.
     * @param[in] membrane Pointer to the membrane. Membrane defines nodes, length and width.
     * @param[in] height Height of the tank in m.
     * @param[in] width Width of the channel in m.
     */
    [[maybe_unused]] inline std::shared_ptr<Tank<T>> addTankToMembrane(const std::shared_ptr<Membrane<T>>& membrane, T height, T width) 
    {
        return addTankToMembrane(membrane->getId(), height, width);
    }

    /**
     * @brief Get pointer to a tank with the specified id.
     * @param tankId Id of the tank.
     * @return Pointer to the tank with this id.
     */
    [[nodiscard]] inline std::shared_ptr<Tank<T>> getTank(size_t tankId) { return tanks.at(tankId); }

    /**
     * @brief Get the tank that lies between two nodes.
     * @param nodeAId Id of nodeA.
     * @param nodeBId Id of nodeB.
     * @return Pointer to the tank that lies between the two nodes.
     */
    [[nodiscard]] std::shared_ptr<Tank<T>> getTankBetweenNodes(size_t nodeAId, size_t nodeBId) const;

    /**
     * @brief Get the tank that lies between two nodes.
     * @param nodeA Pointer to nodeA.
     * @param nodeB Pointer to nodeB.
     * @return Pointer to the tank that lies between the two nodes.
     */
    [[nodiscard]] inline std::shared_ptr<Tank<T>> getTankBetweenNodes(const std::shared_ptr<Node<T>>& nodeA, const std::shared_ptr<Node<T>>& nodeB) const
    {
        return getTankBetweenNodes(nodeA->getId(), nodeB->getId());
    }

    /**
     * @brief Get a map of all tanks of the chip.
     * @return Map that consists of the tank ids and pointers to the corresponding tanks.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Tank<T>>>& getTanks() const { return tanks; }

    /**
     * @brief Checks and returns if an edge is a tank
    */
    [[nodiscard]] bool isTank(size_t edgeId) const { return tanks.find(edgeId) != tanks.end(); }

    // Disable copy constructors
    Network<T>(const Network<T>& src) = delete;
    Network<T>(const Network<T>&& src) = delete;

    // Disable assignment constructors
    Network<T>& operator=(const Network<T>& rhs) = delete;
    Network<T>& operator=(const Network<T>&& rhs) = delete;

    // Default destructor
    ~Network<T>() = default;

    // friend definitions
    friend class nodal::NodalAnalysis<T>;
    friend class sim::Simulation<T>;
    friend class test::definitions::GlobalTest<T>;
    friend void porting::readNodes<T>(json, arch::Network<T>&);
    friend void porting::readChannels<T>(json, arch::Network<T>&);
};

}   // namespace arch

// /**
//  * @brief Constructor of the Network from a JSON string
//  * @param json json string
// */
// Network(std::string jsonFile);
