//#include <pybind11/pybind11.h>

//namespace py = pybind11;

#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    std::cout << "[Main] Create simulation object..." << std::endl;

    // New simulation object
    sim::Simulation<T> testSimulation = sim::Simulation<T>();

    std::cout << "[Main] Load the JSON network..." << std::endl;

    // Load and set the network from a JSON file
    //std::string file("../examples/Hybrid/Network1.JSON");
    std::string file = argv[1];
    arch::Network<T>* network = new arch::Network<T>(file);

    std::cout << "[Main] Add pressure and Flow rate pumps..." << std::endl;
    // Add Pressure and Flow Rate Pumps
    network->setPressurePump(0, T(1e3));
    testSimulation.setNetwork(network);

    std::cout << "[Main] Set the continuous phase fluid..." << std::endl;
    // Define and set the continuous phase fluid
    sim::Fluid<T>* fluid0 = new sim::Fluid<T>(0, T(1000), T(1e-3));
    fluid0->setName("Water");
    testSimulation.setContinuousPhase(fluid0);

    std::cout << "[Main] Set the resistance model..." << std::endl;
    // Define and set the resistance model
    sim::ResistanceModel2DPoiseuille<T>* resistanceModel = new sim::ResistanceModel2DPoiseuille<T>(fluid0->getViscosity());
    testSimulation.setResistanceModel(resistanceModel);


    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    testSimulation.simulate();


    std::cout << "[Main] Results..." << std::endl;
    // Print the results
    testSimulation.printResults();

    return 0;
}