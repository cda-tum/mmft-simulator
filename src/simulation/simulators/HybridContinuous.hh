#include "HybridContinuous.h"

namespace sim {

    template<typename T>
    HybridContinuous<T>::HybridContinuous(arch::Network<T>* network) : Simulation<T>(Type::Hybrid, Platform::Continuous, network) { }

    template<typename T>
    std::shared_ptr<mmft::NaiveScheme<T>> HybridContinuous<T>::setNaiveHybridScheme(T alpha, T beta, int theta) {
        auto naiveScheme = std::make_shared<mmft::NaiveScheme<T>>(this->getNetwork()->getModules(), alpha, beta, theta);
        for (auto& [key, simulator] : cfdSimulators) {
            updateSchemes.try_emplace(simulator->getId(), naiveScheme);
            simulator->setUpdateScheme(updateSchemes.at(simulator->getId()));
        }
        return naiveScheme;
    }

    template<typename T>
    std::shared_ptr<mmft::NaiveScheme<T>> HybridContinuous<T>::setNaiveHybridScheme(int moduleId, T alpha, T beta, int theta) {
        auto naiveScheme = std::make_shared<mmft::NaiveScheme<T>>(this->getNetwork()->getModule(moduleId), alpha, beta, theta);
        updateSchemes.try_emplace(moduleId, naiveScheme);
        cfdSimulators.at(moduleId)->setUpdateScheme(updateSchemes.at(moduleId));
        return naiveScheme;
    }

    template<typename T>
    std::shared_ptr<mmft::NaiveScheme<T>> HybridContinuous<T>::setNaiveHybridScheme(int moduleId, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta) {
        auto naiveScheme = std::make_shared<mmft::NaiveScheme<T>>(this->getNetwork()->getModule(moduleId), alpha, beta, theta);
        updateSchemes.try_emplace(moduleId, naiveScheme);
        cfdSimulators.at(moduleId)->setUpdateScheme(updateSchemes.at(moduleId));
        return naiveScheme;
    }

    template<typename T>
    lbmSimulator<T>* HybridContinuous<T>::addLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings, 
                                                    T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau)
    {
        if (this->getResistanceModel() != nullptr) {
            // create Simulator
            auto id = cfdSimulators.size();
            auto addCfdSimulator = new lbmSimulator<T>(id, name, stlFile, module, openings, this->getResistanceModel(), charPhysLength, charPhysVelocity, resolution, epsilon, tau);

            // add Simulator
            cfdSimulators.try_emplace(id, addCfdSimulator);

            return addCfdSimulator;
        } else {
            throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
        }
    }

    /** TODO: HybridMixingSimulation
     * Enable hybrid mixing simulation and uncomment code below
     */
    // template<typename T>
    // lbmMixingSimulator<T>* HybridContinuous<T>::addLbmMixingSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
    //                                                     std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau)
    // {   
    //     std::cout  << "Trying to add a mixing simulator" << std::endl;
    //     if (this->getResistanceModel() != nullptr) {
    //         // create Simulator
    //         auto id = cfdSimulators.size();
    //         auto addCfdSimulator = new lbmMixingSimulator<T>(id, name, stlFile, module, species, openings, this->getResistanceModel(), charPhysLength, charPhysVelocity, resolution, epsilon, tau);

    //         // add Simulator
    //         cfdSimulators.try_emplace(id, addCfdSimulator);

    //         return addCfdSimulator;
    //     } else {
    //         throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
    //     }
    // }

    /** TODO: HybridOocSimulation
     * Enable hybrid OoC simulation and uncomment code below
     */
    // template<typename T>
    // lbmOocSimulator<T>* HybridContinuous<T>::addLbmOocSimulator(std::string name, std::string stlFile, int tissueId, std::string organStlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
    //                                                     std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau)
    // {
    //     if (this->getResistanceModel() != nullptr) {
    //         // create Simulator
    //         auto id = cfdSimulators.size();
    //         auto addCfdSimulator = new lbmOocSimulator<T>(id, name, stlFile, tissues.at(tissueId), organStlFile, module, species, openings, this->getResistanceModel(), charPhysLength, charPhysVelocity, resolution, epsilon, tau);

    //         // add Simulator
    //         cfdSimulators.try_emplace(id, addCfdSimulator);

    //         return addCfdSimulator;
    //     } else {
    //         throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
    //     }
    // }

    template<typename T>
    essLbmSimulator<T>* HybridContinuous<T>::addEssLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings,
                                                        T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau)
    {
        #ifdef USE_ESSLBM
        if (this->getResistanceModel() != nullptr) {
            // create Simulator
            auto id = cfdSimulators.size();
            auto addCfdSimulator = new essLbmSimulator<T>(id, name, stlFile, module, openings, this->getResistanceModel(), charPhysLength, charPhysVelocity, resolution, epsilon, tau);

            // add Simulator
            cfdSimulators.try_emplace(id, addCfdSimulator);

            return addCfdSimulator;
        } else {
            throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
        }
        #else
        throw std::invalid_argument("MMFT Simulator was not built using the ESS library.");
        #endif
    }

    template<typename T>
    CFDSimulator<T>* HybridContinuous<T>::getCFDSimulator(int simulatorId) const {
        return cfdSimulators.at(simulatorId).get();
    }

    template<typename T>
    std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& HybridContinuous<T>::getCFDSimulators() const {
        return cfdSimulators;
    }

    template<typename T>
    std::tuple<T, T> HybridContinuous<T>::getGlobalPressureBounds() const {
        T minP = std::numeric_limits<T>::max();
        T maxP = 0.0;
        for (auto& [key, simulator] : cfdSimulators) {
            auto localBounds = simulator->getPressureBounds();
            if (std::get<0>(localBounds) < minP) {
                minP = std::get<0>(localBounds);
            }
            if (std::get<1>(localBounds) > maxP) {
                maxP = std::get<1>(localBounds);
            }
        }
        return std::tuple<T, T> {minP, maxP};
    }
    
    template<typename T>
    std::tuple<T, T> HybridContinuous<T>::getGlobalVelocityBounds() const {
        T minVel = std::numeric_limits<T>::max();
        T maxVel = 0.0;
        for (auto& [key, simulator] : cfdSimulators) {
            auto localBounds = simulator->getVelocityBounds();
            if (std::get<0>(localBounds) < minVel) {
                minVel = std::get<0>(localBounds);
            }
            if (std::get<1>(localBounds) > maxVel) {
                maxVel = std::get<1>(localBounds);
            }
        }
        return std::tuple<T, T> {minVel, maxVel};
    }


    template<typename T>
    void HybridContinuous<T>::writePressurePpm(std::tuple<T, T> bounds, int resolution) {
        for (auto& [key, simulator] : cfdSimulators) {
            // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
            simulator->writePressurePpm(0.98*std::get<0>(bounds), 1.02*std::get<1>(bounds), resolution);
        }
    }

    template<typename T>
    void HybridContinuous<T>::writeVelocityPpm(std::tuple<T, T> bounds, int resolution) {
        for (auto& [key, simulator] : cfdSimulators) {
            // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
            simulator->writeVelocityPpm(0.98*std::get<0>(bounds), 1.02*std::get<1>(bounds), resolution);
        }
    }

    template<typename T>
    void HybridContinuous<T>::initialize() {

        Simulation<T>::initialize();    // Initialize base class

        #ifdef VERBOSE
            std::cout << "[Simulation] Initialize CFD simulators..." << std::endl;
        #endif

        // Initialize the CFD simulators
        for (auto& [key, cfdSimulator] : cfdSimulators) {
            cfdSimulator->lbmInit(this->getContinuousPhase()->getViscosity(), this->getContinuousPhase()->getDensity());
        }

        // compute nodal analysis
        #ifdef VERBOSE
            std::cout << "[Simulation] Conduct initial nodal analysis..." << std::endl;
        #endif
        HybridContinuous<T>::conductNodalAnalysis();

        // Prepare CFD geometry and lattice
        #ifdef VERBOSE
            std::cout << "[Simulation] Prepare CFD geometry and lattice..." << std::endl;
        #endif

        for (auto& [key, cfdSimulator] : cfdSimulators) {
            cfdSimulator->prepareGeometry();
            cfdSimulator->prepareLattice();
        }
    }

    template<typename T>
    void HybridContinuous<T>::simulate() {
        this->assertInitialized();              // perform initialization checks
        this->initialize();                     // initialize the simulation
        // Catch runtime error, not enough CFD simulators.
        if (this->getNetwork()->getModules().size() <= 0 ) {
            throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
        }

        bool allConverged = false;
        bool pressureConverged = false;

        // Initialization of CFD domains
        while (! allConverged) {
            allConverged = conductCFDSimulation(cfdSimulators);
        }

        // Iteratively conduct CFD simulation and nodal analysis until both domains are convered 
        // both internally and with respect to each other (i.e., aligned values on the boundaries)
        while (! allConverged || !pressureConverged) {
            // conduct CFD simulations
            allConverged = conductCFDSimulation(cfdSimulators);
            // compute nodal analysis again
            pressureConverged = HybridContinuous<T>::conductNodalAnalysis();

        }

        #ifdef VERBOSE     
            if (pressureConverged && allConverged) {
                std::cout << "[Simulation] All pressures have converged." << std::endl;
            } 
            this->printResults();
        #endif

        if (writePpm) {
            writePressurePpm(getGlobalPressureBounds());
            writeVelocityPpm(getGlobalVelocityBounds());
        }

        saveState();
    }

    template<typename T>
    void HybridContinuous<T>::saveState() {
        std::unordered_map<int, T> savePressures;
        std::unordered_map<int, T> saveFlowRates;
        std::unordered_map<int, std::string> vtkFiles;

        // pressures
        for (auto& [id, node] : this->getNetwork()->getNodes()) {
            savePressures.try_emplace(node->getId(), node->getPressure());
        }

        // flow rates
        for (auto& [id, channel] : this->getNetwork()->getChannels()) {
            saveFlowRates.try_emplace(channel->getId(), channel->getFlowRate());
        }
        for (auto& [id, pump] : this->getNetwork()->getFlowRatePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }
        for (auto& [id, pump] : this->getNetwork()->getPressurePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }

        // vtk Files
        for (auto& [id, simulator] : this->cfdSimulators) {
            vtkFiles.try_emplace(simulator->getId(), simulator->getVtkFile());
        }

        // state
        this->getSimulationResults()->addState(this->getTime(), savePressures, saveFlowRates, vtkFiles);
    }

    /** TODO: HybridOocSimulation
     * Enable Tissue object definition for OoC simulation
     */
    // template<typename T>
    // Tissue<T>* Simulation<T>::addTissue(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> Vmax, std::unordered_map<int, T> kM) {
    //     auto id = tissues.size();
        
    //     auto result = tissues.insert_or_assign(id, std::make_shared<Tissue<T>>(id, species, Vmax, kM));

    //     return result.first->second.get();
    // }

    /** TODO: HybridMixingSimulation
     * Enable hybrid mixing simulation and uncomment code below
     */
    // template<typename T>
    // void HybridMixing<T>::simulate() {
    //     this->assertInitialized();              // perform initialization checks
    //     this->initialize();                     // initialize the simulation

    //     // Catch runtime error, not enough CFD simulators.
    //     if (network->getModules().size() <= 0 ) {
    //         throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
    //     }

    //     bool allConverged = false;
    //     bool pressureConverged = false;

    //     // Initialization of NS CFD domains
    //     while (! allConverged) {
    //         allConverged = conductCFDSimulation(cfdSimulators);
    //     }

    //     // Obtain overal steady-state flow result
    //     while (! allConverged || !pressureConverged) {
    //         // conduct CFD simulations
    //         allConverged = conductCFDSimulation(cfdSimulators);
    //         // compute nodal analysis again
    //         pressureConverged = nodalAnalysis->conductNodalAnalysis(cfdSimulators);
    //     }

    //     #ifdef VERBOSE     
    //         this->printResults();
    //         std::cout << "[Simulation] All pressures have converged." << std::endl; 
    //     #endif

    //     if (writePpm) {
    //         writePressurePpm(getGlobalPressureBounds());
    //         writeVelocityPpm(getGlobalVelocityBounds());
    //     }

    //     saveState();

    //     // Couple the resulting CFD flow field to the AD fields
    //     coupleNsAdLattices(cfdSimulators);

    //     // Obtain overal steady-state concentration results
    //     bool concentrationConverged = false;
    //     while (!concentrationConverged) {
    //         concentrationConverged = conductADSimulation(cfdSimulators);
    //         this->mixingModel->propagateSpecies(network, this);
    //     }
    // }

    /** TODO: HybridOocSimulation
     * Enable hybrid OoC simulation and uncomment code below
     */
    // template<typename T>
    // void HybridOoc<T>::simulate() {
    //     this->assertInitialized();              // perform initialization checks
    //     this->initialize();                     // initialize the simulation

    //     // Catch runtime error, not enough CFD simulators.
    //     if (network->getModules().size() <= 0 ) {
    //         throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
    //     }

    //     bool allConverged = false;
    //     bool pressureConverged = false;

    //     // Initialization of CFD domains
    //     while (! allConverged) {
    //         allConverged = conductCFDSimulation(cfdSimulators);
    //     }

    //     while (! allConverged || !pressureConverged) {
    //         // conduct CFD simulations
    //         allConverged = conductCFDSimulation(cfdSimulators);
    //         // compute nodal analysis again
    //         pressureConverged = nodalAnalysis->conductNodalAnalysis(cfdSimulators);
    //     }

    //     #ifdef VERBOSE     
    //         if (pressureConverged && allConverged) {
    //             std::cout << "[Simulation] All pressures have converged." << std::endl;
    //         } 
    //         this->printResults();
    //     #endif

    //     if (writePpm) {
    //         this->writePressurePpm(getGlobalPressureBounds());
    //         this->writeVelocityPpm(getGlobalVelocityBounds());
    //     }

    //     this->saveState();
    // }    

}   /// namespace sim
