#include "jsonReaders.h"

namespace porting {

template<typename T>
void readNodes(json jsonString, arch::Network<T>& network) {
    for (auto& node : jsonString["network"]["nodes"]) {
        bool ground = false;
        if(node.contains("ground")) {
            ground = node["ground"];
        }
        auto addedNode = network.addNode(T(node["x"]), T(node["y"]), ground);
        if(node.contains("sink")) {
            if (node["sink"]) {
                network.setSink(addedNode->getId());
            }
        }
    }
}

template<typename T>
void readChannels(json jsonString, arch::Network<T>& network) {
    for (auto& channel : jsonString["network"]["channels"]) {
        arch::ChannelType type = arch::ChannelType::NORMAL;
        network.addChannel(channel["node1"], channel["node2"], channel["height"], channel["width"], type);
    }
}

template<typename T>
sim::Platform readPlatform(json jsonString, sim::Simulation<T>& simulation) {
    sim::Platform platform = sim::Platform::Continuous;
    if (!jsonString["simulation"].contains("platform")) {
        throw std::invalid_argument("Please define a platform. The following platforms are possible:\nContinuous\nBigDroplet\nMixing");
    }
    if (jsonString["simulation"]["platform"] == "Continuous") {
        platform = sim::Platform::Continuous;
    } else if (jsonString["simulation"]["platform"] == "BigDroplet") {
        platform = sim::Platform::BigDroplet;
    } else if (jsonString["simulation"]["platform"] == "Mixing") {
        platform = sim::Platform::Mixing;
    } else {
        throw std::invalid_argument("Platform is invalid. The following platforms are possible:\nContinuous\nBigDroplet\nMixing");
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
void readDropletInjections(json jsonString, sim::Simulation<T>& simulation, int activeFixture) {
    if (jsonString["simulation"]["fixtures"][activeFixture].contains("bigDropletInjections")) {
        for (auto& injection : jsonString["simulation"]["fixtures"][activeFixture]["bigDropletInjections"]) {
            int fluid = injection["fluid"];
            T volume = injection["volume"];
            auto newDroplet = simulation.addDroplet(fluid, volume);
            //int dropletId = injection["droplet"];
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
void readSimulators(json jsonString, arch::Network<T>* network) {
        std::string vtkFolder;
        if (!jsonString["simulation"]["settings"].contains("simulators") || jsonString["simulation"]["settings"]["simulators"].empty()) {
            throw std::invalid_argument("Hybrid simulation type was set, but no CFD simulators were defined.");
        }
        if (jsonString["simulation"]["settings"].contains("vtkFolder")) {
            vtkFolder = jsonString["simulation"]["settings"]["vtkFolder"];
        } else {
            vtkFolder = "./tmp/";
        }
        for (auto& module : jsonString["simulation"]["settings"]["simulators"]) {
            std::string name = module["name"];
            std::string stlFile = module["stlFile"];
            std::vector<T> position = { module["posX"], module["posY"] };
            std::vector<T> size = { module["sizeX"], module["sizeY"] };
            T charPhysLength = module["charPhysLength"];
            T charPhysVelocity = module["charPhysVelocity"];
            T alpha = module["alpha"];
            T resolution = module["resolution"];
            T epsilon = module["epsilon"];
            T tau = module["tau"];
            std::unordered_map<int, std::shared_ptr<arch::Node<T>>> Nodes;
            std::unordered_map<int, arch::Opening<T>> Openings;
            for (auto& opening : module["Openings"]) {
                int nodeId = opening["node"];
                Nodes.try_emplace(nodeId, network->getNode(nodeId));
                std::vector<T> normal = { opening["normal"]["x"], opening["normal"]["y"] };
                arch::Opening<T> opening_(network->getNode(nodeId), normal, opening["width"]);
                Openings.try_emplace(nodeId, opening_);
            }
            auto mod = network->addModule(name, stlFile, position, size, Nodes, Openings, charPhysLength, charPhysVelocity,
                                alpha, resolution, epsilon, tau);
            mod->setVtkFolder(vtkFolder);
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
        if (jsonString["simulation"]["resistanceModel"] == "1D") {
            resistanceModel = new sim::ResistanceModel1D<T>(simulation.getContinuousPhase()->getViscosity());
        } else if (jsonString["simulation"]["resistanceModel"] == "Poiseuille") {
            resistanceModel = new sim::ResistanceModelPoiseuille<T>(simulation.getContinuousPhase()->getViscosity());
        } else {
            throw std::invalid_argument("Invalid resistance model.");
        }
    } else {
        throw std::invalid_argument("No resistance model defined.");
    }
    simulation.setResistanceModel(resistanceModel);
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