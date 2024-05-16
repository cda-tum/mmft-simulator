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
    result::SimulationResult<T>* result = sim.getSimulationResults();

    std::cout << "Print last state" << std::endl;
    result->printStates();
    std::cout << "Write diffusive mixtures" << std::endl;

    /**
     * Case 1
    */
    result->writeDiffusiveMixtures(1);
    result->writeDiffusiveMixtures(2);
    result->writeDiffusiveMixtures(3);

    /**
     * Case 2
    */
    // result->writeDiffusiveMixtures(3);
    // result->writeDiffusiveMixtures(9);
    // result->writeDiffusiveMixtures(8);
    // result->writeDiffusiveMixtures(7);

    /**
     * Case 3
    */
    // result->writeDiffusiveMixtures(4);
    // result->writeDiffusiveMixtures(3);
    // result->writeDiffusiveMixtures(13);
    // result->writeDiffusiveMixtures(23);
    // result->writeDiffusiveMixtures(38);
    // result->writeDiffusiveMixtures(35);
    // result->writeDiffusiveMixtures(42);
    // result->writeDiffusiveMixtures(41);

    /**
     * Case 4
    */
    // result->writeDiffusiveMixtures(2);
    // result->writeDiffusiveMixtures(1);
    // result->writeDiffusiveMixtures(7);
    // result->writeDiffusiveMixtures(3);
    // result->writeDiffusiveMixtures(10);
    // result->writeDiffusiveMixtures(11);
    
    /**
     * Case 1
    */
    //result->writeDiffusiveMixtures(2);
    //result->writeDiffusiveMixtures(3);

    /**
     * Case 2
    */
    //result->writeDiffusiveMixtures(2);
    //result->writeDiffusiveMixtures(6);
    //result->writeDiffusiveMixtures(7); // -> C = 1
    //result->writeDiffusiveMixtures(8); // -> C = 1
    //result->writeDiffusiveMixtures(9);

    /**
     * Case 3
    */
    //result->writeDiffusiveMixtures(1);  // 3.1
    //result->writeDiffusiveMixtures(2);  // 3.1
    //result->writeDiffusiveMixtures(0);  // 3.2
    //result->writeDiffusiveMixtures(1);  // 3.2
    //result->writeDiffusiveMixtures(2);  // 3.2
    //result->writeDiffusiveMixtures(3);  // 3.2
    //result->writeDiffusiveMixtures(4);  // 3.2
    //result->writeDiffusiveMixtures(5);  // 3.2
    //result->writeDiffusiveMixtures(40);
    //result->writeDiffusiveMixtures(41);

    /**
     * Case 4
    */
    //result->writeDiffusiveMixtures(13);
    //result->writeDiffusiveMixtures(14);

    return 0;
}