#include "jsonPorter.h"

namespace porting {

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

template<typename T>
std::shared_ptr<arch::Network<T>> networkFromJSON(std::string jsonFile) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    auto network = std::make_shared<arch::Network<T>>();
    readNodes(jsonString, network);
    readChannels(jsonString, network);
    readModules(jsonString, network);

    return network;
}

template<typename T>
std::shared_ptr<sim::Simulation<T>> simulationFromJSON(std::string jsonFile, std::shared_ptr<arch::Network<T>> network_) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    auto simulation = std::make_shared<sim::Simulation<T>>();
    sim::Platform platform = readPlatform<T>(jsonString, simulation);
    sim::Type simType = readType<T>(jsonString, simulation);
    int activeFixture = readActiveFixture<T>(jsonString);

    simulation->setFixtureId(activeFixture);
    simulation->setNetwork(network_);

    readFluids<T>(jsonString, simulation);
    readResistanceModel<T>(jsonString, simulation);

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
        if (simType != sim::Type::Abstract) {
            throw std::invalid_argument("Mixing simulations are currently only supported for Abstract simulations.");
        }
        readMixingModel<T>(jsonString, simulation);
        readSpecies<T>(jsonString, simulation);
        readMixtures<T>(jsonString, simulation);
        readMixtureInjections<T>(jsonString, simulation, activeFixture);
    } else {
        throw std::invalid_argument("Invalid platform. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
    }

    if (simType == sim::Type::Hybrid) {
        readSimulators<T>(jsonString, simulation, network_);
//        network_->sortGroups();
    }

    if (simType == sim::Type::CFD) {
        // NOT YET SUPPORTED
        throw std::invalid_argument("Full CFD simulations are not yet supported in the simulator.");
    }

    readBoundaryConditions<T>(jsonString, simulation, activeFixture);
    readContinuousPhase<T>(jsonString, simulation, activeFixture);
    readPumps<T>(jsonString, network_);

    return simulation;
}

template<typename T>
void resultToJSON(std::string jsonFile, std::shared_ptr<sim::Simulation<T>> simulation) {
    std::ofstream file(jsonFile);

    ordered_json jsonString = resultToJSON<T>(simulation);

    file << jsonString.dump(4) << std::endl;
}

template<typename T>
nlohmann::ordered_json resultToJSON(std::shared_ptr<sim::Simulation<T>> simulation) {

    auto jsonResult = ordered_json::object();
    auto jsonStates = ordered_json::array();

    for (auto const& state : simulation->getSimulationResults()->getStates()) {
        auto jsonState = ordered_json::object();
        jsonState["time"] = state->getTime();
        jsonState["nodes"] = writePressures(state);
        jsonState["channels"] = writeChannels(state);
        if (simulation->getPlatform() == sim::Platform::BigDroplet && simulation->getType() == sim::Type::Abstract) {
            jsonState["bigDroplets"] = writeDroplets(state, simulation);
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
