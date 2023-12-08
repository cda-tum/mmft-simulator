#include "../../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;


TEST(Hybrid, testCase1a) {

    std::string file = "../examples/Hybrid/Network1a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    // Simulate
    testSimulation.simulate();

}

TEST(Hybrid, testCase2a) {

    std::string file = "../examples/Hybrid/Network2a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    // Simulate
    testSimulation.simulate();

}

TEST(Hybrid, testCase3a) {

    std::string file = "../examples/Hybrid/Network3a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    // Simulate
    testSimulation.simulate();

}

TEST(Hybrid, testCase4a) {

    std::string file = "../examples/Hybrid/Network4a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    // Simulate
    testSimulation.simulate();

}