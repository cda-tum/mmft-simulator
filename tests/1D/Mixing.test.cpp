#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Mixing, Network1) {
    // Define JSON files
    std::string networkFile = "../examples/1D/Mixing/Network1.JSON";
    std::string simFile_case1 = "../examples/1D/Mixing/Case1.JSON";
    std::string simFile_case2 = "../examples/1D/Mixing/Case2.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> case1 = porting::simulationFromJSON<T>(simFile_case1, &network);
    sim::Simulation<T> case2 = porting::simulationFromJSON<T>(simFile_case2, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    case1.simulate();
    case2.simulate();

    // results
    result::SimulationResult<T>* result1 = case1.getSimulationResults();
    result::SimulationResult<T>* result2 = case2.getSimulationResults();
/*
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getId(), pump->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeA(), node0->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeB(), node1->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getFlowRate(), flowRate);

    ASSERT_EQ(network.getChannels().at(c1->getId())->getId(), c1->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getNodeA(), node1->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getNodeB(), node2->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c1->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c1->getId())->getLength(), cLength, 1e-16);
    ASSERT_EQ(network.getChannels().at(c1->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getId(), c2->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getNodeA(), node2->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getNodeB(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c2->getId())->getLength(), cLength, 1e-16);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getId(), c3->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getNodeA(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getNodeB(), node4->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c3->getId())->getLength(), cLength, 1e-12);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getId(), c4->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getNodeA(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getNodeB(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c4->getId())->getLength(), cLength, 1e-16);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getId(), c5->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getNodeA(), node4->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getNodeB(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c5->getId())->getLength(), cLength, 1e-12);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getId(), c6->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getNodeA(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getNodeB(), node0->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getHeight(), cHeight);
    ASSERT_NEAR(network.getChannels().at(c6->getId())->getLength(), cLength, 1e-16);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getChannelType(), arch::ChannelType::NORMAL);

    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getId(), fluid0->getId());
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getName(), "");
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getViscosity(), 1e-3);
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getDensity(), 1e3);
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getConcentration(), 1.0);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getId(), fluid1->getId());
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getName(), "");
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getViscosity(), 3e-3);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getDensity(), 1e3);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getConcentration(), 1.0);

    ASSERT_EQ(testSimulation.getDroplet(droplet0->getId())->getId(), droplet0->getId());
    ASSERT_EQ(testSimulation.getDroplet(droplet0->getId())->getName(), "");
    ASSERT_EQ(testSimulation.getDroplet(droplet0->getId())->getVolume(), dropletVolume);

    ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.033203, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.044922, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.111328, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getTime(), 0.125391, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getTime(), 0.239941, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getTime(), 0.254778, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getTime(), 0.321184, 5e-7);

    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(0)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(1)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(2)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(3)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(4)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(5)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(6)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node3->getId()), 347.655305, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node4->getId()), 292.906438, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node5->getId()), 238.157571, 5e-7);
    ASSERT_NEAR(result->getStates().at(7)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    ASSERT_NEAR(result->getStates().at(8)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), 0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), 0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), 0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), 0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
    */
}

