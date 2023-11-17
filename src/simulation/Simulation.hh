#include "Simulation.h"

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <math.h>

#include <olb2D.h>
#include <olb2D.hh>

#include "ResistanceModels.h"

#include "../architecture/Channel.h"
#include "../architecture/FlowRatePump.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Network.h"
#include "../architecture/PressurePump.h"


namespace sim {

    template<typename T>
    Simulation<T>::Simulation() {}

    template<typename T>
    Fluid<T>* Simulation<T>::addFluid(T viscosity, T density, T concentration) {
        auto id = fluids.size();

        auto result = fluids.insert_or_assign(id, std::make_unique<Fluid<T>>(id, viscosity, density, concentration));

        return result.first->second.get();
    }

    template<typename T>
    Droplet<T>* Simulation<T>::addDroplet(int fluidId, T volume) {
        auto id = droplets.size();
        auto fluid = fluids.at(fluidId).get();

        auto result = droplets.insert_or_assign(id, std::make_unique<Droplet<T>>(id, volume, fluid));

        return result.first->second.get();
    }

    template<typename T>
    DropletInjection<T>* Simulation<T>::addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition) {
        auto id = dropletInjections.size();
        auto droplet = droplets.at(dropletId).get();
        auto channel = network->getChannel(channelId);

        // --- check if injection is valid ---
        // for the injection the head and tail of the droplet must lie inside the channel (the volume of the droplet must be small enough)
        // the droplet length is a relative value between 0 and 1
        T dropletLength = droplet->getVolume() / channel->getVolume();
        // channel must be able to fully contain the droplet
        if (dropletLength >= 1.0) {
            throw std::invalid_argument("Injection of droplet " + droplet->getName() + " into channel " + channel->getName() + " is not valid. Channel must be able to fully contain the droplet.");
        }
        // compute tail and head position of the droplet
        T tail = (injectionPosition - dropletLength / 2);
        T head = (injectionPosition + dropletLength / 2);
        // tail and head must not be outside the channel (can happen when the droplet is injected at the beginning or end of the channel)
        if (tail < 0 || head > 1.0) {
            throw std::invalid_argument("Injection of droplet " + droplet->getName() + " is not valid. Tail and head of the droplet must lie inside the channel " + channel->getName() + ". Consider to set the injection position in the middle of the channel.");
        }

        auto result = dropletInjections.insert_or_assign(id, std::make_unique<DropletInjection<T>>(id, droplet, injectionTime, channel, injectionPosition));

        return result.first->second.get();
    }

    template<typename T>
    void Simulation<T>::setPlatform(Platform platform_) {
        this->platform = platform_;
    }

    template<typename T>
    void Simulation<T>::setType(Type type_) {
        this->simType = type_;
    }

    template<typename T>
    void Simulation<T>::setNetwork(arch::Network<T>* network_) {
        this->network = network_;
    }

    template<typename T>
    void Simulation<T>::setFluids(std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids_) {
        this->fluids = std::move(fluids_);
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(int fluidId_) {
        this->continuousPhase = fluidId_;
    }

    template<typename T>
    void Simulation<T>::setResistanceModel(ResistanceModelPoiseuille<T>* model_) {
        this->resistanceModel = model_;
    }

    template<typename T>
    arch::Network<T>* Simulation<T>::getNetwork() {
        return this->network;
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getFluid(int fluidId) {
        return fluids.at(fluidId).get();
    }

    template<typename T>
    Droplet<T>* Simulation<T>::getDroplet(int dropletId) {
        return droplets.at(dropletId).get();
    }

    template<typename T>
    Droplet<T>* Simulation<T>::getDropletAtNode(int nodeId) {
        // loop through all droplets
        for (auto& [id, droplet] : droplets) {
            // do not consider droplets which are not inside the network
            if (droplet->getDropletState() != DropletState::NETWORK) {
                continue;
            }

            // do not consider droplets inside a single channel (because they cannot span over a node)
            if (droplet->isInsideSingleChannel()) {
                continue;
            }

            // check if a boundary is connected with the reference node and if yes then return the droplet immediately
            auto connectedBoundaries = droplet->getConnectedBoundaries(nodeId);
            if (connectedBoundaries.size() != 0) {
                return droplet.get();
            }

            // check if a fully occupied channel is connected with the reference node and if yes then return the droplet immediately
            auto connectedFullyOccupiedChannels = droplet->getConnectedFullyOccupiedChannels(nodeId);
            if (connectedFullyOccupiedChannels.size() != 0) {
                return droplet.get();
            }
        }

        // if nothing was found than return nullptr
        return nullptr;
    }

    template<typename T>
    DropletInjection<T>* Simulation<T>::getDropletInjection(int injectionId) {
        return dropletInjections.at(injectionId).get();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getContinuousPhase() {
        return fluids[continuousPhase].get();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::mixFluids(int fluid0Id, T volume0, int fluid1Id, T volume1) {
        // check if fluids are identically (no merging needed) and if they exist
        if (fluid0Id == fluid1Id) {
            // try to get the fluid (throws error if the fluid is not present)
            return fluids.at(fluid0Id).get();
        }

        // get fluids
        auto fluid0 = fluids.at(fluid0Id).get();
        auto fluid1 = fluids.at(fluid1Id).get();

        // compute ratios
        T volume = volume0 + volume1;
        T ratio0 = volume0 / volume;
        T ratio1 = volume1 / volume;

        // compute new fluid values
        T viscosity = ratio0 * fluid0->getViscosity() + ratio1 * fluid1->getViscosity();
        T density = ratio0 * fluid0->getDensity() + ratio1 * fluid1->getDensity();
        T concentration = ratio0 * fluid0->getConcentration() + ratio1 * fluid1->getConcentration();

        // add new fluid
        auto newFluid = addFluid(viscosity, density, concentration);

        //add previous fluids
        newFluid->addMixedFluid(fluid0);
        newFluid->addMixedFluid(fluid1);

        return newFluid;
    }

    template<typename T>
    Droplet<T>* Simulation<T>::mergeDroplets(int droplet0Id, int droplet1Id) {
        // check if droplets are identically (no merging needed) and if they exist
        if (droplet0Id == droplet1Id) {
            // try to get the droplet (throws error if the droplet is not present)
            return droplets.at(droplet0Id).get();
        }

        // get droplets
        auto droplet0 = getDroplet(droplet0Id);
        auto droplet1 = getDroplet(droplet1Id);

        // compute volumes
        T volume0 = droplet0->getVolume();
        T volume1 = droplet1->getVolume();
        T volume = volume0 + volume1;

        // merge fluids
        auto fluid = mixFluids(droplet0->getFluid()->getId(), volume0, droplet1->getFluid()->getId(), volume1);

        // add new droplet
        auto newDroplet = addDroplet(fluid->getId(), volume);

        //add previous droplets
        newDroplet->addMergedDroplet(droplet0);
        newDroplet->addMergedDroplet(droplet1);

        return newDroplet;
    }

    template<typename T>
    void Simulation<T>::simulate() {

        // initialize the simulation
        initialize();

        //printResults();

        if (network->getModules().size() > 0 ) {
            bool allConverged = false;
            bool pressureConverged = false;

            // Initialization of CFD domains
            while (! allConverged) {
                allConverged = conductCFDSimulation(this->network, 1);
            }

            while (! allConverged || !pressureConverged) {
                //std::cout << "######################## Simulation Iteration no. " << iter << " ####################" << std::endl;

                // conduct CFD simulations
                //std::cout << "[Simulation] Conduct CFD simulation " << iter <<"..." << std::endl;
                allConverged = conductCFDSimulation(this->network, 10);
            
                // compute nodal analysis again
                //std::cout << "[Simulation] Conduct nodal analysis " << iter <<"..." << std::endl;
                pressureConverged = nodal::conductNodalAnalysis(this->network);

            }
            
            #ifdef VERBOSE     
                if (pressureConverged && allConverged) {
                    std::cout << "[Simulation] All pressures have converged." << std::endl;
                } 
                printResults();
            #endif
        }
    }

    template<typename T>
    void Simulation<T>::printResults() {
        std::cout << "\n";
        // print the pressures in all nodes
        for (auto& [key, node] : network->getNodes()) {
            std::cout << "[Result] Node " << node->getId() << " has a pressure of " << node->getPressure() << " Pa.\n";
        }
        std::cout << "\n";
        // print the flow rates in all channels
        for (auto& [key, channel] : network->getChannels()) {
            std::cout << "[Result] Channel " << channel->getId() << " has a flow rate of " << channel->getFlowRate() << " m^3/s.\n";
        }
        /*
        std::cout << "\n";
        // print the resistances in all channels
        for (auto& [key, channel] : network->getChannels()) {
            std::cout << "[Result] Channel " << channel->getId() << " has a resistance of " << channel->getResistance() << " Pas/L.\n";
        }*/
        std::cout << std::endl;
    }

    template<typename T>
    void Simulation<T>::initialize() {
        // set resistance model
        this->resistanceModel = new ResistanceModelPoiseuille(fluids[continuousPhase]->getViscosity());

        // compute and set channel lengths
        #ifdef VERBOSE
            std::cout << "[Simulation] Compute and set channel lengths..." << std::endl;
        #endif
        for (auto& [key, channel] : network->getChannels()) {
            auto& nodeA = network->getNodes().at(channel->getNodeA());
            auto& nodeB = network->getNodes().at(channel->getNodeB());
            T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
            T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
            channel->setLength(sqrt(dx*dx + dy*dy));
        }       

        // compute channel resistances
        #ifdef VERBOSE
            std::cout << "[Simulation] Compute and set channel resistances..." << std::endl;
        #endif
        for (auto& [key, channel] : network->getChannels()) {
            T resistance = resistanceModel->getChannelResistance(channel.get());
            channel->setResistance(resistance);
        }

        for (auto& [key, module] : network->getModules()) {
            module->lbmInit(fluids[continuousPhase]->getViscosity(),
                            fluids[continuousPhase]->getDensity());
        }

        // TODO: this is boilerplate code, and can be done way more efficiently in a recursive manner
        for (auto& [modulekey, module] : network->getModules()) {
            for (auto& [key, channel] : module->getNetwork()->getChannels()) {
                //std::cout << "[Simulation] Channel " << channel->getId();
                auto& nodeA = network->getNodes().at(channel->getNodeA());
                auto& nodeB = network->getNodes().at(channel->getNodeB());
                //std::cout << " has nodes " << nodeA->getId() << " and " << nodeB->getId();
                T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
                T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
                channel->setLength(sqrt(dx*dx + dy*dy));
                //std::cout << " and a length of " << sqrt(dx*dx + dy*dy) <<std::endl;
            }
        }
        // TODO: Also boilerplate code that can be done more efficiently
        for (auto& [modulekey, module] : network->getModules()) {
            for (auto& [key, channel] : module->getNetwork()->getChannels()) {
                T resistance = resistanceModel->getChannelResistance(channel.get());
                channel->setResistance(resistance);
            }
        }

        // compute nodal analysis
        #ifdef VERBOSE
            std::cout << "[Simulation] Conduct initial nodal analysis..." << std::endl;
        #endif
        nodal::conductNodalAnalysis(this->network);

        // Prepare CFD geometry and lattice
        #ifdef VERBOSE
            std::cout << "[Simulation] Prepare CFD geometry and lattice..." << std::endl;
        #endif

        for (auto& [key, module] : network->getModules()) {
            module->prepareGeometry();
            module->prepareLattice();
        }
    }
}
