#include "Network.h"

namespace arch {

template<typename T>
Network<T>::Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                    std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_,
                    std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps_,
                    std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps_,
                    std::unordered_map<int, std::unique_ptr<Module<T>>> modules_) :
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
    for (long unsigned int i = 0; i < nodeIds.size(); ++i){
        for (long unsigned int j = i+1; j < nodeIds.size(); ++j){
            std::shared_ptr<Node<T>> nA = nodes.at(nodeIds[i]);
            std::shared_ptr<Node<T>> nB = nodes.at(nodeIds[j]);
            RectangularChannel<T>* addChannel = new RectangularChannel<T>(channel_counter, nA, nB, (T) 1e-4, (T) 1e-4);
            channels.try_emplace(channel_counter, addChannel);
            ++channel_counter;
        }
    }
}

template<typename T>
Network<T>::Network(std::string jsonFile) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    #ifdef VERBOSE
        std::cout << "Loading Nodes..." << std::endl;
    #endif

    for (auto& node : jsonString["Network"]["Nodes"]) {
        Node<T>* addNode = new Node<T>(node["iD"], T(node["x"]), T(node["y"]));
        nodes.try_emplace(node["iD"], addNode);
    }

    #ifdef VERBOSE
        std::cout << "Loaded Nodes... OK" << std::endl;

        std::cout << "Loading Channels..." << std::endl;
    #endif

    for (auto& channel : jsonString["Network"]["Channels"]) {
        RectangularChannel<T>* addChannel = nullptr;
        if (channel.contains("pieces")) {
            std::vector<Line_segment<T,2>*> line_segments;
            std::vector<Arc<T,2>*> arcs;
            for (auto& piece : channel["pieces"]) {
                std::vector<T> start(2);
                std::vector<T> end(2);
                std::vector<T> center(2);
                if (piece.contains("line_segment")) {
                    if (piece["line_segment"]["start"] == 0) {
                        start[0] = nodes.at(channel["nA"])->getPosition()[0];
                        start[1] = nodes.at(channel["nA"])->getPosition()[1];
                        end[0] = piece["line_segment"]["end"][0];
                        end[1] = piece["line_segment"]["end"][1];
                    } else if (piece["line_segment"]["end"] == 1) {
                        start[0] = piece["line_segment"]["start"][0];
                        start[1] = piece["line_segment"]["start"][1];
                        end[0] = nodes.at(channel["nB"])->getPosition()[0];
                        end[1] = nodes.at(channel["nB"])->getPosition()[1];
                    } else {
                        start[0] = piece["line_segment"]["start"][0];
                        start[1] = piece["line_segment"]["start"][1];
                        end[0] = piece["line_segment"]["end"][0];
                        end[1] = piece["line_segment"]["end"][1];
                    }
                    Line_segment<T,2>* addLineSeg = new Line_segment<T,2> (start, end);
                    line_segments.emplace_back(std::move(addLineSeg));
                } else if (piece.contains("arc")) {
                    if (piece["arc"]["start"] == 0) {
                        start[0] = nodes.at(channel["nA"])->getPosition()[0];
                        start[1] = nodes.at(channel["nA"])->getPosition()[1];
                        end[0] = piece["arc"]["end"][0];
                        end[1] = piece["arc"]["end"][1];
                    } else if (piece["arc"]["end"] == 1) {
                        start[0] = piece["arc"]["start"][0];
                        start[1] = piece["arc"]["start"][1];
                        end[0] = nodes.at(channel["nB"])->getPosition()[0];
                        end[1] = nodes.at(channel["nB"])->getPosition()[1];
                    } else {
                        start[0] = piece["arc"]["start"][0];
                        start[1] = piece["arc"]["start"][1];
                        end[0] = piece["arc"]["end"][0];
                        end[1] = piece["arc"]["end"][1];
                    }
                    center[0] = piece["arc"]["center"][0];
                    center[1] = piece["arc"]["center"][1];
                    Arc<T,2>* addArc = new Arc<T,2> (piece["arc"]["right"], start, end, center);
                    arcs.emplace_back(std::move(addArc));
                }
            }
            addChannel = new RectangularChannel<T>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
                line_segments, arcs, channel["width"], channel["height"]);
        } else {
            addChannel = new RectangularChannel<T>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
                channel["width"], channel["height"]);
        }
        channels.try_emplace(channel["iD"], addChannel);
    }

    #ifdef VERBOSE
        std::cout << "Loaded Channels... OK" << std::endl;

        std::cout << "Loading Modules..." << std::endl;
    #endif
    for (auto& module : jsonString["Network"]["Modules"]) {
        std::unordered_map<int, std::shared_ptr<Node<T>>> Nodes;
        std::unordered_map<int, Opening<T>> Openings;
        for (auto& opening : module["Openings"]){
            T nodeId = opening["nodeId"];
            Nodes.try_emplace(nodeId, nodes.at(nodeId));
            std::vector<T> normal = { opening["normalX"], opening["normalY"] };
            Opening<T> opening_(nodes.at(nodeId), normal, opening["width"]);
            Openings.try_emplace(nodeId, opening_);
        }
        std::vector<T> position = { module["posX"], module["posY"] };
        std::vector<T> size = { module["sizeX"], module["sizeY"] };
        lbmModule<T>* addModule = new lbmModule<T>( module["iD"], module["name"], module["stlFile"],
                                                    position, size, Nodes, Openings,
                                                    module["charPhysLength"], module["charPhysVelocity"],
                                                    module["alpha"], module["resolution"], module["epsilon"], module["tau"]);
        modules.try_emplace(module["iD"], addModule);
    }
    this->sortGroups();
    #ifdef VERBOSE
        std::cout << "Loaded Modules... OK" << std::endl;
    #endif
}

template<typename T>
Network<T>::Network() { }

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
Node<T>* Network<T>::addNode(T x_, T y_, bool ground_) {
    int nodeId = nodes.size();
    auto result = nodes.insert({nodeId, std::make_unique<Node<T>>(nodeId, x_, y_, ground_)});

    if (result.second) {
        // insertion happened and we have to add an additional entry into the reach
        reach.insert_or_assign(nodeId, std::unordered_map<int, RectangularChannel<T>*>{});
    } else {
        std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                            ", " + std::to_string(y_) + "). Nodes out of bounds.");
    }

    if (ground_) {
        groundNodes.emplace(result.first->second.get());
    }

    // return raw pointer to the node
    return result.first->second.get();
}

template<typename T>
Node<T>* Network<T>::addNode(int nodeId, T x_, T y_, bool ground_) {
    auto result = nodes.insert({nodeId, std::make_unique<Node<T>>(nodeId, x_, y_, ground_)});

    if (result.second) {
        // insertion happened and we have to add an additional entry into the reach
        reach.insert_or_assign(nodeId, std::unordered_map<int, RectangularChannel<T>*>{});
    } else {
        std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                            ", " + std::to_string(y_) + "). Nodes out of bounds.");
    }

    if (ground_) {
        groundNodes.emplace(result.first->second.get());
    }

    // return raw pointer to the node
    return result.first->second.get();
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
    auto addChannel = new RectangularChannel<T>(id, nodeA, nodeB, width, height);

    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    reach.at(nodeAId).try_emplace(id, addChannel);
    reach.at(nodeBId).try_emplace(id, addChannel);

    // add channel
    channels.try_emplace(id, addChannel);

    return addChannel;
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, ChannelType type, int channelId) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto addChannel = new RectangularChannel<T>(channelId, nodeA, nodeB, width, height);

    // add to network as long as channel is still a valid pointer
    reach.at(nodeAId).try_emplace(channelId, addChannel);
    reach.at(nodeBId).try_emplace(channelId, addChannel);

    addChannel->setChannelType(type);

    // add channel
    channels.try_emplace(channelId, addChannel);

    return addChannel;
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, T length, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
    auto addChannel = new RectangularChannel<T>(id, nodeA, nodeB, width, height);

    addChannel->setLength(length);
    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    reach.at(nodeAId).try_emplace(id, addChannel);
    reach.at(nodeBId).try_emplace(id, addChannel);

    // add channel
    channels.try_emplace(id, addChannel);

    return addChannel;
}

template<typename T>
RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T resistance, ChannelType type) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
    auto addChannel = new RectangularChannel<T>(id, nodeA, nodeB, 1.0, 1.0);

    addChannel->setResistance(resistance);
    addChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    reach.at(nodeAId).try_emplace(id, addChannel);
    reach.at(nodeBId).try_emplace(id, addChannel);

    // add channel
    channels.try_emplace(id, addChannel);

    return addChannel;
}

template<typename T>
FlowRatePump<T>* Network<T>::addFlowRatePump(int nodeAId, int nodeBId, T flowRate) {
    // create pump
    auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
    auto addPump = new FlowRatePump<T>(id, nodeAId, nodeBId, flowRate);

    // add pump
    flowRatePumps.try_emplace(id, addPump);

    return addPump;
}

template<typename T>
PressurePump<T>* Network<T>::addPressurePump(int nodeAId, int nodeBId, T pressure) {
    // create pump
    auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
    auto addPump = new PressurePump<T>(id, nodeAId, nodeBId, pressure);

    // add pump
    pressurePumps.try_emplace(id, addPump);

    return addPump;
}

template<typename T>
Module<T>* Network<T>::addModule(std::vector<T> position,
                                 std::vector<T> size,
                                 std::unordered_map<int, std::shared_ptr<Node<T>>> nodes) 
{
    // create module
    auto id = modules.size();
    auto addModule = new Module<T>(id, position, size, nodes);

    // add this module to the reach of each node
    for (auto& [k, node] : nodes) {
        modularReach.try_emplace(node->getId(), addModule);
    }

    // add module
    modules.try_emplace(id, addModule);

    return addModule;
}

template<typename T>
Module<T>* Network<T>::addModule(std::vector<T> position,
                                 std::vector<T> size,
                                 std::vector<int> nodeIds) 
{
    // create module
    auto id = modules.size();
    std::unordered_map<int, std::shared_ptr<Node<T>>> localNodes;
    for (int nodeId : nodeIds) {
        localNodes.try_emplace(nodeId, nodes.at(nodeId));
    }
    auto addModule = new Module<T>(id, position, size, localNodes);

    // add this module to the reach of each node
    for (auto& [k, node] : localNodes) {
        modularReach.try_emplace(node->getId(), addModule);
    }

    // add module
    modules.try_emplace(id, addModule);

    return addModule;
}


template<typename T>
bool Network<T>::hasNode(int nodeId_) const {
    return nodes.count(nodeId_);
}

template<typename T>
void Network<T>::setSink(int nodeId_) {
    nodes.at(nodeId_)->setSink(true);
    sinks.emplace(nodes.at(nodeId_).get());
}

template<typename T>
void Network<T>::setGround(int nodeId_) {
    nodes.at(nodeId_)->setGround(true);
    groundNodes.emplace(nodes.at(nodeId_).get());
}

template<typename T>
void Network<T>::setVirtualNodes(int virtualNodes_) {
    this->virtualNodes = virtualNodes_;
}

template<typename T>
void Network<T>::setPressurePump(int channelId_, T pressure_) {
    int nodeAId = channels.at(channelId_).get()->getNodeA();
    int nodeBId = channels.at(channelId_).get()->getNodeB();
    PressurePump<T>* newPump = new PressurePump<T>(channelId_, nodeAId, nodeBId, pressure_);
    pressurePumps.try_emplace(channelId_, newPump);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::setFlowRatePump(int channelId_, T flowRate_) {
    int nodeAId = channels.at(channelId_).get()->getNodeA();
    int nodeBId = channels.at(channelId_).get()->getNodeB();
    FlowRatePump<T>* newPump = new FlowRatePump<T>(channelId_, nodeAId, nodeBId, flowRate_);
    flowRatePumps.try_emplace(channelId_, newPump);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::setModules(std::unordered_map<int, std::unique_ptr<Module<T>>> modules_) {
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
std::set<Node<T>*> Network<T>::getGroundNodes() const {
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
const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& Network<T>::getChannels() const {
    return channels;
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
std::shared_ptr<Module<T>> Network<T>::getModule(int moduleId) const {
    return modules.at(moduleId);
}

template<typename T>
const std::unordered_map<int, std::shared_ptr<Module<T>>>& Network<T>::getModules() const {
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
            for (long unsigned int i=0; i < nodeVector.size(); i++) {
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
                for (long unsigned int i=0; i < nodeVector.size(); i++) {
                    if (nodeVector[i] == connectedNodes.front()) {
                        nodeVector.erase(nodeVector.begin() + i);
                    }
                }
            }
            connectedNodes.pop();
        }

        Group<T>* addGroup = new Group<T>(groupId, nodeIds, edgeIds, this);
        groups.try_emplace(groupId, addGroup);
        
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
        if (v->getLength() <= 0) {
            throw std::invalid_argument("Channel " + std::to_string(k) + ": length is <= 0.");
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

}   // namespace arch
