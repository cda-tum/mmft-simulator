#include <iostream>

#include "JSONPorter.h"

#include "../architecture/Channel.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Module.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"
#include "../architecture/Platform.h"

namespace porting {

    template<typename T>
    arch::Network<T> networkFromJSON(std::string jsonFile) {

        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);

        std::unordered_map<int, std::unique_ptr<arch::Node<T>>> nodes;
        std::unordered_map<int, std::unique_ptr<arch::RectangularChannel<T>>> channels;
        std::unordered_map<int, std::unique_ptr<arch::lbmModule<T>>> modules;

        if (jsonString["Platform"] == "Droplet") {
            arch::Platform platform = arch::Platform::DROPLET;
        } else {
            arch::Platform platform = arch::Platform::CONTINUOUS;
        }

        for (auto& node : jsonString["Network"]["Nodes"]) {
            arch::Node<T>* addNode = new arch::Node<T>(node["iD"], T(node["x"]), T(node["y"]));
            nodes.try_emplace(node["iD"], addNode);
        }

        for (auto& channel : jsonString["Network"]["Channels"]) {
            arch::RectangularChannel<T>* addChannel = new arch::RectangularChannel<T>(channel["iD"], channel["nA"], channel["nB"], 
                channel["width"], channel["height"]);
            channels.try_emplace(channel["iD"], addChannel);
        }

        for (auto& module : jsonString["Network"]["Modules"]) {
            std::unordered_map<int, std::shared_ptr<arch::Node<T>>> Nodes;
            std::unordered_map<int, arch::Opening<T>> Openings;
            for (auto& opening : module["Openings"]){
                T nodeId = opening["nodeId"];
                Nodes.try_emplace(nodeId, nodes.at(nodeId));
                std::vector<T> normal = { opening["normalX"], opening["normalY"] };
                arch::Opening<T> opening_(nodes.at(nodeId), normal, opening["width"]);
                Openings.try_emplace(nodeId, opening_);
            }
            std::vector<T> position = { module["posX"], module["posY"] };
            std::vector<T> size = { module["sizeX"], module["sizeY"] };
            arch::lbmModule<T>* addModule = new arch::lbmModule<T>( module["iD"], module["name"], position,
                                                        size, Nodes, Openings, module["stlFile"], 
                                                        module["charPhysLength"], module["charPhysVelocity"],
                                                        module["alpha"], module["resolution"], module["epsilon"], module["tau"]);
            modules.try_emplace(module["iD"], addModule);
        }
        
        arch::Network<T> network (nodes, channels, modules, platform);

        return network;

    }

    template<typename T>
    arch::Network<T> networkToJSON(std::string jsonFile) {
        // TODO
    }

}   // namespace porting