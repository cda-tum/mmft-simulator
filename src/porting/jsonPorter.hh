#include "jsonPorter.h"

namespace porting {

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

template<typename T>
arch::Network<T> networkFromJSON(std::string jsonFile) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    arch::Network<T> network = networkFromJSON<T>(jsonString);

    return network;
}

template<typename T>
void networkFromJSON(std::string jsonFile, arch::Network<T>& network) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    readNodes(jsonString, network);
    readChannels(jsonString, network);
    readModules(jsonString, network);
}

template<typename T>
arch::Network<T> networkFromJSON(json jsonString) {

    arch::Network<T> network;

    readNodes(jsonString, network);
    readChannels(jsonString, network);
    readModules(jsonString, network);

    return network;
}

template<typename T>
sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_) {
    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    sim::Simulation<T> simulation = sim::Simulation<T>();

    simulationFromJSON<T>(jsonFile, network_, simulation);

    return simulation;
}

template<typename T>
sim::Simulation<T> simulationFromJSON(json jsonString, arch::Network<T>* network_) {
    sim::Simulation<T> simulation = sim::Simulation<T>();

    simulationFromJSON<T>(jsonString, network_, simulation);

    return simulation;
}

template<typename T>
void simulationFromJSON(std::string jsonFile, arch::Network<T>* network_, sim::Simulation<T>& simulation) {
    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    simulationFromJSON<T>(jsonString, network_, simulation);
}

template<typename T>
void simulationFromJSON(json jsonString, arch::Network<T>* network_, sim::Simulation<T>& simulation) {

    sim::Platform platform = readPlatform<T>(jsonString, simulation);
    sim::Type simType = readType<T>(jsonString, simulation);
    int activeFixture = readActiveFixture<T>(jsonString);
    simulation.setFixtureId(activeFixture);

    simulation.setNetwork(network_);

    readFluids<T>(jsonString, simulation);

    // Read an Abstract simulation definition
    if (simType == sim::Type::Abstract) {

        readPumps<T>(jsonString, network_);

        if (platform == sim::Platform::Continuous) {

        } else if (platform == sim::Platform::BigDroplet) {
            readDropletInjections<T>(jsonString, simulation, activeFixture);
        } else if (platform == sim::Platform::Mixing) {
            readMixingModel<T>(jsonString, simulation);
            readSpecies<T>(jsonString, simulation);
            readMixtures<T>(jsonString, simulation);
            readMixtureInjections<T>(jsonString, simulation, activeFixture);
        } else {
            throw std::invalid_argument("Invalid platform for Abstract simulation. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
        }

        readContinuousPhase<T>(jsonString, simulation, activeFixture);
        readResistanceModel<T>(jsonString, simulation);
        network_->sortGroups();
        network_->isNetworkValid();
    }

    // Read a Hybrid simulation definition
    else if (simType == sim::Type::Hybrid) {

        readContinuousPhase<T>(jsonString, simulation, activeFixture);
        readResistanceModel<T>(jsonString, simulation);

        if (platform == sim::Platform::Continuous) {
            readSimulators<T>(jsonString, simulation, network_);
            readUpdateScheme(jsonString, simulation);
            network_->sortGroups();
        } else if (platform == sim::Platform::Mixing) {
            readMixingModel<T>(jsonString, simulation);
            readSpecies<T>(jsonString, simulation);
            readMixtures<T>(jsonString, simulation);
            readMixtureInjections<T>(jsonString, simulation, activeFixture);
            readSimulators<T>(jsonString, simulation, network_);
            readUpdateScheme(jsonString, simulation);
            network_->sortGroups();
        } else if (platform == sim::Platform::Ooc) {
            readMixingModel<T>(jsonString, simulation);
            readSpecies<T>(jsonString, simulation);
            readMixtures<T>(jsonString, simulation);
            readMixtureInjections<T>(jsonString, simulation, activeFixture);
            readTissues<T>(jsonString, simulation);
            readSimulators<T>(jsonString, simulation, network_);
            readUpdateScheme(jsonString, simulation);
            network_->sortGroups();
        } else if (platform == sim::Platform::BigDroplet) {
            throw std::invalid_argument("Droplet simulations are currently only supported for Abstract simulations.");
        } else {
            throw std::invalid_argument("Invalid platform for Hybrid simulation. Please select one of the following:\n\tcontinuous");
        }

        readPumps<T>(jsonString, network_);
        network_->isNetworkValid();
    }

    // Read a CFD simulation definition
    else if (simType == sim::Type::CFD) {
        throw std::invalid_argument("Continuous simulations are currently not supported for CFD simulations.");
    } 
    
    // Invalid simulation definition
    else {
        throw std::invalid_argument("Invalid simulation type. Please select one of the following:\n\tAbstract\n\tHybrid");
    }

}

template<typename T>
void resultToJSON(std::string jsonFile, sim::Simulation<T>* simulation) {
    std::ofstream file(jsonFile);

    ordered_json jsonString = resultToJSON<T>(simulation);

    file << jsonString.dump(4) << std::endl;
}

template<typename T>
nlohmann::ordered_json resultToJSON(sim::Simulation<T>* simulation) {

    auto jsonResult = ordered_json::object();
    auto jsonStates = ordered_json::array();

    for (auto const& state : simulation->getSimulationResults()->getStates()) {
        auto jsonState = ordered_json::object();
        jsonState["time"] = state->getTime();
        jsonState["nodes"] = writePressures(state.get());
        jsonState["channels"] = writeChannels(state.get());
        if (simulation->getPlatform() == sim::Platform::Continuous && simulation->getType() == sim::Type::Hybrid) {
            jsonState["modules"] = writeModules(state.get());
        }
        if (simulation->getPlatform() == sim::Platform::BigDroplet && simulation->getType() == sim::Type::Abstract) {
            jsonState["bigDroplets"] = writeDroplets(state.get(), simulation);
        }
        jsonStates.push_back(jsonState);
    }

    jsonResult["fixture"] = simulation->getFixtureId();
    jsonResult["type"] = writeSimType(simulation);
    jsonResult["platform"] = writeSimPlatform(simulation);
    jsonResult["fluids"] =  writeFluids(simulation);
    if (simulation->getPlatform() == sim::Platform::Mixing && simulation->getType() == sim::Type::Abstract) {
        jsonResult["mixtures"] = writeMixtures(simulation);
    }
    jsonResult.push_back({"network", jsonStates});

    return jsonResult;
}

}   // namespace porting
