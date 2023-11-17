#include <iostream>

#include "Network.h"

#include "Channel.h"
#include "lbmModule.h"
#include "Module.h"
#include "Node.h"

namespace arch {

    template<typename T>
    Network<T>::Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_,
                        std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels_,
                        std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps_,
                        std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps_,
                        std::unordered_map<int, std::unique_ptr<lbmModule<T>>> modules_) :
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
                            std::cout << "Getting here...8" << std::endl;
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
    Node<T>* Network<T>::addNode(T x_, T y_, bool ground_) {
        int nodeId = nodes.size();
        auto result = nodes.insert({nodeId, std::make_unique<Node<T>>(nodeId, x_, y_, ground_)});

        if (result.second) {
            // insertion happened and we have to add an additional entry into the reach
            reach.insert_or_assign(nodeId, std::vector<RectangularChannel<T>*>{});
        } else {
            std::out_of_range(  "Could not add Node " + std::to_string(nodeId) + " at (" + std::to_string(x_) +
                                ", " + std::to_string(y_) + "). Nodes out of bounds.");
        }

        // return raw pointer to the node
        return result.first->second.get();
    }

    template<typename T>
    RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T height, T width, T length, ChannelType type) {
        // create channel
        auto nodeA = addNode(nodeAId);
        auto nodeB = addNode(nodeBId);
        auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
        auto channel = std::make_unique<Channel>(id, nodeA, nodeB, height, width, length, type);

        // add to network as long as channel is still a valid pointer
        reach.at(nodeA->getId()).push_back(channel.get());
        reach.at(nodeB->getId()).push_back(channel.get());

        // add channel
        channels.insert_or_assign(id, std::move(channel));

        return id;
    }

    template<typename T>
    RectangularChannel<T>* Network<T>::addChannel(int nodeAId, int nodeBId, T resistance, ChannelType type) {
        // create channel
        auto nodeA = addNode(nodeAId);
        auto nodeB = addNode(nodeBId);
        auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
        auto channel = std::make_unique<Channel>(id, nodeA, nodeB, resistance, type);

        // add to network as long as channel is still a valid pointer
        reach.at(nodeA->getId()).push_back(channel.get());
        reach.at(nodeB->getId()).push_back(channel.get());

        // add channel
        channels.insert_or_assign(id, std::move(channel));

        return id;
    }

    template<typename T>
    FlowRatePump<T>* Network<T>::addFlowRatePump(int nodeAId, int nodeBId, T flowRate) {
        // create pump
        auto nodeA = addNode(nodeAId);
        auto nodeB = addNode(nodeBId);
        auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
        auto pump = std::make_unique<FlowRatePump>(id, nodeA, nodeB, flowRate);

        // add pump
        flowRatePumps.insert_or_assign(id, std::move(pump));

        return id;
    }

    template<typename T>
    PressurePump<T>* Network<T>::addPressurePump(int nodeAId, int nodeBId, T pressure) {
        // create pump
        auto nodeA = addNode(nodeAId);
        auto nodeB = addNode(nodeBId);
        auto id = channels.size() + flowRatePumps.size() + pressurePumps.size();
        auto pump = std::make_unique<PressurePump>(id, nodeA, nodeB, pressure);

        // add pump
        pressurePumps.insert_or_assign(id, std::move(pump));

        return id;
    }

    template<typename T>
    int Network<T>::addModule() {
        /** TODO
         * 
        */
    }

    template<typename T>
    bool Network<T>::hasNode(int nodeId_) const {
        return nodes.count(nodeId_);
    }

    template<typename T>
    void Network<T>::setSink(int nodeId_) {
        nodes.at(nodeId_)->setSink(true);
    }

    template<typename T>
    void Network<T>::setGround(int nodeId_) {
        nodes.at(nodeId_)->setGround(true);
    }

    template<typename T>
    void Network<T>::setPressurePump(int channelId_, T pressure_) {
        int nodeAId = channels.at(channelId_).get()->getNodeA();
        int nodeBId = channels.at(channelId_).get()->getNodeB();
        PressurePump<T>* newPump = new PressurePump<T>(channelId_, nodeAId, nodeBId, pressure_);
        pressurePumps.try_emplace(channelId_, newPump);
        channels.erase(channelId_);
    }

    template<typename T>
    void Network<T>::setFlowRatePump(int channelId_, T flowRate_) {
        int nodeAId = channels.at(channelId_).get()->getNodeA();
        int nodeBId = channels.at(channelId_).get()->getNodeB();
        FlowRatePump<T>* newPump = new FlowRatePump<T>(channelId_, nodeAId, nodeBId, flowRate_);
        flowRatePumps.try_emplace(channelId_, newPump);
        channels.erase(channelId_);
    }

    template<typename T>
    void Network<T>::setModules(std::unordered_map<int, std::unique_ptr<lbmModule<T>>> modules_) {
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
    std::shared_ptr<Node<T>>& Network<T>::getNode(int nodeId) {
        return nodes.at(nodeId);
    };

    template<typename T>
    const std::unordered_map<int, std::shared_ptr<Node<T>>>& Network<T>::getNodes() const {
        return nodes;
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
    Channel<T>* Network<T>::getChannel(int channelId_) const {
        return std::get<0>(channels.at(channelId_));
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& Network<T>::getChannels() const {
        return channels;
    }

    template<typename T>
    const std::vector<RectangularChannel<T>*>& Network<T>::getChannelsAtNode(int nodeId_) const {
        try {
            return reach.at(nodeId_);
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
    Module<T>* Network<T>::getModule(int moduleId) const {
        return std::get<0>(modules.at(moduleId));
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<lbmModule<T>>>& Network<T>::getModules() const {
        return modules;
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<Group<T>>>& Network<T>::getGroups() const {
        return groups;
    }

    template<typename T>
    void Network<T>::toJson(std::string jsonString) const {
        /** TODO
         * 
        */
    }

    template<typename T>
    void Network<T>::sortGroups() {
        std::vector<int> nodeVector;
        int groupId = 0;
        for (auto& [key, node] : nodes) {
            nodeVector.emplace_back(key);
        }
        while(!nodeVector.empty()){
            std::queue<int> connectedNodes;
            std::unordered_set<int> nodeIds;
            std::unordered_set<int> channelIds;
            auto p = nodeIds.insert(nodeVector.front());
            if (p.second) {
                for (auto& [key, channel] : channels) {
                    if (channel->getNodeA() == nodeVector.front()) {
                        auto t = channelIds.insert(channel->getId());
                        if (t.second) {
                            connectedNodes.push(channel->getNodeB());
                        }
                    }
                    if (channel->getNodeB() == nodeVector.front()) {
                        auto t = channelIds.insert(channel->getId());
                        if (t.second) {
                            connectedNodes.push(channel->getNodeA());
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
                    for (auto& [key, channel] : channels) {
                        if (channel->getNodeA() == connectedNodes.front()) {
                            auto t = channelIds.insert(channel->getId());
                            if (t.second) {
                                connectedNodes.push(channel->getNodeB());
                            }
                        }
                        if (channel->getNodeB() == connectedNodes.front()) {
                            auto t = channelIds.insert(channel->getId());
                            if (t.second) {
                                connectedNodes.push(channel->getNodeA());
                            }
                        }
                    }
                    for (long unsigned int i=0; i < nodeVector.size(); i++) {
                        if (nodeVector[i] == connectedNodes.front()) {
                            nodeVector.erase(nodeVector.begin() + i);
                        }
                    }
                    connectedNodes.pop();
                }
            }

            Group<T>* addGroup = new Group<T>(groupId, nodeIds, channelIds, this);
            groups.try_emplace(groupId, addGroup);
            
            groupId++;
        }
    }

    template<typename T>
    bool Network<T>::isNetworkValid() {
        // checks if all nodes and channels are connected to ground (if channel network is one graph)
        std::unordered_map<int, bool> visitedNodes;
        std::unordered_map<int, bool> visitedChannels;

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
        for (auto const& [k, v] : channels) {
            visitedChannels[k] = false;
        }

        for (auto& node : groundNodes) {
            visitNodes(node->getId(), visitedNodes, visitedChannels);
        }

        std::string errorNodes = "";
        for (auto const& [k, v] : nodes) {
            const auto net = reach.at(k);
            int connections = net.size();
            for (auto const& [key, pump] : pressurePumps) {
                if (pump->getNode0()->getId() == k || pump->getNode1()->getId() == k) {
                    connections +=1 ;
                }
            }
            for (auto const& [key, pump] : flowRatePumps) {
                if (pump->getNode0()->getId() == k || pump->getNode1()->getId() == k) {
                    connections +=1 ;
                }
            }
            if (connections <= 1 && !getGroundIds().count(k)) {
                errorNodes.append(" " + std::to_string(k));
            }
        }

        if (errorNodes.length() != 0) {
            throw std::invalid_argument("Chip is invalid. The following nodes are dangling but not ground nodes: " + errorNodes + ". Please set these nodes to ground nodes." );
            return false;
        }

        for (auto const& [k, v] : nodes) {
            if (visitedNodes[k] == false) {
                errorNodes.append(" " + std::to_string(k));
            }
        }
        std::string errorChannels = "";
        for (auto const& [k, v] : channels) {
            if (visitedChannels[k] == false) {
                errorChannels.append(" " + std::to_string(k));
            }
        }

        if (errorNodes.length() != 0 || errorChannels.length() != 0) {
            throw std::invalid_argument("Chip is invalid. The following nodes are not connected to ground: " + errorNodes + ". The following channels are not connected to ground: " + errorChannels);
            return false;
        }

        return true;
    }

}   // namespace arch