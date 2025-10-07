#include "../src/baseSimulator.h"

#include "gtest/gtest.h"
#include "../test_definitions.h"

using T = double;

class Continuous : public test::definitions::GlobalTest<T> {};

TEST_F(Continuous, allResultValues) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(1e-3, 2e-3, false);
    auto node2 = network->addNode(1e-3, 1e-3, false);
    auto node3 = network->addNode(1e-3, 0.0, false);
    auto node4 = network->addNode(2e-3, 2e-3, false);
    auto node5 = network->addNode(2e-3, 1e-3, false);
    auto node6 = network->addNode(2e-3, 0.0, false);
    auto node7 = network->addNode(3e-3, 1e-3, true);

    // pressure pump
    auto pressure = 1e3;
    auto pump0 = network->addPressurePump(node0->getId(), node1->getId(), pressure);
    auto pump1 = network->addPressurePump(node0->getId(), node2->getId(), pressure);
    auto pump2 = network->addPressurePump(node0->getId(), node3->getId(), pressure);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c1 = network->addRectangularChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network->addRectangularChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network->addRectangularChannel(node6->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network->addRectangularChannel(node5->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    sim::AbstractContinuous<T> testSimulation(network);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 997.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    // Set the resistance model
    testSimulation.set1DResistanceModel();

    // simulate
    testSimulation.simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = testSimulation.getResults();

    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node0->getId()), 0.0, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node1->getId()), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node2->getId()), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node3->getId()), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node4->getId()), 833.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node5->getId()), 666.666667, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node6->getId()), 833.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node7->getId()), 0.0, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump0->getId()), -5.89653042e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump1->getId()), -1.17933205e-09, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump2->getId()), -5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), 1.17935801e-09, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), 2.35871603e-09, 5e-17);

}

TEST_F(Continuous, jsonDefinition) {
    std::string file = "../examples/Abstract/Continuous/Network1.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    auto testSimulation = porting::simulationFromJSON<T>(file, network);

    // Perform simulation and store results
    testSimulation->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = testSimulation->getResults();
    
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(0), 0.0, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(1), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(2), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(3), 1000.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(4), 833.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(5), 666.666667, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(6), 833.333333, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(7), 0.0, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(0), -5.89653042e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(1), -1.17933205e-09, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(2), -5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(3), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(4), 1.17935801e-09, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(5), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(6), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(7), 5.89679007e-10, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(8), 2.35871603e-09, 5e-17);

}

TEST_F(Continuous, triangleNetwork) {
    // define network 1
    auto network1 = arch::Network<T>::createNetwork();

    // define network 2
    auto network2 = arch::Network<T>::createNetwork();

    // define simulation 1
    sim::AbstractContinuous<T> testSimulation1(network1);

    // define simulation 2
    sim::AbstractContinuous<T> testSimulation2(network2);

    // nodes
    auto node11 = network1->addNode(0.0, 0.0, false);
    auto node21 = network1->addNode(1e-3, 0.0, false);
    auto node01 = network1->addNode(2e-3, 0.0, true);
    auto node12 = network2->addNode(0.0, 0.0, false);
    auto node22 = network2->addNode(1e-3, 0.0, false);
    auto node02 = network2->addNode(2e-3, 0.0, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c11 = network1->addRectangularChannel(node11->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c21 = network1->addRectangularChannel(node21->getId(), node01->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c12 = network2->addRectangularChannel(node22->getId(), node12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c22 = network2->addRectangularChannel(node02->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    // pressure pumps
    T pressure = 1000.0;
    auto pump1 = network1->addPressurePump(node01->getId(), node11->getId(), pressure);
    auto pump2 = network2->addPressurePump(node02->getId(), node12->getId(), pressure);

    // fluids
    auto fluid1 = testSimulation1.addFluid(1e-3, 1e3);
    auto fluid2 = testSimulation2.addFluid(1e-3, 1e3);
    //--- continuousPhase ---
    testSimulation1.setContinuousPhase(fluid1->getId());
    testSimulation2.setContinuousPhase(fluid2->getId());

    // Set the resistance model
    testSimulation1.set1DResistanceModel();
    testSimulation2.set1DResistanceModel();

    // simulate
    testSimulation1.simulate();
    testSimulation2.simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result1 = testSimulation1.getResults();
    const std::shared_ptr<result::SimulationResult<T>> result2 = testSimulation2.getResults();

    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node01->getId()),
                result2->getStates().at(0)->getPressures().at(node02->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node11->getId()),
                result2->getStates().at(0)->getPressures().at(node12->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node21->getId()),
                result2->getStates().at(0)->getPressures().at(node22->getId()), 1e-16);

    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(c11->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c12->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(c21->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c22->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump1->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump2->getId()), 1e-16);
}

TEST_F(Continuous, Y_Network) {
    // define network 1
    auto network1 = arch::Network<T>::createNetwork();

    // define network 2
    auto network2 = arch::Network<T>::createNetwork();

    // define simulation 1
    sim::AbstractContinuous<T> testSimulation1(network1);

    // define simulation 2
    sim::AbstractContinuous<T> testSimulation2(network2);

    // nodes
    auto node11 = network1->addNode(0.0, 1e-3, false);
    auto node21 = network1->addNode(1e-3, 1e-3, false);
    auto node31 = network1->addNode(2e-3, 1e-3, false);
    auto node12 = network2->addNode(0.0, 1e-3, false);
    auto node22 = network2->addNode(1e-3, 1e-3, false);
    auto node32 = network2->addNode(2e-3, 1e-3, false);
    auto node01 = network1->addNode(0.0, 0.0, true);
    auto node41 = network1->addNode(2e-3, 2e-3, true);
    auto node51 = network1->addNode(2e-3, 0.0, true);
    auto node02 = network2->addNode(0.0, 0.0, true);
    auto node42 = network2->addNode(2e-3, 2e-3, true);
    auto node52 = network2->addNode(2e-3, 0.0, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 0; // automatically calculate channel length

    auto c11 = network1->addRectangularChannel(node11->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c21 = network1->addRectangularChannel(node21->getId(), node31->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c31 = network1->addRectangularChannel(node21->getId(), node41->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c12 = network2->addRectangularChannel(node22->getId(), node12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c22 = network2->addRectangularChannel(node32->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c32 = network2->addRectangularChannel(node42->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // pressure pumps
    T pressure = 1000.0;
    auto pump11 = network1->addPressurePump(node01->getId(), node11->getId(), pressure);
    auto pump21 = network1->addPressurePump(node51->getId(), node31->getId(), pressure);
    auto pump12 = network2->addPressurePump(node02->getId(), node12->getId(), pressure);
    auto pump22 = network2->addPressurePump(node52->getId(), node32->getId(), pressure);

    // fluids
    auto fluid1 = testSimulation1.addFluid(1e-3, 1e3);
    auto fluid2 = testSimulation2.addFluid(1e-3, 1e3);
    //--- continuousPhase ---
    testSimulation1.setContinuousPhase(fluid1->getId());
    testSimulation2.setContinuousPhase(fluid2->getId());

    // Set the resistance model
    testSimulation1.set1DResistanceModel();
    testSimulation2.set1DResistanceModel();

    // simulate
    testSimulation1.simulate();
    testSimulation2.simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result1 = testSimulation1.getResults();
    const std::shared_ptr<result::SimulationResult<T>> result2 = testSimulation2.getResults();

    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node01->getId()),
                result2->getStates().at(0)->getPressures().at(node02->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node11->getId()),
                result2->getStates().at(0)->getPressures().at(node12->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node21->getId()),
                result2->getStates().at(0)->getPressures().at(node22->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node31->getId()),
                result2->getStates().at(0)->getPressures().at(node32->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node41->getId()),
                result2->getStates().at(0)->getPressures().at(node42->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getPressures().at(node51->getId()),
                result2->getStates().at(0)->getPressures().at(node52->getId()), 1e-16);
    
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(c11->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c12->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(c21->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c22->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(c31->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c32->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump11->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump12->getId()), 1e-16);
    EXPECT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump21->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump22->getId()), 1e-16);

}

TEST_F(Continuous, Network2) {
    std::string file = "../examples/Abstract/Continuous/Network2.JSON";

    // Load and set the network from a JSON file
    auto network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    auto testSimulation = porting::simulationFromJSON<T>(file, network);

    // Perform simulation and store results
    testSimulation->simulate();

    // results
    const std::shared_ptr<result::SimulationResult<T>> result = testSimulation->getResults();
    
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(3), result->getStates().at(0)->getFlowRates().at(4), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(4), result->getStates().at(0)->getFlowRates().at(5), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(5), result->getStates().at(0)->getFlowRates().at(6), 5e-10);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(9), result->getStates().at(0)->getFlowRates().at(10), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(10), result->getStates().at(0)->getFlowRates().at(11), 5e-10);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(12), result->getStates().at(0)->getFlowRates().at(13), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(13), result->getStates().at(0)->getFlowRates().at(14), 5e-10);
}

/**
 * Test ideas:
 * 
 * Add 3 fluids -> simulate -> remove 1 fluid -> simulate -> add 1 fluid -> simulate
 * 
 * Add 2 fluids -> simulate -> change density1, viscosity2, name2 -> simulate
 * 
 * Add 3 fluids -> simulate -> create and set new network -> simulate -> change resistanceModel -> simulate 
 * -> set new continuous phase -> simulate -> try to delete continuous phase (expect error) -> delete other fluid -> simulate
 * 
 * Set a network with modules (expect error, inconsistent network type)
 * 
 * I can change a fluid after it was removed, but it doesn't affect simulation result.
 * 
 * Play with different orders of simulation definition: E.g., define resistance model before adding first fluid
 * Does the resistance model update with new continuous phase?
 */