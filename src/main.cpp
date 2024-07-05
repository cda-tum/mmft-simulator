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

    network.sortGroups();
    network.isNetworkValid();

    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    testSimulation.simulate();

    std::cout << "[Main] Results..." << std::endl;
    // Print the results
    testSimulation.getSimulationResults()->printStates();

    std::cout << "Write diffusive mixtures" << std::endl;

    /**
     * Case 1
    */
    testSimulation.getSimulationResults()->writeMixture(1);
    testSimulation.getSimulationResults()->writeMixture(2);
    testSimulation.getSimulationResults()->writeMixture(3);

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

    #ifdef USE_ESSLBM
    MPI_Finalize();
    #endif

    return 0;
}
