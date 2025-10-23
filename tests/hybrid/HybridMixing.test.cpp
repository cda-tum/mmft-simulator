#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"
#include "../test_definitions.h"

using T = double;

class HybridConcentration : public test::definitions::GlobalTest<T> {};

TEST_F(HybridConcentration, Case1a) {
    // define network
    auto network = arch::Network<T>::createNetwork();
    
    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(1e-3, 2e-3, false);
    auto node2 = network->addNode(1e-3, 1e-3, false);
    auto node3 = network->addNode(1e-3, 0.0, false);
    auto node4 = network->addNode(2e-3, 2e-3, false);
    auto node5 = network->addNode(1.75e-3, 1e-3, false);
    auto node6 = network->addNode(2e-3, 0.0, false);
    auto node7 = network->addNode(2e-3, 1.25e-3, false);
    auto node8 = network->addNode(2e-3, 0.75e-3, false);
    auto node9 = network->addNode(2.25e-3, 1e-3, false);
    auto node10 = network->addNode(3e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 0.0;

    auto c0 = network->addRectangularChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network->addRectangularChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node4->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node6->getId(), node8->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network->addRectangularChannel(node9->getId(), node10->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // module
    std::vector<T> position = { 1.75e-3, 0.75e-3 };
    std::vector<T> size = { 5e-4, 5e-4 };
    std::string stlFile = "../examples/STL/cross.stl";
    std::unordered_map<size_t, arch::Opening<T>> Openings;
    Openings.try_emplace(5, arch::Opening<T>(network->getNode(5), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(7, arch::Opening<T>(network->getNode(7), std::vector<T>({0.0, -1.0}), 1e-4));
    Openings.try_emplace(8, arch::Opening<T>(network->getNode(8), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(9, arch::Opening<T>(network->getNode(9), std::vector<T>({-1.0, 0.0}), 1e-4));

    auto m0 = network->addCfdModule(position, size, stlFile, Openings);

    // define simulation
    sim::HybridMixing<T> testSimulation(network);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    // Set the resistance model
    testSimulation.setPoiseuilleResistanceModel();

    // simulator
    std::string name = "Paper1a-cross-0";
    T charPhysLength = 1e-4;
    T charPhysVelocity = 1e-1;
    size_t resolution = 20;
    T epsilon = 1e-1;
    T tau = 0.55;
    T adTau = 0.55;

    testSimulation.addLbmSimulator(network->getCfdModule(m0->getId()), resolution, epsilon, tau, adTau, charPhysLength, charPhysVelocity, name);
    testSimulation.setNaiveHybridScheme(0.1, 0.5, 10);

    // pressure pump
    auto pressure = 1e3;
    network->setPressurePump(c0->getId(), pressure);
    network->setPressurePump(c1->getId(), pressure);
    network->setPressurePump(c2->getId(), pressure);
    
    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network->getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network->getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network->getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network->getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network->getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network->getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network->getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network->getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network->getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network->getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network->getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network->getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network->getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network->getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network->getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network->getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network->getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);
}
