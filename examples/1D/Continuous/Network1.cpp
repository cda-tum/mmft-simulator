#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Abstract);
    testSimulation.setPlatform(sim::Platform::Continuous);

    // define network
    arch::Network<T> network;
    testSimulation.setNetwork(&network);

    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(1e-3, 2e-3, false);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(1e-3, 0.0, false);
    auto node4 = network.addNode(2e-3, 2e-3, false);
    auto node5 = network.addNode(2e-3, 1e-3, false);
    auto node6 = network.addNode(2e-3, 0.0, false);
    auto node7 = network.addNode(3e-3, 1e-3, true);

    // pressure pump
    auto pressure = 1e3;
    auto pump0 = network.addPressurePump(node0->getId(), node1->getId(), pressure);
    auto pump1 = network.addPressurePump(node0->getId(), node2->getId(), pressure);
    auto pump2 = network.addPressurePump(node0->getId(), node3->getId(), pressure);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c1 = network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node6->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network.addChannel(node5->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 997.0, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

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
    result->printLastState();

    return 0;
}