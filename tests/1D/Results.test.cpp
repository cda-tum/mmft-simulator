#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Results, allResultValues) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node3 = network.addNode(2e-3, 0.0, false);
    auto node4 = network.addNode(2.5e-3, 0.86602540378e-3, false);
    auto node5 = network.addNode(3e-3, 0.0, false);
    auto node0 = network.addNode(4e-3, 0.0, false);

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
    network.sortGroups();

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
}

TEST(Results, inverseDirectionChannels) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node3 = network.addNode(2e-3, 0.0, false);
    auto node4 = network.addNode(2.5e-3, 0.86602540378e-3, false);
    auto node5 = network.addNode(3e-3, 0.0, false);
    auto node0 = network.addNode(4e-3, 0.0, false);

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node2->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node3->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node4->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node5->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node5->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network.addChannel(node0->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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
    network.sortGroups();

    // simulate
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();

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
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), -0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), -0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    ASSERT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
}

TEST(Results, mixedDirectionChannels) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node3 = network.addNode(2e-3, 0.0, false);
    auto node4 = network.addNode(2.5e-3, 0.86602540378e-3, false);
    auto node5 = network.addNode(3e-3, 0.0, false);
    auto node0 = network.addNode(4e-3, 0.0, false);

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node2->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node4->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node3->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node5->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
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
    network.sortGroups();

    // simulate
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();

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
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    ASSERT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    ASSERT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    ASSERT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
}

TEST(Results, noSink1) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);
    
    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}

TEST(Results, noSink2) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);
    
    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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
    testSimulation.addDropletInjection(droplet0->getId(), 0.1, c1->getId(), 0.5);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}

TEST(Results, noSinkTwoDroplets) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::_1D);
    testSimulation.setPlatform(sim::Platform::DROPLET);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);

    // flowRate pump
    auto flowRate = 3e-11;
    auto pump = network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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
    testSimulation.addDropletInjection(droplet0->getId(), 0.0, c2->getId(), 0.5);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}


TEST(Chip, triangleNetwork) {
    // define simulation 1
    sim::Simulation<T> testSimulation1;
    testSimulation1.setType(sim::Type::_1D);
    testSimulation1.setPlatform(sim::Platform::DROPLET);

    // define simulation 2
    sim::Simulation<T> testSimulation2;
    testSimulation2.setType(sim::Type::_1D);
    testSimulation2.setPlatform(sim::Platform::DROPLET);

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

    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node01->getId()),
                result2->getStates().at(0)->getPressures().at(node02->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node11->getId()),
                result2->getStates().at(0)->getPressures().at(node12->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node21->getId()),
                result2->getStates().at(0)->getPressures().at(node22->getId()), 1e-16);

    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(c11->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c12->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(c21->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c22->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump1->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump2->getId()), 1e-16);
}

TEST(Chip, Y_Network) {
    // define simulation 1
    sim::Simulation<T> testSimulation1;
    testSimulation1.setType(sim::Type::_1D);
    testSimulation1.setPlatform(sim::Platform::DROPLET);

    // define simulation 2
    sim::Simulation<T> testSimulation2;
    testSimulation2.setType(sim::Type::_1D);
    testSimulation2.setPlatform(sim::Platform::DROPLET);

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

    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node01->getId()),
                result2->getStates().at(0)->getPressures().at(node02->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node11->getId()),
                result2->getStates().at(0)->getPressures().at(node12->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node21->getId()),
                result2->getStates().at(0)->getPressures().at(node22->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node31->getId()),
                result2->getStates().at(0)->getPressures().at(node32->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node41->getId()),
                result2->getStates().at(0)->getPressures().at(node42->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getPressures().at(node51->getId()),
                result2->getStates().at(0)->getPressures().at(node52->getId()), 1e-16);
    
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(c11->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c12->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(c21->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c22->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(c31->getId()),
                -result2->getStates().at(0)->getFlowRates().at(c32->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump11->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump12->getId()), 1e-16);
    ASSERT_NEAR(result1->getStates().at(0)->getFlowRates().at(pump21->getId()),
                result2->getStates().at(0)->getFlowRates().at(pump22->getId()), 1e-16);

}
