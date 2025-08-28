#include "Network.h"

namespace arch {

template<typename T>
Network<T>::Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_,
                    std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps_,
                    std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps_,
                    std::unordered_map<int, std::unique_ptr<CfdModule<T>>> modules_) :
                    nodes(nodes_), channels(channels_), flowRatePumps(flowRatePumps_),
                    pressurePumps(pressurePumps_), modules(modules_) { }

template<typename T>
Network<T>::Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_) :
                    nodes(nodes_), channels(std::move(channels_)) { }

template<typename T>
Network<T>::Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_) :
                    nodes(nodes_) {

    // Generate all possible channels between the nodes for the fully connected graph
    std::vector<int> nodeIds;
    for (auto& [key, node] : nodes) {
        nodeIds.push_back(key);
    }

    int channel_counter = 0;
    for (size_t i = 0; i < nodeIds.size(); ++i){
        for (size_t j = i+1; j < nodeIds.size(); ++j){
            std::shared_ptr<Node<T>> nA = nodes.at(nodeIds[i]);
            std::shared_ptr<Node<T>> nB = nodes.at(nodeIds[j]);
            auto addChannel = std::make_unique<RectangularChannel<T>>(channel_counter, nA, nB, (T) 1e-4, (T) 1e-4);
            auto [it, is_inserted] = channels.try_emplace(channel_counter, std::move(addChannel));
            assert(is_inserted);
            ++channel_counter;
        }
    }
}

template<typename T>
std::shared_ptr<Network<T>> Network<T>::createNetwork(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                                                    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_,
                                                    std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps_,
                                                    std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps_,
                                                    std::unordered_map<int, std::unique_ptr<CfdModule<T>>> modules_) 
{
    return std::shared_ptr<Network<T>>(new Network<T>(nodes_, channels_, flowRatePumps_, pressurePumps_, modules_));
}

template<typename T>
std::shared_ptr<Network<T>> Network<T>::createNetwork(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                                                    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_)
{
    return std::shared_ptr<Network<T>>(new Network<T>(nodes_, channels_));
}

template<typename T>
std::shared_ptr<Network<T>> Network<T>::createNetwork(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_)
{
    return std::shared_ptr<Network<T>>(new Network<T>(nodes_));
}

template<typename T>
std::shared_ptr<Network<T>> Network<T>::createNetwork()
{
    return std::shared_ptr<Network<T>>(new Network<T>());
}

// template<typename T>
// Network<T>::Network(std::string jsonFile) {
//     std::ifstream f(jsonFile);
//     json jsonString = json::parse(f);

//     #ifdef VERBOSE
//         std::cout << "Loading Nodes..." << std::endl;
//     #endif

//     for (auto& node : jsonString["Network"]["Nodes"]) {
//         auto addNode = std::make_shared<Node<T>>(node["iD"], T(node["x"]), T(node["y"]));
//         auto [it, is_inserted] = nodes.try_emplace(node["iD"], addNode);
//         assert(is_inserted);
//     }

//     #ifdef VERBOSE
//         std::cout << "Loaded Nodes... OK" << std::endl;

//         std::cout << "Loading Channels..." << std::endl;
//     #endif

//     for (auto& channel : jsonString["Network"]["Channels"]) {
//         std::unique_ptr<RectangularChannel<T>> addChannel = nullptr;
//         if (channel.contains("pieces")) {
//             std::vector<std::shared_ptr<Line_segment<T,2>>> line_segments;
//             std::vector<std::shared_ptr<Arc<T,2>>> arcs;
//             for (auto& piece : channel["pieces"]) {
//                 std::vector<T> start(2);
//                 std::vector<T> end(2);
//                 std::vector<T> center(2);
//                 if (piece.contains("line_segment")) {
//                     if (piece["line_segment"]["start"] == 0) {
//                         start[0] = nodes.at(channel["nA"])->getPosition()[0];
//                         start[1] = nodes.at(channel["nA"])->getPosition()[1];
//                         end[0] = piece["line_segment"]["end"][0];
//                         end[1] = piece["line_segment"]["end"][1];
//                     } else if (piece["line_segment"]["end"] == 1) {
//                         start[0] = piece["line_segment"]["start"][0];
//                         start[1] = piece["line_segment"]["start"][1];
//                         end[0] = nodes.at(channel["nB"])->getPosition()[0];
//                         end[1] = nodes.at(channel["nB"])->getPosition()[1];
//                     } else {
//                         start[0] = piece["line_segment"]["start"][0];
//                         start[1] = piece["line_segment"]["start"][1];
//                         end[0] = piece["line_segment"]["end"][0];
//                         end[1] = piece["line_segment"]["end"][1];
//                     }
//                     auto addLineSeg = std::make_shared<Line_segment<T,2>>(start, end);
//                     line_segments.emplace_back(addLineSeg);
//                 } else if (piece.contains("arc")) {
//                     if (piece["arc"]["start"] == 0) {
//                         start[0] = nodes.at(channel["nA"])->getPosition()[0];
//                         start[1] = nodes.at(channel["nA"])->getPosition()[1];
//                         end[0] = piece["arc"]["end"][0];
//                         end[1] = piece["arc"]["end"][1];
//                     } else if (piece["arc"]["end"] == 1) {
//                         start[0] = piece["arc"]["start"][0];
//                         start[1] = piece["arc"]["start"][1];
//                         end[0] = nodes.at(channel["nB"])->getPosition()[0];
//                         end[1] = nodes.at(channel["nB"])->getPosition()[1];
//                     } else {
//                         start[0] = piece["arc"]["start"][0];
//                         start[1] = piece["arc"]["start"][1];
//                         end[0] = piece["arc"]["end"][0];
//                         end[1] = piece["arc"]["end"][1];
//                     }
//                     center[0] = piece["arc"]["center"][0];
//                     center[1] = piece["arc"]["center"][1];
//                     auto addArc = std::make_shared<Arc<T,2>> (piece["arc"]["right"], start, end, center);
//                     arcs.emplace_back(addArc);
//                 }
//             }
//             addChannel = std::make_unique<RectangularChannel<T>>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
//                 line_segments, arcs, channel["width"], channel["height"]);
//         } else {
//             addChannel = std::make_unique<RectangularChannel<T>>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
//                 channel["width"], channel["height"]);
//         }
//         auto [it, is_inserted] = channels.try_emplace(channel["iD"], std::move(addChannel));
//         assert(is_inserted);
//     }

//     #ifdef VERBOSE
//         std::cout << "Loaded Channels... OK" << std::endl;

//         std::cout << "Loading Modules..." << std::endl;
//     #endif
//     for (auto& module : jsonString["Network"]["Modules"]) {
//         std::unordered_map<int, std::shared_ptr<Node<T>>> Nodes;
//         std::unordered_map<int, Opening<T>> Openings;
//         for (auto& opening : module["Openings"]){
//             T nodeId = opening["nodeId"];
//             auto [nodeIt, node_is_inserted] = Nodes.try_emplace(nodeId, nodes.at(nodeId));
//             assert(node_is_inserted);
//             std::vector<T> normal = { opening["normalX"], opening["normalY"] };
//             Opening<T> opening_(nodes.at(nodeId), normal, opening["width"]);
//             auto [openingIt, opening_is_inserted] = Openings.try_emplace(nodeId, opening_);
//             assert(opening_is_inserted);
//         }
//         std::vector<T> position = { module["posX"], module["posY"] };
//         std::vector<T> size = { module["sizeX"], module["sizeY"] };
//         auto addModule = std::make_shared<CfdModule<T>>( module["iD"], module["name"], module["stlFile"],
//                                                         position, size, Nodes, Openings,
//                                                         module["charPhysLength"], module["charPhysVelocity"],
//                                                         module["alpha"], module["resolution"], module["epsilon"], module["tau"]);
//         auto [moduleIt, module_is_inserted] = modules.try_emplace(module["iD"], addModule);
//         assert(module_is_inserted);
//     }
//     this->sortGroups();
//     #ifdef VERBOSE
//         std::cout << "Loaded Modules... OK" << std::endl;
//     #endif
// }


template<typename T>
void Network<T>::visitNodes(int id, std::unordered_map<int, bool>& visitedNodes, std::unordered_map<int, bool>& visitedChannels, std::unordered_map<int, bool>& visitedModules) {
    const auto net = reach.at(id);
    visitedNodes.at(id) = true;
    for (auto [key, channel] : net) {
        if (!(channel->getChannelType() == ChannelType::CLOGGABLE)) {
            if (visitedChannels.at(channel->getId()) == false) {
                visitedChannels.at(channel->getId()) = true;
                if (channel->getNodeA() != id) {
                    visitNodes(channel->getNodeA(), visitedNodes, visitedChannels, visitedModules);
                } else {
                    visitNodes(channel->getNodeB(), visitedNodes, visitedChannels, visitedModules);
                }
            }
        }
    }
    if (modularReach.count(id)) {
        if(visitedModules.at(modularReach.at(id)->getId()) == false) {
            visitedModules.at(modularReach.at(id)->getId()) = true;
            for (auto& [k, node] : modularReach.at(id)->getNodes()) {
                visitNodes(node->getId(), visitedNodes, visitedChannels, visitedModules);
            }
        }
    }
}

template<typename T>
int Network<T>::edgeCount() const {
    return channels.size() + flowRatePumps.size() + pressurePumps.size() + membranes.size() + tanks.size();
}

template<typename T>
std::shared_ptr<Node<T>> Network<T>::addNode(T x_, T y_, bool ground_) {
    int nodeId = nodes.size();
    auto result = nodes.insert({nodeId, std::make_shared<Node<T>>(nodeId, x_, y_, ground_)});

    if (result.second) {
        // insertion happened and we have to add an additional entry into the reach
        reach.insert_or_assign(nodeId, std::unordered_map<int, RectangularChannel<T>*>{});
    } else {
        std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                            ", " + std::to_string(y_) + "). Nodes out of bounds.");
    }

    if (ground_) {
        groundNodes.emplace(result.first->second);
    }

    // return raw pointer to the node
    return result.first->second;
}

template<typename T>
std::shared_ptr<Node<T>> Network<T>::addNode(int nodeId, T x_, T y_, bool ground_) {
    auto result = nodes.insert({nodeId, std::make_shared<Node<T>>(nodeId, x_, y_, ground_)});

    if (result.second) {
        // insertion happened and we have to add an additional entry into the reach
        reach.insert_or_assign(nodeId, std::unordered_map<int, RectangularChannel<T>*>{});
    } else {
        std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                            ", " + std::to_string(y_) + "). Nodes out of bounds.");
    }

    if (ground_) {
        groundNodes.emplace(result.first->second);
    }

    // return raw pointer to the node
    return result.first->second;
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = edgeCount();
    auto addChannel = std::make_unique<RectangularChannel<T>>(id, nodeA, nodeB, width, height);

    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    auto [it1, is_inserted1] = reach.at(nodeAId).try_emplace(id, addChannel.get());
    auto [it2, is_inserted2] = reach.at(nodeBId).try_emplace(id, addChannel.get());
    assert(is_inserted1);
    assert(is_inserted2);

    // add channel
    auto [it, is_inserted] = channels.try_emplace(id, std::move(addChannel));
    assert(is_inserted);
    
    return channels.at(id).get();
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type, int channelId) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto addChannel = std::make_unique<RectangularChannel<T>>(channelId, nodeA, nodeB, width, height);

    // add to network as long as channel is still a valid pointer
    auto [it1, is_inserted1] = reach.at(nodeAId).try_emplace(channelId, addChannel.get());
    auto [it2, is_inserted2] = reach.at(nodeBId).try_emplace(channelId, addChannel.get());
    assert(is_inserted1);
    assert(is_inserted2);

    addChannel->setChannelType(type);

    // add channel
    auto [it, is_inserted] = channels.try_emplace(channelId, std::move(addChannel));
    assert(is_inserted);

    return channels.at(channelId).get();
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, T length, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = edgeCount();
    auto addChannel = std::make_unique<RectangularChannel<T>>(id, nodeA, nodeB, width, height);

    addChannel->setLength(length);
    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    auto [it1, is_inserted1] = reach.at(nodeAId).try_emplace(id, addChannel.get());
    auto [it2, is_inserted2] = reach.at(nodeBId).try_emplace(id, addChannel.get());
    assert(is_inserted1);
    assert(is_inserted2);

    // add channel
    auto [it, is_inserted] = channels.try_emplace(id, std::move(addChannel));
    assert(is_inserted);

    return channels.at(id).get();
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T resistance, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = edgeCount();
    auto addChannel = std::make_unique<RectangularChannel<T>>(id, nodeA, nodeB, 1.0, 1.0);

    addChannel->setResistance(resistance);
    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    auto [it1, is_inserted1] = reach.at(nodeAId).try_emplace(id, addChannel.get());
    auto [it2, is_inserted2] = reach.at(nodeBId).try_emplace(id, addChannel.get());
    assert(is_inserted1);
    assert(is_inserted2);

    // add channel
    auto [it, is_inserted] = channels.try_emplace(id, std::move(addChannel));
    assert(is_inserted);

    return channels.at(id).get();
}

template<typename T>
Membrane<T>* Network<T>::addMembraneToChannel(int channelId, T height, T width, T poreRadius, T porosity) {
    auto channel = getChannel(channelId);
    auto id = edgeCount();
    auto nodeA = this->getNode(channel->getNodeA());
    auto nodeB = this->getNode(channel->getNodeB());
    auto membrane = std::make_unique<Membrane<T>>(id, nodeA, nodeB, height,
                                                  width, channel->getLength(),
                                                  poreRadius, porosity);
    membrane->setChannel(channel);

    auto [it, is_inserted] = membranes.try_emplace(id, std::move(membrane));
    assert(is_inserted);

    return membranes.at(id).get();
}

template<typename T>
Tank<T>* Network<T>::addTankToMembrane(int membraneId, T height, T width) {
    auto membrane = getMembrane(membraneId);
    auto id = edgeCount();
    auto nodeA = this->getNode(membrane->getNodeA());
    auto nodeB = this->getNode(membrane->getNodeB());
    auto tank = std::make_unique<Tank<T>>(id, nodeA, nodeB, height, width,
                                            membrane->getLength());
    membrane->setTank(tank.get());

    auto [it, is_inserted] = tanks.try_emplace(id, std::move(tank));
    assert(is_inserted);

    return tanks.at(id).get();
}

template<typename T>
FlowRatePump<T>* Network<T>::addFlowRatePump(int nodeAId, int nodeBId, T flowRate) {
    // create pump
    auto id = edgeCount();
    auto addPump = std::make_unique<FlowRatePump<T>>(id, nodeAId, nodeBId, flowRate);

    // add pump
    auto [it, is_inserted] = flowRatePumps.try_emplace(id, std::move(addPump));
    assert(is_inserted);

    return flowRatePumps.at(id).get();
}

template<typename T>
PressurePump<T>* Network<T>::addPressurePump(int nodeAId, int nodeBId, T pressure) {
    // create pump
    auto id = edgeCount();
    auto addPump = std::make_unique<PressurePump<T>>(id, nodeAId, nodeBId, pressure);

    // add pump
    auto [it, is_inserted] = pressurePumps.try_emplace(id, std::move(addPump));
    assert(is_inserted);

    return pressurePumps.at(id).get();
}

template<typename T>
std::shared_ptr<CfdModule<T>> Network<T>::addCfdModule(std::vector<T> position,
                                                    std::vector<T> size,
                                                    std::unordered_map<int, std::shared_ptr<Node<T>>> nodes) 
{
    // create module
    auto id = modules.size();
    auto addModule = std::make_shared<CfdModule<T>>(id, position, size, nodes);

    // add this module to the reach of each node
    for (auto& [k, node] : nodes) {
        auto [it, is_inserted] = modularReach.try_emplace(node->getId(), addModule);
        assert(is_inserted);
    }

    // add module
    auto [it, is_inserted] = modules.try_emplace(id, addModule);
    assert(is_inserted);

    return addModule;
}

template<typename T>
std::shared_ptr<CfdModule<T>> Network<T>::addCfdModule(std::vector<T> position,
                                                    std::vector<T> size,
                                                    std::vector<int> nodeIds) 
{
    // create module
    auto id = modules.size();
    std::unordered_map<int, std::shared_ptr<Node<T>>> localNodes;
    for (int nodeId : nodeIds) {
        auto [it, is_inserted] = localNodes.try_emplace(nodeId, nodes.at(nodeId));
        assert(is_inserted);
    }
    auto addModule = std::make_shared<CfdModule<T>>(id, position, size, localNodes);

    // add this module to the reach of each node
    for (auto& [k, node] : localNodes) {
        auto [it, is_inserted] = modularReach.try_emplace(node->getId(), addModule);
        assert(is_inserted);
    }

    // add module
    auto [it, is_inserted] = modules.try_emplace(id, addModule);
    assert(is_inserted);

    return addModule;
}


template<typename T>
bool Network<T>::hasNode(int nodeId_) const {
    return nodes.count(nodeId_);
}

template<typename T>
void Network<T>::setSink(int nodeId_) {
    nodes.at(nodeId_)->setSink(true);
    sinks.emplace(nodes.at(nodeId_));
}

template<typename T>
void Network<T>::setGround(int nodeId_) {
    nodes.at(nodeId_)->setGround(true);
    groundNodes.emplace(nodes.at(nodeId_));
}

template<typename T>
void Network<T>::setVirtualNodes(int virtualNodes_) {
    this->virtualNodes = virtualNodes_;
}

template<typename T>
void Network<T>::setPressurePump(int channelId_, T pressure_) {
    int nodeAId = channels.at(channelId_).get()->getNodeA();
    int nodeBId = channels.at(channelId_).get()->getNodeB();
    auto newPump = std::make_unique<PressurePump<T>>(channelId_, nodeAId, nodeBId, pressure_);
    auto [it, is_inserted] = pressurePumps.try_emplace(channelId_, std::move(newPump));
    assert(is_inserted);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::setFlowRatePump(int channelId_, T flowRate_) {
    int nodeAId = channels.at(channelId_).get()->getNodeA();
    int nodeBId = channels.at(channelId_).get()->getNodeB();
    auto newPump = std::make_unique<FlowRatePump<T>>(channelId_, nodeAId, nodeBId, flowRate_);
    auto [it, is_inserted] = flowRatePumps.try_emplace(channelId_, std::move(newPump));
    assert(is_inserted);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::setModules(std::unordered_map<int, std::unique_ptr<CfdModule<T>>> modules_) {
    this->modules = std::move(modules_);
}

template<typename T>
bool Network<T>::isSink(int nodeId_) const {
    return nodes.at(nodeId_)->getSink();
}

template<typename T>
bool Network<T>::isGround(int nodeId_) const {
    return nodes.at(nodeId_)->getGround();
}

template<typename T>
bool Network<T>::isChannel(int edgeId_) const {
    return channels.count(edgeId_);
}

template<typename T>
bool Network<T>::isPressurePump(int edgeId_) const {
    return pressurePumps.count(edgeId_);
}

template<typename T>
bool Network<T>::isFlowRatePump(int edgeId_) const {
    return flowRatePumps.count(edgeId_);
}

template<typename T>
bool Network<T>::isTank(int edgeId_) const {
    return tanks.count(edgeId_);
}

template<typename T>
bool Network<T>::isMembrane(int edgeId_) const {
    return membranes.count(edgeId_);
}

template<typename T>
std::shared_ptr<Node<T>>& Network<T>::getNode(int nodeId) {
    if (nodes.count(nodeId)) {
        return nodes.at(nodeId);
    } else {
        throw std::invalid_argument("Network does not contain node " + std::to_string(nodeId) + ".");
    }
};

template<typename T>
const std::unordered_map<int, std::shared_ptr<Node<T>>>& Network<T>::getNodes() const {
    return nodes;
}

template<typename T>
int Network<T>::getVirtualNodes() const {
    return virtualNodes;
}

template<typename T>
std::set<int> Network<T>::getGroundIds() const {
    if (groundNodes.empty()) {
        throw std::invalid_argument("Ground node not defined.");
    }

    std::set<int> groundIds;
    for (auto groundNode : groundNodes) {
        groundIds.insert(groundNode->getId());
    }

    return groundIds;
}

template<typename T>
std::set<std::shared_ptr<Node<T>>> Network<T>::getGroundNodes() const {
    return groundNodes;
}

template<typename T>
RectangularChannel<T>* Network<T>::getChannel(int channelId_) const {
    return channels.at(channelId_).get();
}

template<typename T>
PressurePump<T>* Network<T>::getPressurePump(int pumpId_) const {
    return pressurePumps.at(pumpId_).get();
}

template<typename T>
FlowRatePump<T>* Network<T>::getFlowRatePump(int pumpId_) const {
    return flowRatePumps.at(pumpId_).get();
}

template<typename T>
Membrane<T>* Network<T>::getMembrane(int membraneId) {
    try {
        return membranes.at(membraneId).get();
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Membrane with ID " + std::to_string(membraneId) + " does not exist.");
    }
}

template<typename T>
Tank<T>* Network<T>::getTank(int tankId) {
    return tanks.at(tankId).get();
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& Network<T>::getChannels() const {
    return channels;
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<Membrane<T>>>& Network<T>::getMembranes() const {
    return membranes;
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<Tank<T>>>& Network<T>::getTanks() const {
    return tanks;
}

template<typename T>
const std::vector<RectangularChannel<T>*> Network<T>::getChannelsAtNode(int nodeId_) const {
    try {
        std::vector<RectangularChannel<T>*> tmp;
        for (auto& [key, channel] : reach.at(nodeId_)) {
            tmp.push_back(channel);
        }
        return tmp;
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Node with ID " + std::to_string(nodeId_) + " does not exist.");
    }
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>>& Network<T>::getFlowRatePumps() const {
    return flowRatePumps;
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<PressurePump<T>>>& Network<T>::getPressurePumps() const {
    return pressurePumps;
}

template<typename T>
Membrane<T>* Network<T>::getMembraneBetweenNodes(int nodeAId, int nodeBId) {
    for (auto& [key, membrane] : membranes) {
        if (((membrane->getNodeA() == nodeAId) && (membrane->getNodeB() == nodeBId)) || ((membrane->getNodeA() == nodeBId) && (membrane->getNodeB() == nodeAId))) {
            return membrane.get();
        }
    }
    throw std::invalid_argument("Membrane between node " + std::to_string(nodeAId) + " and node " + std::to_string(nodeBId) + " does not exist.");
}

template<typename T>
std::vector<Membrane<T>*> Network<T>::getMembranesAtNode(int nodeId) {
    std::vector<Membrane<T>*> membrane_vector;
    for (auto& [key, membrane] : membranes) {
        if ((membrane->getNodeA() == nodeId) || (membrane->getNodeB() == nodeId)) {
            membrane_vector.push_back(membrane.get());
        }
    }
    return membrane_vector;
}

template<typename T>
Tank<T>* Network<T>::getTankBetweenNodes(int nodeAId, int nodeBId) {
    for (auto& [key, tank] : tanks) {
        if (((tank->getNodeA()->getId() == nodeAId) && (tank->getNodeB()->getId() == nodeBId)) || ((tank->getNodeA()->getId() == nodeBId) && (tank->getNodeB()->getId() == nodeAId))) {
            return tank.get();
        }
    }
    throw std::invalid_argument("Tank between node " + std::to_string(nodeAId) + " and node " + std::to_string(nodeBId) + " does not exist.");
}


template<typename T>
std::shared_ptr<CfdModule<T>> Network<T>::getCfdModule(int moduleId) const {
    return modules.at(moduleId);
}

template<typename T>
const std::unordered_map<int, std::shared_ptr<CfdModule<T>>>& Network<T>::getCfdModules() const {
    return modules;
}

template<typename T>
const std::unordered_map<int, std::unique_ptr<Group<T>>>& Network<T>::getGroups() const {
    return groups;
}

template<typename T>
void Network<T>::sortGroups() {
    std::vector<int> nodeVector;
    std::vector<Edge<T>*> edges;
    int groupId = 0;
    for (auto& [key, node] : nodes) {
        nodeVector.emplace_back(key);
    }
    for (auto& [key, channel] : channels) {
        edges.emplace_back(channel.get());
    }
    /*
    for (auto& [key, pump] : pressurePumps) {
        edges.emplace_back(pump.get());
    }
    for (auto& [key, pump] : flowRatePumps) {
        edges.emplace_back(pump.get());
    }
    */
    while(!nodeVector.empty()){
        std::queue<int> connectedNodes;
        std::unordered_set<int> nodeIds;
        std::unordered_set<int> edgeIds;
        auto p = nodeIds.insert(nodeVector.front());

        if (p.second) {
            for (auto& edge : edges) {
                if (edge->getNodeA() == nodeVector.front()) {
                    auto t = edgeIds.insert(edge->getId());
                    if (t.second) {
                        connectedNodes.push(edge->getNodeB());
                    }
                }
                if (edge->getNodeB() == nodeVector.front()) {
                    auto t = edgeIds.insert(edge->getId());
                    if (t.second) {
                        connectedNodes.push(edge->getNodeA());
                    }
                }
            }
            for (size_t i=0; i < nodeVector.size(); i++) {
                if (nodeVector[i] == nodeVector.front()) {
                    nodeVector.erase(nodeVector.begin() + i);
                }
            }
        }
        while(!connectedNodes.empty()) {
            auto q = nodeIds.insert(connectedNodes.front());
            if (q.second) {
                for (auto& edge : edges) {
                    if (edge->getNodeA() == connectedNodes.front()) {
                        auto t = edgeIds.insert(edge->getId());
                        if (t.second) {
                            connectedNodes.push(edge->getNodeB());
                        }
                    }
                    if (edge->getNodeB() == connectedNodes.front()) {
                        auto t = edgeIds.insert(edge->getId());
                        if (t.second) {
                            connectedNodes.push(edge->getNodeA());
                        }
                    }
                }
                for (size_t i=0; i < nodeVector.size(); i++) {
                    if (nodeVector[i] == connectedNodes.front()) {
                        nodeVector.erase(nodeVector.begin() + i);
                    }
                }
            }
            connectedNodes.pop();
        }

        auto addGroup = std::make_unique<Group<T>>(groupId, nodeIds, edgeIds, this);
        groups.try_emplace(groupId, std::move(addGroup));
        
        groupId++;
    }
}

template<typename T>
bool Network<T>::isNetworkValid() {
    // checks if all nodes and channels are connected to ground (if channel network is one graph)
    std::unordered_map<int, bool> visitedNodes;
    std::unordered_map<int, bool> visitedEdges;
    std::unordered_map<int, bool> visitedModules;

    std::unordered_map<int, Edge<T>*> edges;

    for (auto& [key, channel] : channels) {
        edges.try_emplace(key, channel.get());
    }
    /*
    for (auto& [key, pump] : pressurePumps) {
        edges.try_emplace(key, pump.get());
    }
    for (auto& [key, pump] : flowRatePumps) {
        edges.try_emplace(key, pump.get());
    }
    */
    if (nodes.size() == 0) {
        throw std::invalid_argument("No nodes in network.");
    }

    for (auto const& [k, v] : channels) {
        if (v->getLength() != 0 && v->getLength() < calculateNodeDistance(v->getNodeA(), v->getNodeB())) {
            // if length == 0 the simulation will initialize the channel length with the distance
            // between nodeA and nodeB of the channel
            throw std::invalid_argument("Channel " + std::to_string(k) + ": length is less than the node distance.");
        }
        if (v->getHeight() <= 0) {
            throw std::invalid_argument("Channel " + std::to_string(k) + ": height is <= 0.");
        }
        if (v->getWidth() <= 0) {
            throw std::invalid_argument("Channel " + std::to_string(k) + ": width is <= 0.");
        }
    }

    for (auto const& [k, v] : nodes) {
        visitedNodes[k] = false;
    }
    for (auto const& [k, v] : edges) {
        visitedEdges[k] = false;
    }
    for (auto const& [k, v] : modules) {
        visitedModules[k] = false;
    }

    for (auto& node : groundNodes) {
        visitNodes(node->getId(), visitedNodes, visitedEdges, visitedModules);
    }

    std::string errorNodes = "";
    for (auto const& [k, v] : nodes) {
        const auto net = reach.at(k);
        int connections = net.size();
        for (auto const& [key, pump] : pressurePumps) {
            if (pump->getNodeA() == k || pump->getNodeB() == k) {
                connections += 1;
            }
        }
        for (auto const& [key, pump] : flowRatePumps) {
            if (pump->getNodeA() == k || pump->getNodeB() == k) {
                connections += 1;
            }
        }
        if (modularReach.count(k)) {
            connections += 1;
        }
        if (connections <= 1 && !v->getGround()) {
            errorNodes.append(" " + std::to_string(k));
        }
    }

    if (errorNodes.length() != 0) {
        throw std::invalid_argument("Network is invalid. The following nodes are dangling but not ground nodes: " + errorNodes + ". Please set these nodes to ground nodes." );
        return false;
    }

    for (auto const& [k, v] : nodes) {
        if (visitedNodes[k] == false) {
            errorNodes.append(" " + std::to_string(k));
        }
    }
    std::string errorEdges = "";
    for (auto const& [k, v] : edges) {
        if (visitedEdges[k] == false) {
            errorEdges.append(" " + std::to_string(k));
        }
    }
    std::string errorModules = "";
    for (auto const& [k, v] : modules) {
        if (visitedModules[k] == false) {
            errorModules.append(" " + std::to_string(k));
        }
    }

    if (errorNodes.length() != 0 || errorEdges.length() != 0 || errorModules.length() != 0) {
        throw std::invalid_argument("Network is invalid. The following nodes are not connected to ground: " + errorNodes + ". The following edges are not connected to ground: " 
            + errorEdges + ". The following modules are not connected to ground: " + errorModules);
        return false;
    }

    return true;
}

template<typename T>
void Network<T>::print() {
    std::string printNodes = "";
    std::string printChannels = "";
    std::string printModules = "";

    for (auto const& [k, v] : nodes) {
        printNodes.append(" " + std::to_string(v->getId()));
    }

    for (auto const& [k, v] : channels) {
        printChannels.append("\t id: " + std::to_string(v->getId()) 
                            + "\t nA: " + std::to_string(v->getNodeA())
                            + "\t nB: " + std::to_string(v->getNodeB()) + "\n");
    }

    for (auto const& [k, v] : modules) {
        printModules.append("\t id: " + std::to_string(v->getId()) + "\t nodes:");
        for (auto const& [kN, vN] : v->getNodes()) {
            printModules.append(" " + std::to_string(vN->getId()));
        }
    }

    std::cout << "The network consists of the following components:\n" << std::endl;
    std::cout << "Nodes: " << printNodes << "\n" << std::endl;
    std::cout << "Channels:\n" << printChannels << std::endl;
    std::cout << "Modules:\n" << printModules << std::endl;
}

template<typename T>
T Network<T>::calculateNodeDistance(int nodeAId, int nodeBId) {
    auto& nodeA = this->getNodes().at(nodeAId);
    auto& nodeB = this->getNodes().at(nodeBId);
    T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
    T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
    return sqrt(dx*dx + dy*dy);
}

}   // namespace arch
