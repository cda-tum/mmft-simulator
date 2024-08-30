#include "jsonReaders.h"

namespace porting {

template<typename T>
void readNodes(json jsonString, arch::Network<T>& network) {
    int nodeId = 0;
    int virtualNodes = 0;
    for (auto& node : jsonString["network"]["nodes"]) {
        
        if (node.contains("virtual") && node["virtual"]) {
            nodeId++;
            virtualNodes++;
            continue;
        } else {
            if (!node.contains("x") || !node.contains("y")) {
                throw std::invalid_argument("Node is ill-defined. Please define:\nx\ny");
            }
            bool ground = false;
            if(node.contains("ground")) {
                ground = node["ground"];
            }
            auto addedNode = network.addNode(nodeId, T(node["x"]), T(node["y"]), ground);
            if(node.contains("sink")) {
                if (node["sink"]) {
                    network.setSink(addedNode->getId());
                }
            }
            nodeId++;
        }
    }
    network.setVirtualNodes(virtualNodes);
}

template<typename T>
void readChannels(json jsonString, arch::Network<T>& network) {
    int channelId = 0;
    for (auto& channel : jsonString["network"]["channels"]) {
        if (channel.contains("virtual") && channel["virtual"]) {
            channelId++;
            continue;
        } else {
            if (!channel.contains("node1") || !channel.contains("node2") || !channel.contains("height") || !channel.contains("width")) {
                throw std::invalid_argument("Channel is ill-defined. Please define:\nnode1\nnode2\nheight\nwidth");
            }
            arch::ChannelType type = arch::ChannelType::NORMAL;
            network.addChannel(channel["node1"], channel["node2"], channel["height"], channel["width"], type, channelId);
            channelId++;
        }
    }
}

template<typename T>
void readModules(json jsonString, arch::Network<T>& network) {
    for (auto& module : jsonString["network"]["modules"]) {
        if (!module.contains("position") || !module.contains("size") || !module.contains("nodes")) {
            throw std::invalid_argument("Module is ill-defined. Please define:\nposition\nsize\nnodes");
        }
        std::vector<T> position = { module["position"][0], module["position"][1] };
        std::vector<T> size = { module["size"][0], module["size"][1] };
        std::unordered_map<int, std::shared_ptr<arch::Node<T>>> Nodes;
        for (auto& nodeId : module["nodes"]) {
            Nodes.try_emplace(nodeId, network.getNode(nodeId));
        }
        network.addModule(position, size, std::move(Nodes));
    }
}

template<typename T>
sim::Platform readPlatform(json jsonString, sim::Simulation<T>& simulation) {
    sim::Platform platform = sim::Platform::Continuous;
    if (!jsonString["simulation"].contains("platform")) {
        throw std::invalid_argument("Please define a platform. The following platforms are possible:\nContinuous\nBigDroplet\nMixing\nOoc");
    }
    if (jsonString["simulation"]["platform"] == "Continuous") {
        platform = sim::Platform::Continuous;
    } else if (jsonString["simulation"]["platform"] == "BigDroplet") {
        platform = sim::Platform::BigDroplet;
    } else if (jsonString["simulation"]["platform"] == "Mixing") {
        platform = sim::Platform::Mixing;
    } else if (jsonString["simulation"]["platform"] == "Ooc") {
        platform = sim::Platform::Ooc;
    } else {
        throw std::invalid_argument("Platform is invalid. The following platforms are possible:\nContinuous\nBigDroplet\nMixing\nOoc");
    }
    simulation.setPlatform(platform);
    return platform;
} 

template<typename T>
sim::Type readType(json jsonString, sim::Simulation<T>& simulation) {
    sim::Type simType = sim::Type::Abstract;
    if (!jsonString["simulation"].contains("type")) {
        throw std::invalid_argument("Please define a simulation type. The following types are possible:\nAbstract\nHybrid\nCFD");
    }
    if (jsonString["simulation"]["type"] == "Abstract") {
        simType = sim::Type::Abstract;
    } else if (jsonString["simulation"]["type"] == "Hybrid") {
        simType = sim::Type::Hybrid;
    } else if (jsonString["simulation"]["type"] == "CFD") {
        simType = sim::Type::CFD;
    } else {
        throw std::invalid_argument("Simulation type is invalid. The following types are possible:\nAbstract\nHybrid\nCFD");
    }
    simulation.setType(simType);
    return simType;
}

template<typename T>
void readFluids(json jsonString, sim::Simulation<T>& simulation) {
    if (!jsonString["simulation"].contains("fluids") || jsonString["simulation"]["fluids"].empty()) {
        throw std::invalid_argument("No fluids are defined. Please define at least 1 fluid.");
    }
    for (auto& fluid : jsonString["simulation"]["fluids"]) {
        if (fluid.contains("density") && fluid.contains("viscosity") && fluid.contains("concentration") && fluid.contains("name")) {
            T density = fluid["density"];
            T viscosity = fluid["viscosity"];
            T concentration = fluid["concentration"];
            sim::Fluid<T>* addedFluid = simulation.addFluid(viscosity, density, concentration);
            std::string name = fluid["name"];
            addedFluid->setName(name);
        } else {
            throw std::invalid_argument("Wrongly defined fluid. Please provide following information for fluids:\nname\ndensity\nviscosity\nconcentration");
        }
    }
}

template<typename T>
void readDroplets(json jsonString, sim::Simulation<T>& simulation) {
    for (auto& droplet : jsonString["simulation"]["droplets"]) {
        if (droplet.contains("fluid") && droplet.contains("volume")) {
            int fluid = droplet["fluid"];
            T volume = droplet["volume"];
            auto newDroplet = simulation.addDroplet(fluid, volume);
        } else {
            throw std::invalid_argument("Wrongly defined droplet. Please provide following information for droplets:\nfluid\nvolume");
        }
    }
}

template<typename T>
void readSpecies(json jsonString, sim::Simulation<T>& simulation) {
    for (auto& specie : jsonString["simulation"]["species"]) {
        if (specie.contains("diffusivity") && specie.contains("saturationConcentration")) {
            T diffusivity = specie["diffusivity"];
            T satConc = specie["saturationConcentration"];
            simulation.addSpecie(diffusivity, satConc);
        } else {
            throw std::invalid_argument("Wrongly defined specie. Please provide following information for species:\ndiffusivity\nsaturationConcentration");
        }
    }
}

template<typename T>
void readTissues(json jsonString, sim::Simulation<T>& simulation) {
    for (auto& tissue : jsonString["simulation"]["tissues"]) {
        if (tissue.contains("species") && tissue.contains("Vmax") && tissue.contains("kM")) {
            if (tissue["species"].size() == tissue["Vmax"].size() && tissue["species"].size() == tissue["kM"].size()) {
                int counter = 0;
                std::unordered_map<int, sim::Specie<T>*> species;
                std::unordered_map<int, T> Vmax;
                std::unordered_map<int, T> kM;
                for (auto& specieId : tissue["species"]) {
                    auto specie_ptr = simulation.getSpecie(specieId);
                    species.try_emplace(specieId, specie_ptr);
                    Vmax.try_emplace(specieId, tissue["Vmax"][counter]);
                    kM.try_emplace(specieId, tissue["kM"][counter]);
                    counter++;
                }
                simulation.addTissue(species, Vmax, kM);
            } else {
                throw std::invalid_argument("Please define a Vmax and kM value for each species.");
            }
        } else {
            throw std::invalid_argument("Wrongly defined specie. Please provide following information for species:\ndiffusivity\nsaturationConcentration");
        }
    }
}

template<typename T>
void readMixtures(json jsonString, sim::Simulation<T>& simulation) {
    for (auto& mixture : jsonString["simulation"]["mixtures"]) {
        if (mixture.contains("species") && mixture.contains("concentrations")) {
            if (mixture["species"].size() == mixture["concentrations"].size()) {
                int counter = 0;
                std::unordered_map<int, sim::Specie<T>*> species;
                std::unordered_map<int, T> concentrations;
                for (auto& specie : mixture["species"]) {
                    auto specie_ptr = simulation.getSpecie(specie);
                    species.try_emplace(specie, specie_ptr);
                    concentrations.try_emplace(specie, mixture["concentrations"][counter]);
                    counter++;
                }
                if (simulation.getMixingModel()->isInstantaneous()) {
                    simulation.addMixture(species, concentrations);
                } else if (simulation.getMixingModel()->isDiffusive()) {
                    simulation.addDiffusiveMixture(species, concentrations);
                } else {
                    throw std::invalid_argument("Please define a mixing model before adding mixtures.");
                }
            } else {
                throw std::invalid_argument("Wrongly defined mixture. Please provide as many concentrations as species.");
            }
        } else {
            throw std::invalid_argument("Wrongly defined mixture. Please provide species and corresponding concentrations in this mixture.");
        }
    }
}

template<typename T>
void readDropletInjections(json jsonString, sim::Simulation<T>& simulation, int activeFixture) {
    if (jsonString["simulation"]["fixtures"][activeFixture].contains("bigDropletInjections")) {
        for (auto& injection : jsonString["simulation"]["fixtures"][activeFixture]["bigDropletInjections"]) {
            int fluid = injection["fluid"];
            T volume = injection["volume"];
            auto newDroplet = simulation.addDroplet(fluid, volume);
            int channelId = injection["channel"];
            T injectionTime = injection["t0"];
            T injectionPosition = injection["pos"];
            simulation.addDropletInjection(newDroplet->getId(), injectionTime, channelId, injectionPosition);
        }
    } else {
        throw std::invalid_argument("Please define at least one droplet injection or choose a different platform.");
    }
}

template<typename T>
void readMixtureInjections(json jsonString, sim::Simulation<T>& simulation, int activeFixture) {
    if (jsonString["simulation"]["fixtures"][activeFixture].contains("mixtureInjections")) {
        for (auto& injection : jsonString["simulation"]["fixtures"][activeFixture]["mixtureInjections"]) {
            int mixtureId = injection["mixture"];
            int channelId = injection["channel"];
            T injectionTime = injection["t0"];
            simulation.addMixtureInjection(mixtureId, channelId, injectionTime);
        }
    } else {
        throw std::invalid_argument("Please define at least one mixture injection for the active fixture or choose a different platform.");
    }
}

template<typename T>
void readSimulators(json jsonString, sim::Simulation<T>& simulation, arch::Network<T>* network) {
        std::string vtkFolder;
        if (!jsonString["simulation"]["settings"].contains("simulators") || jsonString["simulation"]["settings"]["simulators"].empty()) {
            throw std::invalid_argument("Hybrid simulation type was set, but no CFD simulators were defined.");
        }
        if (jsonString["simulation"]["settings"].contains("vtkFolder")) {
            vtkFolder = jsonString["simulation"]["settings"]["vtkFolder"];
        } else {
            vtkFolder = "./tmp/";
        }
        for (auto& simulator : jsonString["simulation"]["settings"]["simulators"]) {
            std::string name = simulator["name"];
            std::string stlFile = simulator["stlFile"];
            T charPhysLength = simulator["charPhysLength"];
            T charPhysVelocity = simulator["charPhysVelocity"];
            T resolution = simulator["resolution"];
            T epsilon = simulator["epsilon"];
            T tau = simulator["tau"];
            int moduleId = simulator["moduleId"];
            std::unordered_map<int, arch::Opening<T>> Openings;
            for (auto& opening : simulator["Openings"]) {
                int nodeId = opening["node"];
                std::vector<T> normal = { opening["normal"]["x"], opening["normal"]["y"] };
                arch::Opening<T> opening_(network->getNode(nodeId), normal, opening["width"]);
                Openings.try_emplace(nodeId, opening_);
            }

            if(simulator["Type"] == "LBM")
            {
                auto simulator = simulation.addLbmSimulator(name, stlFile, network->getModule(moduleId), Openings, charPhysLength, 
                                                            charPhysVelocity, resolution, epsilon, tau);
                simulator->setVtkFolder(vtkFolder);
            }
            else if (simulator["Type"] == "Mixing")
            {
                std::unordered_map<int, sim::Specie<T>*> species;
                for (auto& [specieId, speciePtr] : simulation.getSpecies()) {
                    species.try_emplace(specieId, speciePtr.get());
                }
                auto simulator = simulation.addLbmMixingSimulator(name, stlFile, network->getModule(moduleId), species,
                                                            Openings, charPhysLength, charPhysVelocity, resolution, epsilon, tau);
                simulator->setVtkFolder(vtkFolder);
            }
            else if (simulator["Type"] == "Organ")
            {
                std::unordered_map<int, sim::Specie<T>*> species;
                for (auto& [specieId, speciePtr] : simulation.getSpecies()) {
                    species.try_emplace(specieId, speciePtr.get());
                }
                std::string organStlFile = simulator["organStlFile"];
                int tissueId = simulator["tissue"];
                auto simulator = simulation.addLbmOocSimulator(name, stlFile, tissueId, organStlFile, network->getModule(moduleId), species,
                                                            Openings, charPhysLength, charPhysVelocity, resolution, epsilon, tau);
                simulator->setVtkFolder(vtkFolder);
            }
            else if(simulator["Type"] == "ESS_LBM")
            {
                #ifdef USE_ESSLBM
                auto simulator = simulation.addEssLbmSimulator(name, stlFile, network->getModule(moduleId), Openings, charPhysLength, 
                                                            charPhysVelocity, resolution, epsilon, tau);
                simulator->setVtkFolder(vtkFolder);
                #else
                throw std::invalid_argument("The simulator was not build using the ESS library.");
                #endif
            }
        }
}

template<typename T>
void readUpdateScheme(json jsonString, sim::Simulation<T>& simulation) {

    /*  Legacy definition of update scheme for hybrid simulation
        Will be deprecated in next release. */
    if (!jsonString["simulation"].contains("updateScheme")) {
        T alpha = jsonString["simulation"]["settings"]["simulators"][0]["alpha"];
        simulation.setNaiveHybridScheme(alpha, 5*alpha, 10);
    } 
    else {
        if (jsonString["simulation"]["updateScheme"]["scheme"] == "Naive") {
            if (jsonString["simulation"]["updateScheme"]["scheme"].contains("alpha") && 
                jsonString["simulation"]["updateScheme"]["scheme"].contains("beta") &&
                jsonString["simulation"]["updateScheme"]["scheme"].contains("theta")) 
            {
                T alpha = jsonString["simulation"]["updateScheme"]["scheme"]["alpha"];
                T beta = jsonString["simulation"]["updateScheme"]["scheme"]["beta"];
                int theta = jsonString["simulation"]["updateScheme"]["scheme"]["theta"];
                simulation.setNaiveHybridScheme(alpha, beta, theta);
                return;
            } 
            else {
                for (auto& simulator : jsonString["simulation"]["settings"]["simulators"]) {
                    if (simulator.contains("alpha") && simulator.contains("beta") && simulator.contains("theta")) {
                        int moduleCounter = 0;
                        if (simulator["alpha"].is_number() && simulator["beta"].is_number()) {
                            T alpha = simulator["alpha"];
                            T beta = simulator["beta"];
                            int theta = simulator["theta"];
                            simulation.setNaiveHybridScheme(moduleCounter, alpha, beta, theta);
                        } else if (simulator["alpha"].is_array() && simulator["beta"].is_array()) {
                            int nodeCounter = 0;
                            std::unordered_map<int, T> alpha;
                            std::unordered_map<int, T> beta;
                            int theta = simulator["theta"];
                            for (auto& opening : simulator["Openings"]) {
                                alpha.try_emplace(opening["node"], simulator["alpha"][nodeCounter]);
                                alpha.try_emplace(opening["node"], simulator["beta"][nodeCounter]);
                                nodeCounter++;
                            }
                            simulation.setNaiveHybridScheme(moduleCounter, alpha, beta, theta);
                        }
                        moduleCounter++;
                    } else {
                        throw std::invalid_argument("alpha, beta or theta values are either not or ill-defined for Naive update scheme.");
                    }
                }
            }
        }
    }
}

template<typename T>
void readBoundaryConditions(json jsonString, sim::Simulation<T>& simulation, int activeFixture) {
    if (jsonString["simulation"]["fixtures"][activeFixture].contains("boundaryConditions")) {
        throw std::invalid_argument("Setting boundary condition values in fixture is not yets supported.");
    }
}

template<typename T>
void readContinuousPhase(json jsonString, sim::Simulation<T>& simulation, int activeFixture) {
    if (jsonString["simulation"]["fixtures"][activeFixture].contains("phase")) {
        simulation.setContinuousPhase(jsonString["simulation"]["fixtures"][activeFixture]["phase"]);
    } else {
        throw std::invalid_argument("Please set the continuous phase in the active fixture.");
    }
}

template<typename T>
void readPumps(json jsonString, arch::Network<T>* network) {
    if (!jsonString["simulation"].contains("pumps") || jsonString["simulation"]["pumps"].empty()) {
        throw std::invalid_argument("No pumps are defined. Please define at least 1 pump.");
    }
    for (auto& pump : jsonString["simulation"]["pumps"]) {
        if (pump.contains("channel") && pump.contains("type")) {
            int channelId = pump["channel"];
            if (pump["type"] == "PumpPressure") {
                if (pump.contains("deltaP")) {
                    T pressure = pump["deltaP"];
                    network->setPressurePump(channelId, pressure);
                } else {
                    throw std::invalid_argument("Please set the pressure value 'deltaP' for Pressure pump in channel " + channelId);
                }
            } else if (pump["type"] == "PumpFlowrate") {
                if (pump.contains("flowRate")) {
                    T flowRate = pump["flowRate"];
                    network->setFlowRatePump(channelId, flowRate);
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
}

template<typename T>
void readResistanceModel(json jsonString, sim::Simulation<T>& simulation) {
    sim::ResistanceModel<T>* resistanceModel; 
    if (jsonString["simulation"].contains("resistanceModel")) {
        if (jsonString["simulation"]["resistanceModel"] == "Rectangular") {
            resistanceModel = new sim::ResistanceModel1D<T>(simulation.getContinuousPhase()->getViscosity());
        } else if (jsonString["simulation"]["resistanceModel"] == "Poiseuille") {
            resistanceModel = new sim::ResistanceModelPoiseuille<T>(simulation.getContinuousPhase()->getViscosity());
        } else {
            throw std::invalid_argument("Invalid resistance model. Options are:\nRectangular\nPoiseuille");
        }
    } else {
        throw std::invalid_argument("No resistance model defined.");
    }
    simulation.setResistanceModel(resistanceModel);
}

template<typename T>
void readMixingModel(json jsonString, sim::Simulation<T>& simulation) {
    sim::MixingModel<T>* mixingModel;
    if (jsonString["simulation"].contains("mixingModel")) {
        if (jsonString["simulation"]["mixingModel"] == "Instantaneous") {
            mixingModel = new sim::InstantaneousMixingModel<T>();
        } else if (jsonString["simulation"]["mixingModel"] == "Diffusion") {
            mixingModel = new sim::DiffusionMixingModel<T>();
        } else {
            throw std::invalid_argument("Invalid mixing model. Options are:\nInstantaneous\nDiffusion");
        }
    } else {
        throw std::invalid_argument("No mixing model defined.");
    }
    simulation.setMixingModel(mixingModel);
}

template<typename T>
int readActiveFixture(json jsonString) {
    unsigned int activeFixture = 0;
    if (jsonString["simulation"].contains("activeFixture")) {
        activeFixture = jsonString["simulation"]["activeFixture"];
        if (!jsonString["simulation"].contains("fixtures") || jsonString["simulation"]["fixtures"].size()-1 < activeFixture) {
            throw std::invalid_argument("The active fixture does not exist.");
        }
    } else {
        throw std::invalid_argument("Please define an active fixture.");
    }
    return activeFixture;
}

}   // namespace porting
