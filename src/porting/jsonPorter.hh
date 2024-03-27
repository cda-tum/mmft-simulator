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
}

template<typename T>
arch::Network<T> networkFromJSON(json jsonString) {

    arch::Network<T> network;

    readNodes(jsonString, network);
    readChannels(jsonString, network);

    return network;
}

template<typename T>
sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_) {
    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    sim::Simulation<T> simulation = simulationFromJSON<T>(jsonString, network_);

    return simulation;
}

template<typename T>
void simulationFromJSON(std::string jsonFile, arch::Network<T>* network_, sim::Simulation<T>& simulation) {
    
    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    sim::Platform platform = readPlatform<T>(jsonString, simulation);
    sim::Type simType = readType<T>(jsonString, simulation);
    int activeFixture = readActiveFixture<T>(jsonString);
    simulation.setFixtureId(activeFixture);

    simulation.setNetwork(network_);

    readFluids<T>(jsonString, simulation);

    if (platform == sim::Platform::Continuous) {
        if (simType == sim::Type::CFD) {
            throw std::invalid_argument("Continuous simulations are currently not supported for CFD simulations.");
        }
    } else
    if (platform == sim::Platform::BigDroplet) {
        if (simType != sim::Type::Abstract) {
            throw std::invalid_argument("Droplet simulations are currently only supported for Abstract simulations.");
        }
        readDropletInjections<T>(jsonString, simulation, activeFixture);
    } else
    if (platform == sim::Platform::Mixing) {
        // NOT YET SUPPORTED
        throw std::invalid_argument("Mixing simulations are not yet supported in the simulator.");
    } else {
        throw std::invalid_argument("Invalid platform. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
    }

    if (simType == sim::Type::Hybrid) {
        readSimulators<T>(jsonString, network_);
        network_->sortGroups();
    }

    if (simType == sim::Type::CFD) {
        // NOT YET SUPPORTED
        throw std::invalid_argument("Full CFD simulations are not yet supported in the simulator.");
    }

    readBoundaryConditions<T>(jsonString, simulation, activeFixture);
    readContinuousPhase<T>(jsonString, simulation, activeFixture);
    readPumps<T>(jsonString, network_);
    readResistanceModel<T>(jsonString, simulation);
}

template<typename T>
sim::Simulation<T> simulationFromJSON(json jsonString, arch::Network<T>* network_) {

    sim::Simulation<T> simulation = sim::Simulation<T>();
    sim::Platform platform = readPlatform<T>(jsonString, simulation);
    sim::Type simType = readType<T>(jsonString, simulation);
    int activeFixture = readActiveFixture<T>(jsonString);
    simulation.setFixtureId(activeFixture);

    simulation.setNetwork(network_);

    readFluids<T>(jsonString, simulation);

    if (platform == sim::Platform::Continuous) {
        if (simType == sim::Type::CFD) {
            throw std::invalid_argument("Continuous simulations are currently not supported for CFD simulations.");
        }
    } else
    if (platform == sim::Platform::BigDroplet) {
        if (simType != sim::Type::Abstract) {
            throw std::invalid_argument("Droplet simulations are currently only supported for Abstract simulations.");
        }
        //readDroplets<T>(jsonString, simulation);
        readDropletInjections<T>(jsonString, simulation, activeFixture);
    } else
    if (platform == sim::Platform::Mixing) {
        // NOT YET SUPPORTED
        throw std::invalid_argument("Mixing simulations are not yet supported in the simulator.");
    } else {
        throw std::invalid_argument("Invalid platform. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
    }

    if (simType == sim::Type::Hybrid) {
        readSimulators<T>(jsonString, network_);
        network_->sortGroups();
    }

    if (simType == sim::Type::CFD) {
        // NOT YET SUPPORTED
        throw std::invalid_argument("Full CFD simulations are not yet supported in the simulator.");
    }

    readBoundaryConditions<T>(jsonString, simulation, activeFixture);
    readContinuousPhase<T>(jsonString, simulation, activeFixture);
    readPumps<T>(jsonString, network_);
    readResistanceModel<T>(jsonString, simulation);

    return simulation;
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
        jsonState["channels"] = writeFlowRates(state.get());
        if (simulation->getPlatform() == sim::Platform::BigDroplet && simulation->getType() == sim::Type::Abstract) {
            jsonState["bigDroplets"] = writeDroplets(state.get(), simulation);
        }
        jsonStates.push_back(jsonState);
    }

    jsonResult["fixture"] = simulation->getFixtureId();
    jsonResult["type"] = writeSimType(simulation);
    jsonResult["platform"] = writeSimPlatform(simulation);
    jsonResult["fluids"] =  writeFluids(simulation);
    jsonResult.push_back({"network", jsonStates});

    return jsonResult;
}

}   // namespace porting