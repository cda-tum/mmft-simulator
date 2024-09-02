#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"

using T = double;

TEST(Hybrid, Case1a) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Hybrid);
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
    network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node4->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node6->getId(), node8->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node9->getId(), node10->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

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
    T resolution = 20;
    T epsilon = 1e-1;
    T tau = 0.55;
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(5, arch::Opening<T>(network.getNode(5), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(7, arch::Opening<T>(network.getNode(7), std::vector<T>({0.0, -1.0}), 1e-4));
    Openings.try_emplace(8, arch::Opening<T>(network.getNode(8), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(9, arch::Opening<T>(network.getNode(9), std::vector<T>({-1.0, 0.0}), 1e-4));

    testSimulation.addLbmSimulator(name, stlFile, network.getModule(m0->getId()), Openings, charPhysLength, charPhysVelocity, resolution, epsilon, tau);
    testSimulation.setNaiveHybridScheme(0.1, 0.5, 10);
    network.sortGroups();

    // pressure pump
    auto pressure = 1e3;
    network.setPressurePump(c0->getId(), pressure);
    network.setPressurePump(c1->getId(), pressure);
    network.setPressurePump(c2->getId(), pressure);

    network.isNetworkValid();
    
    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);
}

#ifdef USE_ESSLBM
TEST(Hybrid, esstest) {

    MPI_Init(NULL, NULL);
    std::string file = "../examples/Hybrid/Network1a_ESS.JSON";

    // Load and set the network from a JSON file
    arch::Network<float> network = porting::networkFromJSON<float>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<float> testSimulation = porting::simulationFromJSON<float>(file, &network);

    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);

    MPI_Finalize();
}
#endif

TEST(Hybrid, Case1aJSON) {
    
    std::string file = "../examples/Hybrid/Network1a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);

    network.isNetworkValid();
    
    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);
}

TEST(Hybrid, testCase2a) {
    
std::string file = "../examples/Hybrid/Network2a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 577.014, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 729.934, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 491.525, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 469.468, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 641.131, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 607.210, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 427.408, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 270.394, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 3.00073e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), -7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.2957e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 3.00438e-9, 1e-14);

}

TEST(Hybrid, testCase3a) {
    
        std::string file = "../examples/Hybrid/Network3a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 687.204, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 801.008, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 625.223, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 601.422, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 428.435, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 428.007, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 331.775, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 733.690, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 703.349, 1e-2);
    EXPECT_NEAR(network.getNodes().at(11)->getPressure(), 578.736, 1e-2);
    EXPECT_NEAR(network.getNodes().at(12)->getPressure(), 525.094, 1e-2);
    EXPECT_NEAR(network.getNodes().at(13)->getPressure(), 529.993, 1e-2);
    EXPECT_NEAR(network.getNodes().at(14)->getPressure(), 326.021, 1e-2);
    EXPECT_NEAR(network.getNodes().at(15)->getPressure(), 198.163, 1e-2);
    EXPECT_NEAR(network.getNodes().at(16)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.21102e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), -5.16509e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 5.16509e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 1.69878e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 5.16517e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.07399e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.13318e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.07399e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(9)->getFlowRate(), 1.13318e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(10)->getFlowRate(), 2.20181e-9, 1e-14);

}

TEST(Hybrid, testCase4a) {
    
        std::string file = "../examples/Hybrid/Network4a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 781.127, 2e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 616.803, 2e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 604.131, 2e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 535.190, 2e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 447.980, 2e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 371.030, 2e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 698.415, 2e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 695.906, 2e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 552.139, 2e-2);
    EXPECT_NEAR(network.getNodes().at(11)->getPressure(), 540.383, 2e-2);
    EXPECT_NEAR(network.getNodes().at(12)->getPressure(), 522.320, 2e-2);
    EXPECT_NEAR(network.getNodes().at(13)->getPressure(), 454.955, 2e-2);
    EXPECT_NEAR(network.getNodes().at(14)->getPressure(), 355.576, 2e-2);
    EXPECT_NEAR(network.getNodes().at(15)->getPressure(), 217.852, 2e-2);
    EXPECT_NEAR(network.getNodes().at(16)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.43192e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), 9.06803e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.52958e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 9.06793e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 4.96979e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.02671e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.39887e-9, 1e-12);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.02671e-9, 1e-12);
    EXPECT_NEAR(network.getChannels().at(9)->getFlowRate(), 2.42036e-9, 1e-12);

}