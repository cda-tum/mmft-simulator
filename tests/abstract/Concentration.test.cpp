#include "../src/baseSimulator.h"

#include "gtest/gtest.h"
#include "../test_definitions.h"

using T = double;

class InstantaneousMixing : public test::definitions::GlobalTest<T> {};
class DiffusiveMixing : public test::definitions::GlobalTest<T> {};

TEST_F(InstantaneousMixing, Case1) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network1.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case1.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case2) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network1.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case2.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case3) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network2.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case3.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case4) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network2.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case4.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case5) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network3.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case5.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case6) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network3.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case6.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case7) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network4.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case7.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case8) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network4.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case8.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case9) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network5.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case9.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case10) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network5.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case10.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case11) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network6.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case11.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

TEST_F(InstantaneousMixing, Case12) {
    // Define JSON files
    std::string networkFile = "../examples/Abstract/Mixing/Network6.JSON";
    std::string simFile = "../examples/Abstract/Mixing/Case12.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    auto sim = porting::simulationFromJSON<T>(simFile, network);

    // simulate
    sim->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = sim->getResults();

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

/** Diffusive mixing based on Case 1 from:
 *
 * Michel Takken, Maria Emmerich, and Robert Wille. "An Abstract Simulator for Species 
 * Concentrations in Channel-Based Microfluidic Devices." In: IEEE Transactions on 
 * Computer-Aided Design of Integrated Circuits and Systems (2025). pp. 3764 - 3775
 * DOI: 10.1109/TCAD.2025.3549703
 *
 * Employs operations 1 and 2
 */
TEST_F(DiffusiveMixing, case1) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 2000e-6;
    T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
    T flowRate1 = 2e-10;
    T flowRate2 = 1e-10;
    T pecletNr1 = (flowRate1 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
    T pecletNr2 = (flowRate2 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections;
    std::vector<sim::FlowSectionInput<T>> constantFlowSectionsA;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsA1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsA2;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSections.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSectionsA.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    constantFlowSectionsA.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});

    // perform analytical solution for constant input
    auto [fConstantA, segmentedResultConstantA, a_0_ConstantA] = diffusionMixingModelTest.getAnalyticalSolutionConstant(1e-3, cWidth, resolution, pecletNr1, constantFlowSectionsA);
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

    functionFlowSectionsA1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSectionsA2.push_back({0.0, 1.0, 2.0, 1.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunctionA1, segmentedResultFunctionA1, a_0_FunctionA1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr2, functionFlowSectionsA1, zeroFunction);
    auto [fFunctionA2, segmentedResultFunctionA2, a_0_FunctionA2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr2, functionFlowSectionsA2, zeroFunction);

    functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 2.0, 1.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);

    std::vector<T> results1 = {0.2256758334, -0.1400937832, 0.03671329178, -0.005362342632, 0.0005824036027, -4.876015006e-05, 2.926350265e-06};
    std::vector<T> results2 = {1.774324167, 0.1400937832, -0.03671329178, 0.005362342632, -0.0005824036027, 4.876015006e-05, -2.926350265e-06};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos-1), results2.at(pos), 1e-9);
    }

}


/** Diffusive mixing based on Case 2 from:
 *
 * Michel Takken, Maria Emmerich, and Robert Wille. "An Abstract Simulator for Species 
 * Concentrations in Channel-Based Microfluidic Devices." In: IEEE Transactions on 
 * Computer-Aided Design of Integrated Circuits and Systems (2025). pp. 3764 - 3775
 * DOI: 10.1109/TCAD.2025.3549703
 *
 * Employs operations 3 and 4
 */
TEST_F(DiffusiveMixing, case2) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 2000e-6;
    T cLength2 = sqrt(5e-6);
    // T cLength2 = 1e-6;
    T flowRate3 = 4e-10;
    T flowRate4 = 3.16267e-10;
    T flowRate5 = 4.41997e-11;
    T flowRate6 = 3.95334e-11;
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr5 = (flowRate5 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 50;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // top inflow channel (double the width)
    constantFlowSections.push_back({0.25, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections.push_back({0.0, 0.25, 1.0, 0.0, 0.5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel

    // perform analytical solution for constant input
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr3, constantFlowSections);

    // functionFlowSections3.push_back({0.0, 1.0, 11.1317, 0.901167*11.1317, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel
    // functionFlowSections2.push_back({0.0, 1.0, 9.04984, 0.790667*9.04984, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    // functionFlowSections1.push_back({0.0, 1.0, 0.5*2.52951, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel

    functionFlowSections3.push_back({0.0, 1.0, 1.0/0.1, 0.901167, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 1.0/0.11, 0.790667, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    functionFlowSections1.push_back({0.0, 1.0, 1.0/0.79, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel

    // perform analytical solution for function input
    auto [fFunctionC, segmentedResultFunctionC, a_0_FunctionC] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunctionD, segmentedResultFunctionD, a_0_FunctionD] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunctionE, segmentedResultFunctionE, a_0_FunctionE] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunction3, segmentedResultFunction3, a_0_Function3] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);

    std::vector<T> results1 = {1.118113781, -0.1359068368, 0.009947944843, -0.000483563169, 2.325828538e-05, -6.340169129e-07, 9.394702258e-09};
    std::vector<T> results2 = {1.716126457, -0.0002302030334, -2.830038486e-11, -7.691691388e-21, -3.750019126e-35, -2.567501434e-52, -2.698636767e-74};
    std::vector<T> results3 = {1.780169756, -2.258037236e-05, -3.057304139e-13, -1.00006154e-25, -6.159218653e-43, -5.828095089e-65, -8.89415173e-92};
    std::vector<T> resultsC = {1.118113781, -0.1926489363, 0.04016378307, -0.01117328855, 0.006179917418, -0.003892544795, 0.002677899154};
    std::vector<T> resultsD = {1.716126457, -0.02002779484, -0.001621364682, -0.002196446739, -0.0004040051962, -0.0007898834035, -0.0001794484603};
    std::vector<T> resultsE = {1.780169756, -0.005999769181, -0.001523885785, -0.0006601729367, -0.0003801725161, -0.000237472167, -0.0001688995523};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function3, results3.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionC, resultsC.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionD, resultsD.at(0), 1e-9);
    EXPECT_NEAR(a_0_FunctionE, resultsE.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos-1), results2.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction3.at(pos-1), results3.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionC.at(pos-1), resultsC.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionD.at(pos-1), resultsD.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunctionE.at(pos-1), resultsE.at(pos), 1e-9);
    }

}

/** Diffusive mixing based on Case 3 from:
 *
 * Michel Takken, Maria Emmerich, and Robert Wille. "An Abstract Simulator for Species 
 * Concentrations in Channel-Based Microfluidic Devices." In: IEEE Transactions on 
 * Computer-Aided Design of Integrated Circuits and Systems (2025). pp. 3764 - 3775
 * DOI: 10.1109/TCAD.2025.3549703
 *
 * Employs operation 5 twice
 */
TEST_F(DiffusiveMixing, case3) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = sqrt(1e-3*1e-3 + 1e-3*1e-3);
    [[maybe_unused]] T flowRate0 = 3.53553e-09;
    [[maybe_unused]] T flowRate1 = 3.53553e-09;
    [[maybe_unused]] T flowRate2 = 7.85674e-10;
    [[maybe_unused]] T flowRate3 = 6.28539e-09;
    [[maybe_unused]] T flowRate4 = 7.85674e-10;
    [[maybe_unused]] T flowRate5 = 6.28539e-09;
    [[maybe_unused]] T flowRate6 = 6.28539e-09;
    [[maybe_unused]] T flowRate7 = 7.85674e-10;
    T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections0;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    // The constant flow sections that enter and flow through channels 3 and 5
    constantFlowSections0.push_back({0.0, 0.4375, 1.0, 0.222222, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections0.push_back({0.4375, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, 2*cWidth, resolution, pecletNr3, constantFlowSections0);

    // The constant flow section of 0.0 concentration that enters through channels 2 and 4, this is ultimately just a function of 0.0 concentration
    constantFlowSections1.push_back({0.0, 1.0, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant1, segmentedResultConstant1, a_0_Constant1] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, cWidth, resolution, pecletNr2, constantFlowSections1);

    // The top 0.125 part of channel 5 enters channel 7
    functionFlowSections1.push_back({0.0, 1.0, 4.0, 0.875, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // channel 7

    // The flow from channel 4 and the bottom 0.875 part of channel 5 enter channel 6
    functionFlowSections2.push_back({0.0, 0.125, 1.0, 0.0, T(0.0), fConstant1, segmentedResultConstant1, a_0_Constant1}); // channel 6
    functionFlowSections2.push_back({0.125, 1.0, 1.0, -0.125, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0});
    
    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr7, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, 2*cWidth, resolution, pecletNr6, functionFlowSections2, zeroFunction);

    std::vector<T> results1 = {2.000000001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<T> results2 = {0.8749999999, -0.6039315827, 0.1066165206, 0.1307399654, -0.06604561768, -0.03076033584, 0.02954994509};

    EXPECT_NEAR(a_0_Function1, results1.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function2, results2.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results1.size(), results2.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction1.at(pos-1), results1.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction2.at(pos+23), results2.at(pos), 1e-9);
    }

}



/** Diffusive mixing based on Case 4 from:
 *
 * Michel Takken, Maria Emmerich, and Robert Wille. "An Abstract Simulator for Species 
 * Concentrations in Channel-Based Microfluidic Devices." In: IEEE Transactions on 
 * Computer-Aided Design of Integrated Circuits and Systems (2025). pp. 3764 - 3775
 * DOI: 10.1109/TCAD.2025.3549703
 *
 * Employs operations 1, 3 and 6
 */
TEST_F(DiffusiveMixing, case4) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    [[maybe_unused]] T cLength1 = 2e-3;
    [[maybe_unused]] T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
    [[maybe_unused]] T cLength3 = sqrt(2e-3*2e-3 + 2e-3*2e-3);
    [[maybe_unused]] T flowRate0 = 3.23735e-10;
    [[maybe_unused]] T flowRate1 = 3.23735e-10;
    [[maybe_unused]] T flowRate2 = 3.23735e-10;
    [[maybe_unused]] T flowRate3 = 3.23735e-10;
    [[maybe_unused]] T flowRate4 = 6.47446e-10;
    [[maybe_unused]] T flowRate5 = 6.47446e-10;
    [[maybe_unused]] T flowRate6 = 1.29489e-09;
    [[maybe_unused]] T flowRate7 = 1.5942e-09;
    [[maybe_unused]] T flowRate8 = 1.5942e-09;
    [[maybe_unused]] T flowRate9 = 1.89351e-09;
    [[maybe_unused]] T flowRate10 = 6.07277e-10;
    [[maybe_unused]] T flowRate11 = 6.78956e-10;
    [[maybe_unused]] T flowRate12 = 6.07277e-10;
    [[maybe_unused]] T pecletNr0 = (flowRate0 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr9 = (flowRate9 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr10 = (flowRate10 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    [[maybe_unused]] T pecletNr11 = (flowRate11 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // EXPECT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 25;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections4;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections5;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections6;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections7;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections8;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    // The constant flow sections that enter channel 4
    constantFlowSections4.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections4.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant4, segmentedResultConstant4, a_0_Constant4] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections4);

    // The constant flow sections that enter channel 5
    constantFlowSections5.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections5.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant5, segmentedResultConstant5, a_0_Constant5] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections5);

    // The function flow sections that enter channel 6
    functionFlowSections6.push_back({0.0, 0.5, 0.5, 0.0, T(0.0), fConstant4, segmentedResultConstant4, a_0_Constant4});
    functionFlowSections6.push_back({0.5, 1.0, 0.5, 0.0, T(0.0), fConstant5, segmentedResultConstant5, a_0_Constant5});
    auto [fFunction6, segmentedResultFunction6, a_0_Function6] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr6, functionFlowSections6, zeroFunction);

    // The constant flow sections that enter channel 9
    constantFlowSections9.push_back({0.0, 0.320715, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 10
    constantFlowSections9.push_back({0.320715, 0.679285, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 11
    constantFlowSections9.push_back({0.679285, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 12
    auto [fFunction9, segmentedResultFunction9, a_0_Function9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength1, cWidth, resolution, pecletNr9, constantFlowSections9);

    // The function flow sections that enter channel 7
    functionFlowSections7.push_back({0.0, 0.406126, 0.812252, 0.0, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6}); // channel 6
    functionFlowSections7.push_back({0.406126, 1.0, 1.18775, 0.5, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
    auto [fFunction7, segmentedResultFunction7, a_0_Function7] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections7, zeroFunction);

    // The function flow sections that enter channel 8
    functionFlowSections8.push_back({0.0, 0.593874, 1.18775, 0.0, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
    functionFlowSections8.push_back({0.593874, 1.0, 0.812252, 0.5, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6});
    auto [fFunction8, segmentedResultFunction8, a_0_Function8] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections8, zeroFunction);

    std::vector<T> results7 = {1.407907017, 0.245199986, 0.006914550309, -0.05244055709, -0.01749160537, 0.0004411017946, 0.0009288413936};
    std::vector<T> results8 = {1.283420113, 0.4586934067, 0.06845787687, -0.0117700664, 0.0003829626039, 3.990248299e-05, -0.0001994861841};

    EXPECT_NEAR(a_0_Function7, results7.at(0), 1e-9);
    EXPECT_NEAR(a_0_Function8, results8.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results7.size(), results8.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction7.at(pos-1), results7.at(pos), 1e-9);
        EXPECT_NEAR(segmentedResultFunction8.at(pos-1), results8.at(pos), 1e-9);
    }

}


/** 
 * Validation case against experimental results from 
 * Jeon, et. al., “Generation of solution and surface gradients using microfluidic systems,” 
 * Langmuir, vol. 16, no. 22, pp. 8311–8316, 2000.
 * 
 * Concentrations and flowrates are given for 1 mmm/s inlet flow rate
*/
TEST_F(DiffusiveMixing, ValidationCaseJeonEtAl_1mms) {

    // Channel configuration at network end

    //   0 1 2  3 4 5  6 7 8
    //   | | |  | | |  | | |
    //    \|/    \|/    \|/
    //     9      10     11
    //      \     |     /
    //        \   |   /
    //          \ | /
    //            12
    //            |


    // parameters
    [[maybe_unused]] auto cWidth = 50e-6;
    [[maybe_unused]] auto cHeight = 100e-6;
    [[maybe_unused]] auto cLength = 570e-10;  // Channel length

    // Concentrations (for Jeon et al. read out from 10mms simulations)
    T c0 = 0.0;
    T c1 = 0.2167;
    T c2 = 0.4115;
    T c3 = 0.5513;
    T c4 = 0.6021;
    T c5 = 0.5513;
    T c6 = 0.4115;
    T c7 = 0.2267;
    T c8 = 0.0;

    // Diffusion coefficient
    T diffusivity = 5e-10; // m^2/s


    // Flow rates (for Jeon et al. read out from 10mms simulations)
    [[maybe_unused]] T flowRate0 = 1.634586e-12; // channel 0
    [[maybe_unused]] T flowRate1 = 1.62243e-12; // channel 1
    [[maybe_unused]] T flowRate2 = 1.676514e-12; // channel 2

    [[maybe_unused]] T flowRate3 = 1.707595e-12; // channel 3
    [[maybe_unused]] T flowRate4 = 1.717750e-12; // channel 4
    [[maybe_unused]] T flowRate5 = 1.707595e-12; // channel 5

    [[maybe_unused]] T flowRate6 = 1.676514e-12; // channel 6
    [[maybe_unused]] T flowRate7 = 1.62243e-12; // channel 7
    [[maybe_unused]] T flowRate8 = 1.634586e-12; // channel 8

    [[maybe_unused]] T flowRate9 = 4.93353e-12; // channel 9
    [[maybe_unused]] T flowRate10 = 5.13294e-12; // channel 10
    [[maybe_unused]] T flowRate11 = 4.93353e-12; // channel 11

    [[maybe_unused]] T flowRate12 = 1.5e-11; // channel 12

    // Peclet numbers (flowrate / height) / diffusivity
    T pecletNr9 = (flowRate9 / cHeight) / diffusivity;
    T pecletNr10 = (flowRate10 / cHeight) / diffusivity;
    T pecletNr11 = (flowRate11 / cHeight) / diffusivity;
    T pecletNr12 = (flowRate12 / cHeight) / diffusivity;

    // Spectral resolution for Fourier approximation
    int resolution = 25;

     // Required definitions
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

    std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

    // Container for mixtures
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    // Dummy definitions with 0 values
    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    // Mixing model
    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // inflow into channel 9 (aq. to channel 96 in simulation)
    constantFlowSections9.push_back({0.0, 0.331322, 0.331322, 0.0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 0
    constantFlowSections9.push_back({0.331322, 0.660180, 0.328858, 0.0, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 1
    constantFlowSections9.push_back({0.66018, 1.0, 0.339820, 0.0, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 2
    // inflow into channel 10 (aq. to channel 97 in simulation)
    constantFlowSections10.push_back({0.0, 0.332674, 0.332674, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 3
    constantFlowSections10.push_back({0.332674, 0.667326, 0.334652, 0.0, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 4
    constantFlowSections10.push_back({0.667326, 1.0, 0.332674, 0.0, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 5
    // inflow into channel 11 (aq. to channel 98 in simulation)
    constantFlowSections11.push_back({0.0, 0.339820, 0.339820, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 6
    constantFlowSections11.push_back({0.339820, 0.6686780, 0.328858, 0.0, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 7
    constantFlowSections11.push_back({0.668678, 1.0, 0.331322, 0.0, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 8
    
    // Construct inflows for subsequent channels
    auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
    auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
    auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

    // inflow into channel 12
    functionFlowSections12.push_back({0.0, 0.328902, 0.328902, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9}); // bottom of channel 9
    functionFlowSections12.push_back({0.328902, 0.671098, 0.342196, 0.0, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10}); // bottom of channel 10
    functionFlowSections12.push_back({0.671098, 1.0,  0.328902, 0.0, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11}); // bottom of channel 11
        
    auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(500e-6, 18*cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

    std::vector<T> results12 = {0.6578955196, 0.0987088178, 0.0101340062, -0.0614661273, -0.0754240744, -0.0429397187, -0.0037155678};

    EXPECT_NEAR(a_0_Function12, results12.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results12.size(), results12.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction12.at(pos-1), results12.at(pos), 1e-9);
    }

}

/** 
 * Validation case against experimental results from 
 * Jeon, et. al., “Generation of solution and surface gradients using microfluidic systems,” 
 * Langmuir, vol. 16, no. 22, pp. 8311–8316, 2000.
 * 
 * Concentrations and flowrates are given for 10 mmm/s inlet flow rate
*/
TEST_F(DiffusiveMixing, ValidationCaseJeonEtAl_10mms) {

    // Channel configuration at network end

    //   0 1 2  3 4 5  6 7 8
    //   | | |  | | |  | | |
    //    \|/    \|/    \|/
    //     9      10     11
    //      \     |     /
    //        \   |   /
    //          \ | /
    //            12
    //            |


    // parameters
    [[maybe_unused]] auto cWidth = 50e-6;
    [[maybe_unused]] auto cHeight = 100e-6;
    [[maybe_unused]] auto cLength = 570e-10;  // Channel length

    // Concentrations (for Jeon et al. read out from 10mms simulations)
    T c0 = 0.0;
    T c1 = 0.1784;
    T c2 = 0.3991;
    T c3 = 0.5778;
    T c4 = 0.6459;
    T c5 = 0.5778;
    T c6 = 0.3991;
    T c7 = 0.1784;
    T c8 = 0.0;

    // Diffusion coefficient
    T diffusivity = 5e-10; // m^2/s

    // Flow rates (for Jeon et al. read out from 10mms simulations)
    [[maybe_unused]] T flowRate0 = 1.634586e-11; // channel 0
    [[maybe_unused]] T flowRate1 = 1.62243e-11; // channel 1
    [[maybe_unused]] T flowRate2 = 1.676514e-11; // channel 2

    [[maybe_unused]] T flowRate3 = 1.707595e-11; // channel 3
    [[maybe_unused]] T flowRate4 = 1.717750e-11; // channel 4
    [[maybe_unused]] T flowRate5 = 1.707595e-11; // channel 5

    [[maybe_unused]] T flowRate6 = 1.676514e-11; // channel 6
    [[maybe_unused]] T flowRate7 = 1.62243e-11; // channel 7
    [[maybe_unused]] T flowRate8 = 1.634586e-11; // channel 8

    [[maybe_unused]] T flowRate9 = 4.93353e-11; // channel 9
    [[maybe_unused]] T flowRate10 = 5.13294e-11; // channel 10
    [[maybe_unused]] T flowRate11 = 4.93353e-11; // channel 11

    [[maybe_unused]] T flowRate12 = 1.5e-10; // channel 12

    // Peclet numbers (flowrate / height) / diffusivity
    T pecletNr9 = (flowRate9 / cHeight) / diffusivity;
    T pecletNr10 = (flowRate10 / cHeight) / diffusivity;
    T pecletNr11 = (flowRate11 / cHeight) / diffusivity;
    T pecletNr12 = (flowRate12 / cHeight) / diffusivity;

    // Spectral resolution for Fourier approximation
    int resolution = 25;

    // Required definitions
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

    std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

    // Container for mixtures
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    // Dummy definitions with 0 values
    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    // Mixing model
    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // inflow into channel 9 (aq. to channel 96 in simulation)
    constantFlowSections9.push_back({0.0, 0.331322, 0.331322, 0.0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 0
    constantFlowSections9.push_back({0.331322, 0.660180, 0.328858, 0.0, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 1
    constantFlowSections9.push_back({0.66018, 1.0, 0.339820, 0.0, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 2
    // inflow into channel 10 (aq. to channel 97 in simulation)
    constantFlowSections10.push_back({0.0, 0.332674, 0.332674, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 3
    constantFlowSections10.push_back({0.332674, 0.667326, 0.334652, 0.0, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 4
    constantFlowSections10.push_back({0.667326, 1.0, 0.332674, 0.0, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 5
    // inflow into channel 11 (aq. to channel 98 in simulation)
    constantFlowSections11.push_back({0.0, 0.339820, 0.339820, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 6
    constantFlowSections11.push_back({0.339820, 0.6686780, 0.328858, 0.0, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 7
    constantFlowSections11.push_back({0.668678, 1.0, 0.331322, 0.0, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom of channel 8
    
    // Construct inflows for subsequent channels
    auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
    auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
    auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

    // inflow into channel 12
    functionFlowSections12.push_back({0.0, 0.328902, 0.328902, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9}); // bottom of channel 9
    functionFlowSections12.push_back({0.328902, 0.671098, 0.342196, 0.0, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10}); // bottom of channel 10
    functionFlowSections12.push_back({0.671098, 1.0,  0.328902, 0.0, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11}); // bottom of channel 11
        
    auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(500e-6, 18*cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

    std::vector<T> results12 = {0.6560466807, 0.0913713557, 0.00702043, -0.0693464955, -0.0923348244, -0.0579765294, -0.0011505482};

    EXPECT_NEAR(a_0_Function12, results12.at(0), 1e-9);

    for (size_t pos=1; pos<std::min(results12.size(), results12.size()); pos++) {
        EXPECT_NEAR(segmentedResultFunction12.at(pos-1), results12.at(pos), 1e-9);
    }
    
}
