#include <iostream>

#include "jsonPorter.h"

#include "../architecture/Channel.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Module.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"

#include "../simulation/Fluid.h"
#include "../simulation/ResistanceModels.h"

#include "../result/Results.h"

namespace porting {

    using json = nlohmann::json;

    template<typename T>
    void writePressures(json& jsonString, result::State<T>* state) {
        auto nodes = json::array();
        for (auto const& [key, pressure] : state->getPressures()) {
            nodes.push_back({{"pressure", pressure}});
        }
        jsonString["result"].push_back({"nodes", nodes});
    }

    template<typename T>
    void writeFlowRates(json& jsonString, result::State<T>* state) {      
        auto channels = json::array();
        for (auto const& [key, flowRate] : state->getFlowRates()) {
            channels.push_back({{"flowRate", flowRate}});
        }
        jsonString["result"].push_back({"channels", channels});
    }

    template<typename T>
    void writeDroplets(json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation) {      
        auto bigDroplets = json::array();
        for (auto& [key, dropletPosition] : state->getDropletPositions()) {
            //dropletPosition
            auto bigDroplet = json::object();

            //state
            bigDroplet["id"] = key;
            bigDroplet["fluid"] = simulation->getDroplet(key)->getFluid()->getId();

            //boundaries
            bigDroplet["boundaries"] = json::array();
            for(auto& boundary : dropletPosition.boundaries) {
                bigDroplet["boundaries"].push_back({
                    {"volumeTowards1", boundary.isVolumeTowardsNodeA()},
                    {"position", {
                        {"channelId", boundary.getChannelPosition().getChannel()->getId()},
                        {"position", boundary.getChannelPosition().getPosition()}}
                    }
                });
            }

            //channels
            bigDroplet["channels"] = json::array();
            for (auto const& channelId : dropletPosition.channelIds) {
                bigDroplet["channels"].push_back(channelId);
            }
            bigDroplets.push_back(bigDroplet);
        }
        jsonString["result"].push_back({"droplets", bigDroplets});
    }

    template<typename T>
    void writeConcentrations (json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation) {
        //TODO
    }

/*
 "concentrations": [
    {
        "species": 0,
        "distribution": [
            {
                "concentration": 4.0,
                "boundaries": [
                    {
                        "position": {
                            "channel": 1,
                            "position": 0.45
                        },
                        "volumeTowardsNode1": false
                    },
                    {
                        "posision": {
                            "channel": 1,
                            "position": 0.95
                        },
                        "volumeTowardsNode1": true
                    }
                ],
                "channels": []
            },
            {
                "concentration": 2.5,
                "boundaries": [
                    {
                        "position": {
                            "channel": 2,
                            "position": 0.45
                        },
                        "volumeTowardsNode1": false
                    },
                    {
                        "posision": {
                            "channel": 2,
                            "position": 0.95
                        },
                        "volumeTowardsNode1": true
                    }
                ],
                "channels": []
            },
        ]
    }
]
*/

    template<typename T>
    void writeMixtures (json& jsonString, result::State<T>* state, sim::Simulation<T>* simulation) {
        auto mixturePositions = json::array();
        for (auto& [key, mixturePosition] : state->getMixturePositions()) {
            // mixture object
            auto position = json::object();

            // species
            position["species"] = json::array();
            for(auto& specie : mixturePosition.mixture->getSpecies()) {
                mixturePosition["species"].push_back(specie.getId());
            }

            // concentrations
            position["concentrations"] = json::array();
            for(auto& concentration : mixturePosition.mixture->getConcentrations()) {
                mixturePosition["concentrations"].push_back(concentration);
            }

            //boundaries
            position["boundaries"] = json::array();
            for(auto& boundary : mixturePosition.boundaries()) {
                position["boundaries"].push_back({
                    {
                        {"channelId", boundary.getChannel()},
                        {"position1", boundary.getPosition1()},
                        {"position2", boundary.getPosition2()}
                    }
                });
            }

            mixturePositions.push_back(position);
        }
        jsonString["result"].push_back({"mixturePositions", mixturePositions});
    }

}   // namespace porting