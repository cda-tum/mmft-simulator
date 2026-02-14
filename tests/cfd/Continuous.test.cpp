#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"
#include "../test_definitions.h"

using T = double;

class CfdContinuous : public test::definitions::GlobalTest<T> {};

TEST_F(CfdContinuous, Case1a) {
    // define network
    auto network = arch::Network<T>::createNetwork();
    
    // nodes
    auto node1 = network->addNode(1e-3, 2e-3, true);
    auto node2 = network->addNode(1e-3, 1e-3, true);
    auto node3 = network->addNode(1e-3, 0.0, true);
    auto node4 = network->addNode(2e-3, 2e-3, false);
    auto node5 = network->addNode(2e-3, 1e-3, false);
    auto node6 = network->addNode(2e-3, 0.0, false);
    auto node7 = network->addNode(3e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c1 = network->addRectangularChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network->addRectangularChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network->addRectangularChannel(node6->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network->addRectangularChannel(node5->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // define simulation
    sim::CfdContinuous<T> testSimulation(network);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    // Boundary conditions
    testSimulation.addPressureBC(node1, 1e2);
    testSimulation.addPressureBC(node2, 1e2);
    testSimulation.addPressureBC(node3, 1e2);
    testSimulation.addPressureBC(node7, 0.0);
    
    // Simulate
    testSimulation.simulate();
}
