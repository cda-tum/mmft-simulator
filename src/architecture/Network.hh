#include <iostream>

#include "Network.h"

#include <Node.h>
#include <Channel.h>
#include <Module.h>
#include <lbmModule.h>

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
        for (int i = 0; i < nodeIds.size(); ++i){
            for (int j = i+1; j < nodeIds.size(); ++j){
                int nA = nodeIds[i];
                int nB = nodeIds[j];
                RectangularChannel<T>* addChannel = new RectangularChannel<T>(channel_counter, nA, nB, (T) 1e-4, (T) 1e-4);
                channels.try_emplace(channel_counter, addChannel);
                //std::cout << "[Generate fully connected graph] I've added channel " << channel_counter <<
                //    " between nodes " << nA << " and " << nB << std::endl;
                ++channel_counter;
            }
        }
    }
    
    template<typename T>
    Network<T>::Network(std::string jsonFile) {

        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);

        for (auto& node : jsonString["Network"]["Nodes"]) {
            Node<T>* addNode = new Node<T>(node["iD"], T(node["x"]), T(node["y"]));
            nodes.try_emplace(node["iD"], addNode);
        }

        for (auto& channel : jsonString["Network"]["Channels"]) {
            RectangularChannel<T>* addChannel = new RectangularChannel<T>(channel["iD"], channel["nA"], channel["nB"], 
                channel["width"], channel["height"]);
            channels.try_emplace(channel["iD"], addChannel);
        }

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
            lbmModule<T>* addModule = new lbmModule<T>( module["iD"], module["name"], position,
                                                        size, Nodes, Openings, module["stlFile"], 
                                                        module["charPhysLength"], module["charPhysVelocity"],
                                                        module["alpha"], module["resolution"], module["epsilon"], module["tau"]);
            modules.try_emplace(module["iD"], addModule);
        }
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
        std::cerr << "The function roJson(std::string jsonString) is not implemented." << std::endl;
    }

}   // namespace arch