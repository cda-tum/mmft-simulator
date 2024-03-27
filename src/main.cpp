//#include <pybind11/pybind11.h>

//namespace py = pybind11;

#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    std::string file = argv[1];

    std::cout << "[Main] Create simulation object..." << std::endl;

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
   
    network.sortGroups();
    network.isNetworkValid();

    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    testSimulation.simulate();

    std::cout << "[Main] Results..." << std::endl;
    // Print the results
    testSimulation.getSimulationResults()->printStates();

    porting::resultToJSON<T>("result.json", &testSimulation );

    return 0;
}