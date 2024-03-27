#include "jsonWriters.h"

namespace porting {

template<typename T>
auto writePressures(result::State<T>* state) {
    auto nodes = ordered_json::array();
    auto const& pressures = state->getPressures();
    for (long unsigned int i=0; i<pressures.size(); ++i) {
        nodes.push_back({{"pressure", pressures.at(i)}});
    }
    return nodes;
}

template<typename T>
auto writeFlowRates(result::State<T>* state) {      
    auto channels = ordered_json::array();
    auto const& flowRates = state->getFlowRates();
    for (long unsigned int i=0; i<flowRates.size(); ++i) {
        channels.push_back({{"flowRate", flowRates.at(i)}});
    }
    return channels;
}

template<typename T>
auto writeDroplets(result::State<T>* state, sim::Simulation<T>* simulation) {      
    auto BigDroplets = ordered_json::array();
    for (auto& [key, dropletPosition] : state->getDropletPositions()) {
        //dropletPosition
        auto BigDroplet = ordered_json::object();

        //state
        BigDroplet["id"] = key;
        BigDroplet["fluid"] = simulation->getDroplet(key)->getFluid()->getId();
        BigDroplet["volume"] = simulation->getDroplet(key)->getVolume();

        //boundaries
        BigDroplet["boundaries"] = ordered_json::array();
        for(auto& boundary : dropletPosition.boundaries) {
            BigDroplet["boundaries"].push_back({
                {"volumeTowards1", boundary.isVolumeTowardsNodeA()},
                {"position", {
                    {"channel", boundary.getChannelPosition().getChannel()->getId()},
                    {"position", boundary.getChannelPosition().getPosition()}}
                }
            });
        }

        //channels
        BigDroplet["channels"] = ordered_json::array();
        for (auto const& channelId : dropletPosition.channelIds) {
            BigDroplet["channels"].push_back(channelId);
        }
        BigDroplets.push_back(BigDroplet);
    }
    return BigDroplets;
}

template<typename T>
auto writeFluids(sim::Simulation<T>* simulation) {      
    auto Fluids = ordered_json::array();
    auto const& simFluids = simulation->getFluids();
    for (long unsigned int i=0; i<simFluids.size(); ++i) {
        auto Fluid = ordered_json::object();
        auto& simFluid = simFluids.at(i);
        Fluid["id"] = simFluid->getId();
        Fluid["name"] = simFluid->getName();
        Fluid["concentration"] = simFluid->getConcentration();
        Fluid["density"] = simFluid->getDensity();
        Fluid["viscosity"] = simFluid->getViscosity();
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