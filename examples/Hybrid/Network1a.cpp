#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Hybrid);
    testSimulation.setPlatform(sim::Platform::Continuous);

    // define network
    arch::Network<T> network;
    
    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(1e-3, 2e-3, false);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(1e-3, 0.0, false);
    auto node4 = network.addNode(2e-3, 2e-3, false);
    auto node5 = network.addNode(1.75e-3, 1e-3, false);
    auto node6 = network.addNode(2e-3, 0.0, false);
    auto node7 = network.addNode(2e-3, 1.25e-3, false);
    auto node8 = network.addNode(2e-3, 0.75e-3, false);
    auto node9 = network.addNode(2.25e-3, 1e-3, false);
    auto node10 = network.addNode(3e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network.addChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network.addChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network.addChannel(node4->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c7 = network.addChannel(node6->getId(), node8->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c8 = network.addChannel(node9->getId(), node10->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // module
    std::vector<T> position = { 1.75e-3, 0.75e-3 };
    std::vector<T> size = { 5e-4, 5e-4 };
    std::unordered_map<int, std::shared_ptr<arch::Node<T>>> nodes;

    nodes.try_emplace(5, network.getNode(5));
    nodes.try_emplace(7, network.getNode(7));
    nodes.try_emplace(8, network.getNode(8));
    nodes.try_emplace(9, network.getNode(9));

    auto m0 = network.addModule(position, size, nodes);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // simulator
    std::string name = "Paper1a-cross-0";
    std::string stlFile = "../examples/STL/cross.stl";
    T charPhysLength = 1e-4;
    T charPhysVelocity = 1e-1;
    T alpha = 0.1;
    T resolution = 20;
    T epsilon = 1e-1;
    T tau = 0.55;
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(5, arch::Opening<T>(network.getNode(5), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(7, arch::Opening<T>(network.getNode(7), std::vector<T>({0.0, -1.0}), 1e-4));
    Openings.try_emplace(8, arch::Opening<T>(network.getNode(8), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(9, arch::Opening<T>(network.getNode(9), std::vector<T>({-1.0, 0.0}), 1e-4));

    testSimulation.addLbmSimulator(name, stlFile, network.getModule(m0->getId()), Openings, charPhysLength, charPhysVelocity, alpha, resolution, epsilon, tau);
    network.sortGroups();

    // pressure pump
    auto pressure = 1e3;
    network.setPressurePump(c0->getId(), pressure);
    network.setPressurePump(c1->getId(), pressure);
    network.setPressurePump(c2->getId(), pressure);

    network.isNetworkValid();
    
    // Simulate
    testSimulation.setNetwork(&network);
    testSimulation.simulate();

    // results
    result::SimulationResult<T>* result = testSimulation.getSimulationResults();
    result->printLastState();

    return 0;
}