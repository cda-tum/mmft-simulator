//#include <pybind11/pybind11.h>

//namespace py = pybind11;

#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    // Define JSON files
    std::string file = "../examples/1D/Mixing/DiffusionCase1.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulations from the JSON files
    sim::Simulation<T> sim = porting::simulationFromJSON<T>(file, &network);

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    //result::SimulationResult<T>* result = sim.getSimulationResults();

    return 0;
}