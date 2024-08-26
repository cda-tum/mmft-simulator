#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(InstantaneousMixing, Case1) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network1.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case1.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 1:
     * 
     * State 0 - Pre-Injection
     * State 1 - Post-Injection
     * State 2 - Mixture 0 reaches node 4
     * State 3 - Mixture 1 reaches node 5
    */

    EXPECT_EQ(result->getStates().size(), 4);
    EXPECT_EQ(result->getMixtures().size(), 2);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.912023, 5e-7);

    EXPECT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().size(), 2);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);

    EXPECT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
}

TEST(InstantaneousMixing, Case2) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network1.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case2.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 2:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 reaches node 4 and creates Mixture 2
     * State 4 - Mixture 2 reaches node 5
     * State 5 - Mixture 1 reaches node 4 and creates Mixture 3
     * State 6 - Mixture 3 reaches node 5
    */

    EXPECT_EQ(result->getStates().size(), 7);
    EXPECT_EQ(result->getMixtures().size(), 4);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.912023, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 1.245356, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 1.412023, 5e-7);

    EXPECT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.670820, 5e-7);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 0.329180, 5e-7);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 2);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position2, 0.552786, 5e-7);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().mixtureId, 2);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().mixtureId, 3);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().mixtureId, 2);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().channel, 4);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().mixtureId, 3);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().channel, 4);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 1);

    EXPECT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
    EXPECT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) + 
        0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
    
}

TEST(InstantaneousMixing, Case3) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network2.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case3.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 3:
     * 
     * State 0 - Pre-Injection
     * State 1 - Post-Injection
     * State 2 - Mixture 0 reaches node 2
     * State 3 - Mixture 0 reaches nodes 3 and 4
    */

    EXPECT_EQ(result->getStates().size(), 4);
    EXPECT_EQ(result->getMixtures().size(), 1);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.666667, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 2.157379, 5e-7);

    EXPECT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

}

TEST(InstantaneousMixing, Case4) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network2.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case4.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 4:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Mixture 0 reaches node 2
     * State 3 - Mixture 0 reaches nodes 3 and 4
     * State 4 - Post-Injection 2
     * State 5 - Mixture 1 reaches node 2
     * State 6 - Mixture 1 reaches node 3 and 4
    */

    EXPECT_EQ(result->getStates().size(), 7);
    EXPECT_EQ(result->getMixtures().size(), 2);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.666667, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 2.157379, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 3.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 3.666667, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 5.157379, 5e-7);

    EXPECT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(1).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(1).back().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(1).back().channel, 1);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).back().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).back().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(1).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).back().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(2).back().channel, 2);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).back().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).back().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).back().mixtureId, 0);
    EXPECT_EQ(result->getStates().at(5)->getMixturePositions().at(3).back().channel, 3);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).back().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).back().position2, 1.0, 1e-12);

    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().size(), 3);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(1).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(1).front().channel, 1);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().channel, 2);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().mixtureId, 1);
    EXPECT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().channel, 3);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

}

TEST(InstantaneousMixing, Case5) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network3.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case5.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 5:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Mixture 0 reaches node 2
     * State 3 - Mixture 0 reaches node 4
     * State 4 - Mixture 0 reaches nodes 3 and 5
    */

    EXPECT_EQ(result->getStates().size(), 5);
    EXPECT_EQ(result->getMixtures().size(), 1);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 2.192570, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 2.657379, 5e-7);

}

TEST(InstantaneousMixing, Case6) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network3.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case6.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 6:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Mixture 0 reaches node 2
     * State 3 - Post-Injection 2
     * State 4 - Mixture 1 reaches node 2
     * State 5 - Mixture 0 reaches node 4
     * State 6 - Mixture 0 reaches node 3 and 5
     * State 7 - Mixture 1 reaches node 4
     * State 8 - Mixture 1 reaches node 3 and 5
    */

    EXPECT_EQ(result->getStates().size(), 9);
    EXPECT_EQ(result->getMixtures().size(), 2);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.500000, 1e-12);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.833333, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 2.192570, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 2.657379, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 2.692570, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 3.157379, 5e-7);

}

TEST(InstantaneousMixing, Case7) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network4.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case7.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 7:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 reaches node 4
     * State 4 - Mixture 0 reaches node 5 and 6
    */

    EXPECT_EQ(result->getStates().size(), 5);
    EXPECT_EQ(result->getMixtures().size(), 1);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);

}

TEST(InstantaneousMixing, Case8) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network4.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case8.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 8:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 and 1 reach node 4
     * State 4 - Mixture 3 reaches node 5 and 6
     * State 5 - Post-Injection 3
     * State 6 - Mixture 2 reaches node 4
     * State 7 - Mixture 4 reaches node 5 and 6
    */

    EXPECT_EQ(result->getStates().size(), 8);
    EXPECT_EQ(result->getMixtures().size(), 5);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 2.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 2.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 3.216761, 5e-7);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
    EXPECT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 2);
    EXPECT_EQ(result->getMixtures().at(4)->getSpecieConcentrations().size(), 2);

    EXPECT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) +
        0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
    EXPECT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(1), 
        0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(1), 1e-7);
    EXPECT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
    EXPECT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(1), 
        0.5*result->getMixtures().at(2)->getSpecieConcentrations().at(1), 1e-7);
    

}

TEST(InstantaneousMixing, Case9) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network5.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case9.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 9:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 reaches node 6
     * State 4 - Mixture 1 reaches node 7
     * State 5 - Mixture 0 reaches node 6
     * State 6 - Mixture 2 reaches node 7
     * State 7 - Post-Injection 3
     * State 8 - Mixture 0 reaches node 6
     * State 9 - Mixture 0 reaches node 7
    */

    EXPECT_EQ(result->getStates().size(), 10);
    EXPECT_EQ(result->getMixtures().size(), 3);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.856467, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 1.166667, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 1.277778, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 2.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 2.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(9)->getTime(), 2.856467, 5e-7);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);

    EXPECT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0),
        result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0, 1e-7);
    EXPECT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0),
        result->getMixtures().at(0)->getSpecieConcentrations().at(0)/1.5, 1e-7);

}

TEST(InstantaneousMixing, Case10) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network5.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case10.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 10:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 reaches node 6
     * State 4 - Mixture 3 reaches node 7
     * State 5 - Mixture 1 reaches node 6
     * State 6 - Mixture 4 reaches node 7
     * State 7 - Post-Injection 3
     * State 8 - Mixture 2 reaches node 6
     * State 9 - Mixture 5 reaches node 7
    */

    EXPECT_EQ(result->getStates().size(), 10);
    EXPECT_EQ(result->getMixtures().size(), 6);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.856467, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 1.166667, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 1.277778, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 2.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 2.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(9)->getTime(), 2.856467, 5e-7);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
    EXPECT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(4)->getSpecieConcentrations().size(), 2);
    EXPECT_EQ(result->getMixtures().at(5)->getSpecieConcentrations().size(), 2);

    EXPECT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
        result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0, 1e-7);
    EXPECT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(0), 
        result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0 +
        result->getMixtures().at(1)->getSpecieConcentrations().at(0)/3.0, 1e-7);
    EXPECT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(1), 
        result->getMixtures().at(1)->getSpecieConcentrations().at(1)/3.0, 1e-7);
    EXPECT_NEAR(result->getMixtures().at(5)->getSpecieConcentrations().at(0), 
        result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0 +
        result->getMixtures().at(1)->getSpecieConcentrations().at(0)/3.0, 1e-7);
    EXPECT_NEAR(result->getMixtures().at(5)->getSpecieConcentrations().at(1), 
        result->getMixtures().at(1)->getSpecieConcentrations().at(1)/3.0 +
        result->getMixtures().at(2)->getSpecieConcentrations().at(1)/3.0, 1e-7);

}

TEST(InstantaneousMixing, Case11) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network6.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case11.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 11:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Mixture 0 reaches node 4
     * State 3 - Mixture 1 reaches nodes 5 and 6
    */

    EXPECT_EQ(result->getStates().size(), 4);
    EXPECT_EQ(result->getMixtures().size(), 2);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 1.216761, 5e-7);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);

    EXPECT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
}

TEST(InstantaneousMixing, Case12) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network6.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case12.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();

    /**
     * Case 12:
     * 
     * State 0 - Pre-Injections
     * State 1 - Post-Injection 1
     * State 2 - Post-Injection 2
     * State 3 - Mixture 0 and 1 reach node 4
     * State 4 - Mixture 2 reaches nodes 5 and 6
    */

    EXPECT_EQ(result->getStates().size(), 5);
    EXPECT_EQ(result->getMixtures().size(), 3);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);

    EXPECT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
    EXPECT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
    EXPECT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 2);

    EXPECT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0), 
        0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) +
        0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
    EXPECT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(1), 
        0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(1), 1e-7);

}
