//#include <pybind11/pybind11.h>

//namespace py = pybind11;

#include <iostream>
#include <memory>

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
    std::shared_ptr<arch::Network<T>> network = std::make_shared<arch::Network<T>>();
    porting::networkFromJSON<T>(file, *network);

    // Load and set the simulation from a JSON file
    std::cout << "[Main] Create simulation object..." << std::endl;
    std::shared_ptr<sim::Simulation<T>> testSimulation = std::make_shared<sim::Simulation<T>>();
    porting::simulationFromJSON<T>(file, network, *testSimulation);

    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    testSimulation->simulate();

    std::cout << "[Main] Results..." << std::endl;
    // Print the results
    testSimulation->getSimulationResults()->printStates();

    //std::cout << "Write diffusive mixtures" << std::endl;
    //testSimulation.getSimulationResults()->writeMixture(1);

    #ifdef USE_ESSLBM
    MPI_Finalize();
    #endif

    return 0;
}
