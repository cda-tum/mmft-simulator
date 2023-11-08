#include "../../src/baseSimulator.h"
#include "../../src/baseSimulator.hh"

#include "gtest/gtest.h"

using T = double;


TEST(Hybrid, testCase1a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network1a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));
    network->setPressurePump(1, T(1e3));
    network->setPressurePump(2, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

}

TEST(Hybrid, testCase2a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network2a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

}

TEST(Hybrid, testCase3a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network3a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

}

TEST(Hybrid, testCase4a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network4a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

}