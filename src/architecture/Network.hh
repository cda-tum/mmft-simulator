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

        std::cout << "Loading Nodes..." << std::endl;

        for (auto& node : jsonString["Network"]["Nodes"]) {
            Node<T>* addNode = new Node<T>(node["iD"], T(node["x"]), T(node["y"]));
            nodes.try_emplace(node["iD"], addNode);
        }

        std::cout << "Loaded Nodes... OK" << std::endl;

        std::cout << "Loading Channels..." << std::endl;

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

        std::cout << "Loaded Channels... OK" << std::endl;

        std::cout << "Loading Modules..." << std::endl;

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

        std::cout << "Loaded Modules... OK" << std::endl;
    }

    template<typename T>
    int Network<T>::addNode() {
        // TODO
        std::cerr << "The function addNode() is not implemented." << std::endl;
        return -1;
    }

    template<typename T>
    std::shared_ptr<Node<T>>& Network<T>::getNode(int nodeId) const {
        return nodes.at(nodeId);
    };

    template<typename T>
    int Network<T>::addChannel() {
        // TODO
        std::cerr << "The function addChannel() is not implemented." << std::endl;
        return -1;
    }

    template<typename T>
    Channel<T>* Network<T>::getChannel(int channelId) const {
        return std::get<0>(channels.at(channelId));
    }

    template<typename T>
    int Network<T>::addModule() {
        // TODO
        std::cerr << "The function addModule() is not implemented." << std::endl;
        return -1;
    }

    template<typename T>
    Module<T>* Network<T>::getModule(int moduleId) const {
        return std::get<0>(modules.at(moduleId));
    }

    template<typename T>
    const std::unordered_map<int, std::shared_ptr<Node<T>>>& Network<T>::getNodes() const {
        return nodes;
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& Network<T>::getChannels() const {
        return channels;
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<lbmModule<T>>>& Network<T>::getModules() const {
        return modules;
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
    const std::unordered_map<int, std::unique_ptr<Group<T>>>& Network<T>::getGroups() const {
        return groups;
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
    void Network<T>::toJson(std::string jsonString) const {
        // TODO
        std::cerr << "The function toJson(std::string jsonString) is not implemented." << std::endl;
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

            Group<T>* addGroup = new Group<T>(groupId, nodeIds, channelIds);
            groups.try_emplace(groupId, addGroup);
            
            groupId++;
        }
    }

}   // namespace arch