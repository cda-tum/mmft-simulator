//#include <pybind11/pybind11.h>

//namespace py = pybind11;
#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

#include <chrono>

#ifdef USE_ESSLBM
    #include <mpi.h>
#endif

using T = double;

int main(int argc, char const* argv []) {

    #ifdef USE_ESSLBM
    MPI_Init(NULL,NULL);
    #endif

    //std::string file = argv[1];
    std::string n = argv[1];
    std::string m = argv[2];
    std::string i = argv[3];

    std::string file = "../../network_generation/mVLSI-Benchmark/n_"+n+"_m_"+m+"/Network_n"+n+"_m"+m+"_"+i+".json";

    // Load and set the network from a JSON file
    std::cout << "[Main] Create network object..." << std::endl;
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    std::cout << "[Main] Create simulation object..." << std::endl;
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);

    std::cout << "[Main] Simulation..." << std::endl;
    // Perform simulation and store results
    auto start_time = std::chrono::high_resolution_clock::now();
    testSimulation.simulate();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time = end_time - start_time;

    std::cout << "[Main] Results..." << std::endl;
    std::cout << time/std::chrono::milliseconds(1) << "," << std::to_string(testSimulation.getNodalAnalysis()->iteration) << std::endl;

    // Print the results
    //testSimulation.getSimulationResults()->printStates();

    //std::cout << "Write diffusive mixtures" << std::endl;
    //testSimulation.getSimulationResults()->writeMixture(1);

    // Write csv
    try {
        std::string outputFileName = "Time_n"+n+"_m"+m+"_i"+i+".csv";
        std::cout << "Generating CSV file: " << outputFileName << std::endl;

        std::ofstream outputFile;
        outputFile.open(outputFileName);

        outputFile << time/std::chrono::milliseconds(1) << "," << std::to_string(testSimulation.getNodalAnalysis()->iteration) << std::endl;

        // Close the file
        outputFile.close();

    }
    catch(...) {
        throw std::invalid_argument("Couldn't write csv file");
    }

    #ifdef USE_ESSLBM
    MPI_Finalize();
    #endif

    return 0;
}
