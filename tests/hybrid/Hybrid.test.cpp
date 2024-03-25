#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

using T = double;

TEST(Continuous, Case1a) {
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
    std::string name = "Paper1a-cross-0";
    std::string stlFile = "../examples/STL/cross.stl";
    std::vector<T> position = { 1.75e-3, 0.75e-3 };
    std::vector<T> size = { 5e-4, 5e-4 };
    T charPhysLength = 1e-4;
    T charPhysVelocity = 1e-1;
    T alpha = 0.1;
    T resolution = 20;
    T epsilon = 1e-1;
    T tau = 0.55;
    std::unordered_map<int, std::shared_ptr<arch::Node<T>>> Nodes;
    Nodes.try_emplace(5, network.getNode(5));
    Nodes.try_emplace(7, network.getNode(7));
    Nodes.try_emplace(8, network.getNode(8));
    Nodes.try_emplace(9, network.getNode(9));
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(5, arch::Opening<T>(network.getNode(5), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(7, arch::Opening<T>(network.getNode(7), std::vector<T>({0.0, -1.0}), 1e-4));
    Openings.try_emplace(8, arch::Opening<T>(network.getNode(8), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(9, arch::Opening<T>(network.getNode(9), std::vector<T>({-1.0, 0.0}), 1e-4));

    network.addModule(name, stlFile, position, size, Nodes, Openings, charPhysLength, charPhysVelocity,
                        alpha, resolution, epsilon, tau);
    
    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

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

    ASSERT_NEAR(network.getNodes().at(node0->getId())->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node1->getId())->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node2->getId())->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node3->getId())->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node4->getId())->getPressure(), 859.216, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node5->getId())->getPressure(), 791.962, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node6->getId())->getPressure(), 859.216, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node7->getId())->getPressure(), 753.628, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node8->getId())->getPressure(), 753.628, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node9->getId())->getPressure(), 422.270, 1e-3);
    ASSERT_NEAR(network.getNodes().at(node10->getId())->getPressure(), 0, 1e-3);

    ASSERT_NEAR(network.getChannels().at(c3->getId())->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(c4->getId())->getFlowRate(), 2.31153e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(c5->getId())->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(c6->getId())->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(c7->getId())->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(c8->getId())->getFlowRate(), 4.69188e-9, 1e-14);

}

// TEST(Continuous, Case1aJSON) {
    
//     std::string file = "../examples/Hybrid/Network1a.JSON";
    
//     // Load and set the network from a JSON file
//     arch::Network<T> network = porting::networkFromJSON<T>(file);

//     // Load and set the simulation from a JSON file
//     sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);

//     network.isNetworkValid();
    
//     // Simulate
//     testSimulation.simulate();

//     ASSERT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-3);
//     ASSERT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-3);

//     ASSERT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
//     ASSERT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
//     ASSERT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
//     ASSERT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
//     ASSERT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
//     ASSERT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);

// }

/*
TEST(Hybrid, testCase2a) {
    
std::string file = "../examples/Hybrid/Network2a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    ASSERT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(2)->getPressure(), 577.014, 1e-3);
    ASSERT_NEAR(network.getNodes().at(3)->getPressure(), 729.934, 1e-3);
    ASSERT_NEAR(network.getNodes().at(4)->getPressure(), 491.525, 1e-3);
    ASSERT_NEAR(network.getNodes().at(5)->getPressure(), 469.468, 1e-3);
    ASSERT_NEAR(network.getNodes().at(6)->getPressure(), 641.131, 1e-3);
    ASSERT_NEAR(network.getNodes().at(7)->getPressure(), 607.210, 1e-3);
    ASSERT_NEAR(network.getNodes().at(8)->getPressure(), 427.408, 1e-3);
    ASSERT_NEAR(network.getNodes().at(9)->getPressure(), 270.394, 1e-3);
    ASSERT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-3);

    ASSERT_NEAR(network.getChannels().at(1)->getFlowRate(), 3.00073e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(2)->getFlowRate(), -7.12409e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(3)->getFlowRate(), 7.12409e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.2957e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(5)->getFlowRate(), 7.12409e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(6)->getFlowRate(), 3.00438e-9, 1e-14);

}*/
/*
TEST(Hybrid, testCase3a) {
    
        std::string file = "../examples/Hybrid/Network3a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    ASSERT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(2)->getPressure(), 687.204, 1e-3);
    ASSERT_NEAR(network.getNodes().at(3)->getPressure(), 801.008, 1e-3);
    ASSERT_NEAR(network.getNodes().at(4)->getPressure(), 625.223, 1e-3);
    ASSERT_NEAR(network.getNodes().at(5)->getPressure(), 601.422, 1e-3);
    ASSERT_NEAR(network.getNodes().at(6)->getPressure(), 428.435, 1e-3);
    ASSERT_NEAR(network.getNodes().at(7)->getPressure(), 428.007, 1e-3);
    ASSERT_NEAR(network.getNodes().at(8)->getPressure(), 331.775, 1e-3);
    ASSERT_NEAR(network.getNodes().at(9)->getPressure(), 733.690, 1e-3);
    ASSERT_NEAR(network.getNodes().at(10)->getPressure(), 703.349, 1e-3);
    ASSERT_NEAR(network.getNodes().at(11)->getPressure(), 578.736, 1e-3);
    ASSERT_NEAR(network.getNodes().at(12)->getPressure(), 525.094, 1e-3);
    ASSERT_NEAR(network.getNodes().at(13)->getPressure(), 0, 1e-3);

    ASSERT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.21102e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(2)->getFlowRate(), -5.16509e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(3)->getFlowRate(), 5.16509e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(4)->getFlowRate(), 1.69878e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(5)->getFlowRate(), 5.16517e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.07399e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.13318e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.07399e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(9)->getFlowRate(), 1.13318e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(10)->getFlowRate(), 2.20181e-9, 1e-14);

}*/

/*
TEST(Hybrid, testCase4a) {
    
        std::string file = "../examples/Hybrid/Network4a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    ASSERT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network.getNodes().at(2)->getPressure(), 781.127, 1e-3);
    ASSERT_NEAR(network.getNodes().at(3)->getPressure(), 616.808, 1e-3);
    ASSERT_NEAR(network.getNodes().at(4)->getPressure(), 604.131, 1e-3);
    ASSERT_NEAR(network.getNodes().at(5)->getPressure(), 535.195, 1e-3);
    ASSERT_NEAR(network.getNodes().at(6)->getPressure(), 447.970, 1e-3);
    ASSERT_NEAR(network.getNodes().at(7)->getPressure(), 371.019, 1e-3);
    ASSERT_NEAR(network.getNodes().at(8)->getPressure(), 698.420, 1e-3);
    ASSERT_NEAR(network.getNodes().at(9)->getPressure(), 695.906, 1e-3);
    ASSERT_NEAR(network.getNodes().at(10)->getPressure(), 552.144, 1e-3);
    ASSERT_NEAR(network.getNodes().at(11)->getPressure(), 540.373, 1e-3);
    ASSERT_NEAR(network.getNodes().at(12)->getPressure(), 522.325, 1e-3);
    ASSERT_NEAR(network.getNodes().at(13)->getPressure(), 454.945, 1e-3);
    ASSERT_NEAR(network.getNodes().at(14)->getPressure(), 355.566, 1e-3);
    ASSERT_NEAR(network.getNodes().at(15)->getPressure(), 217.853, 1e-3);
    ASSERT_NEAR(network.getNodes().at(16)->getPressure(), 0, 1e-3);

    ASSERT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.43192e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(2)->getFlowRate(), 9.06805e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.52958e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(4)->getFlowRate(), 9.06814e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(5)->getFlowRate(), 4.96981e-10, 1e-14);
    ASSERT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.02671e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.39876e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.02671e-9, 1e-14);
    ASSERT_NEAR(network.getChannels().at(9)->getFlowRate(), 2.42058e-9, 1e-14);

}*/