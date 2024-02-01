#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Mixing, Case1) {
    // Define JSON files
    std::string networkFile = "../examples/1D/Mixing/Network1.JSON";
    std::string simFile = "../examples/1D/Mixing/Case1.JSON";

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
     * State 3 - Mixture 0 reaches node 5
    */

    ASSERT_EQ(result->getStates().size(), 4);

    ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.745356, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.912023, 5e-7);

    ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 2);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);
}

TEST(Mixing, Case2) {
    // Define JSON files
    std::string networkFile = "../examples/1D/Mixing/Network1.JSON";
    std::string simFile = "../examples/1D/Mixing/Case2.JSON";

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

    ASSERT_EQ(result->getStates().size(), 7);

    ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
    ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.912023, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getTime(), 1.245356, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getTime(), 1.412023, 5e-7);

    ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.670820, 5e-7);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 0.329180, 5e-7);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 2);
    ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().size(), 3);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().channel, 3);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position2, 0.552786, 5e-7);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().mixtureId, 2);
    ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().size(), 3);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().channel, 3);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().mixtureId, 3);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().mixtureId, 2);
    ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().channel, 4);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position2, 1.0, 1e-12);

    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().size(), 3);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().mixtureId, 0);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().channel, 2);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().mixtureId, 1);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().channel, 3);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().mixtureId, 3);
    ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().channel, 4);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
    ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);
    
}

