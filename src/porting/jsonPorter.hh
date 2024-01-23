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

        if (platform == sim::Platform::CONTINUOUS) {
            if (simType == sim::Type::CFD) {
                throw std::invalid_argument("Continuous simulations are currently not supported for CFD simulations.");
            }
        } else
        if (platform == sim::Platform::DROPLET) {
            if (simType != sim::Type::_1D) {
                throw std::invalid_argument("Droplet simulations are currently only supported for 1D simulations.");
            }
            readDroplets<T>(jsonString, simulation);
            readDropletInjections<T>(jsonString, simulation, activeFixture);
        } else
        if (platform == sim::Platform::MIXING) {
            if (simType != sim::Type::_1D) {
                throw std::invalid_argument("Mixing simulations are currently only supported for 1D simulations.");
            }
            readSpecies<T>(jsonString, simulation);
            readMixtures<T>(jsonString, simulation);
            readMixtureInjections<T>(jsonString, simulation, activeFixture);
            readMixingModel<T>(jsonString, simulation);
        } else {
            throw std::invalid_argument("Invalid platform. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
        }

        if (simType == sim::Type::HYBRID) {
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
            if (simulation->getPlatform() == sim::Platform::DROPLET && simulation->getType() == sim::Type::_1D) {
                writeDroplets(jsonString, state.get(), simulation);
            }
            if (simulation->getPlatform() == sim::Platform::MIXING && simulation->getType() == sim::Type::_1D) {
                writeConcentrations(jsonString, state.get(), simulation);
            }
        }

        file << jsonString.dump(4) << std::endl;
    }

}   // namespace porting