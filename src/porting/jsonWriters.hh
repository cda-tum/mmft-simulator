#include "jsonWriters.h"

namespace porting {

template<typename T>
auto writePressures(result::State<T>* state) {
    auto nodes = json::array();
    for (auto const& [key, pressure] : state->getPressures()) {
        nodes.push_back({{"pressure", pressure}});
    }
    return nodes;
}

template<typename T>
auto writeFlowRates(result::State<T>* state) {      
    auto channels = json::array();
    for (auto const& [key, flowRate] : state->getFlowRates()) {
        channels.push_back({{"flowRate", flowRate}});
    }
    return channels;
}

template<typename T>
auto writeDroplets(result::State<T>* state, sim::Simulation<T>* simulation) {      
    auto BigDroplets = json::array();
    for (auto& [key, dropletPosition] : state->getDropletPositions()) {
        //dropletPosition
        auto BigDroplet = json::object();

        //state
        BigDroplet["id"] = key;
        BigDroplet["fluid"] = simulation->getDroplet(key)->getFluid()->getId();
        BigDroplet["volume"] = simulation->getDroplet(key)->getVolume();

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
    return BigDroplets;
}

template<typename T>
auto writeFluids(sim::Simulation<T>* simulation) {      
    auto Fluids = json::array();
    for (auto const& [key, fluid] : simulation->getFluids()) {
        auto Fluid = json::object();
        Fluid["id"] = key;
        Fluid["name"] = fluid->getName();
        Fluid["concentration"] = fluid->getConcentration();
        Fluid["density"] = fluid->getDensity();
        Fluid["viscosity"] = fluid->getViscosity();
        Fluids.push_back(Fluid);
    }
    return Fluids;
}

template<typename T>
std::string writeSimType(sim::Simulation<T>* simulation) {      
    if(simulation->getType() == sim::Type::Hybrid) {
        return("Hybrid");
    } else if (simulation->getType() == sim::Type::CFD) {
        return("CFD");
    }
    return("Abstract");
}

template<typename T>
std::string writeSimPlatform(sim::Simulation<T>* simulation) {      
    if(simulation->getPlatform() == sim::Platform::BigDroplet) {
        return("BigDroplet");
    } else if (simulation->getPlatform() == sim::Platform::Mixing) {
        return("Mixing");
    }
    return("Continuous");
}

}   // namespace porting