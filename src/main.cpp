//#include <pybind11/pybind11.h>

//namespace py = pybind11;
#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

#ifdef USE_ESSLBM
    #include <mpi.h>
#endif

using T = double;

int main(int argc, char const* argv []) {

    #ifdef USE_ESSLBM
    MPI_Init(NULL,NULL);
    #endif

    std::string file = argv[1];

    // Load and set the network from a JSON file
    std::cout << "[Main] Create network object..." << std::endl;
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    std::cout << "[Main] Create simulation object..." << std::endl;
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);

    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    testSimulation.simulate();

    std::cout << "[Main] Results..." << std::endl;
    // Print the results
    testSimulation.getSimulationResults()->printStates();

    //std::cout << "Write diffusive mixtures" << std::endl;
    //testSimulation.getSimulationResults()->writeMixture(1);

    #ifdef USE_ESSLBM
    MPI_Finalize();
    #endif

    return 0;
}
