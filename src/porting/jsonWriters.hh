#include "jsonWriters.h"

namespace porting {

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
    auto BigDroplets = json::array();
    for (auto& [key, dropletPosition] : state->getDropletPositions()) {
        //dropletPosition
        auto BigDroplet = json::object();

        //state
        BigDroplet["id"] = key;
        BigDroplet["fluid"] = simulation->getDroplet(key)->getFluid()->getId();

        //boundaries
        BigDroplet["boundaries"] = json::array();
        for(auto& boundary : dropletPosition.boundaries) {
            BigDroplet["boundaries"].push_back({
                {"volumeTowards1", boundary.isVolumeTowardsNodeA()},
                {"position", {
                    {"channelId", boundary.getChannelPosition().getChannel()->getId()},
                    {"position", boundary.getChannelPosition().getPosition()}}
                }
            });
        }

        //channels
        BigDroplet["channels"] = json::array();
        for (auto const& channelId : dropletPosition.channelIds) {
            BigDroplet["channels"].push_back(channelId);
        }
        BigDroplets.push_back(BigDroplet);
    }
    jsonString["result"].push_back({"droplets", BigDroplets});
}

}   // namespace porting