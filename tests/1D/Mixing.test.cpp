#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

/** Case 1:
 * 
 *  3 way node
 *  - 2 inflows
 *  - 1 outflow
*/
TEST(Topology, case1) {
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(0.0, 2e-3, true);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    c0->setPressure(flowRate);
    c1->setPressure(0.5*flowRate);
    c2->setPressure(1.5*flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node2->getId());

    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.9269908, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(1).channelId, 1);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);

}

/** Case 2:
 * 
 *  3 way node
 *  - 1 inflow
 *  - 2 outflows
*/
TEST(Topology, case2) {
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 1e-3, true);
    auto node1 = network.addNode(1e-3, 1e-3, false);
    auto node2 = network.addNode(2e-3, 0.0, true);
    auto node3 = network.addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    c0->setPressure(2*flowRate);
    c1->setPressure(flowRate);
    c2->setPressure(flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node1->getId());

    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 0.5, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.5, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);

}

/** Case 3:
 * 
 *  4 way node
 *  - 1 inflow
 *  - 3 outflows
*/
TEST(Topology, case3) {
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 1e-3, true);
    auto node1 = network.addNode(1e-3, 1e-3);
    auto node2 = network.addNode(2e-3, 0.0, true);
    auto node3 = network.addNode(2e-3, 1e-3, true);
    auto node4 = network.addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  
    c0->setPressure(3*flowRate);
    c1->setPressure(0.5*flowRate);
    c2->setPressure(1.5*flowRate);
    c3->setPressure(flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);
    c3->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node1->getId());

    T r1 = network.getChannel(1)->getFlowRate()/network.getChannel(0)->getFlowRate();
    T r2 = network.getChannel(2)->getFlowRate()/network.getChannel(0)->getFlowRate();
    T r3 = network.getChannel(3)->getFlowRate()/network.getChannel(0)->getFlowRate();
    
    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.0, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 0.7853982, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 3);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, r1, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, r1*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, r1, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, r1+r2, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, r2*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, r1+r2, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, r3*network.getChannel(0)->getFlowRate(), 1e-12);

}

/** Case 4:
 * 
 *  4 way node
 *  - 2 adjacent inflows
 *  - 2 adjacent outflow
*/
TEST(Topology, case4) {
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(0.0, 2e-3, true);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(2e-3, 0.0, true);
    auto node4 = network.addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node2->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    c0->setPressure(flowRate);
    c1->setPressure(flowRate);
    c2->setPressure(0.5*flowRate);
    c3->setPressure(1.5*flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);
    c3->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node2->getId());
    
    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 2.3561945, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(1).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(1).radialAngle, 3.9269908, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 0.5, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.5, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);

}

/** Case 5:
 * 
 *  4 way node
 *  - 3 inflows
 *  - 1 outflow
*/
TEST(Topology, case5) {
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(0.0, 1e-3, true);
    auto node2 = network.addNode(0.0, 2e-3, true);
    auto node3 = network.addNode(1e-3, 1e-3, false);
    auto node4 = network.addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    c0->setPressure(flowRate);
    c1->setPressure(0.5*flowRate);
    c2->setPressure(1.5*flowRate);
    c3->setPressure(3*flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);
    c3->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node3->getId());

    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.1415927, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 3.9269908, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 1);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 3);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(2).channelId, 2);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).flowRate, network.getChannel(2)->getFlowRate(), 1e-12);

}

/** Case 6:
 * 
 *  4 way node (saddlepoint)
 *  - 2 inflows opposed
 *  - 2 outflows opposed
*/
TEST(Topology, case6) {    
    // define network
    arch::Network<T> network;

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(0.0, 1e-3, true);
    auto node2 = network.addNode(0.0, 2e-3, true);
    auto node3 = network.addNode(1e-3, 1e-3, false);
    auto node4 = network.addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node3->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    c0->setPressure(flowRate);
    c1->setPressure(0.5*flowRate);
    c2->setPressure(1.5*flowRate);
    c3->setPressure(2*flowRate);

    c0->setResistance(1);
    c1->setResistance(1);
    c2->setResistance(1);
    c3->setResistance(1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(&network, node3->getId());

    ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 4);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(2).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(3).size(), 1);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(2).at(0).inFlow, false);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(2).at(0).radialAngle, 3.1415927, 1e-7);
    ASSERT_EQ(mixingModel.getConcatenatedFlows().at(3).at(0).inFlow, true);
    ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(3).at(0).radialAngle, 3.9269908, 1e-7);

    ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 2);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 0.8, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.8*network.getChannel(0)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 2);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 1./5., 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, (4./5.)*network.getChannel(2)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 2);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 1./5., 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, (1./5.)*network.getChannel(2)->getFlowRate(), 1e-12);
    ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(1).channelId, 0);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionStart, 0.8, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionEnd, 1.0, 1e-12);
    ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).flowRate, 0.2*network.getChannel(0)->getFlowRate(), 1e-12);

}

/** Case1:
 * 
 * operations 1 and 2
*/
TEST(DiffusionMixing, case1) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 1000e-6;
    T cLength2 = 0;
    T flowRate1 = 3e-11;
    T flowRate2 = 3e-11 * 0.5;
    T pecletNr1 = (flowRate1 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 100;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    constantFlowSections.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});

    // perform analytical solution for constant input
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

    functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 2.0, 0.5, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);

    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;

    outputFile0.open("Case1out0.csv");
    outputFile1.open("Case1out1.csv");
    outputFile2.open("Case1out2.csv");
    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";

    int numValues = 1001;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        T y0;
        T y1;
        T y2;
        y0 = fConstant0(x);
        y1 = fFunction1(x);
        y2 = fFunction2(x);
        outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
        outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
    }
    // Close the files
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();

}

/** Case2:
 * 
 * operations 3 and 4
*/
TEST(DiffusionMixing, case2) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 1000e-6;
    T cLength2 = 0;
    T flowRate1 = 3e-11;
    T flowRate2 = 3e-11 * 0.5;
    T pecletNr1 = (flowRate1 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 100;

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
    constantFlowSections.push_back({0.0, 0.25, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections.push_back({0.25, 0.5, 1.0, 0.0, 0.5, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // top inflow channel (double the width)

    // perform analytical solution for constant input
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

    functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 4.0, 0.5, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    functionFlowSections3.push_back({0.0, 1.0, 4.0, 0.75, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

    // perform analytical solution for function input
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);
    auto [fFunction3, segmentedResultFunction3, a_0_Function3] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections3, zeroFunction);


    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;
    std::ofstream outputFile3;

    outputFile0.open("Case2out0.csv");
    outputFile1.open("Case2out1.csv");
    outputFile2.open("Case2out2.csv");
    outputFile3.open("Case2out3.csv");
    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";
    outputFile3 << "x,f(x)\n";

    int numValues = 1001;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        T y0;
        T y1;
        T y2;
        T y3;
        y0 = fConstant0(x);
        y1 = fFunction1(x);
        y2 = fFunction2(x);
        y3 = fFunction3(x);
        outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
        outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
        outputFile3 << std::setprecision(4) << x << "," << y3 << "\n";
    }
    // Close the file
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();

}


// TEST(Mixing, Case1) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network1.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case1.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 1:
//      * 
//      * State 0 - Pre-Injection
//      * State 1 - Post-Injection
//      * State 2 - Mixture 0 reaches node 4
//      * State 3 - Mixture 1 reaches node 5
//     */

//     ASSERT_EQ(result->getStates().size(), 4);
//     ASSERT_EQ(result->getMixtures().size(), 2);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.912023, 5e-7);

//     ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 2);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);

//     ASSERT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
// }

// TEST(Mixing, Case2) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network1.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case2.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 2:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 reaches node 4 and creates Mixture 2
//      * State 4 - Mixture 2 reaches node 5
//      * State 5 - Mixture 1 reaches node 4 and creates Mixture 3
//      * State 6 - Mixture 3 reaches node 5
//     */

//     ASSERT_EQ(result->getStates().size(), 7);
//     ASSERT_EQ(result->getMixtures().size(), 4);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.912023, 5e-7);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 1.245356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 1.412023, 5e-7);

//     ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 2);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.670820, 5e-7);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 0.329180, 5e-7);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().mixtureId, 2);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position2, 0.552786, 5e-7);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().mixtureId, 2);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().mixtureId, 3);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().mixtureId, 2);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(4).back().channel, 4);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(4).back().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().mixtureId, 3);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(4).front().channel, 4);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(4).front().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 1);

//     ASSERT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) + 
//         0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
    
// }

// TEST(Mixing, Case3) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network2.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case3.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 3:
//      * 
//      * State 0 - Pre-Injection
//      * State 1 - Post-Injection
//      * State 2 - Mixture 0 reaches node 2
//      * State 3 - Mixture 0 reaches nodes 3 and 4
//     */

//     ASSERT_EQ(result->getStates().size(), 4);
//     ASSERT_EQ(result->getMixtures().size(), 1);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.666667, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 2.157379, 5e-7);

//     ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

// }

// TEST(Mixing, Case4) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network2.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case4.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 4:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Mixture 0 reaches node 2
//      * State 3 - Mixture 0 reaches nodes 3 and 4
//      * State 4 - Post-Injection 2
//      * State 5 - Mixture 1 reaches node 2
//      * State 6 - Mixture 1 reaches node 3 and 4
//     */

//     ASSERT_EQ(result->getStates().size(), 7);
//     ASSERT_EQ(result->getMixtures().size(), 2);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.666667, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 2.157379, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 3.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 3.666667, 5e-7);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 5.157379, 5e-7);

//     ASSERT_EQ(result->getStates().at(0)->getMixturePositions().size(), 0);

//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().size(), 1);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(1)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(2)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(3)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(1).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(1).back().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(1).back().channel, 1);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).back().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(1).back().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(4)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(1).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).back().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(2).back().channel, 2);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).back().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(2).back().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).front().position2, 0.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).back().mixtureId, 0);
//     ASSERT_EQ(result->getStates().at(5)->getMixturePositions().at(3).back().channel, 3);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).back().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(5)->getMixturePositions().at(3).back().position2, 1.0, 1e-12);

//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().size(), 3);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(1).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(1).front().channel, 1);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(1).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(1).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(2).front().channel, 2);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(2).front().position2, 1.0, 1e-12);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().mixtureId, 1);
//     ASSERT_EQ(result->getStates().at(6)->getMixturePositions().at(3).front().channel, 3);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position1, 0.0, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getMixturePositions().at(3).front().position2, 1.0, 1e-12);

// }

// TEST(Mixing, Case5) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network3.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case5.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 5:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Mixture 0 reaches node 2
//      * State 3 - Mixture 0 reaches node 4
//      * State 4 - Mixture 0 reaches nodes 3 and 5
//     */

//     ASSERT_EQ(result->getStates().size(), 5);
//     ASSERT_EQ(result->getMixtures().size(), 1);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.333333, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 2.192570, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 2.657379, 5e-7);

// }

// TEST(Mixing, Case6) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network3.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case6.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 6:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Mixture 0 reaches node 2
//      * State 3 - Post-Injection 2
//      * State 4 - Mixture 1 reaches node 2
//      * State 5 - Mixture 0 reaches node 4
//      * State 6 - Mixture 0 reaches node 3 and 5
//      * State 7 - Mixture 1 reaches node 4
//      * State 8 - Mixture 1 reaches node 3 and 5
//     */

//     ASSERT_EQ(result->getStates().size(), 9);
//     ASSERT_EQ(result->getMixtures().size(), 2);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.333333, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.500000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.833333, 5e-7);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 2.192570, 5e-7);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 2.657379, 5e-7);
//     ASSERT_NEAR(result->getStates().at(7)->getTime(), 2.692570, 5e-7);
//     ASSERT_NEAR(result->getStates().at(8)->getTime(), 3.157379, 5e-7);

// }

// TEST(Mixing, Case7) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network4.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case7.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 7:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 reaches node 4
//      * State 4 - Mixture 0 reaches node 5 and 6
//     */

//     ASSERT_EQ(result->getStates().size(), 5);
//     ASSERT_EQ(result->getMixtures().size(), 1);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);

// }

// TEST(Mixing, Case8) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network4.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case8.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 8:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 and 1 reach node 4
//      * State 4 - Mixture 3 reaches node 5 and 6
//      * State 5 - Post-Injection 3
//      * State 6 - Mixture 2 reaches node 4
//      * State 7 - Mixture 4 reaches node 5 and 6
//     */

//     ASSERT_EQ(result->getStates().size(), 8);
//     ASSERT_EQ(result->getMixtures().size(), 5);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 2.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 2.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(7)->getTime(), 3.216761, 5e-7);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
//     ASSERT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 2);
//     ASSERT_EQ(result->getMixtures().at(4)->getSpecieConcentrations().size(), 2);

//     ASSERT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) +
//         0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(1), 
//         0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(1), 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(1), 
//         0.5*result->getMixtures().at(2)->getSpecieConcentrations().at(1), 1e-7);
    

// }

// TEST(Mixing, Case9) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network5.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case9.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();
//     result->printLastState();

//     /**
//      * Case 9:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 reaches node 6
//      * State 4 - Mixture 1 reaches node 7
//      * State 5 - Mixture 0 reaches node 6
//      * State 6 - Mixture 2 reaches node 7
//      * State 7 - Post-Injection 3
//      * State 8 - Mixture 0 reaches node 6
//      * State 9 - Mixture 0 reaches node 7
//     */

//     ASSERT_EQ(result->getStates().size(), 10);
//     ASSERT_EQ(result->getMixtures().size(), 3);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.856467, 5e-7);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 1.166667, 5e-7);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 1.277778, 5e-7);
//     ASSERT_NEAR(result->getStates().at(7)->getTime(), 2.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(8)->getTime(), 2.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(9)->getTime(), 2.856467, 5e-7);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);

//     ASSERT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0),
//         result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0, 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0),
//         result->getMixtures().at(0)->getSpecieConcentrations().at(0)/1.5, 1e-7);

// }

// TEST(Mixing, Case10) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network5.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case10.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 10:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 reaches node 6
//      * State 4 - Mixture 3 reaches node 7
//      * State 5 - Mixture 1 reaches node 6
//      * State 6 - Mixture 4 reaches node 7
//      * State 7 - Post-Injection 3
//      * State 8 - Mixture 2 reaches node 6
//      * State 9 - Mixture 5 reaches node 7
//     */

//     ASSERT_EQ(result->getStates().size(), 10);
//     ASSERT_EQ(result->getMixtures().size(), 6);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.500000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 0.856467, 5e-7);
//     ASSERT_NEAR(result->getStates().at(5)->getTime(), 1.166667, 5e-7);
//     ASSERT_NEAR(result->getStates().at(6)->getTime(), 1.277778, 5e-7);
//     ASSERT_NEAR(result->getStates().at(7)->getTime(), 2.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(8)->getTime(), 2.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(9)->getTime(), 2.856467, 5e-7);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
//     ASSERT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(3)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(4)->getSpecieConcentrations().size(), 2);
//     ASSERT_EQ(result->getMixtures().at(5)->getSpecieConcentrations().size(), 2);

//     ASSERT_NEAR(result->getMixtures().at(3)->getSpecieConcentrations().at(0), 
//         result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0, 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(0), 
//         result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0 +
//         result->getMixtures().at(1)->getSpecieConcentrations().at(0)/3.0, 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(4)->getSpecieConcentrations().at(1), 
//         result->getMixtures().at(1)->getSpecieConcentrations().at(1)/3.0, 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(5)->getSpecieConcentrations().at(0), 
//         result->getMixtures().at(0)->getSpecieConcentrations().at(0)/3.0 +
//         result->getMixtures().at(1)->getSpecieConcentrations().at(0)/3.0, 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(5)->getSpecieConcentrations().at(1), 
//         result->getMixtures().at(1)->getSpecieConcentrations().at(1)/3.0 +
//         result->getMixtures().at(2)->getSpecieConcentrations().at(1)/3.0, 1e-7);

// }

// TEST(Mixing, Case11) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network6.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case11.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 11:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Mixture 0 reaches node 4
//      * State 3 - Mixture 1 reaches nodes 5 and 6
//     */

//     ASSERT_EQ(result->getStates().size(), 4);
//     ASSERT_EQ(result->getMixtures().size(), 2);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 1.216761, 5e-7);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 1);

//     ASSERT_NEAR(result->getMixtures().at(1)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0), 1e-7);
// }

// TEST(Mixing, Case12) {
//     // Define JSON files
//     std::string networkFile = "../examples/1D/Mixing/Network6.JSON";
//     std::string simFile = "../examples/1D/Mixing/Case12.JSON";

//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

//     // Load and set the simulations from the JSON files
//     sim::Simulation<T> sim = porting::simulationFromJSON<T>(simFile, &network);

//     // Check if network is valid
//     network.isNetworkValid();
//     network.sortGroups();

//     // simulate
//     sim.simulate();

//     // results
//     result::SimulationResult<T>* result = sim.getSimulationResults();

//     /**
//      * Case 12:
//      * 
//      * State 0 - Pre-Injections
//      * State 1 - Post-Injection 1
//      * State 2 - Post-Injection 2
//      * State 3 - Mixture 0 and 1 reach node 4
//      * State 4 - Mixture 2 reaches nodes 5 and 6
//     */

//     ASSERT_EQ(result->getStates().size(), 5);
//     ASSERT_EQ(result->getMixtures().size(), 3);

//     ASSERT_NEAR(result->getStates().at(0)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(1)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(2)->getTime(), 0.000000, 1e-12);
//     ASSERT_NEAR(result->getStates().at(3)->getTime(), 0.745356, 5e-7);
//     ASSERT_NEAR(result->getStates().at(4)->getTime(), 1.216761, 5e-7);

//     ASSERT_EQ(result->getMixtures().at(0)->getSpecieConcentrations().size(), 1);
//     ASSERT_EQ(result->getMixtures().at(1)->getSpecieConcentrations().size(), 2);
//     ASSERT_EQ(result->getMixtures().at(2)->getSpecieConcentrations().size(), 2);

//     ASSERT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(0), 
//         0.5*result->getMixtures().at(0)->getSpecieConcentrations().at(0) +
//         0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(0), 1e-7);
//     ASSERT_NEAR(result->getMixtures().at(2)->getSpecieConcentrations().at(1), 
//         0.5*result->getMixtures().at(1)->getSpecieConcentrations().at(1), 1e-7);

// }