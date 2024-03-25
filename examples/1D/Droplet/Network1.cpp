#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {
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
    result->printLastState();

    return 0;
}