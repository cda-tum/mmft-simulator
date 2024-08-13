#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(BigDroplet, allResultValues) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

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

    EXPECT_EQ(network.getFlowRatePumps().at(pump->getId())->getId(), pump->getId());
    EXPECT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeA(), node0->getId());
    EXPECT_EQ(network.getFlowRatePumps().at(pump->getId())->getNodeB(), node1->getId());
    EXPECT_EQ(network.getFlowRatePumps().at(pump->getId())->getFlowRate(), flowRate);

    EXPECT_EQ(network.getChannels().at(c1->getId())->getId(), c1->getId());
    EXPECT_EQ(network.getChannels().at(c1->getId())->getNodeA(), node1->getId());
    EXPECT_EQ(network.getChannels().at(c1->getId())->getNodeB(), node2->getId());
    EXPECT_EQ(network.getChannels().at(c1->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c1->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c1->getId())->getLength(), cLength, 1e-16);
    EXPECT_EQ(network.getChannels().at(c1->getId())->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(network.getChannels().at(c2->getId())->getId(), c2->getId());
    EXPECT_EQ(network.getChannels().at(c2->getId())->getNodeA(), node2->getId());
    EXPECT_EQ(network.getChannels().at(c2->getId())->getNodeB(), node3->getId());
    EXPECT_EQ(network.getChannels().at(c2->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c2->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c2->getId())->getLength(), cLength, 1e-16);
    EXPECT_EQ(network.getChannels().at(c2->getId())->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(network.getChannels().at(c3->getId())->getId(), c3->getId());
    EXPECT_EQ(network.getChannels().at(c3->getId())->getNodeA(), node3->getId());
    EXPECT_EQ(network.getChannels().at(c3->getId())->getNodeB(), node4->getId());
    EXPECT_EQ(network.getChannels().at(c3->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c3->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c3->getId())->getLength(), cLength, 1e-12);
    EXPECT_EQ(network.getChannels().at(c3->getId())->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(network.getChannels().at(c4->getId())->getId(), c4->getId());
    EXPECT_EQ(network.getChannels().at(c4->getId())->getNodeA(), node3->getId());
    EXPECT_EQ(network.getChannels().at(c4->getId())->getNodeB(), node5->getId());
    EXPECT_EQ(network.getChannels().at(c4->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c4->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c4->getId())->getLength(), cLength, 1e-16);
    EXPECT_EQ(network.getChannels().at(c4->getId())->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(network.getChannels().at(c5->getId())->getId(), c5->getId());
    EXPECT_EQ(network.getChannels().at(c5->getId())->getNodeA(), node4->getId());
    EXPECT_EQ(network.getChannels().at(c5->getId())->getNodeB(), node5->getId());
    EXPECT_EQ(network.getChannels().at(c5->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c5->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c5->getId())->getLength(), cLength, 1e-12);
    EXPECT_EQ(network.getChannels().at(c5->getId())->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(network.getChannels().at(c6->getId())->getId(), c6->getId());
    EXPECT_EQ(network.getChannels().at(c6->getId())->getNodeA(), node5->getId());
    EXPECT_EQ(network.getChannels().at(c6->getId())->getNodeB(), node0->getId());
    EXPECT_EQ(network.getChannels().at(c6->getId())->getWidth(), cWidth);
    EXPECT_EQ(network.getChannels().at(c6->getId())->getHeight(), cHeight);
    EXPECT_NEAR(network.getChannels().at(c6->getId())->getLength(), cLength, 1e-16);
    EXPECT_EQ(network.getChannels().at(c6->getId())->getChannelType(), arch::ChannelType::NORMAL);

    EXPECT_EQ(testSimulation.getFluid(fluid0->getId())->getId(), fluid0->getId());
    EXPECT_EQ(testSimulation.getFluid(fluid0->getId())->getName(), "");
    EXPECT_EQ(testSimulation.getFluid(fluid0->getId())->getViscosity(), 1e-3);
    EXPECT_EQ(testSimulation.getFluid(fluid0->getId())->getDensity(), 1e3);
    EXPECT_EQ(testSimulation.getFluid(fluid0->getId())->getConcentration(), 1.0);
    EXPECT_EQ(testSimulation.getFluid(fluid1->getId())->getId(), fluid1->getId());
    EXPECT_EQ(testSimulation.getFluid(fluid1->getId())->getName(), "");
    EXPECT_EQ(testSimulation.getFluid(fluid1->getId())->getViscosity(), 3e-3);
    EXPECT_EQ(testSimulation.getFluid(fluid1->getId())->getDensity(), 1e3);
    EXPECT_EQ(testSimulation.getFluid(fluid1->getId())->getConcentration(), 1.0);

    EXPECT_EQ(testSimulation.getDroplet(droplet0->getId())->getId(), droplet0->getId());
    EXPECT_EQ(testSimulation.getDroplet(droplet0->getId())->getName(), "");
    EXPECT_EQ(testSimulation.getDroplet(droplet0->getId())->getVolume(), dropletVolume);

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.033203, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.044922, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.111328, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 0.125391, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 0.239941, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 0.254778, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 0.321184, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node3->getId()), 347.655305, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node4->getId()), 292.906438, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node5->getId()), 238.157571, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
}

TEST(BigDroplet, inverseDirectionChannels) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

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

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.033203, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.044922, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.111328, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 0.125391, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 0.239941, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 0.254778, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 0.321184, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node3->getId()), 347.655305, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node4->getId()), 292.906438, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node5->getId()), 238.157571, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), -0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), -0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), -0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), -0.00000000003, 5e-17);

    EXPECT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
}

TEST(BigDroplet, mixedDirectionChannels) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

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

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.033203, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.044922, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.111328, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 0.125391, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 0.239941, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 0.254778, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 0.321184, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node1->getId()), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node2->getId()), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node3->getId()), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node4->getId()), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node1->getId()), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node2->getId()), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node3->getId()), 347.655305, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node4->getId()), 292.906438, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node5->getId()), 238.157571, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node1->getId()), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node2->getId()), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node3->getId()), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node4->getId()), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node5->getId()), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(node0->getId()), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c3->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c4->getId()), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c5->getId()), -0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(pump->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c1->getId()), -0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c2->getId()), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c3->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c4->getId()), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c5->getId()), -0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(c6->getId()), 0.00000000003, 5e-17);

    EXPECT_EQ(testSimulation.getContinuousPhase()->getId(), fluid0->getId());
}

TEST(BigDroplet, jsonDefinition) {
    std::string file = "../examples/Abstract/Droplet/Network1.JSON";

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

    EXPECT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getTime(), 0.033203, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getTime(), 0.044922, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getTime(), 0.111328, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getTime(), 0.125391, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getTime(), 0.239941, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getTime(), 0.254778, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getTime(), 0.321184, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(0), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(1), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(0)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(0), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(1), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(1)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(0), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(1), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(2)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(0), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(1), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(3)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(0), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(1), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(4)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(0), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(1), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(2), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(3), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(5)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(0), 630.802163, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(1), 466.555562, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(2), 302.308961, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(3), 233.277781, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(6)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(0), 676.148507, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(1), 511.901906, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(2), 347.655305, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(3), 292.906438, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(4), 238.157571, 5e-7);
    EXPECT_NEAR(result->getStates().at(7)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(0), 602.237537, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(1), 437.990936, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(2), 273.744335, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(3), 218.995468, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(4), 164.246601, 5e-7);
    EXPECT_NEAR(result->getStates().at(8)->getPressures().at(5), 0.000000, 5e-7);

    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(0)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(1)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(2)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(3)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(4)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(2), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(3), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(4), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(5)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(2), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(3), 0.0000000000173913, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(4), 0.0000000000126087, 5e-17);
    EXPECT_NEAR(result->getStates().at(6)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(7)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(6), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(0), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(1), 0.00000000003, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(2), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(3), 0.00000000002, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(4), 0.00000000001, 5e-17);
    EXPECT_NEAR(result->getStates().at(8)->getFlowRates().at(5), 0.00000000003, 5e-17);

    EXPECT_EQ(testSimulation.getContinuousPhase()->getId(), 0);
}

TEST(BigDroplet, noSink1) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);
    
    // flowRate pump
    auto flowRate = 3e-11;
    network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel = sim::ResistanceModel1D<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}

TEST(BigDroplet, noSink2) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);
    
    // flowRate pump
    auto flowRate = 3e-11;
    network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel = sim::ResistanceModel1D<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}

TEST(BigDroplet, noSinkTwoDroplets) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::BigDroplet);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(1e-3, 0.0, false);
    auto node0 = network.addNode(2e-3, 0.0, false);

    // flowRate pump
    auto flowRate = 3e-11;
    network.addFlowRatePump(node0->getId(), node1->getId(), flowRate);

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

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel = sim::ResistanceModel1D<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // check if chip is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    testSimulation.simulate();
}
