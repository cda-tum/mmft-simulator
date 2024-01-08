#include "../../src/baseSimulator.h"
#include "../../src/baseSimulator.hh"

#include "gtest/gtest.h"

using T = double;

TEST(Hybrid, testCase1a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network1a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));
    network->setPressurePump(1, T(1e3));
    network->setPressurePump(2, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

    ASSERT_NEAR(network->getNodes().at(-1)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(2)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(3)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(4)->getPressure(), 859.216, 1e-3);
    ASSERT_NEAR(network->getNodes().at(5)->getPressure(), 791.962, 1e-3);
    ASSERT_NEAR(network->getNodes().at(6)->getPressure(), 859.216, 1e-3);
    ASSERT_NEAR(network->getNodes().at(7)->getPressure(), 753.628, 1e-3);
    ASSERT_NEAR(network->getNodes().at(8)->getPressure(), 753.628, 1e-3);
    ASSERT_NEAR(network->getNodes().at(9)->getPressure(), 422.270, 1e-3);

    ASSERT_NEAR(network->getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);

}

TEST(Hybrid, testCase2a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network2a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

    ASSERT_NEAR(network->getNodes().at(-1)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(2)->getPressure(), 577.014, 1e-3);
    ASSERT_NEAR(network->getNodes().at(3)->getPressure(), 729.934, 1e-3);
    ASSERT_NEAR(network->getNodes().at(4)->getPressure(), 491.525, 1e-3);
    ASSERT_NEAR(network->getNodes().at(5)->getPressure(), 469.468, 1e-3);
    ASSERT_NEAR(network->getNodes().at(6)->getPressure(), 641.131, 1e-3);
    ASSERT_NEAR(network->getNodes().at(7)->getPressure(), 607.210, 1e-3);
    ASSERT_NEAR(network->getNodes().at(8)->getPressure(), 427.408, 1e-3);
    ASSERT_NEAR(network->getNodes().at(9)->getPressure(), 270.394, 1e-3);

    ASSERT_NEAR(network->getChannels().at(1)->getFlowRate(), 3.00073e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(2)->getFlowRate(), -7.12409e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(3)->getFlowRate(), 7.12409-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(4)->getFlowRate(), 2.2957-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(5)->getFlowRate(), 7.12409e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(6)->getFlowRate(), 3.00438-9, 1e-14);

}

TEST(Hybrid, testCase3a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network3a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

    ASSERT_NEAR(network->getNodes().at(-1)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(2)->getPressure(), 687.204, 1e-3);
    ASSERT_NEAR(network->getNodes().at(3)->getPressure(), 801.008, 1e-3);
    ASSERT_NEAR(network->getNodes().at(4)->getPressure(), 625.223, 1e-3);
    ASSERT_NEAR(network->getNodes().at(5)->getPressure(), 601.422, 1e-3);
    ASSERT_NEAR(network->getNodes().at(6)->getPressure(), 428.435, 1e-3);
    ASSERT_NEAR(network->getNodes().at(7)->getPressure(), 428.007, 1e-3);
    ASSERT_NEAR(network->getNodes().at(8)->getPressure(), 331.775, 1e-3);
    ASSERT_NEAR(network->getNodes().at(9)->getPressure(), 733.690, 1e-3);
    ASSERT_NEAR(network->getNodes().at(10)->getPressure(), 703.349, 1e-3);
    ASSERT_NEAR(network->getNodes().at(11)->getPressure(), 578.736, 1e-3);
    ASSERT_NEAR(network->getNodes().at(12)->getPressure(), 525.094, 1e-3);

    ASSERT_NEAR(network->getChannels().at(1)->getFlowRate(), 2.21102e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(2)->getFlowRate(), -5.16509e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(3)->getFlowRate(), 5.16509e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(4)->getFlowRate(), 1.69878e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(5)->getFlowRate(), 5.16517e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(6)->getFlowRate(), 1.07399e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(7)->getFlowRate(), 1.13318e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(8)->getFlowRate(), 1.07399e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(9)->getFlowRate(), 1.13318e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(10)->getFlowRate(), 2.20181e-9, 1e-14);

}

TEST(Hybrid, testCase4a) {
    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    // Load the network from a JSON file
    std::string file = "../examples/Hybrid/Network4a.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);

    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));

    // Define the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));

    // Define the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    
    testSimulation.setContinuousPhase(fluid0);
    testSimulation.setResistanceModel(resistanceModel);
    testSimulation.setNetwork(network);
    testSimulation.simulate();

    ASSERT_NEAR(network->getNodes().at(-1)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(0)->getPressure(), 0, 1e-3);
    ASSERT_NEAR(network->getNodes().at(1)->getPressure(), 1000, 1e-3);
    ASSERT_NEAR(network->getNodes().at(2)->getPressure(), 781.127, 1e-3);
    ASSERT_NEAR(network->getNodes().at(3)->getPressure(), 616.808, 1e-3);
    ASSERT_NEAR(network->getNodes().at(4)->getPressure(), 604.131, 1e-3);
    ASSERT_NEAR(network->getNodes().at(5)->getPressure(), 535.195, 1e-3);
    ASSERT_NEAR(network->getNodes().at(6)->getPressure(), 447.970, 1e-3);
    ASSERT_NEAR(network->getNodes().at(7)->getPressure(), 371.019, 1e-3);
    ASSERT_NEAR(network->getNodes().at(8)->getPressure(), 698.420, 1e-3);
    ASSERT_NEAR(network->getNodes().at(9)->getPressure(), 695.906, 1e-3);
    ASSERT_NEAR(network->getNodes().at(10)->getPressure(), 552.144, 1e-3);
    ASSERT_NEAR(network->getNodes().at(11)->getPressure(), 540.373, 1e-3);
    ASSERT_NEAR(network->getNodes().at(12)->getPressure(), 522.325, 1e-3);
    ASSERT_NEAR(network->getNodes().at(13)->getPressure(), 454.945, 1e-3);
    ASSERT_NEAR(network->getNodes().at(14)->getPressure(), 355.566, 1e-3);
    ASSERT_NEAR(network->getNodes().at(15)->getPressure(), 217.853, 1e-3);

    ASSERT_NEAR(network->getChannels().at(1)->getFlowRate(), 2.43192e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(2)->getFlowRate(), 9.06805e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(3)->getFlowRate(), 1.52958e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(4)->getFlowRate(), 9.06814e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(5)->getFlowRate(), 4.96981e-10, 1e-14);
    ASSERT_NEAR(network->getChannels().at(6)->getFlowRate(), 1.02671e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(7)->getFlowRate(), 1.39876e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(8)->getFlowRate(), 1.02671e-9, 1e-14);
    ASSERT_NEAR(network->getChannels().at(9)->getFlowRate(), 2.42058e-9, 1e-14);

}