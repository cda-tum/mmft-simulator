#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Continuous, allResultValues) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::Continuous);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(1e-3, 2e-3, false);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(1e-3, 0.0, false);
    auto node4 = network.addNode(2e-3, 2e-3, false);
    auto node5 = network.addNode(2e-3, 1e-3, false);
    auto node6 = network.addNode(2e-3, 0.0, false);
    auto node7 = network.addNode(3e-3, 1e-3, true);

    // pressure pump
    auto pressure = 1e3;
    auto pump0 = network.addPressurePump(node0->getId(), node1->getId(), pressure);
    auto pump1 = network.addPressurePump(node0->getId(), node2->getId(), pressure);
    auto pump2 = network.addPressurePump(node0->getId(), node3->getId(), pressure);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node6->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network.addChannel(node5->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 997.0, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

   // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel = sim::ResistanceModel1D<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();

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

TEST(Continuous, jsonDefinition) {
    std::string file = "../examples/Abstract/Continuous/Network1.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
   
    network.sortGroups();
    network.isNetworkValid();

    // Perform simulation and store results
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();
    
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

TEST(Continuous, triangleNetwork) {
    // define simulation 1
    sim::Simulation<T> testSimulation1;
    testSimulation1.setType(sim::Type::Abstract);
    testSimulation1.setPlatform(sim::Platform::Continuous);

    // define simulation 2
    sim::Simulation<T> testSimulation2;
    testSimulation2.setType(sim::Type::Abstract);
    testSimulation2.setPlatform(sim::Platform::Continuous);

    // define network 1
    arch::Network<T> network1;
    testSimulation1.setNetwork(&network1);

    // define network 2
    arch::Network<T> network2;
    testSimulation2.setNetwork(&network2);

    // nodes
    auto node11 = network1.addNode(0.0, 0.0, false);
    auto node21 = network1.addNode(1e-3, 0.0, false);
    auto node01 = network1.addNode(2e-3, 0.0, true);
    auto node12 = network2.addNode(0.0, 0.0, false);
    auto node22 = network2.addNode(1e-3, 0.0, false);
    auto node02 = network2.addNode(2e-3, 0.0, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c11 = network1.addChannel(node11->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c21 = network1.addChannel(node21->getId(), node01->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c12 = network2.addChannel(node22->getId(), node12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c22 = network2.addChannel(node02->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    // pressure pumps
    T pressure = 1000.0;
    auto pump1 = network1.addPressurePump(node01->getId(), node11->getId(), pressure);
    auto pump2 = network2.addPressurePump(node02->getId(), node12->getId(), pressure);

    // fluids
    auto fluid1 = testSimulation1.addFluid(1e-3, 1e3, 1.0);
    auto fluid2 = testSimulation2.addFluid(1e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation1.setContinuousPhase(fluid1->getId());
    testSimulation2.setContinuousPhase(fluid2->getId());

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel1 = sim::ResistanceModel1D<T>(testSimulation1.getContinuousPhase()->getViscosity());
    testSimulation1.setResistanceModel(&resistanceModel1);
    sim::ResistanceModel1D<T> resistanceModel2 = sim::ResistanceModel1D<T>(testSimulation2.getContinuousPhase()->getViscosity());
    testSimulation2.setResistanceModel(&resistanceModel2);

    // check if chips are valid
    network1.isNetworkValid();
    network2.isNetworkValid();
    network1.sortGroups();
    network2.sortGroups();

    // simulate
    testSimulation1.simulate();
    testSimulation2.simulate();

    // results
    result::SimulationResult<T>* result1 = testSimulation1.getSimulationResults();
    result::SimulationResult<T>* result2 = testSimulation2.getSimulationResults();

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

TEST(Continuous, Y_Network) {
    // define simulation 1
    sim::Simulation<T> testSimulation1;
    testSimulation1.setType(sim::Type::Abstract);
    testSimulation1.setPlatform(sim::Platform::Continuous);

    // define simulation 2
    sim::Simulation<T> testSimulation2;
    testSimulation2.setType(sim::Type::Abstract);
    testSimulation2.setPlatform(sim::Platform::BigDroplet);

    // define network 1
    arch::Network<T> network1;
    testSimulation1.setNetwork(&network1);

    // define network 2
    arch::Network<T> network2;
    testSimulation2.setNetwork(&network2);

    // nodes
    auto node11 = network1.addNode(0.0, 1e-3, false);
    auto node21 = network1.addNode(1e-3, 1e-3, false);
    auto node31 = network1.addNode(2e-3, 1e-3, false);
    auto node12 = network2.addNode(0.0, 1e-3, false);
    auto node22 = network2.addNode(1e-3, 1e-3, false);
    auto node32 = network2.addNode(2e-3, 1e-3, false);
    auto node01 = network1.addNode(0.0, 0.0, true);
    auto node41 = network1.addNode(2e-3, 2e-3, true);
    auto node51 = network1.addNode(2e-3, 0.0, true);
    auto node02 = network2.addNode(0.0, 0.0, true);
    auto node42 = network2.addNode(2e-3, 2e-3, true);
    auto node52 = network2.addNode(2e-3, 0.0, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c11 = network1.addChannel(node11->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c21 = network1.addChannel(node21->getId(), node31->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c31 = network1.addChannel(node21->getId(), node41->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c12 = network2.addChannel(node22->getId(), node12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c22 = network2.addChannel(node32->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c32 = network2.addChannel(node42->getId(), node22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // pressure pumps
    T pressure = 1000.0;
    auto pump11 = network1.addPressurePump(node01->getId(), node11->getId(), pressure);
    auto pump21 = network1.addPressurePump(node51->getId(), node31->getId(), pressure);
    auto pump12 = network2.addPressurePump(node02->getId(), node12->getId(), pressure);
    auto pump22 = network2.addPressurePump(node52->getId(), node32->getId(), pressure);

    // fluids
    auto fluid1 = testSimulation1.addFluid(1e-3, 1e3, 1.0);
    auto fluid2 = testSimulation2.addFluid(1e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation1.setContinuousPhase(fluid1->getId());
    testSimulation2.setContinuousPhase(fluid2->getId());

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel1 = sim::ResistanceModel1D<T>(testSimulation1.getContinuousPhase()->getViscosity());
    testSimulation1.setResistanceModel(&resistanceModel1);
    sim::ResistanceModel1D<T> resistanceModel2 = sim::ResistanceModel1D<T>(testSimulation2.getContinuousPhase()->getViscosity());
    testSimulation2.setResistanceModel(&resistanceModel2);

    // check if chips are valid
    network1.isNetworkValid();
    network2.isNetworkValid();
    network1.sortGroups();
    network2.sortGroups();

    // simulate
    testSimulation1.simulate();
    testSimulation2.simulate();

    // results
    result::SimulationResult<T>* result1 = testSimulation1.getSimulationResults();
    result::SimulationResult<T>* result2 = testSimulation2.getSimulationResults();

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

TEST(Continuous, Network2) {
    std::string file = "../examples/Abstract/Continuous/Network2.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
   
    network.sortGroups();
    network.isNetworkValid();

    // Perform simulation and store results
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();
    
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(3), result->getStates().at(0)->getFlowRates().at(4), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(4), result->getStates().at(0)->getFlowRates().at(5), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(5), result->getStates().at(0)->getFlowRates().at(6), 5e-10);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(9), result->getStates().at(0)->getFlowRates().at(10), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(10), result->getStates().at(0)->getFlowRates().at(11), 5e-10);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(12), result->getStates().at(0)->getFlowRates().at(13), 5e-10);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(13), result->getStates().at(0)->getFlowRates().at(14), 5e-10);
}