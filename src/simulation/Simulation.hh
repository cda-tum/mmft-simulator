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

#include <Channel.h>
#include <FlowRatePump.h>
#include <lbmModule.h>
#include <Network.h>
#include <NodalAnalysis.h>
#include <Node.h>
#include <PressurePump.h>
#include <ResistanceModels.h>


namespace sim {

    template<typename T>
    Simulation<T>::Simulation() {}

    template<typename T>
    void Simulation<T>::setNetwork(arch::Network<T>* network_) {
        this->network = network_;
    }

    template<typename T>
    arch::Network<T>* Simulation<T>::getNetwork() {
        return this->network;
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(Fluid<T>* fluid_) {
        this->continuousPhase = fluid_;
    }

    template<typename T>
    void Simulation<T>::setResistanceModel(ResistanceModel2DPoiseuille<T>* model_) {
        this->resistanceModel = model_;
    }

    template<typename T>
    void Simulation<T>::simulate() {

        // initialize the simulation
        initialize();

        // compute nodal analysis
        std::cout << "[Simulation] Conduct initial nodal analysis..." << std::endl;
        nodal::conductNodalAnalysis(this->network);
        //printResults();

        if (network->getModules().size() > 0 ) {
            bool allConverged = false;
            bool pressureConverged = false;

            for (int iter = 0; iter < 1000; ++iter) {
                std::cout << "######################## Simulation Iteration no. " << iter << " ####################" << std::endl;

                // conduct CFD simulations
                std::cout << "[Simulation] Conduct CFD simulation " << iter <<"..." << std::endl;
                allConverged = conductCFDSimulation(this->network, iter);
                // compute nodal analysis again
                std::cout << "[Simulation] Conduct nodal analysis " << iter <<"..." << std::endl;
                pressureConverged = nodal::conductNodalAnalysis(this->network);

                if (pressureConverged) {
                    std::cout << "[Simulation] The pressures have converged." << std::endl;
                    break;
                }
            }
            
            //printResults();
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
        this->resistanceModel = new ResistanceModel2DPoiseuille(continuousPhase->getViscosity());

        // compute and set channel lengths
        std::cout << "[Simulation] Compute and set channel lengths..." << std::endl;
        for (auto& [key, channel] : network->getChannels()) {
            auto& nodeA = network->getNodes().at(channel->getNodeA());
            auto& nodeB = network->getNodes().at(channel->getNodeB());
            T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
            T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
            channel->setLength(sqrt(dx*dx + dy*dy));
        }       

        // compute channel resistances
        std::cout << "[Simulation] Compute and set channel resistances..." << std::endl;
        for (auto& [key, channel] : network->getChannels()) {
            T resistance = resistanceModel->getChannelResistance(channel.get());
            channel->setResistance(resistance);
        }

        // Prepare CFD geometry and lattice
        std::cout << "[Simulation] Prepare CFD geometry and lattice..." << std::endl;

        for (auto& [key, module] : network->getModules()) {
            module->lbmInit(continuousPhase->getViscosity(),
                            continuousPhase->getDensity());
            module->prepareGeometry();
            module->prepareLattice();
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
    }
}
