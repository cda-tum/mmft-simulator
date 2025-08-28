#include "HybridContinuous.h"

namespace sim {

template<typename T>
HybridContinuous<T>::HybridContinuous(std::shared_ptr<arch::Network<T>> network) : Simulation<T>(Type::Hybrid, Platform::Continuous, network) { }

template<typename T>
void HybridContinuous<T>::set1DResistanceModel() {
    throw std::logic_error("Cannot set 1D resistance model for hybrid simulation.");
}

template<typename T>
void HybridContinuous<T>::setPoiseuilleResistanceModel() {
    Simulation<T>::setPoiseuilleResistanceModel();

    // Update the fully connected network of all simulators for new resistance model.
    for (auto& [key, simulator] : cfdSimulators) {
        simulator->initialize(this->getResistanceModel());
    }
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridContinuous<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, std::string name)
{
    size_t resolutionDefault = 20;      // Some reasonable value

    return addLbmSimulator(module, resolutionDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridContinuous<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, size_t resolution, std::string name)
{
    T epsilonDefault = 1e-1;                                        // Some reasonable value
    T tauDefault = 0.932;                                           // For quadratic accuracy
    T charPhysLengthDefault = this->getCharacteristicLength();      // Smallest opening width / height
    T charPhysVelocityDefault = this->getCharacteristicVelocity();  // Largest expected average velocity in the system

    return addLbmSimulator(module, resolution, epsilonDefault, tauDefault, charPhysLengthDefault, charPhysVelocityDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridContinuous<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, 
    size_t resolution, T epsilon, T tau, T charPhysLength, T charPhysVelocity, std::string name)
{
    if (this->hasValidResistanceModel()) {
        // create Simulator
        auto id = CFDSimulator<T>::getSimulatorCounter();
        auto addCfdSimulator = std::shared_ptr<lbmSimulator<T>>(new lbmSimulator<T>(id, std::move(name), module, resolution, charPhysLength, charPhysVelocity, epsilon, tau));

        // add Simulator
        const auto& [it, inserted] = cfdSimulators.try_emplace(id, addCfdSimulator);
        if (inserted) {
            addCfdSimulator->initialize(this->getResistanceModel());
        } else {
            throw std::logic_error("Could not emplace new lbmSimulator.");
        }

        return addCfdSimulator;
    } else {
        throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
    }
}

template<typename T>
void HybridContinuous<T>::removeLbmSimulator(const std::shared_ptr<CFDSimulator<T>>& simulator) {
    auto key = simulator->getId();
    if (cfdSimulators.erase(key)) {
        updateSchemes.erase(key);
    } else {
        throw std::logic_error("Cannot remove Lbm simulator " + std::to_string(key) + ". Simulator not found.");
    }
}

template<typename T>
void HybridContinuous<T>::setNaiveHybridScheme(T alpha, T beta, int theta) {
    for (auto& [key, simulator] : cfdSimulators) {
        auto it = updateSchemes.find(key);

        // If the scheme does not exist or is not naive, create a new one
        if (it == updateSchemes.end() || !it->second->isNaive()) {
            auto naiveScheme = std::unique_ptr<mmft::NaiveScheme<T>>(new mmft::NaiveScheme<T>(simulator->getModule(), alpha, beta, theta));
            simulator->setUpdateScheme(naiveScheme.get());
            if (it == updateSchemes.end()) {
                updateSchemes.emplace(key, std::move(naiveScheme));
            } else {
                it->second = std::move(naiveScheme);
            }
        } else {
            // Scheme exists and is naive
            it->second->setParameters(alpha, beta, theta);
        }
    }
}

template<typename T>
void HybridContinuous<T>::setNaiveHybridScheme(const std::shared_ptr<CFDSimulator<T>>& simulator, T alpha, T beta, int theta) {
    const auto& simIt = cfdSimulators.find(simulator->getId());
    
    if (simIt != cfdSimulators.end()) {
        auto it = updateSchemes.find(simulator->getId());
        // If the scheme does not exist or is not naive, create a new one
        if (it == updateSchemes.end() || !it->second->isNaive()) {
            auto naiveScheme = std::unique_ptr<mmft::NaiveScheme<T>>(new mmft::NaiveScheme(simulator->getModule(), alpha, beta, theta));
            simulator->setUpdateScheme(naiveScheme.get());
            if (it == updateSchemes.end()) {
                updateSchemes.emplace(simIt->second->getId(), std::move(naiveScheme));
            } else {
                it->second = std::move(naiveScheme);
            }
        } else {
            // Scheme exists and is naive
            it->second->setParameters(alpha, beta, theta);
        }
    } else {
        // The provided simulator pointer is not listed in this hybrid simulation
        throw std::logic_error("Cannot set Scheme for Lbm Simulator " + std::to_string(simulator->getId()) + ". Simulator not found.");
    }
}

template<typename T>
void HybridContinuous<T>::setNaiveHybridScheme(const std::shared_ptr<CFDSimulator<T>>& simulator, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta) {
    const auto& simIt = cfdSimulators.find(simulator->getId());

    if (simIt != cfdSimulators.end()) {
        auto it = updateSchemes.find(simulator->getId());
        // If the scheme does not exist or is not naive, create a new one
        if (it == updateSchemes.end() || !it->second->isNaive()) {
            auto naiveScheme = std::unique_ptr<mmft::NaiveScheme<T>>(new mmft::NaiveScheme(simulator->getModule(), alpha, beta, theta));
            simulator->setUpdateScheme(naiveScheme.get());
            if (it == updateSchemes.end()) {
                updateSchemes.emplace(simIt->second->getId(), std::move(naiveScheme));
            } else {
                it->second = std::move(naiveScheme);
            }
        } else {
            // Scheme exists and is Naive
            it->second->setParameters(alpha, beta, theta);
        }
    } else {
        // The provided simulator pointer is not listed in this hybrid simulation
        throw std::logic_error("Cannot set Scheme for Lbm Simulator " + std::to_string(simulator->getId()) + ". Simulator not found.");
    }
}

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
std::pair<T,T> HybridContinuous<T>::getGlobalPressureBounds() const {
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
    return std::pair<T,T> {minP, maxP};
}

template<typename T>
std::pair<T,T> HybridContinuous<T>::getGlobalVelocityBounds() const {
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
    return std::pair<T,T> {minVel, maxVel};
}


template<typename T>
void HybridContinuous<T>::writePressurePpm(std::pair<T,T> bounds, int resolution) const {
    for (auto& [key, simulator] : cfdSimulators) {
        // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
        simulator->writePressurePpm(0.98*bounds.first, 1.02*bounds.second, resolution);
    }
}

template<typename T>
void HybridContinuous<T>::writeVelocityPpm(std::pair<T,T> bounds, int resolution) const {
    for (auto& [key, simulator] : cfdSimulators) {
        // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
        simulator->writeVelocityPpm(0.98*bounds.first, 1.02*bounds.second, resolution);
    }
}

template<typename T>
void HybridContinuous<T>::assertInitialized() const {
    // Assert initialization of base class
    Simulation<T>::assertInitialized();

    // Check if the mapping of cfdModules and cfdSimulators is bijective (One-to-one and onto)
    std::set<int> coupledModules;
    for (const auto& [simId, simulator] : cfdSimulators) {
        // Check if the module is listed
        const auto& it = this->getNetwork()->getCfdModules().find(simulator->getModule()->getId());
        if (it != this->getNetwork()->getCfdModules().end()) {
            it->second->assertInitialized();
            coupledModules.insert(it->second->getId());
        } else {
            throw std::logic_error("CFD simulator is pointing to non-existent CFD module.");
        }
    }
    if (coupledModules.size() != this->getNetwork()->getCfdModules().size()) {
        throw std::logic_error("Number of CFD simulators does not match number of CFD modules.");
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
    if (this->getNetwork()->getCfdModules().size() <= 0 ) {
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
        pressureConverged = HybridContinuous<T>::conductNodalAnalysis().value();

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
    
template<typename T>
bool HybridContinuous<T>::hasValidResistanceModel() {
    return this->getResistanceModel()->isPoiseuilleModel();
}

}   /// namespace sim


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

/** TODO: HybridMixingSimulation
 * Enable hybrid mixing simulation and uncomment code below
 */
// template<typename T>
// void HybridMixing<T>::simulate() {
//     this->assertInitialized();              // perform initialization checks
//     this->initialize();                     // initialize the simulation

//     // Catch runtime error, not enough CFD simulators.
//     if (network->getCfdModules().size() <= 0 ) {
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

/** TODO: HybridOocSimulation
 * Enable Tissue object definition for OoC simulation
 */
// template<typename T>
// Tissue<T>* Simulation<T>::addTissue(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> Vmax, std::unordered_map<int, T> kM) {
//     auto id = tissues.size();
    
//     auto result = tissues.insert_or_assign(id, std::make_shared<Tissue<T>>(id, species, Vmax, kM));

//     return result.first->second.get();
// }

/** TODO: HybridOocSimulation
 * Enable hybrid OoC simulation and uncomment code below
 */
// template<typename T>
// void HybridOoc<T>::simulate() {
//     this->assertInitialized();              // perform initialization checks
//     this->initialize();                     // initialize the simulation

//     // Catch runtime error, not enough CFD simulators.
//     if (network->getCfdModules().size() <= 0 ) {
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