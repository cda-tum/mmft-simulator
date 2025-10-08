#include "Network.h"

namespace arch {

template<typename T>
Network<T>::Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes_,
                    std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels_,
                    std::unordered_map<size_t, std::shared_ptr<FlowRatePump<T>>> flowRatePumps_,
                    std::unordered_map<size_t, std::shared_ptr<PressurePump<T>>> pressurePumps_,
                    std::unordered_map<size_t, std::shared_ptr<CfdModule<T>>> modules_) :
                    nodes(std::move(nodes_)), channels(std::move(channels_)), flowRatePumps(std::move(flowRatePumps_)),
                    pressurePumps(std::move(pressurePumps_)), modules(std::move(modules_)) { }

template<typename T>
Network<T>::Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes_,
                    std::unordered_map<size_t, std::shared_ptr<Channel<T>>> channels_) :
                    nodes(std::move(nodes_)), channels(std::move(channels_)) { }

template<typename T>
Network<T>::Network(std::unordered_map<size_t, std::shared_ptr<Node<T>>> nodes_) :
                    nodes(std::move(nodes_)) {
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
            auto addRectangularChannel = std::shared_ptr<RectangularChannel<T>>(new RectangularChannel<T>(channel_counter, nA, nB, (T) 1e-4, (T) 1e-4));
            auto [it, is_inserted] = channels.try_emplace(channel_counter, std::move(addRectangularChannel));
            assert(is_inserted);
            ++channel_counter;
        }
    }
}

template<typename T>
void Network<T>::visitNodes(size_t id, std::unordered_map<size_t, bool>& visitedNodes, std::unordered_map<size_t, bool>& visitedChannels, std::unordered_map<size_t, bool>& visitedModules) {
    const auto net = reach.at(id);
    visitedNodes.at(id) = true;
    for (auto [key, channel] : net) {
        if (!(channel->getChannelType() == ChannelType::CLOGGABLE)) {
            if (visitedChannels.at(channel->getId()) == false) {
                visitedChannels.at(channel->getId()) = true;
                if (channel->getNodeAId() != id) {
                    visitNodes(channel->getNodeAId(), visitedNodes, visitedChannels, visitedModules);
                } else {
                    visitNodes(channel->getNodeBId(), visitedNodes, visitedChannels, visitedModules);
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
size_t Network<T>::edgeCount() const {
    return channels.size() + flowRatePumps.size() + pressurePumps.size() + membranes.size() + tanks.size();
}

template<typename T>
std::shared_ptr<Node<T>> Network<T>::addNode(T x_, T y_, bool ground_) {
    int nodeId = nodes.size();

    return addNode(nodeId, x_, y_, ground_);
}

template<typename T>
std::shared_ptr<Node<T>> Network<T>::addNode(size_t nodeId, T x_, T y_, bool ground_) {
    auto nodePtr = std::shared_ptr<Node<T>>(new Node<T>(nodeId, x_, y_, ground_));
    auto result = nodes.insert({nodeId, nodePtr});

    if (result.second) {
        // insertion happened and we have to add an additional entry into the reach
        reach.insert_or_assign(nodeId, std::unordered_map<size_t, std::shared_ptr<Channel<T>>>{});
    } else {
        std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                            ", " + std::to_string(y_) + "). Nodes out of bounds.");
    }

    if (ground_) {
        groundNodes.emplace(nodePtr);
    }

    // return pointer to the node
    return nodePtr;
}

template<typename T>
std::shared_ptr<Node<T>> Network<T>::getNode(size_t nodeId) const {
    if (nodes.count(nodeId)) {
        return nodes.at(nodeId);
    } else {
        throw std::invalid_argument("Network does not contain node " + std::to_string(nodeId) + ".");
    }
};

template<typename T>
std::set<size_t> Network<T>::getGroundNodeIds() const {
    if (groundNodes.empty()) {
        throw std::invalid_argument("Ground node not defined.");
    }

    std::set<size_t> groundIds;
    for (auto groundNode : groundNodes) {
        groundIds.insert(groundNode->getId());
    }

    return groundIds;
}

template<typename T>
void Network<T>::setSink(size_t nodeId_) {
    nodes.at(nodeId_)->setSink(true);
    sinks.emplace(nodes.at(nodeId_));
}

template<typename T>
void Network<T>::setGround(size_t nodeId_) {
    nodes.at(nodeId_)->setGround(true);
    groundNodes.emplace(nodes.at(nodeId_));
}

template<typename T>
T Network<T>::calculateNodeDistance(size_t nodeAId, size_t nodeBId) {
    auto& nodeA = this->getNodes().at(nodeAId);
    auto& nodeB = this->getNodes().at(nodeBId);
    T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
    T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
    return sqrt(dx*dx + dy*dy);
}

template<typename T>
void Network<T>::removeNode(const std::shared_ptr<Node<T>>& node) {
    int nodeId = node->getId();
    if (nodes.find(nodeId) != nodes.end()) {
        // remove all edges connected to this node
        removeEdgesFromNodeReach(nodeId);   // Remove all edges that aren't channels
        for (auto& channel : reach.at(nodeId)) {
            removeChannel(channel.second);
        }

        // remove node from connected module
        for (auto& module : modularReach.at(nodeId)) {
            module.second->removeNode(nodeId);
        }

        // remove the node from the reach map
        reach.erase(nodeId);
        modularReach.erase(nodeId);

        // remove the node from the nodes map
        sinks.erase(node);
        groundNodes.erase(node);
        nodes.erase(nodeId);
    } else {
        throw std::logic_error("Network does not contain node " + std::to_string(nodeId) + ".");
    }

}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, T length, ChannelType type, size_t channelId) {
    // create channel
    auto nodeA = nodes.at(nodeAId);
    auto nodeB = nodes.at(nodeBId);
    auto addRectangularChannel = std::shared_ptr<RectangularChannel<T>>(new RectangularChannel<T>(channelId, nodeA, nodeB, width, height));

    addRectangularChannel->setLength(length);
    addRectangularChannel->setChannelType(type);

    // add to network as long as channel is still a valid pointer
    auto [it1, is_inserted1] = reach.at(nodeAId).try_emplace(channelId, addRectangularChannel);
    auto [it2, is_inserted2] = reach.at(nodeBId).try_emplace(channelId, addRectangularChannel);
    assert(is_inserted1);
    assert(is_inserted2);

    // add channel
    auto [it, is_inserted] = channels.try_emplace(channelId, addRectangularChannel);
    assert(is_inserted);

    return addRectangularChannel;
}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, T length, ChannelType type) {
    // create channel
    size_t id = edgeCount();
    return addRectangularChannel(nodeAId, nodeBId, height, width, length, type, id);
}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, ChannelType type) {
    // create channel
    size_t id = edgeCount();
    T length = calculateNodeDistance(nodeAId, nodeBId);
    return addRectangularChannel(nodeAId, nodeBId, height, width, length, type, id);
}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::addRectangularChannel(size_t nodeAId, size_t nodeBId, T height, T width, ChannelType type, size_t channelId) {
    // create channel
    T length = calculateNodeDistance(nodeAId, nodeBId);
    return addRectangularChannel(nodeAId, nodeBId, height, width, length, type, channelId);
}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::addRectangularChannel(size_t nodeAId, size_t nodeBId, T resistance, ChannelType type) {
    // create channel
    size_t id = edgeCount();
    T length = calculateNodeDistance(nodeAId, nodeBId);
    std::shared_ptr<RectangularChannel<T>> addChannel = addRectangularChannel(nodeAId, nodeBId, 1.0, 1.0, length, type, id);

    // Set custom resistance
    addChannel->setResistance(resistance);

    return addChannel;
}

template<typename T>
std::shared_ptr<RectangularChannel<T>> Network<T>::getRectangularChannel(size_t channelId) const {
    if (channels.at(channelId)->isRectangular()) {
        return std::dynamic_pointer_cast<RectangularChannel<T>>(channels.at(channelId));
    } else {
        throw std::bad_cast();
    }
}

template<typename T>
const std::vector<std::shared_ptr<Channel<T>>> Network<T>::getChannelsAtNode(size_t nodeId_) const {
    try {
        std::vector<std::shared_ptr<Channel<T>>> tmp;
        for (auto& [key, channel] : reach.at(nodeId_)) {
            tmp.push_back(channel);
        }
        return tmp;
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Node with ID " + std::to_string(nodeId_) + " does not exist.");
    }
}

template<typename T>
void Network<T>::removeChannel(const std::shared_ptr<Channel<T>>& channel) {
    int channelId = channel->getId();
    if (channels.find(channelId) != channels.end()) {
        // remove channel from reach of both nodes
        reach.at(channel->getNodeAId()).erase(channelId);
        reach.at(channel->getNodeBId()).erase(channelId);

        // remove channel from channels map
        channels.erase(channelId);
    } else {
        throw std::logic_error("Network does not contain channel " + std::to_string(channelId) + ".");
    }
}

template<typename T>
std::shared_ptr<FlowRatePump<T>> Network<T>::addFlowRatePump(size_t nodeAId, size_t nodeBId, T flowRate) {
    // create pump
    auto id = edgeCount();
    auto addPump = std::shared_ptr<FlowRatePump<T>>(new FlowRatePump<T>(id, nodeAId, nodeBId, flowRate));

    // add pump
    auto [it, is_inserted] = flowRatePumps.try_emplace(id, addPump);
    assert(is_inserted);

    return addPump;
}

template<typename T>
void Network<T>::setFlowRatePump(size_t channelId_, T flowRate_) {
    int nodeAId = channels.at(channelId_).get()->getNodeAId();
    int nodeBId = channels.at(channelId_).get()->getNodeBId();
    auto newPump = std::shared_ptr<FlowRatePump<T>>(new FlowRatePump<T>(channelId_, nodeAId, nodeBId, flowRate_));
    auto [it, is_inserted] = flowRatePumps.try_emplace(channelId_, std::move(newPump));
    assert(is_inserted);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::removeFlowRatePump(const std::shared_ptr<FlowRatePump<T>>& pump) {
    int pumpId = pump->getId();
    if (flowRatePumps.find(pumpId) != flowRatePumps.end()) {
        // remove pump from flow rate pumps map
        flowRatePumps.erase(pumpId);
    } else {
        throw std::logic_error("Network does not contain flow rate pump " + std::to_string(pumpId) + ".");
    }
}

template<typename T>
std::shared_ptr<PressurePump<T>> Network<T>::addPressurePump(size_t nodeAId, size_t nodeBId, T pressure) {
    // create pump
    auto id = edgeCount();
    auto addPump = std::shared_ptr<PressurePump<T>>(new PressurePump<T>(id, nodeAId, nodeBId, pressure));

    // add pump
    auto [it, is_inserted] = pressurePumps.try_emplace(id, addPump);
    assert(is_inserted);

    return addPump;
}

template<typename T>
void Network<T>::setPressurePump(size_t channelId_, T pressure_) {
    int nodeAId = channels.at(channelId_).get()->getNodeAId();
    int nodeBId = channels.at(channelId_).get()->getNodeBId();
    auto newPump = std::shared_ptr<PressurePump<T>>(new PressurePump<T>(channelId_, nodeAId, nodeBId, pressure_));
    auto [it, is_inserted] = pressurePumps.try_emplace(channelId_, std::move(newPump));
    assert(is_inserted);
    channels.erase(channelId_);
    reach.at(nodeAId).erase(channelId_);
    reach.at(nodeBId).erase(channelId_);
}

template<typename T>
void Network<T>::removePressurePump(const std::shared_ptr<PressurePump<T>>& pump) {
    int pumpId = pump->getId();
    if (pressurePumps.find(pumpId) != pressurePumps.end()) {
        // remove pump from pressure pumps map
        pressurePumps.erase(pumpId);
    } else {
        throw std::logic_error("Network does not contain pressure pump " + std::to_string(pumpId) + ".");
    }
}

template<typename T>
std::shared_ptr<CfdModule<T>> Network<T>::addCfdModule(std::vector<T> position,
                                                    std::vector<T> size,
                                                    std::string stlFile,
                                                    std::unordered_map<size_t, Opening<T>> openings) 
{
    // create module
    auto id = modules.size();
    auto addModule = std::shared_ptr<CfdModule<T>>(new CfdModule<T>(id, position, size, stlFile, openings));

    // add this module to the reach of each node
    for (auto& [k, opening] : openings) {
        auto [it, is_inserted] = modularReach.try_emplace(opening.node->getId(), addModule);
        assert(is_inserted);
    }

    // add module
    auto [it, is_inserted] = modules.try_emplace(id, addModule);
    assert(is_inserted);

    return addModule;
}

template<typename T>
void Network<T>::removeModule(const std::shared_ptr<CfdModule<T>>& module_) {
    int moduleId = module_->getId();
    if (modules.find(moduleId) != modules.end()) {
        // remove module from reach of all nodes
        for (auto& [k, node] : module_->getNodes()) {
            modularReach.erase(k);
        }
        // remove module from modules map
        modules.erase(moduleId);
    } else {
        throw std::logic_error("Network does not contain module " + std::to_string(moduleId) + ".");
    }
}

template<typename T>
std::shared_ptr<Membrane<T>> Network<T>::addMembraneToChannel(size_t channelId, T height, T width, T poreRadius, T porosity) {
    auto channel = getChannel(channelId);
    auto id = edgeCount();
    auto nodeA = this->getNode(channel->getNodeAId());
    auto nodeB = this->getNode(channel->getNodeBId());
    auto membrane = std::shared_ptr<Membrane<T>>(new Membrane<T>(id, nodeA, nodeB, height,
                                                                width, channel->getLength(), 
                                                                poreRadius, porosity));
    membrane->setChannel(channel);

    auto [it, is_inserted] = membranes.try_emplace(id, membrane);
    assert(is_inserted);

    return membrane;
}

template<typename T>
std::shared_ptr<Membrane<T>> Network<T>::getMembrane(size_t membraneId) {
    try {
        return membranes.at(membraneId);
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Membrane with ID " + std::to_string(membraneId) + " does not exist.");
    }
}

template<typename T>
std::shared_ptr<Membrane<T>> Network<T>::getMembraneBetweenNodes(size_t nodeAId, size_t nodeBId) {
    for (auto& [key, membrane] : membranes) {
        if (((membrane->getNodeAId() == nodeAId) && (membrane->getNodeBId() == nodeBId)) || ((membrane->getNodeAId() == nodeBId) && (membrane->getNodeBId() == nodeAId))) {
            return membrane;
        }
    }
    throw std::invalid_argument("Membrane between node " + std::to_string(nodeAId) + " and node " + std::to_string(nodeBId) + " does not exist.");
}

template<typename T>
std::vector<std::shared_ptr<Membrane<T>>> Network<T>::getMembranesAtNode(size_t nodeId) {
    std::vector<std::shared_ptr<Membrane<T>>> membrane_vector;
    for (auto& [key, membrane] : membranes) {
        if ((membrane->getNodeAId() == nodeId) || (membrane->getNodeBId() == nodeId)) {
            membrane_vector.push_back(membrane);
        }
    }
    return membrane_vector;
}

template<typename T>
std::shared_ptr<Tank<T>> Network<T>::addTankToMembrane(size_t membraneId, T height, T width) {
    auto membrane = getMembrane(membraneId);
    auto id = edgeCount();
    auto nodeA = this->getNode(membrane->getNodeAId());
    auto nodeB = this->getNode(membrane->getNodeBId());
    auto tank = std::shared_ptr<Tank<T>>(new Tank<T>(id, nodeA, nodeB, height, 
                                                    width, membrane->getLength()));
    membrane->setTank(tank);

    auto [it, is_inserted] = tanks.try_emplace(id, tank);
    assert(is_inserted);

    return tank;
}

template<typename T>
std::shared_ptr<Tank<T>> Network<T>::getTankBetweenNodes(size_t nodeAId, size_t nodeBId) {
    for (auto& [key, tank] : tanks) {
        if (((tank->getNodeAId()->getId() == nodeAId) && (tank->getNodeBId()->getId() == nodeBId)) || ((tank->getNodeAId()->getId() == nodeBId) && (tank->getNodeBId()->getId() == nodeAId))) {
            return tank;
        }
    }
    throw std::invalid_argument("Tank between node " + std::to_string(nodeAId) + " and node " + std::to_string(nodeBId) + " does not exist.");
}

template<typename T>
void Network<T>::sortGroups() {
    std::vector<size_t> nodeVector;
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
        std::queue<size_t> connectedNodes;
        std::unordered_set<size_t> nodeIds;
        std::unordered_set<size_t> edgeIds;
        auto p = nodeIds.insert(nodeVector.front());

        if (p.second) {
            for (auto& edge : edges) {
                if (edge->getNodeAId() == nodeVector.front()) {
                    auto t = edgeIds.insert(edge->getId());
                    if (t.second) {
                        connectedNodes.push(edge->getNodeBId());
                    }
                }
                if (edge->getNodeBId() == nodeVector.front()) {
                    auto t = edgeIds.insert(edge->getId());
                    if (t.second) {
                        connectedNodes.push(edge->getNodeAId());
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
                    if (edge->getNodeAId() == connectedNodes.front()) {
                        auto t = edgeIds.insert(edge->getId());
                        if (t.second) {
                            connectedNodes.push(edge->getNodeBId());
                        }
                    }
                    if (edge->getNodeBId() == connectedNodes.front()) {
                        auto t = edgeIds.insert(edge->getId());
                        if (t.second) {
                            connectedNodes.push(edge->getNodeAId());
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

template<typename>
std::set<std::shared_ptr<Node<T>>> getDanglingNodes() {
    std::set<std::shared_ptr<Node<T>>> danglingNodes;

    for (auto const& [k, v] : nodes) {
        const auto net = reach.at(k);
        int connections = net.size();
        for (auto const& [key, pump] : pressurePumps) {
            if (pump->getNodeAId() == k || pump->getNodeBId() == k) {
                connections += 1;
            }
        }
        for (auto const& [key, pump] : flowRatePumps) {
            if (pump->getNodeAId() == k || pump->getNodeBId() == k) {
                connections += 1;
            }
        }
        if (modularReach.count(k)) {
            connections += 1;
        }
        if (connections == 1) {
            danglingNodes.emplace(v);
        } else if (connections == 0) {
            throw std::invalid_argument("Provided network has one or more disconnected nodes.");
        }
    }

    return danglingNodes;
}

template<typename T>
bool Network<T>::isNetworkValid() {
    // checks if all nodes and channels are connected to ground (if channel network is one graph)
    std::unordered_map<size_t, bool> visitedNodes;
    std::unordered_map<size_t, bool> visitedEdges;
    std::unordered_map<size_t, bool> visitedModules;

    std::unordered_map<size_t, Edge<T>*> edges;

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

    // Check if channels are valid
    for (auto const& [k, v] : channels) {
        if (v->getLength() != 0 && v->getLength() < calculateNodeDistance(v->getNodeAId(), v->getNodeBId())) {
            // if length == 0 the simulation will initialize the channel length with the distance
            // between nodeA and nodeB of the channel
            throw std::invalid_argument("Channel " + std::to_string(k) + ": length is less than the node distance.");
        }
        // Custom validity checks for each channel type
        if (!v->isChannelValid()) {
            throw std::invalid_argument("Channel " + std::to_string(k) + " is not valid.");
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
            if (pump->getNodeAId() == k || pump->getNodeBId() == k) {
                connections += 1;
            }
        }
        for (auto const& [key, pump] : flowRatePumps) {
            if (pump->getNodeAId() == k || pump->getNodeBId() == k) {
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
                            + "\t nA: " + std::to_string(v->getNodeAId())
                            + "\t nB: " + std::to_string(v->getNodeBId()) + "\n");
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
//         std::unique_ptr<RectangularChannel<T>> addRectangularChannel = nullptr;
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
//             addRectangularChannel = std::make_unique<RectangularChannel<T>>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
//                 line_segments, arcs, channel["width"], channel["height"]);
//         } else {
//             addRectangularChannel = std::make_unique<RectangularChannel<T>>(channel["iD"], nodes.at(channel["nA"]), nodes.at(channel["nB"]),
//                 channel["width"], channel["height"]);
//         }
//         auto [it, is_inserted] = channels.try_emplace(channel["iD"], std::move(addRectangularChannel));
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
