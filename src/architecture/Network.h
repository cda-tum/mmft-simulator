/**
 * @file Network.h
 */

#pragma once

#include <fstream>
#include <memory>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace arch {

// Forward declared dependencies
enum class ChannelType;
template<typename T>
class FlowRatePump;
template<typename T>
class lbmModule;
template<typename T>
class essLbmModule;
template<typename T>
class Module;
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

/**
 * @brief A struct that defines an group, which is a detached abstract network, neighbouring the ground node(s) and/or CFD domains.
*/
template<typename T>
struct Group {

    int groupId;                    ///< Id of the group.
    bool initialized = false;       ///< Initialization of the group.
    bool grounded = false;          ///< Is this group connected to ground node(s)?
    int groundNodeId = -1;          ///< The node with pressure = pMin at the initial timestep.
    int groundChannelId = -1;       ///< The channel that contains the ground node as node.
    std::unordered_set<int> nodeIds;            ///< Ids of nodes in this group.
    std::unordered_set<int> channelIds;         ///< Ids of channels in this group.
    std::unordered_set<int> flowRatePumpIds;    ///< Ids of flow rate pumps in this group.
    std::unordered_set<int> pressurePumpIds;    ///< Ids of pressure pumps in this group.

    // In-/Outlets nodes of the group that are not ground nodes
    std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> Openings; 

    // The reference pressure of the group
    T pRef = 0.;

    /**
     * @brief Constructor of a group.
     * @param[in] groupId Id of the group.
     * @param[in] nodeIds Ids of the nodes that constitute this group.
     * @param[in] channelIds Ids of the channels that constitute this group.
    */
    Group(int groupId_, std::unordered_set<int> nodeIds_, std::unordered_set<int> channelIds_, Network<T>* network_) :
        groupId(groupId_), nodeIds(nodeIds_), channelIds(channelIds_) {
        for (auto& nodeId : nodeIds) {
            if (network_->getNode(nodeId)->getGround()) {
                grounded = true;
            }
        }
    }
};

/**
 * @brief Class to specify a Network of Nodes, Channels, and Models for a Platform on a Chip.
*/
template<typename T>
class Network {
private:
    std::unordered_map<int, std::shared_ptr<Node<T>>> nodes;                    ///< Nodes the network consists of.
    std::set<Node<T>*> sinks;                                                   ///< Ids of nodes that are sinks.
    std::set<Node<T>*> groundNodes;                                             ///< Ids of nodes that are ground nodes.
    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels;   ///< Map of ids and channel pointers to channels in the network.
    std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps;    ///< Map of ids and channel pointers to flow rate pumps in the network.
    std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps;    ///< Map of ids and channel pointers to pressure pumps in the network.
    std::unordered_map<int, std::shared_ptr<Module<T>>> modules;             ///< Map of ids and module pointers to modules in the network.
    std::unordered_map<int, std::unique_ptr<Group<T>>> groups;                  ///< Map of ids and pointers to groups that form the (unconnected) 1D parts of the network
    std::unordered_map<int, std::unordered_map<int, RectangularChannel<T>*>> reach; ///< Set of nodes and corresponding channels (reach) at these nodes in the network.
    std::unordered_map<int, Module<T>*> modularReach;                        ///< Set of nodes with corresponding module (or none) at these nodes in the network.

    int virtualNodes = 0;

    /**
     * @brief Goes through network and sets all nodes and channels that are visited to true.
     * @param[in] id Id of the node that is visited.
     * @param[in, out] visitedNodes Reference to a map that stores which nodes have already been visited.
     * @param[in, out] visitedChannels Reference to a map that stores which channels have already been visited.
     */
    void visitNodes(int id, std::unordered_map<int, bool>& visitedNodes, std::unordered_map<int, bool>& visitedChannels, std::unordered_map<int, bool>& visitedModules);
    
public:
    /**
     * @brief Constructor of the Network
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
     * @param[in] flowRatePump Flow rate pumps of the network.
     * @param[in] pressurePump Pressure pumps of the network.
     * @param[in] modules Modules of the network.
    */
    Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels,
            std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePump,
            std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePump,
            std::unordered_map<int, std::unique_ptr<Module<T>>> modules);

    /**
     * @brief Constructor of the Network
     * @param[in] nodes Nodes of the network.
     * @param[in] channels Channels of the network.
    */
    Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels);

    /**
     * @brief Constructor of the Network that generates a fully connected graph between the nodes.
     * @param[in] nodes Nodes of the network.
    */
    Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes);

    /**
     * @brief Constructor of the Network from a JSON string
     * @param json json string
                simulationResult->printLastState();
     * @return SimulationResult struct
    */
    Network(std::string jsonFile);

    /**
     * @brief Constructor of a Network object.
    */
    Network();

    /**
     * @brief Adds a new node to the network.
    */
    Node<T>* addNode(T x, T y, bool ground=false);

    /**
     * @brief Adds a new node to the network.
    */
    Node<T>* addNode(int nodeId, T x, T y, bool ground=false);

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
    RectangularChannel<T>* addChannel(int nodeAId, int nodeBId, T height, T width, T length, ChannelType type);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    RectangularChannel<T>* addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] height Height of the channel in m.
     * @param[in] width Width of the channel in m.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    RectangularChannel<T>* addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type, int channelId);

    /**
     * @brief Adds a new channel to the chip.
     * @param[in] nodeAId Id of the node at one end of the channel.
     * @param[in] nodeBId Id of the node at the other end of the channel.
     * @param[in] resistance Resistance of the channel in Pas/L.
     * @param[in] type What kind of channel it is.
     * @return Id of the newly created channel.
     */
    RectangularChannel<T>* addChannel(int nodeAId, int nodeBId, T resistance, ChannelType type);

    /**
     * @brief Adds a new flow rate pump to the chip.
     * @param[in] node0Id Id of the node at one end of the flow rate pump.
     * @param[in] node1Id Id of the node at the other end of the flow rate pump.
     * @param[in] flowRate Volumetric flow rate of the pump in m^3/s.
     * @return Id of the newly created flow rate pump.
     */
    FlowRatePump<T>* addFlowRatePump(int nodeAId, int nodeBId, T flowRate);

    /**
     * @brief Adds a new pressure pump to the chip.
     * @param[in] node0Id Id of the node at one end of the pressure pump.
     * @param[in] node1Id Id of the node at the other end of the pressure pump.
     * @param[in] pressure Pressure of the pump in Pas/L.
     * @return Id of the newly created pressure pump.
     */
    PressurePump<T>* addPressurePump(int nodeAId, int nodeBId, T pressure);

    /**
     * @brief Adds a new module to the network.
     * @param[in] position Absolute position of the module in the network w.r.t. bottom left corner.
     * @param[in] size Absolute size of the module in m.
     * @param[in] nodes Map of nodes that are on the module boundary.
     * @return Pointer to the newly created module.
    */
    Module<T>* addModule(std::vector<T> position,
                         std::vector<T> size,
                         std::unordered_map<int, std::shared_ptr<Node<T>>> nodes);

    /**
     * @brief Adds a new module to the network.
     * @param[in] position Absolute position of the module in the network w.r.t. bottom left corner.
     * @param[in] size Absolute size of the module in m.
     * @param[in] nodes Vector of node id's of nodes that are on the module boundary.
     * @return Pointer to the newly created module.
    */
    Module<T>* addModule(std::vector<T> position,
                         std::vector<T> size,
                         std::vector<int> nodes);

    /**
     * @brief Adds a new module to the network.
    */
    int addModule();

    /**
     * @brief Checks if a node with the specified id exists in the network.
     * @param[in] nodeId Id of the node to check.
     * @return If such a node exists.
     */
    bool hasNode(int nodeId) const;

    /**
     * @brief Specifies a node as sink.
     * @param[in] nodeId Id of the node that is a sink.
     */
    void setSink(int nodeId);

    /**
     * @brief Sets a node as the ground node, i.e., this node has a pressure value of 0 and acts as a reference node for all other nodes.
     * @param[in] nodeId Id of the node that should be the ground node of the network.
     */
    void setGround(int nodeId);   

    /**
     * @brief Sets the amount of virtual nodes read from the GUI.
     * @param[in] virtualNodes Amount of virtual nodes.
     */
    void setVirtualNodes(int virtualNodes);

    /**
     * @brief Turns a channel with the specific id into a pressurepump with given pressure.
     * @param channelID id of the channel.
     * @param pressure pressure value of the pressure pump.
    */
    void setPressurePump(int channelId, T pressure);

    /**
     * @brief Turns a channel with the specific id into a pressurepump with given pressure.
     * @param channelID id of the channel.
     * @param pressure pressure value of the pressure pump.
    */
    void setFlowRatePump(int channelId, T pressure);

    /**
     * @brief Set the modules of the network for a hybrid simulation.
     * @param[in] modules The modules that handle the CFD simulations.
    */
    void setModules(std::unordered_map<int, std::unique_ptr<Module<T>>> modules);

    /**
     * @brief Checks and returns if a node is a sink.
     * @param[in] nodeId Id of the node that should be checked.
     * @return If the node with the specified id is a sink.
     */
    bool isSink(int nodeId) const;

    /**
     * @brief Checks and returns if a node is a ground node.
     * @param[in] nodeId Id of the node that should be checked.
     * @return If the node with the specified id is a ground node.
     */
    bool isGround(int nodeId) const;

    /**
     * @brief Checks and returns if an edge is a channel
    */
    bool isChannel(int edgeId) const;

    /**
     * @brief Checks and returns if an edge is a pressure pump
    */
    bool isPressurePump(int edgeId) const;

    /**
     * @brief Checks and returns if an edge is a flowRate pump
    */
    bool isFlowRatePump(int edgeId) const;

    /**
     * @brief Get a pointer to the node with the specific id.
    */
    std::shared_ptr<Node<T>>& getNode(int nodeId);

    /**
     * @brief Get the nodes of the network.
     * @returns Nodes.
    */
    const std::unordered_map<int, std::shared_ptr<Node<T>>>& getNodes() const;

    /**
     * @brief Returns the id of the ground node.
     * @return Id of the ground node.
     */
    std::set<int> getGroundIds() const;

    /**
     * @brief Returns a pointer to the ground node.
     * @return Pointer to the ground node.
     */
    std::set<Node<T>*> getGroundNodes() const;

    /**
     * @brief Returns the amount of virtual nodes given by the GUI.
     * @return Amount of virtual nodes in the original network.
     */
    int getVirtualNodes() const;

    /**
     * @brief Get a pointer to the channel with the specific id.
    */
    RectangularChannel<T>* getChannel(int channelId) const;

    /**
     * @brief Get a pointer to the pressure pump with the specific id.
    */
    PressurePump<T>* getPressurePump(int pumpId) const;

    /**
     * @brief Get a pointer to the flowrate pump with the specific id.
    */
    FlowRatePump<T>* getFlowRatePump(int pumpId) const;

    /**
     * @brief Get the channels of the network.
     * @returns Channels.
    */
    const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& getChannels() const;

    /**
     * @brief Get a map of all channels at a specific node.
     * @param[in] nodeId Id of the node at which the adherent channels should be returned.
     * @return Vector of pointers to channels adherent to this node.
     */
    const std::vector<RectangularChannel<T>*> getChannelsAtNode(int nodeId) const;
        
    /**
     * @brief Get the flow rate pumps of the network.
     * @returns Flow rate pumps.
    */
    const std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>>& getFlowRatePumps() const;

    /**
     * @brief Get the pressure pumps of the network.
     * @returns Pressure pumps.
    */
    const std::unordered_map<int, std::unique_ptr<PressurePump<T>>>& getPressurePumps() const;

    /**
     * @brief Get a pointer to the module with the specidic id.
    */
    std::shared_ptr<Module<T>> getModule(int moduleId) const;

    /**
     * @brief Get the modules of the network.
     * @returns Modules.
    */
    const std::unordered_map<int, std::shared_ptr<Module<T>>>& getModules() const;

    /**
     * @brief Get the groups in the network.
     * @returns Groups
    */
    const std::unordered_map<int, std::unique_ptr<Group<T>>>& getGroups() const;

    /**
     * @brief Store the network object in a JSON file.
    */
    void toJson(std::string jsonString) const;

    /**
     * @brief Sorts the nodes and channels into detached abstract domain groups
    */
    void sortGroups();

    /**
     * @brief Checks if chip network is valid.
     * @return If the network is valid.
     */
    bool isNetworkValid();

    /**
     * @brief Prints the contents of this network
     */
    void print();
};

}   // namespace arch
