template<typename T>
lbmMixingSimulator<T>* HybridContinuous<T>::addLbmMixingSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
                                                    std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau)
{   
    std::cout  << "Trying to add a mixing simulator" << std::endl;
    if (this->getResistanceModel() != nullptr) {
        // create Simulator
        auto id = cfdSimulators.size();
        auto addCfdSimulator = new lbmMixingSimulator<T>(id, name, stlFile, module, species, openings, this->getResistanceModel(), charPhysLength, charPhysVelocity, resolution, epsilon, tau);

        // add Simulator
        cfdSimulators.try_emplace(id, addCfdSimulator);

        return addCfdSimulator;
    } else {
        throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
    }
}

template<typename T>
void HybridMixing<T>::simulate() {
    this->assertInitialized();              // perform initialization checks
    this->initialize();                     // initialize the simulation

    // Catch runtime error, not enough CFD simulators.
    if (network->getCfdModules().size() <= 0 ) {
        throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
    }

    bool allConverged = false;
    bool pressureConverged = false;

    // Initialization of NS CFD domains
    while (! allConverged) {
        allConverged = conductCFDSimulation(cfdSimulators);
    }

    // Obtain overal steady-state flow result
    while (! allConverged || !pressureConverged) {
        // conduct CFD simulations
        allConverged = conductCFDSimulation(cfdSimulators);
        // compute nodal analysis again
        pressureConverged = nodalAnalysis->conductNodalAnalysis(cfdSimulators);
    }

    #ifdef VERBOSE     
        this->printResults();
        std::cout << "[Simulation] All pressures have converged." << std::endl; 
    #endif

    if (writePpm) {
        writePressurePpm(getGlobalPressureBounds());
        writeVelocityPpm(getGlobalVelocityBounds());
    }

    saveState();

    // Couple the resulting CFD flow field to the AD fields
    coupleNsAdLattices(cfdSimulators);

    // Obtain overal steady-state concentration results
    bool concentrationConverged = false;
    while (!concentrationConverged) {
        concentrationConverged = conductADSimulation(cfdSimulators);
        this->mixingModel->propagateSpecies(network, this);
    }
}