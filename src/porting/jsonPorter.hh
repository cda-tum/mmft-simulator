#include <iostream>

#include "jsonPorter.h"

#include "../architecture/Channel.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Module.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"

#include "../simulation/Fluid.h"
#include "../simulation/ResistanceModels.h"

namespace porting {

    using json = nlohmann::json;

    template<typename T>
    arch::Network<T> networkFromJSON(std::string jsonFile) {

        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);

        std::unordered_map<int, std::shared_ptr<arch::Node<T>>> nodes;
        std::unordered_map<int, std::unique_ptr<arch::RectangularChannel<T>>> channels;

        int count=0;
        for (auto& node : jsonString["network"]["nodes"]) {
            arch::Node<T>* addNode = new arch::Node<T>(count, T(node["x"]), T(node["y"]));
            if(node["ground"]) {
                addNode->setGround(true);
            }
            nodes.try_emplace(count, addNode);
            count++;
        }

        count=0;
        for (auto& channel : jsonString["network"]["channels"]) {
            arch::RectangularChannel<T>* addChannel = new arch::RectangularChannel<T>(count, nodes.at(channel["node1"]), nodes.at(channel["node2"]), 
                channel["width"], channel["height"]);
            channels.try_emplace(count, addChannel);
            count++;
        }
        
        arch::Network<T> network (nodes, std::move(channels));

        return network;
    }

    template<typename T>
    sim::Simulation<T> simulationFromJSON(std::string jsonFile, arch::Network<T>* network_) {

        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);

        sim::Simulation<T> simulation = sim::Simulation<T>();

        int counter;

        // Define the platform
        sim::Platform platform = sim::Platform::NONE;
        if (!jsonString["simulation"].contains("platform")) {
            throw std::invalid_argument("Please define a platform. The following platforms are possible:\ncontinuous\nbigDroplet\nmixing");
        }
        if (jsonString["simulation"]["platform"] == "continuous") {
            platform = sim::Platform::CONTINUOUS;
        } else if (jsonString["simulation"]["platform"] == "bigDroplet") {
            platform = sim::Platform::DROPLET;
        } else if (jsonString["simulation"]["platform"] == "mixing") {
            platform = sim::Platform::MIXING;
        } else {
            throw std::invalid_argument("Platform is invalid. The following platforms are possible:\ncontinuous\nbigDroplet\nmixing");
        }
        simulation.setPlatform(platform);

        // Define the simulation type
        sim::Type simType = sim::Type::NONE;
        if (!jsonString["simulation"].contains("type")) {
            throw std::invalid_argument("Please define a simulation type. The following types are possible:\n1D\nhybrid\nCFD");
        }
        if (jsonString["simulation"]["type"] == "1D") {
            simType = sim::Type::_1D;
        } else if (jsonString["simulation"]["type"] == "hybrid") {
            simType = sim::Type::HYBRID;
        } else if (jsonString["simulation"]["type"] == "CFD") {
            simType = sim::Type::CFD;
        } else {
            throw std::invalid_argument("Simulation type is invalid. The following types are possible:\n1D\nhybrid\nCFD");
        }
        simulation.setType(simType);

        // Import fluids
        if (!jsonString["simulation"].contains("fluids") || jsonString["simulation"]["fluids"].empty()) {
            throw std::invalid_argument("No fluids are defined. Please define at least 1 fluid.");
        }
        std::unordered_map<int, std::unique_ptr<sim::Fluid<T>>> fluids;
        counter = 0;
        for (auto& fluid : jsonString["simulation"]["fluids"]) {
            if (fluid.contains("density") && fluid.contains("viscosity") && fluid.contains("concentration") && fluid.contains("name")) {
                T density = fluid["density"];
                T viscosity = fluid["viscosity"];
                std::string name = fluid["name"];
                std::unique_ptr<sim::Fluid<T>> newFluid = std::make_unique<sim::Fluid<T>>( counter, density, viscosity, 1.0, name );
                fluids.try_emplace(counter, std::move(newFluid));
                counter++;
            } else {
                throw std::invalid_argument("Wrongly defined fluid. Please provide following information for fluids:\nname\ndensity\nviscosity\nconcentration");
            }
        }
        simulation.setFluids(std::move(fluids));

        if (platform == sim::Platform::DROPLET) {
            // NOT YET SUPPORTED
            throw std::invalid_argument("Droplet simulations are not yet supported in the simulator.");
            // Import bigDroplet injections
            int activeFixture = 0;
            if (!jsonString["simulation"].contains("fixtures")) {
                throw std::invalid_argument("Please define at least one fixture.");
            }
            if (!jsonString["simulation"].contains("activeFixture")) {
                throw std::invalid_argument("Please define an active fixture.");
            } else {
                activeFixture = jsonString["simulation"]["activeFixture"];
            }
            /**TODO
             * Add droplets with droplets.push_back (similar to fluids above)
            */
        }

        if (platform == sim::Platform::MIXING) {
            // NOT YET SUPPORTED
            throw std::invalid_argument("Mixing simulations are not yet supported in the simulator.");
            // Import Species for Mixing platform
                // TODO
            // Import bolus injections in fixture
                // TODO
        }

        if (simType == sim::Type::HYBRID) {
            // Import simulators (modules)
            counter = 0;
            if (!jsonString["simulation"]["settings"].contains("simulators") || jsonString["simulation"]["settings"]["simulators"].empty()) {
                throw std::invalid_argument("Hybrid simulation type was set, but no CFD simulators were defined.");
            }
            std::unordered_map<int, std::unique_ptr<arch::lbmModule<T>>> modules;
            for (auto& module : jsonString["simulation"]["settings"]["simulators"]) {
                std::unordered_map<int, std::shared_ptr<arch::Node<T>>> Nodes;
                std::unordered_map<int, arch::Opening<T>> Openings;
                for (auto& opening : module["Openings"]) {
                    int nodeId = opening["node"];
                    Nodes.try_emplace(nodeId, network_->getNode(nodeId));
                    std::vector<T> normal = { opening["normal"]["x"], opening["normal"]["y"] };
                    arch::Opening<T> opening_(network_->getNode(nodeId), normal, opening["width"]);
                    Openings.try_emplace(nodeId, opening_);
                }
                std::vector<T> position = { module["posX"], module["posY"] };
                std::vector<T> size = { module["sizeX"], module["sizeY"] };
                arch::lbmModule<T>* addModule = new arch::lbmModule<T>( counter, module["name"], module["stlFile"],
                                                        position, size, Nodes, Openings, 
                                                        module["charPhysLength"], module["charPhysVelocity"],
                                                        module["alpha"], module["resolution"], module["epsilon"], module["tau"]);
                modules.try_emplace(counter, addModule);
                counter++;
            }
            network_->setModules(std::move(modules));
        }
        network_->sortGroups();

        if (simType == sim::Type::CFD) {
            // NOT YET SUPPORTED
            throw std::invalid_argument("Full CFD simulations are not yet supported in the simulator.");
        }

        // Import pumps
        if (!jsonString["simulation"].contains("pumps") || jsonString["simulation"]["pumps"].empty()) {
            throw std::invalid_argument("No pumps are defined. Please define at least 1 pump.");
        }
        for (auto& pump : jsonString["simulation"]["pumps"]) {
            if (pump.contains("channel") && pump.contains("type")) {
                int channelId = pump["channel"];
                if (pump["type"] == "PumpPressure") {
                    if (pump.contains("deltaP")) {
                        T pressure = pump["deltaP"];
                        network_->setPressurePump(channelId, pressure);
                    } else {
                        throw std::invalid_argument("Please set the pressure value 'deltaP' for Pressure pump in channel " + channelId);
                    }
                } else if (pump["type"] == "PumpFlowrate") {
                    if (pump.contains("flowRate")) {
                        T flowRate = pump["flowRate"];
                        network_->setPressurePump(channelId, flowRate);
                    } else {
                        throw std::invalid_argument("Please set the flow rate value 'flowRate' for Flowrate pump in channel " + channelId);
                    }
                } else {
                    throw std::invalid_argument("Invalid pump type. Please choose one of the following:\nPumpPressure\nPumpFlowrate");
                }
            } else {
                throw std::invalid_argument("Wrongly defined pump. Please provide following information for pumps:\nchannel\ntype");
            }
        }

        // Import Boundary Conditions and set Continuous phase
        if (jsonString["simulation"].contains("activeFixture")) {
            if (!jsonString["simulation"].contains("fixtures") || jsonString["simulation"]["fixtures"].size()-1 < jsonString["simulation"]["activeFixture"]) {
                throw std::invalid_argument("The active fixture does not exist.");
            }
            if (jsonString["simulation"]["fixtures"].contains("boundaryConditions")) {
                /** TODO
                 * Set new default values for pressure/flowrate pump
                */
            }
            if (jsonString["simulation"]["fixtures"].contains("phase")) {
                simulation.setContinuousPhase(jsonString["simulation"]["fixtures"]["phase"]);
            }
        }

        // Import resistance model
        sim::ResistanceModelPoiseuille<T>* resistanceModel = new sim::ResistanceModelPoiseuille<T>(simulation.getContinuousPhase()->getViscosity());
        simulation.setResistanceModel(resistanceModel);

        simulation.setNetwork(network_);

        return simulation;

    }

    template<typename T>
    void resultToJSON(std::string jsonFile) {
        
        std::ifstream f(jsonFile);
        json jsonString = json::parse(f);

    }

}   // namespace porting