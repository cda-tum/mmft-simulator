#include "../../src/baseSimulator.h"
#include "../../src/baseSimulator.hh"

#include "gtest/gtest.h"

using T = double;

TEST(Results, allResultValues) {
    // define simulation
    sim::Simulation<T> testSimulation;

    // define network
    arch::Network<T> network;

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node3 = network.addNode(0.0, 0.0, false);
    auto node4 = network.addNode(0.0, 0.0, false);
    auto node5 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, false);

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node3->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network.addChannel(node5->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    //--- sink ---
    network.setSink(node0->getId());
    //--- ground ---
    network.setGround(node0->getId());

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3, 1.0);
    auto fluid1 = testSimulation.addFluid(3e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    // droplet
    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = testSimulation.addDroplet(fluid1->getId(), dropletVolume);
    testSimulation.addDropletInjection(droplet0->getId(), 0.0, c1->getId(), 0.5);

    // check if chip is valid
    network.isNetworkValid();
    testSimulation.setNetwork(&network);

    // simulate
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();

    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getId(), pump->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeA(), node0->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeB(), node1->getId());
    ASSERT_EQ(network.getFlowRatePumps().at(pump->getId())->getFlowRate(), flowRate);

    ASSERT_EQ(network.getChannels().at(c1->getId())->getId(), c1->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getNodeA(), node1->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getNodeB(), node2->getId());
    ASSERT_EQ(network.getChannels().at(c1->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c1->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c1->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c1->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getId(), c2->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getNodeA(), node2->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getNodeB(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c2->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c2->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getId(), c3->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getNodeA(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getNodeB(), node4->getId());
    ASSERT_EQ(network.getChannels().at(c3->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c3->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getId(), c4->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getNodeA(), node3->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getNodeB(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c4->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c4->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getId(), c5->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getNodeA(), node4->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getNodeB(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c5->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c5->getId())->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getId(), c6->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getNodeA(), node5->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getNodeB(), node0->getId());
    ASSERT_EQ(network.getChannels().at(c6->getId())->getWidth(), cWidth);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getHeight(), cHeight);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getLength(), cLength);
    ASSERT_EQ(network.getChannels().at(c6->getId())->getChannelType(), arch::ChannelType::NORMAL);

    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getId(), fluid0->getId());
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getName(), "");
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getViscosity(), 1e-3);
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getDensity(), 1e3);
    ASSERT_EQ(testSimulation.getFluid(fluid0->getId())->getConcentration(), 0.0);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getId(), fluid0->getId());
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getName(), "");
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getViscosity(), 3e-3);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getDensity(), 1e3);
    ASSERT_EQ(testSimulation.getFluid(fluid1->getId())->getConcentration(), 0.0);

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
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), 0.00000000003, 5e-17);
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

}
/*
TEST(Results, inverseDirectionChannels) {
    //--- API ---
    droplet::Simulator sim;

    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;
    int node3Id = 3;
    int node4Id = 4;

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = sim.addFlowRatePump(nodeGroundId, node0Id, flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = sim.addChannel(node1Id, node0Id, cHeight, cWidth, cLength);
    auto c2 = sim.addChannel(node2Id, node1Id, cHeight, cWidth, cLength);
    auto c3 = sim.addChannel(node3Id, node2Id, cHeight, cWidth, cLength);
    auto c4 = sim.addChannel(node4Id, node2Id, cHeight, cWidth, cLength);
    auto c5 = sim.addChannel(node4Id, node3Id, cHeight, cWidth, cLength);
    auto c6 = sim.addChannel(nodeGroundId, node4Id, cHeight, cWidth, cLength);

    //--- sink ---
    sim.addSink(-1);
    //--- ground ---
    sim.addGround(-1);

    // fluids
    auto fluid0 = sim.addFluid(1e-3, 1e3);
    auto fluid1 = sim.addFluid(3e-3, 1e3);
    //--- continuousPhase ---
    sim.setContinuousPhase(fluid0);

    // droplet
    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = sim.addDroplet(fluid1, dropletVolume, 0.0, c1, 0.5);

    // check if chip is valid
    sim.checkChipValidity();

    // simulate
    droplet::SimulationResult result = sim.simulate();

    ASSERT_NEAR(result.states.at(0).time, 0.000000, 5e-7);
    ASSERT_NEAR(result.states.at(1).time, 0.000000, 5e-7);
    ASSERT_NEAR(result.states.at(2).time, 0.033203, 5e-7);
    ASSERT_NEAR(result.states.at(3).time, 0.044922, 5e-7);
    ASSERT_NEAR(result.states.at(4).time, 0.111328, 5e-7);
    ASSERT_NEAR(result.states.at(5).time, 0.125391, 5e-7);
    ASSERT_NEAR(result.states.at(6).time, 0.239941, 5e-7);
    ASSERT_NEAR(result.states.at(7).time, 0.254778, 5e-7);
    ASSERT_NEAR(result.states.at(8).time, 0.321184, 5e-7);

    ASSERT_NEAR(result.states.at(0).pressures.at(0), 602.237537, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(1).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(2).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(3).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(4).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(5).pressures.at(0), 630.802163, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(1), 466.555562, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(2), 302.308961, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(3), 233.277781, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(6).pressures.at(0), 630.802163, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(1), 466.555562, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(2), 302.308961, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(3), 233.277781, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(7).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(2), 347.655305, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(3), 292.906438, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(4), 238.157571, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(8).pressures.at(0), 602.237537, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(0).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(1).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(2).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(3).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(4).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(5).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(3), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(4), -0.0000000000173913, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(5), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(6).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(3), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(4), -0.0000000000173913, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(5), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(7).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(6), -0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(8).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(2), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(4), -0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(6), -0.00000000003, 5e-17);

    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).headPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(1).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.425000, 5e-7);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).tailPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(1).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.575000, 5e-7);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).headPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(2).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).tailPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(2).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).headPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(3).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).tailPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(3).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).headPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(4).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).tailPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(4).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).headPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(5).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).tailPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(5).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).headPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(6).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).tailPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(6).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).headPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(7).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).tailPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(7).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    ASSERT_EQ(result.continuousPhaseId, fluid0);

    ASSERT_EQ(result.maximalAdaptiveTimeStep, 0);

    ASSERT_EQ(result.resistanceModel, 0);
}

TEST(Results, mixedDirectionChannels) {
    //--- API ---
    droplet::Simulator sim;

    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;
    int node3Id = 3;
    int node4Id = 4;

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = sim.addFlowRatePump(nodeGroundId, node0Id, flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = sim.addChannel(node1Id, node0Id, cHeight, cWidth, cLength);
    auto c2 = sim.addChannel(node1Id, node2Id, cHeight, cWidth, cLength);
    auto c3 = sim.addChannel(node3Id, node2Id, cHeight, cWidth, cLength);
    auto c4 = sim.addChannel(node2Id, node4Id, cHeight, cWidth, cLength);
    auto c5 = sim.addChannel(node4Id, node3Id, cHeight, cWidth, cLength);
    auto c6 = sim.addChannel(node4Id, nodeGroundId, cHeight, cWidth, cLength);

    //--- sink ---
    sim.addSink(-1);
    //--- ground ---
    sim.addGround(-1);

    // fluids
    auto fluid0 = sim.addFluid(1e-3, 1e3);
    auto fluid1 = sim.addFluid(3e-3, 1e3);
    //--- continuousPhase ---
    sim.setContinuousPhase(fluid0);

    // droplet
    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = sim.addDroplet(fluid1, dropletVolume, 0.0, c1, 0.5);

    // check if chip is valid
    sim.checkChipValidity();

    // simulate
    droplet::SimulationResult result = sim.simulate();

    ASSERT_NEAR(result.states.at(0).time, 0.000000, 5e-7);
    ASSERT_NEAR(result.states.at(1).time, 0.000000, 5e-7);
    ASSERT_NEAR(result.states.at(2).time, 0.033203, 5e-7);
    ASSERT_NEAR(result.states.at(3).time, 0.044922, 5e-7);
    ASSERT_NEAR(result.states.at(4).time, 0.111328, 5e-7);
    ASSERT_NEAR(result.states.at(5).time, 0.125391, 5e-7);
    ASSERT_NEAR(result.states.at(6).time, 0.239941, 5e-7);
    ASSERT_NEAR(result.states.at(7).time, 0.254778, 5e-7);
    ASSERT_NEAR(result.states.at(8).time, 0.321184, 5e-7);

    ASSERT_NEAR(result.states.at(0).pressures.at(0), 602.237537, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(0).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(1).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(1).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(2).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(2).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(3).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(3).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(4).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(4).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(5).pressures.at(0), 630.802163, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(1), 466.555562, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(2), 302.308961, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(3), 233.277781, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(5).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(6).pressures.at(0), 630.802163, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(1), 466.555562, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(2), 302.308961, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(3), 233.277781, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(6).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(7).pressures.at(0), 676.148507, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(1), 511.901906, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(2), 347.655305, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(3), 292.906438, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(4), 238.157571, 5e-7);
    ASSERT_NEAR(result.states.at(7).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(8).pressures.at(0), 602.237537, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(1), 437.990936, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(2), 273.744335, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(3), 218.995468, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(4), 164.246601, 5e-7);
    ASSERT_NEAR(result.states.at(8).pressures.at(-1), 0.000000, 5e-7);

    ASSERT_NEAR(result.states.at(0).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(0).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(1).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(1).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(2).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(2).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(3).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(3).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(4).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(4).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(5).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(3), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(4), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(5), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(5).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(6).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(3), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(4), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(5), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result.states.at(6).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(7).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(7).flowRates.at(6), 0.00000000003, 5e-17);

    ASSERT_NEAR(result.states.at(8).flowRates.at(0), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(1), -0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(2), 0.00000000003, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(3), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(4), 0.00000000002, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(5), -0.00000000001, 5e-17);
    ASSERT_NEAR(result.states.at(8).flowRates.at(6), 0.00000000003, 5e-17);

    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).headPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(1).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.425000, 5e-7);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).tailPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(1).dropletPositions.at(droplet0).tailPosition.position.position, 1 - 0.575000, 5e-7);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(1).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).headPosition.position.channelId, 1);
    // ASSERT_NEAR(result.states.at(2).dropletPositions.at(droplet0).headPosition.position.position, 1 - 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).headPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).tailPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(2).dropletPositions.at(droplet0).tailPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(2).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).headPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(3).dropletPositions.at(droplet0).headPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).headPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).tailPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(3).dropletPositions.at(droplet0).tailPosition.position.position, 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(3).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).headPosition.position.channelId, 2);
    // ASSERT_NEAR(result.states.at(4).dropletPositions.at(droplet0).headPosition.position.position, 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).headPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).tailPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(4).dropletPositions.at(droplet0).tailPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(4).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).headPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(5).dropletPositions.at(droplet0).headPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).headPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).tailPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(5).dropletPositions.at(droplet0).tailPosition.position.position, 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(5).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).headPosition.position.channelId, 4);
    // ASSERT_NEAR(result.states.at(6).dropletPositions.at(droplet0).headPosition.position.position, 0.850000, 5e-7);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).headPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).tailPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(6).dropletPositions.at(droplet0).tailPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(6).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).headPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(7).dropletPositions.at(droplet0).headPosition.position.position, 0.000000, 5e-7);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).headPosition.volumeTowards0, 0);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).tailPosition.position.channelId, 6);
    // ASSERT_NEAR(result.states.at(7).dropletPositions.at(droplet0).tailPosition.position.position, 0.150000, 5e-7);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).tailPosition.volumeTowards0, 1);
    // ASSERT_EQ(result.states.at(7).dropletPositions.at(droplet0).channelIds, std::vector<int>{});

    ASSERT_EQ(result.continuousPhaseId, fluid0);

    ASSERT_EQ(result.maximalAdaptiveTimeStep, 0);

    ASSERT_EQ(result.resistanceModel, 0);
}

TEST(Results, noSink1) {
    droplet::Simulator sim;

    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;

    auto flowRate = 3e-11;
    auto pump = sim.addFlowRatePump(nodeGroundId, node0Id, flowRate);

    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = sim.addChannel(node0Id, node1Id, cHeight, cWidth, cLength);
    auto c2 = sim.addChannel(node1Id, nodeGroundId, cHeight, cWidth, cLength);

    sim.addGround(nodeGroundId);

    auto fluid0 = sim.addFluid(1e-3, 1e3);
    auto fluid1 = sim.addFluid(3e-3, 1e3);
    sim.setContinuousPhase(fluid0);

    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = sim.addDroplet(fluid1, dropletVolume, 0.0, c1, 0.5);

    sim.checkChipValidity();

    droplet::SimulationResult result = sim.simulate();

    ASSERT_EQ(result.chip.name, "");
}

TEST(Results, noSink2) {
    droplet::Simulator sim;

    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;

    auto flowRate = 3e-11;
    auto pump = sim.addFlowRatePump(nodeGroundId, node0Id, flowRate);

    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = sim.addChannel(node0Id, node1Id, cHeight, cWidth, cLength);
    auto c2 = sim.addChannel(node1Id, nodeGroundId, cHeight, cWidth, cLength);

    sim.addGround(nodeGroundId);

    auto fluid0 = sim.addFluid(1e-3, 1e3);
    auto fluid1 = sim.addFluid(3e-3, 1e3);
    sim.setContinuousPhase(fluid0);

    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = sim.addDroplet(fluid1, dropletVolume, 0.1, c1, 0.5);

    sim.checkChipValidity();

    droplet::SimulationResult result = sim.simulate();

    ASSERT_EQ(result.chip.name, "");
}

TEST(Results, noSinkTwoDroplets) {
    droplet::Simulator sim;

    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;

    auto flowRate = 3e-11;
    auto pump = sim.addFlowRatePump(nodeGroundId, node0Id, flowRate);

    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = sim.addChannel(node0Id, node1Id, cHeight, cWidth, cLength);
    auto c2 = sim.addChannel(node1Id, nodeGroundId, cHeight, cWidth, cLength);

    sim.addGround(nodeGroundId);

    auto fluid0 = sim.addFluid(1e-3, 1e3);
    auto fluid1 = sim.addFluid(3e-3, 1e3);
    sim.setContinuousPhase(fluid0);

    auto dropletVolume = 1.5 * cWidth * cWidth * cHeight;
    auto droplet0 = sim.addDroplet(fluid1, dropletVolume, 0.0, c2, 0.5);
    auto droplet1 = sim.addDroplet(fluid1, dropletVolume, 0.0, c1, 0.5);

    sim.checkChipValidity();

    droplet::SimulationResult result = sim.simulate();

    ASSERT_EQ(result.chip.name, "");
}


TEST(Chip, triangleNetwork) {
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    
    // define network 1
    droplet::Simulator sim1;

    int c1 = sim1.addChannel(1, 2, cHeight, cWidth, cLength);
    int c2 = sim1.addChannel(2, 0, cHeight, cWidth, cLength);
    int v0 = sim1.addPressurePump(0, 1, 1000.0);

    sim1.addGround(0);
    auto fluid0 = sim1.addFluid(1e-3, 1e3);
    sim1.setContinuousPhase(fluid0);
    sim1.checkChipValidity();

    // define network 2
    droplet::Simulator sim2;

    int c3 = sim2.addChannel(2, 1, cHeight, cWidth, cLength);
    int c4 = sim2.addChannel(0, 2, cHeight, cWidth, cLength);
    int v1 = sim2.addPressurePump(0, 1, 1000.0);

    sim2.addGround(0);
    auto fluid1 = sim2.addFluid(1e-3, 1e3);
    sim2.setContinuousPhase(fluid1);
    sim2.checkChipValidity();

    // Simulate

    auto result1 = sim1.simulate();
    auto result2 = sim2.simulate();

    auto pressures1 = result1.getPressures();
    auto pressures2 = result2.getPressures();
    auto flowrates1 = result1.getFlowRates();
    auto flowrates2 = result2.getFlowRates();

    // Print results
    std::cout << "Node\tDiff" << std::endl;
    for (auto const& [key, pressure] : pressures1) {
        std::cout << key <<"\t" << pressure - pressures2[key] << std::endl;
    }
    std::cout << "\nChannel\tDiff" << std::endl;
    std::cout << 0 <<"\t" << flowrates1[0] + flowrates2[0] << std::endl;
    std::cout << 1 <<"\t" << flowrates1[1] + flowrates2[1] << std::endl;
    std::cout << 2 <<"\t" << flowrates1[2] - flowrates2[2] << std::endl;
}

TEST(Chip, Y_Network) {
    auto cWidth = 10e-6;
    auto cHeight = 3e-6;
    auto cLength = 100e-6;
    
    // define network 1
    droplet::Simulator sim1;

    int c1 = sim1.addChannel(1, 2, cHeight, cWidth, cLength);
    int c2 = sim1.addChannel(2, 3, cHeight, cWidth, cLength);
    int c3 = sim1.addChannel(2, 4, cHeight, cWidth, cLength);
    int v0 = sim1.addPressurePump(0, 1, 1000.0);
    int v1 = sim1.addPressurePump(5, 3, 1000.0);

    sim1.addGround(0);
    sim1.addGround(4);
    sim1.addGround(5);

    auto fluid0 = sim1.addFluid(1e-3, 1e3);
    sim1.setContinuousPhase(fluid0);
    sim1.checkChipValidity();

    // define network 2
    droplet::Simulator sim2;

    int c4 = sim2.addChannel(2, 1, cHeight, cWidth, cLength);
    int c5 = sim2.addChannel(3, 2, cHeight, cWidth, cLength);
    int c6 = sim2.addChannel(4, 2, cHeight, cWidth, cLength);
    int v2 = sim2.addPressurePump(0, 1, 1000.0);
    int v3 = sim2.addPressurePump(5, 3, 1000.0);

    sim2.addGround(0);
    sim2.addGround(4);
    sim2.addGround(5);

    auto fluid1 = sim2.addFluid(1e-3, 1e3);
    sim2.setContinuousPhase(fluid1);
    sim2.checkChipValidity();

    // Simulate

    auto result1 = sim1.simulate();
    auto result2 = sim2.simulate();

    auto pressures1 = result1.getPressures();
    auto pressures2 = result2.getPressures();
    auto flowrates1 = result1.getFlowRates();
    auto flowrates2 = result2.getFlowRates();

    // Print results
    std::cout << "Node\tDiff" << std::endl;
    for (auto const& [key, pressure] : pressures1) {
        std::cout << key <<"\t" << pressure - pressures2[key] << std::endl;
    }
    std::cout << "\nChannel\tDiff" << std::endl;
    std::cout << 0 <<"\t" << flowrates1[0] + flowrates2[0] << std::endl;
    std::cout << 1 <<"\t" << flowrates1[1] + flowrates2[1] << std::endl;
    std::cout << 2 <<"\t" << flowrates1[2] + flowrates2[2] << std::endl;
    std::cout << 3 <<"\t" << flowrates1[3] - flowrates2[3] << std::endl;
    std::cout << 4 <<"\t" << flowrates1[4] - flowrates2[4] << std::endl;
}
*/