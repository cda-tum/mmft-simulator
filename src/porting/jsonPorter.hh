#include "jsonPorter.h"

namespace porting {

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

template<typename T>
std::shared_ptr<arch::Network<T>> networkFromJSON(std::string jsonFile) {
    json jsonString;
    // Transform given path to jsonFile into json object
    try {
        std::ifstream f(jsonFile);
        jsonString = json::parse(f);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string(__func__) + " in file " + __FILE__ + ", line " + std::to_string(__LINE__) + ": Could not read provided json file.");
    }
    // Forward json object to networkFromJSON overload and 
    // return network object
    return networkFromJSON<T>(jsonString);
}

template<typename T>
void networkFromJSON(std::string jsonFile, std::shared_ptr<arch::Network<T>>& network) {
    try {
        // Transform given path to jsonFile into json object
        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);
        // Read network components
        readNodes(jsonString, *network);
        readChannels(jsonString, *network);
        readModules(jsonString, *network);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string(__func__) + " in file " + __FILE__ + ", line " + std::to_string(__LINE__) + ": Could not read provided json file.");
    }
}

template<typename T>
std::shared_ptr<arch::Network<T>> networkFromJSON(json jsonString) {

    auto network = arch::Network<T>::createNetwork();

    // Read network components
    readNodes(jsonString, *network);
    readChannels(jsonString, *network);
    readModules(jsonString, *network);

    return network;
}

template<typename T>
std::unique_ptr<sim::Simulation<T>> simulationFromJSON(std::string jsonFile, std::shared_ptr<arch::Network<T>> network_) {
    json jsonString;
    // Transform given path to jsonFile into json object
    try {
        std::ifstream f(jsonFile);
        jsonString = json::parse(f);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string(__func__) + " in file " + __FILE__ + ", line " + std::to_string(__LINE__) + ": Could not read provided json file.");
    }
    // Forward json object to simulationFromJSON overload and 
    // return unique_ptr to simulation object
    return simulationFromJSON<T>(jsonString, network_);
}

template<typename T>
std::unique_ptr<sim::Simulation<T>> simulationFromJSON(json jsonString, std::shared_ptr<arch::Network<T>> network_) {

    std::unique_ptr<sim::Simulation<T>> simPtr = nullptr;

    sim::Platform platform = readPlatform<T>(jsonString);
    sim::Type simType = readType<T>(jsonString);
    size_t activeFixture = readActiveFixture<T>(jsonString);

    // Read an Abstract simulation definition
    if (simType == sim::Type::Abstract) {
        if (platform == sim::Platform::Continuous) {
            simPtr = std::make_unique<sim::AbstractContinuous<T>>(network_);
            simPtr->setFixtureId(activeFixture);
            readFluids<T>(jsonString, *simPtr);
            readPumps<T>(jsonString, network_.get());
        } else if (platform == sim::Platform::BigDroplet) {
            simPtr = std::make_unique<sim::AbstractDroplet<T>>(network_);
            simPtr->setFixtureId(activeFixture);
            readFluids<T>(jsonString, *simPtr);
            readPumps<T>(jsonString, network_.get());
            readDropletInjections<T>(jsonString, *(dynamic_cast<sim::AbstractDroplet<T>*>(simPtr.get())), activeFixture);
        } else if (platform == sim::Platform::Mixing) {
            simPtr = std::make_unique<sim::AbstractMixing<T>>(network_);
            simPtr->setFixtureId(activeFixture);
            readFluids<T>(jsonString, *simPtr);
            readPumps<T>(jsonString, network_.get());
            readMixingModel<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readSpecies<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readMixtures<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readMixtureInjections<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())), activeFixture);
        } else {
            throw std::invalid_argument("Invalid platform for Abstract simulation. Please select one of the following:\n\tcontinuous\n\tdroplet\n\tmixing");
        }

        readContinuousPhase<T>(jsonString, *simPtr, activeFixture);
        readResistanceModel<T>(jsonString, *simPtr);
    }

    // Read a Hybrid simulation definition
    else if (simType == sim::Type::Hybrid) {
        if (platform == sim::Platform::Continuous) {
            simPtr = std::make_unique<sim::HybridContinuous<T>>(network_);
            simPtr->setFixtureId(activeFixture);
            readFluids<T>(jsonString, *simPtr);
            readContinuousPhase<T>(jsonString, *simPtr, activeFixture);
            readResistanceModel<T>(jsonString, *simPtr);
            readSimulators<T>(jsonString, *(dynamic_cast<sim::HybridContinuous<T>*>(simPtr.get())), network_.get());
            readUpdateScheme(jsonString, *(dynamic_cast<sim::HybridContinuous<T>*>(simPtr.get())));
        } else if (platform == sim::Platform::Mixing) {
            simPtr = std::make_unique<sim::HybridMixing<T>>(network_);
            simPtr->setFixtureId(activeFixture);
            readFluids<T>(jsonString, *simPtr);
            readContinuousPhase<T>(jsonString, *simPtr, activeFixture);
            readResistanceModel<T>(jsonString, *simPtr);
            readMixingModel<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readSpecies<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readMixtures<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())));
            readMixtureInjections<T>(jsonString, *(dynamic_cast<sim::ConcentrationSemantics<T>*>(simPtr.get())), activeFixture);
            readSimulators<T>(jsonString, *(dynamic_cast<sim::HybridMixing<T>*>(simPtr.get())), network_.get());
            readUpdateScheme(jsonString, *(dynamic_cast<sim::HybridMixing<T>*>(simPtr.get())));
        } else if (platform == sim::Platform::Ooc) {
            throw std::invalid_argument("OoC simulations are currently not supported.");
            /** TODO: HybridOocSimulation
             * Enable hybrid OoC simulation and uncomment code below
             */
            // readContinuousPhase<T>(jsonString, *simPtr, activeFixture);
            // readResistanceModel<T>(jsonString, *simPtr);
            // readMixingModel<T>(jsonString, *simPtr);
            // readSpecies<T>(jsonString, *simPtr);
            // readMixtures<T>(jsonString, *simPtr);
            // readMixtureInjections<T>(jsonString, *simPtr, activeFixture);
            // readTissues<T>(jsonString, *simPtr);
            // readSimulators<T>(jsonString, *simPtr, network_);
            // readUpdateScheme(jsonString, *simPtr);
            // network_->sortGroups();
        } else if (platform == sim::Platform::BigDroplet) {
            throw std::invalid_argument("Droplet simulations are currently only supported for Abstract simulations.");
        } else {
            throw std::invalid_argument("Invalid platform for Hybrid simulation. Please select one of the following:\n\tcontinuous");
        }

        readPumps<T>(jsonString, network_.get());
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

    if (!simPtr) {
        throw std::runtime_error("Error in constructing the simulation object from the given JSON definition: nullPtr returned.");
    }

    return simPtr;
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

    for (auto const& state : simulation->getResults()->getStates()) {
        auto jsonState = ordered_json::object();
        jsonState["time"] = state->getTime();
        jsonState["nodes"] = writePressures(state.get());
        jsonState["channels"] = writeChannels(simulation->getNetwork().get(), state.get());
        if (simulation->getPlatform() == sim::Platform::Continuous && simulation->getType() == sim::Type::Hybrid) {
            jsonState["modules"] = writeModules(state.get());
        }
        if (simulation->getPlatform() == sim::Platform::BigDroplet && simulation->getType() == sim::Type::Abstract) {
            jsonState["bigDroplets"] = writeDroplets(state.get(), dynamic_cast<sim::AbstractDroplet<T>*>(simulation));
        }
        jsonStates.push_back(jsonState);
    }

    jsonResult["fixture"] = simulation->getFixtureId();
    jsonResult["type"] = writeSimType(simulation);
    jsonResult["platform"] = writeSimPlatform(simulation);
    jsonResult["fluids"] =  writeFluids(simulation);
    if (simulation->getPlatform() == sim::Platform::Mixing && simulation->getType() == sim::Type::Abstract) {
        jsonResult["mixtures"] = writeMixtures(dynamic_cast<sim::AbstractMixing<T>*>(simulation));
    }
    jsonResult.push_back({"network", jsonStates});

    return jsonResult;
}

}   // namespace porting
