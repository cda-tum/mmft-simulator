#include "NodalAnalysis.h"

#include <iostream>
#include <memory>
#include <vector>

#include "Channel.h"
#include "FlowRatePump.h"
#include "Node.h"
#include "PressurePump.h"
#include "gtest/gtest.h"

using namespace std;
using namespace arch;
using namespace nodal;

TEST(NodalAnalysis, testNetwork1) {
    // define network
    // nodes
    Node nodeGround(-1);
    Node node0(0);
    Node node1(1);
    Node node2(2);
    Node node3(3);
    unordered_map<int, INode *> nodes{{nodeGround.getId(), &nodeGround}, {node0.getId(), &node0}, {node1.getId(), &node1}, {node2.getId(), &node2}, {node3.getId(), &node3}};

    // pressure pump (voltage sources)
    PressurePump v0(0, &nodeGround, &node0, 1.0);
    unordered_map<int, PressurePump *> pressurePumps{{v0.getId(), &v0}};

    // flowRate pump (current source)
    FlowRatePump i0(0, &nodeGround, &node2, 1.0);
    unordered_map<int, FlowRatePump *> flowRatePumps{{i0.getId(), &i0}};

    // channels
    Channel c1(1, &node0, &node1, 5);
    Channel c2(2, &node1, &nodeGround, 10);
    Channel c3(3, &node2, &node3, 5);
    Channel c4(4, &node3, &nodeGround, 10);
    unordered_map<int, Channel *> channels{{c1.getId(), &c1}, {c2.getId(), &c2}, {c3.getId(), &c3}, {c4.getId(), &c4}};

    // compute network
    std::unordered_map<int, std::tuple<nodal::INode *, int>> nodesNA;
    int matrixId = 0;
    for (auto &[nodeId, node] : nodes) {
        // if node is ground node set the matrixId to -1
        if (nodeId == nodeGround.getId()) {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, -1));
        } else {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channelsNA;
    for (auto &[key, channel] : channels) {
        channelsNA.push_back(channel);
    }
    std::vector<arch::FlowRatePump *> flowRatePumpsNA;
    for (auto &[key, pump] : flowRatePumps) {
        flowRatePumpsNA.push_back(pump);
    }
    std::vector<arch::PressurePump *> pressurePumpsNA;
    for (auto &[key, pump] : pressurePumps) {
        pressurePumpsNA.push_back(pump);
    }

    nodal::conductNodalAnalysis(nodesNA, channelsNA, pressurePumpsNA, flowRatePumpsNA);

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(node0.getPressure(), 1.0, errorTolerance);
    ASSERT_NEAR(node1.getPressure(), 2.0 / 3.0, errorTolerance);
    ASSERT_NEAR(node2.getPressure(), 15.0, errorTolerance);
    ASSERT_NEAR(node3.getPressure(), 10.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(v0.getFlowRate(), -0.2 / 3.0, errorTolerance);
    /* current at channels
    ASSERT_NEAR(c1.getCurrent(), 0.2 / 3.0, errorTolerance);
    ASSERT_NEAR(c2.getCurrent(), 0.2 / 3.0, errorTolerance);
    ASSERT_NEAR(c3.getCurrent(), 1.0, errorTolerance);
    ASSERT_NEAR(c4.getCurrent(), 1.0, errorTolerance);
    */
}

TEST(NodalAnalysis, testNetwork2) {
    // define network
    // nodes
    Node nodeGround(-1);
    Node node0(0);
    Node node1(1);
    Node node2(2);
    Node node3(3);
    Node node4(4);
    unordered_map<int, INode *> nodes{{nodeGround.getId(), &nodeGround}, {node0.getId(), &node0}, {node1.getId(), &node1}, {node2.getId(), &node2}, {node3.getId(), &node3}, {node4.getId(), &node4}};

    // pressure pump (voltage sources)
    PressurePump v0(0, &nodeGround, &node0, 1.0);
    PressurePump v1(1, &node4, &nodeGround, 2.0);
    unordered_map<int, PressurePump *> pressurePumps{{v0.getId(), &v0}, {v1.getId(), &v1}};

    // flowRate pump (current source)
    FlowRatePump i0(0, &nodeGround, &node1, 1.0);
    unordered_map<int, FlowRatePump *> flowRatePumps{{i0.getId(), &i0}};

    // channels
    Channel c1(1, &node0, &node1, 5);
    Channel c2(2, &nodeGround, &node1, 10);
    Channel c3(3, &node1, &node2, 20);
    Channel c4(4, &node2, &node3, 30);
    unordered_map<int, Channel *> channels{{c1.getId(), &c1}, {c2.getId(), &c2}, {c3.getId(), &c3}, {c4.getId(), &c4}};

    // compute network
    // compute network
    std::unordered_map<int, std::tuple<nodal::INode *, int>> nodesNA;
    int matrixId = 0;
    for (auto &[nodeId, node] : nodes) {
        // if node is ground node set the matrixId to -1
        if (nodeId == nodeGround.getId()) {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, -1));
        } else {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channelsNA;
    for (auto &[key, channel] : channels) {
        channelsNA.push_back(channel);
    }
    std::vector<arch::FlowRatePump *> flowRatePumpsNA;
    for (auto &[key, pump] : flowRatePumps) {
        flowRatePumpsNA.push_back(pump);
    }
    std::vector<arch::PressurePump *> pressurePumpsNA;
    for (auto &[key, pump] : pressurePumps) {
        pressurePumpsNA.push_back(pump);
    }

    nodal::conductNodalAnalysis(nodesNA, channelsNA, pressurePumpsNA, flowRatePumpsNA);

    // check result
    // // pressure at nodes
    // ASSERT_NEAR(node0.getPressure(), 1.0, 1e-6);
    // ASSERT_NEAR(node1.getPressure(), 3.9999999999999996, 1e-6);
    // ASSERT_NEAR(node2.getPressure(), 3.9999999999999987, 1e-6);
    // ASSERT_NEAR(node3.getPressure(), 3.9999999999999987, 1e-6);
    // ASSERT_NEAR(node4.getPressure(), -2.0, 1e-6);
    // // flow rate at pressure pumps
    // ASSERT_NEAR(v0.getFlowRate(), 0.5999999999999998, 1e-6);
    // ASSERT_NEAR(v1.getFlowRate(), -0.0, 1e-6);
    // // current at channels
    // ASSERT_NEAR(c1.getCurrent(), -0.59999999999999987, 1e-6);
    // ASSERT_NEAR(c2.getCurrent(), -0.39999999999999997, 1e-6);
    // ASSERT_NEAR(c3.getCurrent(), 4.4408920985006262e-16, 1e-6);
    // ASSERT_NEAR(c4.getCurrent(), -1.4802973661668754e-17, 1e-6);

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(node0.getPressure(), 1.0, errorTolerance);
    ASSERT_NEAR(node1.getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(node2.getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(node3.getPressure(), 4.0, errorTolerance);
    ASSERT_NEAR(node4.getPressure(), -2.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(v0.getFlowRate(), 0.6, errorTolerance);
    ASSERT_NEAR(v1.getFlowRate(), 0.0, errorTolerance);
    /* current at channels
    ASSERT_NEAR(c1.getCurrent(), -0.6, errorTolerance);
    ASSERT_NEAR(c2.getCurrent(), -0.4, errorTolerance);
    ASSERT_NEAR(c3.getCurrent(), 0.0, errorTolerance);
    ASSERT_NEAR(c4.getCurrent(), 0.0, errorTolerance);
    */
}

TEST(NodalAnalysis, testNetwork3) {
    // define network
    // nodes
    Node nodeGround(-1);
    Node node0(0);
    Node node1(1);
    Node node2(2);
    unordered_map<int, INode *> nodes{{nodeGround.getId(), &nodeGround}, {node0.getId(), &node0}, {node1.getId(), &node1}, {node2.getId(), &node2}};

    // pressure pump (voltage sources)
    PressurePump v0(0, &node1, &node0, 32.0);
    PressurePump v1(1, &node2, &nodeGround, 20.0);
    unordered_map<int, PressurePump *> pressurePumps{{v0.getId(), &v0}, {v1.getId(), &v1}};

    // flowRate pump (current source)
    // FlowRatePump i0(&nodeGround, &node1, 1.0);
    unordered_map<int, FlowRatePump *> flowRatePumps{};

    // channels
    Channel c1(1, &nodeGround, &node0, 2);
    Channel c2(2, &node1, &node2, 4);
    Channel c3(3, &node1, &nodeGround, 8);
    unordered_map<int, Channel *> channels{{c1.getId(), &c1}, {c2.getId(), &c2}, {c3.getId(), &c3}};

    // compute network
    // compute network
    std::unordered_map<int, std::tuple<nodal::INode *, int>> nodesNA;
    int matrixId = 0;
    for (auto &[nodeId, node] : nodes) {
        // if node is ground node set the matrixId to -1
        if (nodeId == nodeGround.getId()) {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, -1));
        } else {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channelsNA;
    for (auto &[key, channel] : channels) {
        channelsNA.push_back(channel);
    }
    std::vector<arch::FlowRatePump *> flowRatePumpsNA;
    for (auto &[key, pump] : flowRatePumps) {
        flowRatePumpsNA.push_back(pump);
    }
    std::vector<arch::PressurePump *> pressurePumpsNA;
    for (auto &[key, pump] : pressurePumps) {
        pressurePumpsNA.push_back(pump);
    }

    nodal::conductNodalAnalysis(nodesNA, channelsNA, pressurePumpsNA, flowRatePumpsNA);

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(node0.getPressure(), 8.0, errorTolerance);
    ASSERT_NEAR(node1.getPressure(), -24.0, errorTolerance);
    ASSERT_NEAR(node2.getPressure(), -20.0, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(v0.getFlowRate(), -4.0, errorTolerance);
    ASSERT_NEAR(v1.getFlowRate(), 1.0, errorTolerance);
    /* current at channels
    ASSERT_NEAR(c1.getCurrent(), -4.0, errorTolerance);
    ASSERT_NEAR(c2.getCurrent(), -1.0, errorTolerance);
    ASSERT_NEAR(c3.getCurrent(), -3.0, errorTolerance);
    */
}

TEST(NodalAnalysis, testNetwork4) {
    // define network
    // nodes
    Node nodeGround(-1);
    Node node0(0);
    Node node1(1);
    unordered_map<int, INode *> nodes{{nodeGround.getId(), &nodeGround}, {node0.getId(), &node0}, {node1.getId(), &node1}};

    // pressure pump (voltage sources)
    PressurePump v0(0, &node0, &node1, 32.0);
    unordered_map<int, PressurePump *> pressurePumps{{v0.getId(), &v0}};

    // flowRate pump (current source)
    FlowRatePump i0(0, &node0, &nodeGround, 20.0);
    unordered_map<int, FlowRatePump *> flowRatePumps{{i0.getId(), &i0}};

    // channels
    Channel c1(1, &nodeGround, &node0, 2);
    Channel c2(2, &node0, &node1, 4);
    Channel c3(3, &node1, &nodeGround, 8);
    unordered_map<int, Channel *> channels{{c1.getId(), &c1}, {c2.getId(), &c2}, {c3.getId(), &c3}};

    // compute network
    // compute network
    std::unordered_map<int, std::tuple<nodal::INode *, int>> nodesNA;
    int matrixId = 0;
    for (auto &[nodeId, node] : nodes) {
        // if node is ground node set the matrixId to -1
        if (nodeId == nodeGround.getId()) {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, -1));
        } else {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channelsNA;
    for (auto &[key, channel] : channels) {
        channelsNA.push_back(channel);
    }
    std::vector<arch::FlowRatePump *> flowRatePumpsNA;
    for (auto &[key, pump] : flowRatePumps) {
        flowRatePumpsNA.push_back(pump);
    }
    std::vector<arch::PressurePump *> pressurePumpsNA;
    for (auto &[key, pump] : pressurePumps) {
        pressurePumpsNA.push_back(pump);
    }

    nodal::conductNodalAnalysis(nodesNA, channelsNA, pressurePumpsNA, flowRatePumpsNA);

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(node0.getPressure(), -38.4, errorTolerance);
    ASSERT_NEAR(node1.getPressure(), -6.4, errorTolerance);
    // flow rate at pressure pumps
    ASSERT_NEAR(v0.getFlowRate(), -7.2, errorTolerance);
    /* current at channels
    ASSERT_NEAR(c1.getCurrent(), 19.2, errorTolerance);
    ASSERT_NEAR(c2.getCurrent(), -8.0, errorTolerance);
    ASSERT_NEAR(c3.getCurrent(), -0.8, errorTolerance);
    */
}

TEST(NodalAnalysis, testNetwork5) {
    // define network
    // nodes
    Node nodeGround(-1);
    Node node0(0);
    Node node1(1);
    Node node2(2);
    unordered_map<int, INode *> nodes{{nodeGround.getId(), &nodeGround}, {node0.getId(), &node0}, {node1.getId(), &node1}, {node2.getId(), &node2}};

    // pressure pump (voltage sources)
    unordered_map<int, PressurePump *> pressurePumps{};

    // flowRate pump (current source)
    FlowRatePump i0(0, &node0, &nodeGround, 1.0);
    FlowRatePump i1(1, &node2, &nodeGround, 1.5);
    unordered_map<int, FlowRatePump *> flowRatePumps{{i0.getId(), &i0}, {i1.getId(), &i1}};

    // channels
    Channel c1(1, &node0, &node1, 5);
    Channel c2(2, &node1, &node2, 7);
    Channel c3(3, &node1, &nodeGround, 10);
    unordered_map<int, Channel *> channels{{c1.getId(), &c1}, {c2.getId(), &c2}, {c3.getId(), &c3}};

    // compute network
    // compute network
    std::unordered_map<int, std::tuple<nodal::INode *, int>> nodesNA;
    int matrixId = 0;
    for (auto &[nodeId, node] : nodes) {
        // if node is ground node set the matrixId to -1
        if (nodeId == nodeGround.getId()) {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, -1));
        } else {
            nodesNA.insert_or_assign(nodeId, std::make_tuple(node, matrixId));
            matrixId++;
        }
    }
    std::vector<arch::Channel *> channelsNA;
    for (auto &[key, channel] : channels) {
        channelsNA.push_back(channel);
    }
    std::vector<arch::FlowRatePump *> flowRatePumpsNA;
    for (auto &[key, pump] : flowRatePumps) {
        flowRatePumpsNA.push_back(pump);
    }
    std::vector<::PressurePump *> pressurePumpsNA;
    for (auto &[key, pump] : pressurePumps) {
        pressurePumpsNA.push_back(pump);
    }

    nodal::conductNodalAnalysis(nodesNA, channelsNA, pressurePumpsNA, flowRatePumpsNA);

    // check result
    const double errorTolerance = 1e-6;
    // pressure at nodes
    ASSERT_NEAR(node0.getPressure(), -30.0, errorTolerance);
    ASSERT_NEAR(node1.getPressure(), -25.0, errorTolerance);
    ASSERT_NEAR(node2.getPressure(), -35.5, errorTolerance);
    /* current at channels
    ASSERT_NEAR(c1.getCurrent(), -1, errorTolerance);
    ASSERT_NEAR(c2.getCurrent(), 1.5, errorTolerance);
    ASSERT_NEAR(c3.getCurrent(), -2.5, errorTolerance);
    */
}
