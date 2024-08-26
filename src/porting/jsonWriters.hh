#include "jsonWriters.h"

namespace porting {

template<typename T>
auto writePressures(result::State<T>* state) {
    auto nodes = ordered_json::array();
    auto const& pressures = state->getPressures();
    for (auto& [key, pressure] : pressures) {
        nodes.push_back({{"pressure", pressure}});
    }
    return nodes;
}

template<typename T>
auto writeChannels(result::State<T>* state) {      
    auto channels = ordered_json::array();
    auto const& flowRates = state->getFlowRates();
    if (state->getMixturePositions().empty()) {
        for (auto& [key, flowRate] : flowRates) {
            channels.push_back({{"flowRate", flowRate}});
        }
    } else if ( !state->getMixturePositions().empty() ) {
        for (long unsigned int i=0; i<flowRates.size(); ++i) {
            auto channel = ordered_json::object();
            channel["flowRate"] = flowRates.at(i);
            if ( state->getMixturePositions().count(i) ) {
                channel["mixturePositions"] = ordered_json::array();
                for (auto& position : state->getMixturePositions().at(i)) {
                    channel["mixturePositions"].push_back({
                        {"mixture", position.mixtureId},
                        {"start", position.position1},
                        {"end", position.position2}
                    });
                }
            }
            channels.push_back(channel);
        }
    }
    return channels;
}

template<typename T>
auto writeModules(result::State<T>* state) {      
    auto modules = ordered_json::array();
    auto const& vtkFiles = state->getVtkFiles();
    for (auto& [key, vtkFile] : vtkFiles) {
        auto module = ordered_json::object();
        module["id"] = key;
        module["vtkFile"] = vtkFile;
        modules.push_back(module);
    }
    return modules;
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
auto writeMixtures (sim::Simulation<T>* simulation) {
    auto Mixtures = ordered_json::array();
    auto const& simMixtures = simulation->getMixtures();
    for (long unsigned int i=0; i<simMixtures.size(); ++i) {
        auto Mixture = ordered_json::object();
        auto& simMixture = simMixtures.at(i);
        for(auto& [key, specie] : simMixture->getSpecies()) {
            Mixture["species"].push_back(specie->getId());
            Mixture["concentrations"].push_back(simMixture->getConcentrationOfSpecie(specie->getId()));
        }
        Mixtures.push_back(Mixture);
    }
    return Mixtures;
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