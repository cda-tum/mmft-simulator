#include "../../src/baseSimulator.h"
#include "../../src/baseSimulator.hh"

#include "gtest/gtest.h"

using T = double;


TEST(Network, testNetwork1) {
    // define network
    arch::Network<T> network;
    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;
    int node3Id = 3;

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(nodeGroundId, node0Id, 1.0);

    // flowRate pump (current source)
    auto i0 = network.addFlowRatePump(nodeGroundId, node2Id, 1.0);

    // channels
    auto c1 = network.addChannel(node0Id, node1Id, 5, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node1Id, nodeGroundId, 10, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node2Id, node3Id, 5, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node3Id, nodeGroundId, 10, arch::ChannelType::NORMAL);

    ASSERT_THROW(network.getGroundIds(), std::invalid_argument);

    network.addGround(nodeGroundId);

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode*, int>> nodes;
    int matrixId = 0;
    for (auto& [nodeId, node] : network.getNodes()) {
        // if node is ground node set the matrixId to -1
        if (network.getGroundIds().count(nodeId)) {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), -1));
        } else {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channels;
    for (const auto& [key, channel] : network.getChannels()) {
        channels.push_back(channel.get());
    }
    std::vector<arch::FlowRatePump*> flowRatePumps;
    for (const auto& [key, pump] : network.getFlowRatePumps()) {
        flowRatePumps.push_back(pump.get());
    }
    std::vector<arch::PressurePump*> pressurePumps;
    for (const auto& [key, pump] : network.getPressurePumps()) {
        pressurePumps.push_back(pump.get());
    }

    nodal::conductNodalAnalysis(nodes, channels, pressurePumps, flowRatePumps);

    const auto& networkNodes = network.getNodes();
    const auto& networkPressurePump = network.getPressurePumps();
    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(networkNodes.at(node0Id)->getPressure(), 1.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node1Id)->getPressure(), 2.0 / 3.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node2Id)->getPressure(), 15.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node3Id)->getPressure(), 10.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(networkPressurePump.at(v0)->getFlowRate(), -0.2 / 3.0, errorTolerance);
}

TEST(Network, testNetwork2) {
    // define network
    arch::Network network;
    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;
    int node3Id = 3;
    int node4Id = 4;

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(nodeGroundId, node0Id, 1.0);
    auto v1 = network.addPressurePump(node4Id, nodeGroundId, 2.0);

    // flowRate pump (current source)
    auto i0 = network.addFlowRatePump(nodeGroundId, node1Id, 1.0);

    // channels
    auto c1 = network.addChannel(node0Id, node1Id, 5, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(nodeGroundId, node1Id, 10, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1Id, node2Id, 20, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node2Id, node3Id, 30, arch::ChannelType::NORMAL);

    network.addGround(nodeGroundId);

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode*, int>> nodes;
    int matrixId = 0;
    for (const auto& [nodeId, node] : network.getNodes()) {
        // if node is ground node set the matrixId to -1
        if (network.getGroundIds().count(nodeId)) {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), -1));
        } else {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channels;
    for (const auto& [key, channel] : network.getChannels()) {
        channels.push_back(channel.get());
    }
    std::vector<arch::FlowRatePump*> flowRatePumps;
    for (const auto& [key, pump] : network.getFlowRatePumps()) {
        flowRatePumps.push_back(pump.get());
    }
    std::vector<arch::PressurePump*> pressurePumps;
    for (const auto& [key, pump] : network.getPressurePumps()) {
        pressurePumps.push_back(pump.get());
    }

    nodal::conductNodalAnalysis(nodes, channels, pressurePumps, flowRatePumps);

    const auto& networkNodes = network.getNodes();
    const auto& networkPressurePump = network.getPressurePumps();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(networkNodes.at(node0Id)->getPressure(), 1.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node1Id)->getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node2Id)->getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node3Id)->getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node4Id)->getPressure(), -2.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(networkPressurePump.at(v0)->getFlowRate(), 0.6, errorTolerance);
    ASSERT_NEAR(networkPressurePump.at(v1)->getFlowRate(), 0.0, errorTolerance);
}

TEST(Network, testNetwork3) {
    // define network
    arch::Network network;
    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node1Id, node0Id, 32.0);
    auto v1 = network.addPressurePump(node2Id, nodeGroundId, 20.0);

    // flowRate pump (current source)

    // channels
    auto c1 = network.addChannel(nodeGroundId, node0Id, 2, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node1Id, node2Id, 4, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1Id, nodeGroundId, 8, arch::ChannelType::NORMAL);

    network.addGround(nodeGroundId);

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode*, int>> nodes;
    int matrixId = 0;
    for (const auto& [nodeId, node] : network.getNodes()) {
        // if node is ground node set the matrixId to -1
        if (network.getGroundIds().count(nodeId)) {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), -1));
        } else {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channels;
    for (const auto& [key, channel] : network.getChannels()) {
        channels.push_back(channel.get());
    }
    std::vector<arch::FlowRatePump*> flowRatePumps;
    for (const auto& [key, pump] : network.getFlowRatePumps()) {
        flowRatePumps.push_back(pump.get());
    }
    std::vector<arch::PressurePump*> pressurePumps;
    for (const auto& [key, pump] : network.getPressurePumps()) {
        pressurePumps.push_back(pump.get());
    }

    nodal::conductNodalAnalysis(nodes, channels, pressurePumps, flowRatePumps);

    const auto& networkNodes = network.getNodes();
    const auto& networkPressurePumps = network.getPressurePumps();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(networkNodes.at(node0Id)->getPressure(), 8.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node1Id)->getPressure(), -24.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node2Id)->getPressure(), -20.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(networkPressurePumps.at(v0)->getFlowRate(), -4.0, errorTolerance);
    ASSERT_NEAR(networkPressurePumps.at(v1)->getFlowRate(), 1.0, errorTolerance);
}

TEST(Network, testNetwork4) {
    // define network
    arch::Network network;
    // nodes
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;

    // pressure pump (voltage sources)
    auto v0 = network.addPressurePump(node0Id, node1Id, 32.0);

    // flowRate pump (current source)
    auto i0 = network.addFlowRatePump(node0Id, nodeGroundId, 20.0);

    // channels
    auto c1 = network.addChannel(nodeGroundId, node0Id, 2, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node0Id, node1Id, 4, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1Id, nodeGroundId, 8, arch::ChannelType::NORMAL);

    network.addGround(nodeGroundId);

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode*, int>> nodes;
    int matrixId = 0;
    for (const auto& [nodeId, node] : network.getNodes()) {
        // if node is ground node set the matrixId to -1
        if (network.getGroundIds().count(nodeId)) {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), -1));
        } else {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channels;
    for (const auto& [key, channel] : network.getChannels()) {
        channels.push_back(channel.get());
    }
    std::vector<arch::FlowRatePump*> flowRatePumps;
    for (const auto& [key, pump] : network.getFlowRatePumps()) {
        flowRatePumps.push_back(pump.get());
    }
    std::vector<arch::PressurePump*> pressurePumps;
    for (const auto& [key, pump] : network.getPressurePumps()) {
        pressurePumps.push_back(pump.get());
    }

    nodal::conductNodalAnalysis(nodes, channels, pressurePumps, flowRatePumps);

    const auto& networkNodes = network.getNodes();
    const auto& networkPressurePumps = network.getPressurePumps();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(networkNodes.at(node0Id)->getPressure(), -38.4, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node1Id)->getPressure(), -6.4, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(networkPressurePumps.at(v0)->getFlowRate(), -7.2, errorTolerance);
}

TEST(Network, testNetwork5) {
    // define network
    arch::Network network;
    // nodes
    auto nodeGroundId = -1;
    auto node0Id = 0;
    auto node1Id = 1;
    auto node2Id = 2;

    // pressure pump (voltage sources)

    // flowRate pump (current source)
    auto i0 = network.addFlowRatePump(node0Id, nodeGroundId, 1.0);
    auto i1 = network.addFlowRatePump(node2Id, nodeGroundId, 1.5);

    // channels
    auto c1 = network.addChannel(node0Id, node1Id, 5, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node1Id, node2Id, 7, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1Id, nodeGroundId, 10, arch::ChannelType::NORMAL);

    network.addGround(nodeGroundId);

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode*, int>> nodes;
    int matrixId = 0;
    for (const auto& [nodeId, node] : network.getNodes()) {
        // if node is ground node set the matrixId to -1
        if (network.getGroundIds().count(nodeId)) {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), -1));
        } else {
            nodes.insert_or_assign(nodeId, std::make_tuple(node.get(), matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channels;
    for (const auto& [key, channel] : network.getChannels()) {
        channels.push_back(channel.get());
    }
    std::vector<arch::FlowRatePump*> flowRatePumps;
    for (const auto& [key, pump] : network.getFlowRatePumps()) {
        flowRatePumps.push_back(pump.get());
    }
    std::vector<arch::PressurePump*> pressurePumps;
    for (const auto& [key, pump] : network.getPressurePumps()) {
        pressurePumps.push_back(pump.get());
    }

    nodal::conductNodalAnalysis(nodes, channels, pressurePumps, flowRatePumps);

    const auto& networkNodes = network.getNodes();
    const auto& networkPressurePumps = network.getPressurePumps();

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(networkNodes.at(node0Id)->getPressure(), -30.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node1Id)->getPressure(), -25.0, errorTolerance);
    ASSERT_NEAR(networkNodes.at(node2Id)->getPressure(), -35.5, errorTolerance);
}

TEST(Network, networkArchitectureDefinition) {
    // define network
    arch::Network bionetwork;
    bionetwork.setName("testnetwork");
    int nodeGroundId = -1;
    int node0Id = 0;
    int node1Id = 1;
    int node2Id = 2;
    int node3Id = 3;
    int node4Id = 4;

    // pressure pump (voltage sources)
    int v0 = bionetwork.addPressurePump(node0Id, node1Id, 32.0);

    // flowRate pump (current source)
    int i0 = bionetwork.addFlowRatePump(node0Id, nodeGroundId, 20.0);

    // channels
    int c1 = bionetwork.addChannel(nodeGroundId, node0Id, 2, arch::ChannelType::NORMAL);
    int c2 = bionetwork.addChannel(node0Id, node1Id, 4, arch::ChannelType::BYPASS);
    int c3 = bionetwork.addChannel(node1Id, nodeGroundId, 8, arch::ChannelType::CLOGGABLE);

    ASSERT_EQ(bionetwork.getName(), "testnetwork");
    ASSERT_EQ(bionetwork.getPressurePumps().at(v0)->getNode0()->getId(), node0Id);
    ASSERT_EQ(bionetwork.getPressurePumps().at(v0)->getNode1()->getId(), node1Id);
    ASSERT_EQ(bionetwork.getPressurePumps().at(v0)->getPressure(), 32.0);
    ASSERT_EQ(bionetwork.getFlowRatePumps().at(i0)->getNode0()->getId(), node0Id);
    ASSERT_EQ(bionetwork.getFlowRatePumps().at(i0)->getNode1()->getId(), nodeGroundId);
    ASSERT_EQ(bionetwork.getFlowRatePumps().at(i0)->getFlowRate(), 20.0);
    ASSERT_EQ(bionetwork.getChannels().at(c1)->getNode0()->getId(), nodeGroundId);
    ASSERT_EQ(bionetwork.getChannels().at(c1)->getNode1()->getId(), node0Id);
    ASSERT_EQ(bionetwork.getChannels().at(c1)->getChannelType(), arch::ChannelType::NORMAL);
    ASSERT_EQ(bionetwork.getChannels().at(c2)->getNode0()->getId(), node0Id);
    ASSERT_EQ(bionetwork.getChannels().at(c2)->getNode1()->getId(), node1Id);
    ASSERT_EQ(bionetwork.getChannels().at(c2)->getChannelType(), arch::ChannelType::BYPASS);
    ASSERT_EQ(bionetwork.getChannels().at(c3)->getNode0()->getId(), node1Id);
    ASSERT_EQ(bionetwork.getChannels().at(c3)->getNode1()->getId(), nodeGroundId);
    ASSERT_EQ(bionetwork.getChannels().at(c3)->getChannelType(), arch::ChannelType::CLOGGABLE);
}

TEST(Network, networkName) {
    // define network
    arch::Network bionetwork;
    bionetwork.setName("testnetwork");
    ASSERT_EQ(bionetwork.getName(), "testnetwork");
}