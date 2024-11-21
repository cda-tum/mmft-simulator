#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Network, testNetwork1) {
    // define network
    arch::Network<T> network;
    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node3 = network.addNode(0.0, 0.0, false);
    auto node4 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, false);

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node0->getId(), node1->getId(), 1.0);

    // flowRate pump (current source)
    network.addFlowRatePump(node0->getId(), node3->getId(), 1.0);

    // channels
    network.addChannel(node1->getId(), node2->getId(), 5.0, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), 10.0, arch::ChannelType::NORMAL);
    network.addChannel(node3->getId(), node4->getId(), 5.0, arch::ChannelType::NORMAL);
    network.addChannel(node4->getId(), node0->getId(), 10.0, arch::ChannelType::NORMAL);

    EXPECT_THROW(network.getGroundIds(), std::invalid_argument);

    network.setGround(node0->getId());

    // compute network
    network.sortGroups();
    nodal::NodalAnalysis<T> nodalAnalysis(&network);
    nodalAnalysis.conductNodalAnalysis();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    EXPECT_NEAR(node1->getPressure(), 1.0, errorTolerance);
    EXPECT_NEAR(node2->getPressure(), 2.0 / 3.0, errorTolerance);
    EXPECT_NEAR(node3->getPressure(), 15.0, errorTolerance);
    EXPECT_NEAR(node4->getPressure(), 10.0, errorTolerance);
    // flow rate at pressure pumps
    EXPECT_NEAR(v0->getFlowRate(), -0.2 / 3.0, errorTolerance);
}

TEST(Network, testNetwork2) {
    // define network
    arch::Network<T> network;
    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node3 = network.addNode(0.0, 0.0, false);
    auto node4 = network.addNode(0.0, 0.0, false);
    auto node5 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, true);

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node0->getId(), node1->getId(), 1.0);
    auto v1 = network.addPressurePump(node5->getId(), node0->getId(), 2.0);

    // flowRate pump (current source)
    network.addFlowRatePump(node0->getId(), node2->getId(), 1.0);

    // channels
    network.addChannel(node1->getId(), node2->getId(), 5, arch::ChannelType::NORMAL);
    network.addChannel(node0->getId(), node2->getId(), 10, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node3->getId(), 20, arch::ChannelType::NORMAL);
    network.addChannel(node3->getId(), node4->getId(), 30, arch::ChannelType::NORMAL);

    // compute network
    network.sortGroups();
    nodal::NodalAnalysis<T> nodalAnalysis(&network);
    nodalAnalysis.conductNodalAnalysis();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    EXPECT_NEAR(node1->getPressure(), 1.0, errorTolerance);
    EXPECT_NEAR(node2->getPressure(), 4.0, errorTolerance);
    EXPECT_NEAR(node3->getPressure(), 4.0, errorTolerance);
    EXPECT_NEAR(node4->getPressure(), 4.0, errorTolerance);
    EXPECT_NEAR(node5->getPressure(), -2.0, errorTolerance);
    // flow rate at pressure pumps
    EXPECT_NEAR(v0->getFlowRate(), 0.6, errorTolerance);
    EXPECT_NEAR(v1->getFlowRate(), 0.0, errorTolerance);
}

TEST(Network, testNetwork3) {
    // define network
    arch::Network<T> network;
    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node3 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, true);

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node2->getId(), node1->getId(), 32.0);
    auto v1 = network.addPressurePump(node3->getId(), node0->getId(), 20.0);

    // flowRate pump (current source)

    // channels
    network.addChannel(node0->getId(), node1->getId(), 2, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node3->getId(), 4, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), 8, arch::ChannelType::NORMAL);

    // compute network
    network.sortGroups();
    nodal::NodalAnalysis<T> nodalAnalysis(&network);
    nodalAnalysis.conductNodalAnalysis();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    EXPECT_NEAR(node1->getPressure(), 8.0, errorTolerance);
    EXPECT_NEAR(node2->getPressure(), -24.0, errorTolerance);
    EXPECT_NEAR(node3->getPressure(), -20.0, errorTolerance);
    // flow rate at pressure pumps
    EXPECT_NEAR(v0->getFlowRate(), -4.0, errorTolerance);
    EXPECT_NEAR(v1->getFlowRate(), 1.0, errorTolerance);
}

TEST(Network, testNetwork4) {
    // define network
    arch::Network<T> network;
    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, true);

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node1->getId(), node2->getId(), 32.0);

    // flowRate pump (current source)
    network.addFlowRatePump(node1->getId(), node0->getId(), 20.0);

    // channels
    network.addChannel(node0->getId(), node1->getId(), 2, arch::ChannelType::NORMAL);
    network.addChannel(node1->getId(), node2->getId(), 4, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), 8, arch::ChannelType::NORMAL);

    // compute network
    network.sortGroups();
    nodal::NodalAnalysis<T> nodalAnalysis(&network);
    nodalAnalysis.conductNodalAnalysis();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    EXPECT_NEAR(node1->getPressure(), -38.4, errorTolerance);
    EXPECT_NEAR(node2->getPressure(), -6.4, errorTolerance);
    // flow rate at pressure pumps
    EXPECT_NEAR(v0->getFlowRate(), -7.2, errorTolerance);
}

TEST(Network, testNetwork5) {
    // define network
    arch::Network<T> network;
    // nodes
    auto node1 = network.addNode(0.0, 0.0, false);
    auto node2 = network.addNode(0.0, 0.0, false);
    auto node3 = network.addNode(0.0, 0.0, false);
    auto node0 = network.addNode(0.0, 0.0, true);

    // pressure pump (voltage sources)

    // flowRate pump (current source)
    network.addFlowRatePump(node1->getId(), node0->getId(), 1.0);
    network.addFlowRatePump(node3->getId(), node0->getId(), 1.5);

    // channels
    network.addChannel(node1->getId(), node2->getId(), 5, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node3->getId(), 7, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node0->getId(), 10, arch::ChannelType::NORMAL);

    // compute network
    network.sortGroups();
    nodal::NodalAnalysis<T> nodalAnalysis(&network);
    nodalAnalysis.conductNodalAnalysis();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    EXPECT_NEAR(node1->getPressure(), -30.0, errorTolerance);
    EXPECT_NEAR(node2->getPressure(), -25.0, errorTolerance);
    EXPECT_NEAR(node3->getPressure(), -35.5, errorTolerance);
}

TEST(Network, networkArchitectureDefinition) {
    // define network
    arch::Network<T> bionetwork;
    auto node1 = bionetwork.addNode(0.0, 0.0, false);
    auto node2 = bionetwork.addNode(0.0, 0.0, false);
    auto node0 = bionetwork.addNode(0.0, 0.0, true);

    // pressure pump (voltage sources)
    auto v0 = bionetwork.addPressurePump(node1->getId(), node2->getId(), 32.0);

    // flowRate pump (current source)
    auto i0 = bionetwork.addFlowRatePump(node1->getId(), node0->getId(), 20.0);

    // channels
    auto c1 = bionetwork.addChannel(node0->getId(), node1->getId(), 2, arch::ChannelType::NORMAL);
    auto c2 = bionetwork.addChannel(node1->getId(), node2->getId(), 4, arch::ChannelType::BYPASS);
    auto c3 = bionetwork.addChannel(node2->getId(), node0->getId(), 8, arch::ChannelType::CLOGGABLE);

    EXPECT_EQ(v0->getNodeA(), node1->getId());
    EXPECT_EQ(v0->getNodeB(), node2->getId());
    EXPECT_EQ(v0->getPressure(), 32.0);
    EXPECT_EQ(i0->getNodeA(), node1->getId());
    EXPECT_EQ(i0->getNodeB(), node0->getId());
    EXPECT_EQ(i0->getFlowRate(), 20.0);
    EXPECT_EQ(c1->getNodeA(), node0->getId());
    EXPECT_EQ(c1->getNodeB(), node1->getId());
    EXPECT_EQ(c1->getChannelType(), arch::ChannelType::NORMAL);
    EXPECT_EQ(c2->getNodeA(), node1->getId());
    EXPECT_EQ(c2->getNodeB(), node2->getId());
    EXPECT_EQ(c2->getChannelType(), arch::ChannelType::BYPASS);
    EXPECT_EQ(c3->getNodeA(), node2->getId());
    EXPECT_EQ(c3->getNodeB(), node0->getId());
    EXPECT_EQ(c3->getChannelType(), arch::ChannelType::CLOGGABLE);
}