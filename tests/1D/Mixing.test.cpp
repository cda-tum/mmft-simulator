#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

/** Case 1:
 * 
 *  3 way node
 *  - 2 inflows
 *  - 1 outflow
*/
// TEST(Topology, case1) {
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 0.0, true);
//     auto node1 = network.addNode(0.0, 2e-3, true);
//     auto node2 = network.addNode(1e-3, 1e-3, false);
//     auto node3 = network.addNode(2e-3, 1e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
//     c0->setPressure(flowRate);
//     c1->setPressure(0.5*flowRate);
//     c2->setPressure(1.5*flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node2->getId());

//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.9269908, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(1).channelId, 1);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);

// }

// /** Case 2:
//  * 
//  *  3 way node
//  *  - 1 inflow
//  *  - 2 outflows
// */
// TEST(Topology, case2) {
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 1e-3, true);
//     auto node1 = network.addNode(1e-3, 1e-3, false);
//     auto node2 = network.addNode(2e-3, 0.0, true);
//     auto node3 = network.addNode(2e-3, 2e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
//     c0->setPressure(2*flowRate);
//     c1->setPressure(flowRate);
//     c2->setPressure(flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node1->getId());

//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 0.5, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.5, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);

// }

// /** Case 3:
//  * 
//  *  4 way node
//  *  - 1 inflow
//  *  - 3 outflows
// */
// TEST(Topology, case3) {
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 1e-3, true);
//     auto node1 = network.addNode(1e-3, 1e-3);
//     auto node2 = network.addNode(2e-3, 0.0, true);
//     auto node3 = network.addNode(2e-3, 1e-3, true);
//     auto node4 = network.addNode(2e-3, 2e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c3 = network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  
//     c0->setPressure(3*flowRate);
//     c1->setPressure(0.5*flowRate);
//     c2->setPressure(1.5*flowRate);
//     c3->setPressure(flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);
//     c3->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node1->getId());

//     T r1 = network.getChannel(1)->getFlowRate()/network.getChannel(0)->getFlowRate();
//     T r2 = network.getChannel(2)->getFlowRate()/network.getChannel(0)->getFlowRate();
//     T r3 = network.getChannel(3)->getFlowRate()/network.getChannel(0)->getFlowRate();
    
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.0, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 0.7853982, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 3);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, r1, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, r1*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, r1, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, r1+r2, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, r2*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, r1+r2, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, r3*network.getChannel(0)->getFlowRate(), 1e-12);

// }

// /** Case 4:
//  * 
//  *  4 way node
//  *  - 2 adjacent inflows
//  *  - 2 adjacent outflow
// */
// TEST(Topology, case4) {
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 0.0, true);
//     auto node1 = network.addNode(0.0, 2e-3, true);
//     auto node2 = network.addNode(1e-3, 1e-3, false);
//     auto node3 = network.addNode(2e-3, 0.0, true);
//     auto node4 = network.addNode(2e-3, 2e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c3 = network.addChannel(node2->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
//     c0->setPressure(flowRate);
//     c1->setPressure(flowRate);
//     c2->setPressure(0.5*flowRate);
//     c3->setPressure(1.5*flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);
//     c3->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node2->getId());
    
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 2.3561945, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(1).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(1).radialAngle, 3.9269908, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 0.5, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.5, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.5*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);

// }

// /** Case 5:
//  * 
//  *  4 way node
//  *  - 3 inflows
//  *  - 1 outflow
// */
// TEST(Topology, case5) {
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 0.0, true);
//     auto node1 = network.addNode(0.0, 1e-3, true);
//     auto node2 = network.addNode(0.0, 2e-3, true);
//     auto node3 = network.addNode(1e-3, 1e-3, false);
//     auto node4 = network.addNode(2e-3, 1e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c3 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
//     c0->setPressure(flowRate);
//     c1->setPressure(0.5*flowRate);
//     c2->setPressure(1.5*flowRate);
//     c3->setPressure(3*flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);
//     c3->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node3->getId());

//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.1415927, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 3.9269908, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 1);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 3);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network.getChannel(1)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(2).channelId, 2);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).flowRate, network.getChannel(2)->getFlowRate(), 1e-12);

// }

// /** Case 6:
//  * 
//  *  4 way node (saddlepoint)
//  *  - 2 inflows opposed
//  *  - 2 outflows opposed
// */
// TEST(Topology, case6) {    
//     // define network
//     arch::Network<T> network;

//     // nodes
//     auto node0 = network.addNode(0.0, 0.0, true);
//     auto node1 = network.addNode(0.0, 1e-3, true);
//     auto node2 = network.addNode(0.0, 2e-3, true);
//     auto node3 = network.addNode(1e-3, 1e-3, false);
//     auto node4 = network.addNode(2e-3, 1e-3, true);

//     // channels
//     auto cWidth = 100e-6;
//     auto cHeight = 30e-6;
//     auto cLength = 1000e-6;
//     T flowRate = 3e-11;
    
//     auto c0 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c1 = network.addChannel(node3->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c2 = network.addChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
//     auto c3 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
//     c0->setPressure(flowRate);
//     c1->setPressure(0.5*flowRate);
//     c2->setPressure(1.5*flowRate);
//     c3->setPressure(2*flowRate);

//     c0->setResistance(1);
//     c1->setResistance(1);
//     c2->setResistance(1);
//     c3->setResistance(1);

//     // mixing model
//     sim::DiffusionMixingModel<T> mixingModel;
//     mixingModel.topologyAnalysis(&network, node3->getId());

//     ASSERT_EQ(mixingModel.getConcatenatedFlows().size(), 4);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(2).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(3).size(), 1);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(2).at(0).inFlow, false);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(2).at(0).radialAngle, 3.1415927, 1e-7);
//     ASSERT_EQ(mixingModel.getConcatenatedFlows().at(3).at(0).inFlow, true);
//     ASSERT_NEAR(mixingModel.getConcatenatedFlows().at(3).at(0).radialAngle, 3.9269908, 1e-7);

//     ASSERT_EQ(mixingModel.getOutflowDistributions().size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 2);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 0.8, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.8*network.getChannel(0)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 2);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 1./5., 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, (4./5.)*network.getChannel(2)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 2);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 1./5., 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, (1./5.)*network.getChannel(2)->getFlowRate(), 1e-12);
//     ASSERT_EQ(mixingModel.getOutflowDistributions().at(1).at(1).channelId, 0);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionStart, 0.8, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionEnd, 1.0, 1e-12);
//     ASSERT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).flowRate, 0.2*network.getChannel(0)->getFlowRate(), 1e-12);

// }

/** Case1:
 * 
 * operations 1 and 2
*/
// TEST(DiffusionMixing, case1) {

//     // parameters
//     T cWidth = 100e-6;
//     T cHeight = 100e-6;
//     T cLength = 2000e-6;
//     T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
//     T flowRate1 = 1.96723e-9;
//     T flowRate2 = flowRate1 * 0.5;
//     T pecletNr1 = (flowRate1 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
//     T pecletNr2 = (flowRate2 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
//     // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
//     int resolution = 1000;

//     // create necessary objects
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
//     std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

//     std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
//     std::vector<T> zeroSegmentedResult = {0};

//     sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

//     // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
//     constantFlowSections.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
//     constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});

//     // perform analytical solution for constant input
//     auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

//     functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
//     functionFlowSections2.push_back({0.0, 1.0, 2.0, 0.5, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

//     // perform analytical solution for function input
//     auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
//     auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);

//     // generate resulting csv files
//     std::ofstream outputFile0;
//     std::ofstream outputFile1;
//     std::ofstream outputFile2;

//     outputFile0.open("Case1out0.csv");
//     outputFile1.open("Case1out1.csv");
//     outputFile2.open("Case1out2.csv");
    
//     outputFile0 << "x,f(x)\n";
//     outputFile1 << "x,f(x)\n";
//     outputFile2 << "x,f(x)\n";

//     int numValues = 1001;
//     double xStart = 0.0, xEnd = 1.0;
//     double range = xEnd - xStart;
//     double step = range / (numValues - 1);

//     for (int i = 0; i < numValues; ++i) {
//         T x = xStart + i * step;
//         T y0;
//         T y1;
//         T y2;
//         y0 = fConstant0(x);
//         y1 = fFunction1(x);
//         y2 = fFunction2(x);
//         outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
//         outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
//         outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
//     }
//     // Close the files
//     outputFile0.close();
//     outputFile1.close();
//     outputFile2.close();

// }
// /** Case1:
//  * 
//  * operations 1 and 2
// */
// TEST(DiffusionMixing, caseH) {

//     // parameters
//     T cWidth = 100e-6;
//     T cHeight = 100e-6;
//     T cLength = 2000e-6;
//     T cLength2 = 2000e-6 + 0.5*M_PI*150e-6;
//     T flowRate1 = 0.1e-9;
//     T flowRate2 = flowRate1 * 0.5;
//     T pecletNr1 = (flowRate1 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
//     T pecletNr2 = (flowRate2 / cHeight) / 1e-9; // (flowrate / height) / diffusivity
//     // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
//     int resolution = 1000;

//     // create necessary objects
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
//     std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

//     std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
//     std::vector<T> zeroSegmentedResult = {0};

//     sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

//     // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
//     constantFlowSections.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)});
//     constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)});

//     // perform analytical solution for constant input
//     auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr1, constantFlowSections);

//     functionFlowSections1.push_back({0.0, 1.0, 2.0, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel
//     functionFlowSections2.push_back({0.0, 1.0, 2.0, 0.5, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel

//     // perform analytical solution for function input
//     auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections1, zeroFunction);
//     auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr2, functionFlowSections2, zeroFunction);

//     // generate resulting csv files
//     std::ofstream outputFile0;
//     std::ofstream outputFile1;
//     std::ofstream outputFile2;

//     outputFile0.open("Case1out0.csv");
//     outputFile1.open("Case1out1.csv");
//     outputFile2.open("Case1out2.csv");
    
//     outputFile0 << "x,f(x)\n";
//     outputFile1 << "x,f(x)\n";
//     outputFile2 << "x,f(x)\n";

//     int numValues = 1001;
//     double xStart = 0.0, xEnd = 1.0;
//     double range = xEnd - xStart;
//     double step = range / (numValues - 1);

//     for (int i = 0; i < numValues; ++i) {
//         T x = xStart + i * step;
//         T y0;
//         T y1;
//         T y2;
//         y0 = fConstant0(x);
//         y1 = fFunction1(x);
//         y2 = fFunction2(x);
//         outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
//         outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
//         outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
//     }
//     // Close the files
//     outputFile0.close();
//     outputFile1.close();
//     outputFile2.close();

// }


/** Case2:
 * 
 * operations 3 and 4
*/
TEST(DiffusionMixing, case2) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = 2000e-6;
    T cLength2 = sqrt(5e-6);
    T flowRate3 = 4e-10;
    T flowRate4 = 3.16267e-10;
    T flowRate5 = 4.41997e-11;
    T flowRate6 = 3.95334e-11;
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr5 = (flowRate5 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
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
    constantFlowSections.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // top inflow channel (double the width)
    constantFlowSections.push_back({0.25, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections.push_back({0.0, 0.25, 1.0, 0.0, 0.5, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel

    // perform analytical solution for constant input
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, cWidth, resolution, pecletNr3, constantFlowSections);

    functionFlowSections3.push_back({0.0, 1.0, 11.1317, 0.901167*11.1317, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // top outflow channel
    functionFlowSections2.push_back({0.0, 1.0, 9.04984, 0.790667*9.04984, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // middle outflow channel
    functionFlowSections1.push_back({0.0, 1.0, 0.5*2.52951, 0.0, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // bottom outflow channel

    // perform analytical solution for function input
    auto [fFunctionC, segmentedResultFunctionC, a_0_FunctionC] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunctionD, segmentedResultFunctionD, a_0_FunctionD] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunctionE, segmentedResultFunctionE, a_0_FunctionE] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, 2*cWidth, resolution, pecletNr4, functionFlowSections1, zeroFunction);
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr5, functionFlowSections2, zeroFunction);
    auto [fFunction3, segmentedResultFunction3, a_0_Function3] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength2, cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);


    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;
    std::ofstream outputFile3;
    std::ofstream outputFileC;
    std::ofstream outputFileD;
    std::ofstream outputFileE;

    outputFile0.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-B.csv");
    outputFileC.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-C.csv");
    outputFileD.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-D.csv");
    outputFileE.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-E.csv");
    outputFile1.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-F.csv");
    outputFile2.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-G.csv");
    outputFile3.open("../../mmft-CFD-cases/DiffusiveMixing/Xi-mixer/Case2Test-H.csv");
    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";
    outputFile3 << "x,f(x)\n";
    outputFileC << "x,f(x)\n";
    outputFileD << "x,f(x)\n";
    outputFileE << "x,f(x)\n";

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
        T yC;
        T yD;
        T yE;
        y0 = fConstant0(x);
        y1 = fFunction1(x);
        y2 = fFunction2(x);
        y3 = fFunction3(x);
        yC = fFunctionC(x);
        yD = fFunctionD(x);
        yE = fFunctionE(x);
        outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
        outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
        outputFile3 << std::setprecision(4) << x << "," << y3 << "\n"; 
        outputFileC << std::setprecision(4) << x << "," << yC << "\n"; 
        outputFileD << std::setprecision(4) << x << "," << yD << "\n"; 
        outputFileE << std::setprecision(4) << x << "," << yE << "\n";
    }
    // Close the file
    outputFile0.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();
    outputFileC.close();
    outputFileD.close();
    outputFileE.close();

}

/** Case3:
 * 
 * operation 5 twice
*/
// TEST(DiffusionMixing, case3) {

//     // parameters
//     T cWidth = 100e-6;
//     T cHeight = 100e-6;
//     T cLength = sqrt(1e-3*1e-3 + 1e-3*1e-3);
//     T flowRate0 = 3.53553e-09;
//     T flowRate1 = 3.53553e-09;
//     T flowRate2 = 7.85674e-10;
//     T flowRate3 = 6.28539e-09;
//     T flowRate4 = 7.85674e-10;
//     T flowRate5 = 6.28539e-09;
//     T flowRate6 = 6.28539e-09;
//     T flowRate7 = 7.85674e-10;
//     T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
//     int resolution = 1000;

//     // create necessary objects
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections0;
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections1;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
//     std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

//     std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
//     std::vector<T> zeroSegmentedResult = {0};

//     sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

//     // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
//     // The constant flow sections that enter and flow through channels 3 and 5
//     constantFlowSections0.push_back({0.0, 0.4375, 1.0, 0.222222, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     constantFlowSections0.push_back({0.4375, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, 2*cWidth, resolution, pecletNr3, constantFlowSections0);

//     // The constant flow section of 0.0 concentration that enters through channels 2 and 4, this is ultimately just a function of 0.0 concentration
//     constantFlowSections1.push_back({0.0, 1.0, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     auto [fConstant1, segmentedResultConstant1, a_0_Constant1] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, cWidth, resolution, pecletNr2, constantFlowSections1);

//     // The top 0.125 part of channel 5 enters channel 7
//     functionFlowSections1.push_back({0.0, 1.0, 4.0, 0.875, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // channel 7

//     // The flow from channel 4 and the bottom 0.875 part of channel 5 enter channel 6
//     functionFlowSections2.push_back({0.0, 0.125, 1.0, 0.0, T(0.0), fConstant1, segmentedResultConstant1, a_0_Constant1}); // channel 6
//     functionFlowSections2.push_back({0.125, 1.0, 1.0, -0.125, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0});
    
//     // perform analytical solution for function input
//     auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr7, functionFlowSections1, zeroFunction);
//     auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, 2*cWidth, resolution, pecletNr6, functionFlowSections2, zeroFunction);


//     // generate resulting csv files
//     std::ofstream outputFile0;
//     std::ofstream outputFile1;
//     std::ofstream outputFile2;

//     //outputFile0.open("Case3out0.csv");
//     outputFile1.open("Case3out1.csv");  // Channel 7 output
//     outputFile2.open("Case3out2.csv");  // Channel 6 output
    
//     outputFile0 << "x,f(x)\n";
//     outputFile1 << "x,f(x)\n";
//     outputFile2 << "x,f(x)\n";

//     int numValues = 101;
//     double xStart = 0.0, xEnd = 1.0;
//     double range = xEnd - xStart;
//     double step = range / (numValues - 1);

//     for (int i = 0; i < numValues; ++i) {
//         T x = xStart + i * step;
//         T y0;
//         T y1;
//         T y2;
//         //y0 = fConstant0(x);
//         y1 = fFunction1(x);
//         y2 = fFunction2(x);
//         //outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
//         outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
//         outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
//     }
//     // Close the file
//     //outputFile0.close();
//     outputFile1.close();
//     outputFile2.close();

// }

/** Case3:
 * 
 * operation 5 four times
*/
TEST(DiffusionMixing, case3_1) {

    // parameters
    T cWidth = 100e-6;
    T cHeight = 100e-6;
    T cLength = sqrt(1e-3*1e-3 + 1e-3*1e-3);
    T flowRate0 = 1e-10;
    T flowRate1 = 1e-10;
    T flowRate2 = 2.2222e-11;
    T flowRate3 = 1.7778e-10;
    T flowRate4 = 2.2222e-11;
    T flowRate5 = 1.7778e-10;
    T flowRate6 = 1.7778e-10;
    T flowRate7 = 2.2222e-11;
    T flowRate8 = 1.7778e-10;
    T flowRate9 = 2.2222e-11;
    T flowRate10 = 2.2222e-11;
    T flowRate11 = 1.7778e-10;
    T flowRate12 = 2.2222e-11;
    T flowRate13 = 1.7778e-10;
    T flowRate14 = 1e-10;
    T flowRate15 = 1e-10;
    T pecletNr2 = (flowRate2 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr3 = (flowRate3 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    T pecletNr15 = (flowRate15 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
    // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 100;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections0;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsE;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsF;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections0;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections1;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsI;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsJ;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections2;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections3;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsM;
    std::vector<sim::FlowSectionInput<T>> functionFlowSectionsN;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections4;
    std::vector<sim::FlowSectionInput<T>> functionFlowSections5;
    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    /**
     * C
    */
    // The constant flow section of 0.0 concentration that enters through channels 2 and 4, this is ultimately just a function of 0.0 concentration
    constantFlowSections1.push_back({0.0, 1.0, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant1, segmentedResultConstant1, a_0_Constant1] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, cWidth, resolution, pecletNr2, constantFlowSections1);

    /**
     * D
    */
    // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
    // The constant flow sections that enter and flow through channels 3 and 5
    constantFlowSections0.push_back({0.0, 0.4375, 1.0, 0.22222, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections0.push_back({0.4375, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    auto [fConstant0, segmentedResultConstant0, a_0_Constant0] = diffusionMixingModelTest.getAnalyticalSolutionConstant(2*cLength, 2*cWidth, resolution, pecletNr3, constantFlowSections0);

    /**
     * E
    */
    // The flow from channel 4 and the bottom 0.875 part of channel 5 enter channel 6
    functionFlowSectionsE.push_back({0.0, 0.125, 0.25, 0.0, T(0.0), fConstant1, segmentedResultConstant1, a_0_Constant1}); // channel 6
    functionFlowSectionsE.push_back({0.125, 1.0, 1.0, -0.125, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0});
    auto [fFunctionE, segmentedResultFunctionE, a_0_FunctionE] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, 2*cWidth, resolution, pecletNr6, functionFlowSectionsE, zeroFunction);

    /**
     * F
    */
    // The top 0.125 part of channel 5 enters channel 7
    functionFlowSectionsF.push_back({0.0, 1.0, 4.0*2, 0.875*8, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // channel 7
    auto [fFunctionF, segmentedResultFunctionF, a_0_FunctionF] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr7, functionFlowSectionsF, zeroFunction);

    /**
     * G
    */
    // The flow from channel 4 and the bottom 0.875 part of channel 5 enter channel 6
    functionFlowSections1.push_back({0.0, 0.125, 0.25, 0.0, T(0.0), fConstant1, segmentedResultConstant1, a_0_Constant1}); // channel 6
    functionFlowSections1.push_back({0.125, 1.0, 1.0, -0.125, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0});
    auto [fFunction1, segmentedResultFunction1, a_0_Function1] = diffusionMixingModelTest.getAnalyticalSolutionFunction(2*cLength, 2*cWidth, resolution, pecletNr6, functionFlowSections1, zeroFunction);

    /**
     * H
    */
    // The top 0.125 part of channel 5 enters channel 7
    functionFlowSections0.push_back({0.0, 1.0, 4.0*2, 0.875*8, T(0.0), fConstant0, segmentedResultConstant0, a_0_Constant0}); // channel 7
    auto [fFunction0, segmentedResultFunction0, a_0_Function0] = diffusionMixingModelTest.getAnalyticalSolutionFunction(2*cLength, cWidth, resolution, pecletNr7, functionFlowSections0, zeroFunction);

    /**
     * I
    */
    // Flow into channel 10 generated from channel 8
    functionFlowSectionsI.push_back({0.0, 1.0, 4.0*2, 0.0, T(0.0), fFunction1, segmentedResultFunction1, a_0_Function1});
    auto [fFunctionI, segmentedResultFunctionI, a_0_FunctionI] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr6, functionFlowSectionsI, zeroFunction);

    /**
     * J
    */
    // Flow into channel 11 generated from channel 8+9
    functionFlowSectionsJ.push_back({0.0, 0.875, 1.0, 0.125, T(0.0), fFunction1, segmentedResultFunction1, a_0_Function1});
    functionFlowSectionsJ.push_back({0.875, 1.0, 0.125, 0.0, T(0.0), fFunction0, segmentedResultFunction0, a_0_Function0});
    auto [fFunctionJ, segmentedResultFunctionJ, a_0_FunctionJ] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, 2*cWidth, resolution, pecletNr6, functionFlowSectionsJ, zeroFunction);


    /**
     * K
    */
    // Flow into channel 10 generated from channel 8
    functionFlowSections2.push_back({0.0, 1.0, 4.0*2, 0.0, T(0.0), fFunction1, segmentedResultFunction1, a_0_Function1});
    auto [fFunction2, segmentedResultFunction2, a_0_Function2] = diffusionMixingModelTest.getAnalyticalSolutionFunction(2*cLength, cWidth, resolution, pecletNr6, functionFlowSections2, zeroFunction);

    /**
     * L
    */
    // Flow into channel 11 generated from channel 8+9
    functionFlowSections3.push_back({0.0, 0.875, 1.0, 0.125, T(0.0), fFunction1, segmentedResultFunction1, a_0_Function1});
    functionFlowSections3.push_back({0.875, 1.0, 0.25, 0.0, T(0.0), fFunction0, segmentedResultFunction0, a_0_Function0});
    auto [fFunction3, segmentedResultFunction3, a_0_Function3] = diffusionMixingModelTest.getAnalyticalSolutionFunction(2*cLength, 2*cWidth, resolution, pecletNr6, functionFlowSections3, zeroFunction);

    /**
     * M
    */
    // Flow into channel 15 generated from channel 12+13
    functionFlowSectionsN.push_back({0.0, 0.222222, 0.222222, 0.0, T(0.0), fFunction2, segmentedResultFunction2, a_0_Function2});
    functionFlowSectionsN.push_back({0.222222, 1.0, 0.888889*2, 0.0, T(0.0), fFunction3, segmentedResultFunction3, a_0_Function3});
    auto [fFunctionN, segmentedResultFunctionN, a_0_FunctionN] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr15, functionFlowSectionsN, zeroFunction);

    /**
     * N
    */
    // Flow into channel 14 generated from channel 13
    functionFlowSectionsM.push_back({0.0, 1.0, 0.888889*2, 0.4375*2, T(0.0), fFunction3, segmentedResultFunction3, a_0_Function3});
    auto [fFunctionM, segmentedResultFunctionM, a_0_FunctionM] = diffusionMixingModelTest.getAnalyticalSolutionFunction(0.0, cWidth, resolution, pecletNr15, functionFlowSectionsM, zeroFunction);

    /**
     * O
    */
    // Flow into channel 14 generated from channel 12+13
    functionFlowSections4.push_back({0.0, 1.0, 0.888889*2, 0.4375*2, T(0.0), fFunction3, segmentedResultFunction3, a_0_Function3});
    auto [fFunction4, segmentedResultFunction4, a_0_Function4] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr15, functionFlowSections4, zeroFunction);

    /**
     * P
    */
    // Flow into channel 15 generated from channel 13
    functionFlowSections5.push_back({0.0, 0.222222, 0.222222, 0.0, T(0.0), fFunction2, segmentedResultFunction2, a_0_Function2});
    functionFlowSections5.push_back({0.222222, 1.0, 0.888889*2, 0.0, T(0.0), fFunction3, segmentedResultFunction3, a_0_Function3});
    auto [fFunction5, segmentedResultFunction5, a_0_Function5] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr15, functionFlowSections5, zeroFunction);

    // perform analytical solution for function input

    // generate resulting csv files
    //std::ofstream outputFile0;
    std::ofstream outputFileC;
    std::ofstream outputFileD;
    std::ofstream outputFileE;
    std::ofstream outputFileF;
    std::ofstream outputFileG;
    std::ofstream outputFileH;
    std::ofstream outputFileI;
    std::ofstream outputFileJ;
    std::ofstream outputFileK;
    std::ofstream outputFileL;
    std::ofstream outputFileM;
    std::ofstream outputFileN;
    std::ofstream outputFileO;
    std::ofstream outputFileP;

    //outputFile0.open("Case3out0.csv");
    outputFileC.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-C.csv");  // Channel 4 output
    outputFileD.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-D.csv");  // Channel 5 output
    outputFileE.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-E.csv");  
    outputFileF.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-F.csv");  
    outputFileG.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-G.csv");  // Channel 8 output
    outputFileH.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-H.csv");  // Channel 9 output
    outputFileI.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-I.csv");
    outputFileJ.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-J.csv");
    outputFileK.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-K.csv");  // Channel 12 output
    outputFileL.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-L.csv");  // Channel 13 output
    outputFileM.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-M.csv");
    outputFileN.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-N.csv");
    outputFileO.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-O.csv");  // Channel 14 output
    outputFileP.open("../../mmft-CFD-cases/DiffusiveMixing/grid-mixer/Case3Test-P.csv");  // Channel 15 output
    
    //outputFile0 << "x,f(x)\n";
    outputFileC << "x,f(x)\n";
    outputFileD << "x,f(x)\n";
    outputFileE << "x,f(x)\n";
    outputFileF << "x,f(x)\n";
    outputFileG << "x,f(x)\n";
    outputFileH << "x,f(x)\n";
    outputFileI << "x,f(x)\n";
    outputFileJ << "x,f(x)\n";
    outputFileK << "x,f(x)\n";
    outputFileL << "x,f(x)\n";
    outputFileM << "x,f(x)\n";
    outputFileN << "x,f(x)\n";
    outputFileO << "x,f(x)\n";
    outputFileP << "x,f(x)\n";

    int numValues = 1001;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        //T y0;
        T yC;
        T yD;
        T yE;
        T yF;
        T yG;
        T yH;
        T yI;
        T yJ;
        T yK;
        T yL;
        T yM;
        T yN;
        T yO;
        T yP;
        //y0 = fConstant0(x);
        yC = fConstant1(x);
        yD = fConstant0(x);
        yE = fFunctionE(x);
        yF = fFunctionF(x);
        yG = fFunction1(x);
        yH = fFunction0(x);
        yI = fFunctionI(x);
        yJ = fFunctionJ(x);
        yK = fFunction2(x);
        yL = fFunction3(x);
        yM = fFunctionN(x);
        yN = fFunctionM(x);
        yO = fFunction5(x);
        yP = fFunction4(x);
        //outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
        outputFileC << std::setprecision(4) << x << "," << yC << "\n"; 
        outputFileD << std::setprecision(4) << x << "," << yD << "\n"; 
        outputFileE << std::setprecision(4) << x << "," << yE << "\n"; 
        outputFileF << std::setprecision(4) << x << "," << yF << "\n"; 
        outputFileG << std::setprecision(4) << x << "," << yG << "\n"; 
        outputFileH << std::setprecision(4) << x << "," << yH << "\n"; 
        outputFileI << std::setprecision(4) << x << "," << yI << "\n"; 
        outputFileJ << std::setprecision(4) << x << "," << yJ << "\n"; 
        outputFileK << std::setprecision(4) << x << "," << yK << "\n"; 
        outputFileL << std::setprecision(4) << x << "," << yL << "\n"; 
        outputFileM << std::setprecision(4) << x << "," << yM << "\n"; 
        outputFileN << std::setprecision(4) << x << "," << yN << "\n"; 
        outputFileO << std::setprecision(4) << x << "," << yO << "\n"; 
        outputFileP << std::setprecision(4) << x << "," << yP << "\n"; 
    }
    // Close the file
    //outputFile0.close();
    outputFileC.close();
    outputFileD.close();
    outputFileE.close();
    outputFileF.close();
    outputFileG.close();
    outputFileH.close();
    outputFileI.close();
    outputFileJ.close();
    outputFileK.close();
    outputFileL.close();
    outputFileM.close();
    outputFileN.close();
    outputFileO.close();
    outputFileP.close();

}

/** Case4:
 * 
 * operations 1, 3 and 6
*/
// TEST(DiffusionMixing, case4) {

//     // parameters
//     T cWidth = 100e-6;
//     T cHeight = 100e-6;
//     T cLength1 = 2e-3;
//     T cLength2 = sqrt(2e-3*2e-3 + 1e-3*1e-3);
//     T cLength3 = sqrt(2e-3*2e-3 + 2e-3*2e-3);
//     T flowRate0 = 3.23735e-10;
//     T flowRate1 = 3.23735e-10;
//     T flowRate2 = 3.23735e-10;
//     T flowRate3 = 3.23735e-10;
//     T flowRate4 = 6.47446e-10;
//     T flowRate5 = 6.47446e-10;
//     T flowRate6 = 1.29489e-09;
//     T flowRate7 = 1.5942e-09;
//     T flowRate8 = 1.5942e-09;
//     T flowRate9 = 1.89351e-09;
//     T flowRate10 = 6.07277e-10;
//     T flowRate11 = 6.78956e-10;
//     T flowRate12 = 6.07277e-10;
//     T pecletNr0 = (flowRate0 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr4 = (flowRate4 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr6 = (flowRate6 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr7 = (flowRate7 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr9 = (flowRate9 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr10 = (flowRate10 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     T pecletNr11 = (flowRate11 / cHeight) / 1e-8; // (flowrate / height) / diffusivity
//     // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
//     int resolution = 1000;

//     // create necessary objects
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections4;
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections5;
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections6;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections7;
//     std::vector<sim::FlowSectionInput<T>> functionFlowSections8;
//     std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

//     std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
//     std::vector<T> zeroSegmentedResult = {0};

//     sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

//     // start outflowChannel, end outFlowchannel, stretchFactor, start inflowChannel, concentration
//     // The constant flow sections that enter channel 4
//     constantFlowSections4.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     constantFlowSections4.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     auto [fConstant4, segmentedResultConstant4, a_0_Constant4] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections4);

//     // The constant flow sections that enter channel 5
//     constantFlowSections5.push_back({0.0, 0.5, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     constantFlowSections5.push_back({0.5, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     auto [fConstant5, segmentedResultConstant5, a_0_Constant5] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength3, cWidth, resolution, pecletNr4, constantFlowSections5);

//     // The function flow sections that enter channel 6
//     functionFlowSections6.push_back({0.0, 0.5, 0.5, 0.0, T(0.0), fConstant4, segmentedResultConstant4, a_0_Constant4});
//     functionFlowSections6.push_back({0.5, 1.0, 0.5, 0.0, T(0.0), fConstant5, segmentedResultConstant5, a_0_Constant5});
//     auto [fFunction6, segmentedResultFunction6, a_0_Function6] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr6, functionFlowSections6, zeroFunction);

//     // The constant flow sections that enter channel 9
//     constantFlowSections9.push_back({0.0, 0.320715, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 10
//     constantFlowSections9.push_back({0.320715, 0.679285, 1.0, 0.0, 0.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 11
//     constantFlowSections9.push_back({0.679285, 1.0, 1.0, 0.0, 1.0, zeroFunction, zeroSegmentedResult, T(0.0)}); // channel 12
//     auto [fFunction9, segmentedResultFunction9, a_0_Function9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength1, cWidth, resolution, pecletNr9, constantFlowSections9);

//     // The function flow sections that enter channel 7
//     functionFlowSections7.push_back({0.0, 0.406126, 0.812252, 0.0, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6}); // channel 6
//     functionFlowSections7.push_back({0.406126, 1.0, 1.18775, 0.5, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
//     auto [fFunction7, segmentedResultFunction7, a_0_Function7] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections7, zeroFunction);

//     // The function flow sections that enter channel 8
//     functionFlowSections8.push_back({0.0, 0.593874, 1.18775, 0.0, T(0.0), fFunction9, segmentedResultFunction9, a_0_Function9});
//     functionFlowSections8.push_back({0.593874, 1.0, 0.812252, 0.5, T(0.0), fFunction6, segmentedResultFunction6, a_0_Function6});
//     auto [fFunction8, segmentedResultFunction8, a_0_Function8] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength1, cWidth, resolution, pecletNr7, functionFlowSections8, zeroFunction);

//     // generate resulting csv files
//     //std::ofstream outputFile0;
//     std::ofstream outputFile1;
//     std::ofstream outputFile2;

//     //outputFile0.open("Case3out0.csv");
//     outputFile1.open("Case4out1.csv");  // Channel 14 output
//     outputFile2.open("Case4out2.csv");  // Channel 15 output
    
//     //outputFile0 << "x,f(x)\n";
//     outputFile1 << "x,f(x)\n";
//     outputFile2 << "x,f(x)\n";

//     int numValues = 101;
//     double xStart = 0.0, xEnd = 1.0;
//     double range = xEnd - xStart;
//     double step = range / (numValues - 1);

//     for (int i = 0; i < numValues; ++i) {
//         T x = xStart + i * step;
//         //T y0;
//         T y1;
//         T y2;
//         //y0 = fConstant0(x);
//         y1 = fFunction7(x);
//         y2 = fFunction8(x);
//         //outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
//         outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
//         outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
//     }
//     // Close the file
//     //outputFile0.close();
//     outputFile1.close();
//     outputFile2.close();

// }

/** Case3:
 * 
 * operation 5 four times
*/
TEST(DiffusionMixing, caseJeonEtAl) {

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
    auto cWidth = 50e-6;
    auto cHeight = 100e-6;
    auto cLength = 570e-10;  // Channel length

    // Concentrations (from Jeon et al. read out from simulations)
    T c0 = 0.0;
    T c1 = 0.1784;
    T c2 = 0.3991;
    T c3 = 0.5778;
    T c4 = 0.6459;
    T c5 = 0.5778;
    T c6 = 0.3991;
    T c7 = 0.1784;
    T c8 = 0.0;

    /**
     * 1 mm/s
    */
    // T c0 = 0.0;
    // T c1 = 0.2167;
    // T c2 = 0.4115;
    // T c3 = 0.5513;
    // T c4 = 0.6021;
    // T c5 = 0.5513;
    // T c6 = 0.4115;
    // T c7 = 0.2267;
    // T c8 = 0.0;

    T diffusivity = 5e-10; // m^2/s

    // Flowrates (calculated using 1D simulation)

    T flowRate0 = 5.44862e-12; // channel 87
    T flowRate1 = 5.4081e-12; // channel 88
    T flowRate2 = 5.58838e-12; // channel 89
    T flowRate3 = 5.69198e-12; // channel 90
    T flowRate4 = 5.72583e-12; // channel 91
    T flowRate5 = 5.69198e-12; // channel 92
    T flowRate6 = 5.58838e-12; // channel 93
    T flowRate7 = 5.4081e-12; // channel 94
    T flowRate8 = 5.44862e-12; // channel 95

    T flowRate9 = 4.93353e-11; // channel 96
    T flowRate10 = 5.13294e-11; // channel 97
    T flowRate11 = 4.93353e-11; // channel 98
    T flowRate12 = 1.5e-10; // channel 99

    /**
     * 1mm/s
    */
    // T flowRate0 = 5.44862e-13; // channel 87
    // T flowRate1 = 5.4081e-13; // channel 88
    // T flowRate2 = 5.58838e-13; // channel 89
    // T flowRate3 = 5.69198e-13; // channel 90
    // T flowRate4 = 5.72583e-13; // channel 91
    // T flowRate5 = 5.69198e-13; // channel 92
    // T flowRate6 = 5.58838e-13; // channel 93
    // T flowRate7 = 5.4081e-13; // channel 94
    // T flowRate8 = 5.44862e-13; // channel 95

    // T flowRate9 = 1.64451e-12; // channel 96
    // T flowRate10 = 1.71098e-12; // channel 97
    // T flowRate11 = 1.64451e-12; // channel 98
    // T flowRate12 = 5e-12; // channel 99


    T pecletNr9 = (flowRate9 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr10 = (flowRate10 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr11 = (flowRate11 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    T pecletNr12 = (flowRate12 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
    // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
    int resolution = 1000;

    // create necessary objects
    std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
    std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

    std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

    std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

    std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
    std::vector<T> zeroSegmentedResult = {0};

    sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

    // inflow into channel 9 (aq. to channel 96 in main.cpp)
    constantFlowSections9.push_back({0.0, 0.331277, 1.0, 0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections9.push_back({0.331277, 0.660117, 0.328839, -0.331277, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections9.push_back({0.660117, 1.0, 1.0, -0.660117, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    // inflow into channel 10 (aq. to channel 97 in main.cpp)
    constantFlowSections10.push_back({0.0, 0.332338, 0.332338, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections10.push_back({0.332338, 0.666925, 0.334588, -0.332338, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections10.push_back({0.666925, 1.0, 0.333075, -0.666925, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    // inflow into channel 11 (aq. to channel 98 in main.cpp)
    constantFlowSections11.push_back({0.0, 0.339695, 1.0, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    constantFlowSections11.push_back({0.339695, 0.668593, 0.328898, -0.339695, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
    constantFlowSections11.push_back({0.668593, 1.0, 1.0, -0.668593, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    
    auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
    auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
    auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(0.0, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

    // Flow into channel 10 generated from channel 8
    functionFlowSections12.push_back({0.0, 0.328231, 0.328231, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9});
    functionFlowSections12.push_back({0.328231, 0.670296, 0.342066, -0.328231, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10});
    functionFlowSections12.push_back({0.670296, 1.0,  0.329704, -0.670296, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11});
        
    auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(500e-6, 18*cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

    // perform analytical solution for function input

    // generate resulting csv files
    std::ofstream outputFile0;
    std::ofstream outputFile1;
    std::ofstream outputFile2;
    std::ofstream outputFile3;


    //
    outputFile0.open("Jeon_out0.csv");  // Channel 9 output
    outputFile1.open("Jeon_out1.csv");  // Channel 10 output
    outputFile2.open("Jeon_out2.csv");  // Channel 11 output
    outputFile3.open("Jeon_out3.csv");  // Channel 12 output

    
    outputFile0 << "x,f(x)\n";
    outputFile1 << "x,f(x)\n";
    outputFile2 << "x,f(x)\n";
    outputFile3 << "x,f(x)\n";

    int numValues = 101;
    double xStart = 0.0, xEnd = 1.0;
    double range = xEnd - xStart;
    double step = range / (numValues - 1);

    for (int i = 0; i < numValues; ++i) {
        T x = xStart + i * step;
        //y0 = fConstant0(x);
        T y0 = fConstant9(x);
        T y1 = fConstant10(x);
        T y2 = fConstant11(x);
        T y3 = fFunction12(x);
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

// TEST(DiffusionMixing, caseJeonEtAlFAKE) { // test with normalized values

//     //   0 1 2  3 4 5  6 7 8
//     //   | | |  | | |  | | |
//     //    \|/    \|/    \|/
//     //     9      10     11
//     //      \     |     /
//     //        \   |   /
//     //          \ | /
//     //            12
//     //            |


//     // parameters
//     auto cWidth = 50e-6;
//     auto cHeight = 100e-6;
//     auto cLength = 570e-10;  // Channel length

//     // Concentrations (from Jeon et al. read out from simulations)
//     T c0 = 0.0;
//     T c1 = 0.2051;
//     T c2 = 0.4068;
//     T c3 = 0.5582;
//     T c4 = 0.6145;
//     T c5 = 0.5594;
//     T c6 = 0.4107;
//     T c7 = 0.206;
//     T c8 = 0.0;

//     T diffusivity = 5e-10; // m^2/s

//     // Flowrates (calculated using 1D simulation)
//     T flowRate0 = 1.63103e-12; // channel 87
//     T flowRate1 = 1.61903e-12; // channel 88
//     T flowRate2 = 1.6734e-12; // channel 89
//     T flowRate3 = 1.70522e-12; // channel 90
//     T flowRate4 = 1.71676e-12; // channel 91
//     T flowRate5 = 1.709e-12; // channel 92
//     T flowRate6 = 1.67998e-12; // channel 93
//     T flowRate7 = 1.62658e-12; // channel 94
//     T flowRate8 = 1.63899e-12; // channel 95

//     T flowRate9 = 5e-12; // channel 96
//     T flowRate10 = 5e-12; // channel 97
//     T flowRate11 = 5e-12; // channel 98
//     T flowRate12 = 1.5e-11; // channel 99


//     T pecletNr9 = (flowRate9 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
//     T pecletNr10 = (flowRate10 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
//     T pecletNr11 = (flowRate11 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
//     T pecletNr12 = (flowRate12 / cHeight) / diffusivity; // (flowrate / height) / diffusivity
//     // ASSERT_NEAR(pecletNr, 30.0, 1e-7);
//     int resolution = 1000;

//     // create necessary objects
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections9;
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections10;
//     std::vector<sim::FlowSectionInput<T>> constantFlowSections11;

//     std::vector<sim::FlowSectionInput<T>> functionFlowSections12;

//     std::unordered_map<int, std::unique_ptr<sim::DiffusiveMixture<T>>> diffusiveMixtures;

//     std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
//     std::vector<T> zeroSegmentedResult = {0};

//     sim::DiffusionMixingModel<T> diffusionMixingModelTest = sim::DiffusionMixingModel<T>();

//     // inflow into channel 9 (aq. to channel 96 in main.cpp)
//     constantFlowSections9.push_back({0.0, 0.33, 0.33, 0, c0, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     constantFlowSections9.push_back({0.33, 0.66, 0.33, -0.33, c1, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     constantFlowSections9.push_back({0.66, 1.0, 0.33, -0.66, c2, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     // inflow into channel 10 (aq. to channel 97 in main.cpp)
//     constantFlowSections10.push_back({0.0, 0.33, 0.33, 0.0, c3, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     constantFlowSections10.push_back({0.33, 0.66, 0.33, -0.33, c4, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     constantFlowSections10.push_back({0.66, 1.0, 0.33, -0.66, c5, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     // inflow into channel 11 (aq. to channel 98 in main.cpp)
//     constantFlowSections11.push_back({0.0, 0.33, 0.33, 0.0, c6, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
//     constantFlowSections11.push_back({0.33, 0.66, 0.33, -0.33, c7, zeroFunction, zeroSegmentedResult, T(0.0)}); // middle inflow channel
//     constantFlowSections11.push_back({0.66, 1.0, 0.33, -0.66, c8, zeroFunction, zeroSegmentedResult, T(0.0)}); // bottom inflow channel
    
//     auto [fConstant9, segmentedResultConstant9, a_0_Constant9] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, 3*cWidth, resolution, pecletNr9, constantFlowSections9);
//     auto [fConstant10, segmentedResultConstant10, a_0_Constant10] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, 3*cWidth, resolution, pecletNr10, constantFlowSections10);
//     auto [fConstant11, segmentedResultConstant11, a_0_Constant11] = diffusionMixingModelTest.getAnalyticalSolutionConstant(cLength, 3*cWidth, resolution, pecletNr11, constantFlowSections11);

//     // Flow into channel 10 generated from channel 8
//     functionFlowSections12.push_back({0.0, 0.33, 0.33, 0.0, T(0.0), fConstant9, segmentedResultConstant9, a_0_Constant9});
//     functionFlowSections12.push_back({0.33, 0.66, 0.33, -1.0, T(0.0), fConstant10, segmentedResultConstant10, a_0_Constant10});
//     functionFlowSections12.push_back({0.66, 1.0,  0.33, -2.0, T(0.0), fConstant11, segmentedResultConstant11, a_0_Constant11});
        
//     auto [fFunction12, segmentedResultFunction12, a_0_Function12] = diffusionMixingModelTest.getAnalyticalSolutionFunction(cLength, cWidth, resolution, pecletNr12, functionFlowSections12, zeroFunction);

//     // perform analytical solution for function input

//     // generate resulting csv files
//     std::ofstream outputFile0;
//     std::ofstream outputFile1;
//     std::ofstream outputFile2;
//     std::ofstream outputFile3;


//     //
//     outputFile0.open("Jeon_out0.csv");  // Channel 9 output
//     outputFile1.open("Jeon_out1.csv");  // Channel 10 output
//     outputFile2.open("Jeon_out2.csv");  // Channel 11 output
//     outputFile3.open("Jeon_out3.csv");  // Channel 12 output

    
//     outputFile0 << "x,f(x)\n";
//     outputFile1 << "x,f(x)\n";
//     outputFile2 << "x,f(x)\n";
//     outputFile3 << "x,f(x)\n";

//     int numValues = 101;
//     double xStart = 0.0, xEnd = 1.0;
//     double range = xEnd - xStart;
//     double step = range / (numValues - 1);

//     for (int i = 0; i < numValues; ++i) {
//         T x = xStart + i * step;
//         //y0 = fConstant0(x);
//         T y0 = fConstant9(x);
//         T y1 = fConstant10(x);
//         T y2 = fConstant11(x);
//         T y3 = fFunction12(x);
//         outputFile0 << std::setprecision(4) << x << "," << y0 << "\n"; 
//         outputFile1 << std::setprecision(4) << x << "," << y1 << "\n"; 
//         outputFile2 << std::setprecision(4) << x << "," << y2 << "\n"; 
//         outputFile3 << std::setprecision(4) << x << "," << y3 << "\n";
//     }
//     // Close the file
//     outputFile0.close();
//     outputFile1.close();
//     outputFile2.close();
//     outputFile3.close();

// }

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