#include "jsonPorter.h"

namespace porting {

using json = nlohmann::json;

template<typename T>
arch::Network<T> networkFromJSON(std::string jsonFile) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    arch::Network<T> network;

    readNodes(jsonString, network);
    readChannels(jsonString, network);

    return network;
}

template<typename T>
sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_) {

    std::ifstream f(jsonFile);
    json jsonString = json::parse(f);

    sim::Simulation<T> simulation = sim::Simulation<T>();
    sim::Platform platform = readPlatform<T>(jsonString, simulation);
    sim::Type simType = readType<T>(jsonString, simulation);
    int activeFixture = readActiveFixture<T>(jsonString);

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
        // Import Species for Mixing platform
            // TODO
        // Import bolus injections in fixture
            // TODO
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
    json jsonString;
    std::ofstream file(jsonFile);

    for (auto const& state : simulation->getSimulationResults()->getStates()) {
        jsonString["result"].push_back({"time", state->getTime()});
        writePressures(jsonString, state.get());
        writeFlowRates(jsonString, state.get());
        if (simulation->getPlatform() == sim::Platform::BigDroplet && simulation->getType() == sim::Type::Abstract) {
            writeDroplets(jsonString, state.get(), simulation);
        }
    }

    file << jsonString.dump(4) << std::endl;
}

}   // namespace porting