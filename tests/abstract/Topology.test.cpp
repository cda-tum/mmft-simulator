#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

#include "../test_helpers.h"
#include "../test_definitions.h"

using T = double;

class Topology : public test::definitions::TopologyTest<T> { };

/** Case 1:
 * 
 *  3 way node
 *  - 2 inflows
 *  - 1 outflow
*/
TEST_F(Topology, case1) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(0.0, 2e-3, true);
    auto node2 = network->addNode(1e-3, 1e-3, false);
    auto node3 = network->addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    
    this->setChannelPressure(c0, flowRate);
    this->setChannelPressure(c1, 0.5*flowRate);
    this->setChannelPressure(c2, 1.5*flowRate);
    
    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node2->getId());

    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.9269908, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).at(1).channelId, 1);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(1).flowRate, network->getChannel(1)->getFlowRate(), 1e-12);

}

/** Case 2:
 * 
 *  3 way node
 *  - 1 inflow
 *  - 2 outflows
*/
TEST_F(Topology, case2) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 1e-3, true);
    auto node1 = network->addNode(1e-3, 1e-3, false);
    auto node2 = network->addNode(2e-3, 0.0, true);
    auto node3 = network->addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    this->setChannelPressure(c0, 2*flowRate);
    this->setChannelPressure(c1, flowRate);
    this->setChannelPressure(c2, flowRate);

    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node1->getId());

    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 0.5, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, 0.5*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.5, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network->getChannel(0)->getFlowRate(), 1e-12);

}

/** Case 3:
 * 
 *  4 way node
 *  - 1 inflow
 *  - 3 outflows
*/
TEST_F(Topology, case3) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 1e-3, true);
    auto node1 = network->addNode(1e-3, 1e-3);
    auto node2 = network->addNode(2e-3, 0.0, true);
    auto node3 = network->addNode(2e-3, 1e-3, true);
    auto node4 = network->addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    this->setChannelPressure(c0, 3*flowRate);
    this->setChannelPressure(c1, 0.5*flowRate);
    this->setChannelPressure(c2, 1.5*flowRate);
    this->setChannelPressure(c3, flowRate);

    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);
    this->setChannelResistance(c3, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node1->getId());

    T r1 = network->getChannel(1)->getFlowRate()/network->getChannel(0)->getFlowRate();
    T r2 = network->getChannel(2)->getFlowRate()/network->getChannel(0)->getFlowRate();
    T r3 = network->getChannel(3)->getFlowRate()/network->getChannel(0)->getFlowRate();
    
    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 3.1415927, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.0, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 0.7853982, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 3);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, r1, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, r1*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, r1, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, r1+r2, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, r2*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, r1+r2, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, r3*network->getChannel(0)->getFlowRate(), 1e-12);

}

/** Case 4:
 * 
 *  4 way node
 *  - 2 adjacent inflows
 *  - 2 adjacent outflow
*/
TEST_F(Topology, case4) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(0.0, 2e-3, true);
    auto node2 = network->addNode(1e-3, 1e-3, false);
    auto node3 = network->addNode(2e-3, 0.0, true);
    auto node4 = network->addNode(2e-3, 2e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node1->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node2->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    this->setChannelPressure(c0, flowRate);
    this->setChannelPressure(c1, flowRate);
    this->setChannelPressure(c2, 0.5*flowRate);
    this->setChannelPressure(c3, 1.5*flowRate);

    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);
    this->setChannelResistance(c3, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node2->getId());
    
    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 2.3561945, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(1).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(1).radialAngle, 3.9269908, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 5.4977871, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 0.7853982, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(2).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).sectionEnd, 0.5, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(2).at(0).flowRate, 0.5*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.5, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.5*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network->getChannel(1)->getFlowRate(), 1e-12);

}

/** Case 5:
 * 
 *  4 way node
 *  - 3 inflows
 *  - 1 outflow
*/
TEST_F(Topology, case5) {
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(0.0, 1e-3, true);
    auto node2 = network->addNode(0.0, 2e-3, true);
    auto node3 = network->addNode(1e-3, 1e-3, false);
    auto node4 = network->addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node1->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    this->setChannelPressure(c0, flowRate);
    this->setChannelPressure(c1, 0.5*flowRate);
    this->setChannelPressure(c2, 1.5*flowRate);
    this->setChannelPressure(c3, 3*flowRate);

    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);
    this->setChannelResistance(c3, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node3->getId());

    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 2);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 3);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(1).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(1).radialAngle, 3.1415927, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(2).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(2).radialAngle, 3.9269908, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 1);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 3);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 1);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, network->getChannel(1)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(2).channelId, 2);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(2).flowRate, network->getChannel(2)->getFlowRate(), 1e-12);

}

/** Case 6:
 * 
 *  4 way node (saddlepoint)
 *  - 2 inflows opposed
 *  - 2 outflows opposed
*/
TEST_F(Topology, case6) {    
    // define network
    auto network = arch::Network<T>::createNetwork();

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(0.0, 1e-3, true);
    auto node2 = network->addNode(0.0, 2e-3, true);
    auto node3 = network->addNode(1e-3, 1e-3, false);
    auto node4 = network->addNode(2e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 30e-6;
    auto cLength = 1000e-6;
    T flowRate = 3e-11;
    
    auto c0 = network->addRectangularChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node3->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node3->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    this->setChannelPressure(c0, flowRate);
    this->setChannelPressure(c1, 0.5*flowRate);
    this->setChannelPressure(c2, 1.5*flowRate);
    this->setChannelPressure(c3, 2*flowRate);

    this->setChannelResistance(c0, 1);
    this->setChannelResistance(c1, 1);
    this->setChannelResistance(c2, 1);
    this->setChannelResistance(c3, 1);

    // mixing model
    sim::DiffusionMixingModel<T> mixingModel;
    mixingModel.topologyAnalysis(network.get(), node3->getId());

    EXPECT_EQ(mixingModel.getConcatenatedFlows().size(), 4);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(2).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(3).size(), 1);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(0).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(0).at(0).radialAngle, 0.0, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(1).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(1).at(0).radialAngle, 2.3561945, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(2).at(0).inFlow, false);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(2).at(0).radialAngle, 3.1415927, 1e-7);
    EXPECT_EQ(mixingModel.getConcatenatedFlows().at(3).at(0).inFlow, true);
    EXPECT_NEAR(mixingModel.getConcatenatedFlows().at(3).at(0).radialAngle, 3.9269908, 1e-7);

    EXPECT_EQ(mixingModel.getOutflowDistributions().size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).size(), 2);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(0).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).sectionEnd, 0.8, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(0).flowRate, 0.8*network->getChannel(0)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(3).at(1).channelId, 2);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionStart, 1./5., 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(3).at(1).flowRate, (4./5.)*network->getChannel(2)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).at(0).channelId, 2);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionStart, 0.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).sectionEnd, 1./5., 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(0).flowRate, (1./5.)*network->getChannel(2)->getFlowRate(), 1e-12);
    EXPECT_EQ(mixingModel.getOutflowDistributions().at(1).at(1).channelId, 0);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionStart, 0.8, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).sectionEnd, 1.0, 1e-12);
    EXPECT_NEAR(mixingModel.getOutflowDistributions().at(1).at(1).flowRate, 0.2*network->getChannel(0)->getFlowRate(), 1e-12);

}
