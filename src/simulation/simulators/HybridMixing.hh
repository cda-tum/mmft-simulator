#include "HybridMixing.h"

namespace sim {

template<typename T>
HybridMixing<T>::HybridMixing(std::shared_ptr<arch::Network<T>> network) : HybridContinuous<T>(Platform::Mixing, network) { }

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, std::string name)
{
    size_t resolutionDefault = 20;      // Some reasonable value

    return addLbmSimulator(module, resolutionDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, size_t resolution, std::string name)
{
    T epsilonDefault = 1e-1;                                        // Some reasonable value
    T tauDefault = 0.932;                                           // For quadratic accuracy
    T adTauDefault = 0.932;                                         // For quadratic accuracy
    T charPhysLengthDefault = this->getCharacteristicLength();      // Smallest opening width / height
    T charPhysVelocityDefault = this->getCharacteristicVelocity();  // Largest expected average velocity in the system

    return addLbmSimulator(module, resolution, epsilonDefault, tauDefault, adTauDefault, charPhysLengthDefault, charPhysVelocityDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, 
    size_t resolution, T epsilon, T tau, T adTau, T charPhysLength, T charPhysVelocity, std::string name)
{
    if (this->hasValidResistanceModel()) {
        // create Simulator
        auto id = CFDSimulator<T>::getSimulatorCounter();
        auto addCfdSimulator = std::shared_ptr<lbmMixingSimulator<T>>(new lbmMixingSimulator<T>(id, std::move(name), module, this->getSpecies(), resolution, charPhysLength, charPhysVelocity, epsilon, tau, adTau));

        // add Simulator
        const auto& [it, inserted] = this->getCFDSimulators().try_emplace(id, addCfdSimulator);
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
std::pair<T,T> HybridMixing<T>::getGlobalConcentrationBounds() const {
    /** TODO:
     * 
     */
}

template<typename T>
void HybridMixing<T>::writeConcentrationPpm(std::pair<T,T> bounds, int resolution) const {
    /** TODO:
     * 
     */
}

template<typename T>
void HybridMixing<T>::simulate() {
    this->assertInitialized();              // perform initialization checks
    this->initialize();                     // initialize the simulation

    // Catch runtime error, not enough CFD simulators.
    if (this->getNetwork()->getCfdModules().size() <= 0 ) {
        throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
    }

    bool allConverged = false;
    bool pressureConverged = false;

    // Initialization of NS CFD domains
    while (! allConverged) {
        allConverged = conductCFDSimulation(this->getCFDSimulators());
    }

    // Obtain overal steady-state flow result
    while (! allConverged || !pressureConverged) {
        // conduct CFD simulations
        allConverged = conductCFDSimulation(this->getCFDSimulators());
        // compute nodal analysis again
        pressureConverged = this->getNodalAnalysis()->conductNodalAnalysis(this->getCFDSimulators());
    }

    #ifdef VERBOSE     
        this->printResults();
        std::cout << "[Simulation] All pressures have converged." << std::endl; 
    #endif

    if (this->getWritePpm) {
        this->writePressurePpm(this->getGlobalPressureBounds());
        this->writeVelocityPpm(this->getGlobalVelocityBounds());
        writeConcentrationPpm(getGlobalConcentrationBounds());
    }

    saveState();

    // Couple the resulting CFD flow field to the AD fields
    coupleNsAdLattices(this->getCFDSimulators());

    // Obtain overal steady-state concentration results
    bool concentrationConverged = false;
    while (!concentrationConverged) {
        concentrationConverged = conductADSimulation(this->getCFDSimulators());
        this->mixingModel->propagateSpecies(this->getNetwork(), this);
    }
}

}   // namespace sim
