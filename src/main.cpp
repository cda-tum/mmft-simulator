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

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();
    result->printLastState();

    return 0;
}