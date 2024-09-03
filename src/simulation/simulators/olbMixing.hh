#include "olbMixing.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmMixingSimulator<T>::lbmMixingSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, Specie<T>*> species_,
    std::unordered_map<int, arch::Opening<T>> openings_, std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, 
    T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmMixingSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, updateScheme_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, 
                            relaxationTime_, species_, adRelaxationTime_)
{   
    std::cout << "Creating module and setting its type to lbm" << std::endl;
    this->cfdModule->setModuleTypeLbm();
    fluxWall.try_emplace(int(0), &zeroFlux);
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmMixingSimulator<T>::lbmInit (T dynViscosity, T density) {

    this->setOutputDir();
    initValueContainers();
    this->initNsConverter(dynViscosity, density);
    initAdConverters(density);
    this->initNsConvergeTracker();
    initAdConvergenceTracker();

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmMixingSimulator<T>::prepareLattice () {

    /**
     * Prepare the NS lattice
    */
    const T omega = this->converter->getLatticeRelaxationFrequency();
    this->prepareNsLattice(omega);
    
    /**
     * Prepare the AD lattices
    */
    for (auto& [speciesId, converter] : adConverges) {
        const T adOmega = getAdConverter(speciesId).getLatticeRelaxationFrequency();
        prepareAdLattice(adOmega, speciesId);
    }
    
    /**
     * Initialize the integral fluxes for the in- and outlets
     */
    this->initPressureIntegralPlane();
    this->initFlowRateIntegralPlane();
    initConcentrationIntegralPlane();

    /**
     * Initialize the lattices
     */
    this->initNsLattice(omega);
    for (auto& [speciesId, converter] : adConverges) {
        initAdLattice(speciesId);
    }

    std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;

    prepareCoupling();

    std::cout << "[lbmSimulator] prepare coupling " << this->name << "... OK" << std::endl;
}

template<typename T>
void lbmMixingSimulator<T>::storeCfdResults (int iT) {
    int input[1] = { };
    T output[10];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            this->meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
        } else {
            this->fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
        }
    }
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        // If the node is an outflow, write the concentration value
        if (this->flowRates.at(key) < 0.0) {
            storeAdResults(key, input, output);
        }
    }
}

template<typename T>
void lbmMixingSimulator<T>::solve() {
    // theta = 10
    this->setBoundaryValues(this->step);
    for (int iT = 0; iT < 10; ++iT){
        this->collideAndStream();
        executeCoupling();
        collideAndStreamAD();
        writeVTK(this->step);
        this->step += 1;
    }
    storeCfdResults(this->step);
}

template<typename T>
void lbmMixingSimulator<T>::nsSolve() {
    // theta = 10
    this->setBoundaryValues(this->step);
    for (int iT = 0; iT < 10; ++iT){
        this->collideAndStream();
        writeVTK(this->step);
        this->step += 1;
    }
    storeCfdResults(this->step);
}

template<typename T>
void lbmMixingSimulator<T>::adSolve() {
    // theta = 10
    this->setBoundaryValues(this->step);
    for (int iT = 0; iT < 100; ++iT){
        collideAndStreamAD();
        writeVTK(this->step);
        this->step += 1;
    }
    storeCfdResults(this->step);
}

template<typename T>
void lbmMixingSimulator<T>::initValueContainers () {
    // Initialize pressure, flowRate and concentration value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        this->pressures.try_emplace(key, (T) 0.0);
        this->flowRates.try_emplace(key, (T) 0.0);
        std::unordered_map<int, T> tmpConcentrations;
        for (auto& [speciesId, speciesPtr] : species) {
            tmpConcentrations.try_emplace(speciesId, 0.0);
        }
        this->concentrations.try_emplace(key, tmpConcentrations);
    }
}

template<typename T>
void lbmMixingSimulator<T>::initAdConvergenceTracker () {
    // Initialize a convergence tracker for concentrations
    for (auto& [speciesId, specie] : species) {
        this->adConverges.try_emplace(speciesId, std::make_unique<olb::util::ValueTracer<T>> (1000, 1e-1));
        this->averageDensities.try_emplace(speciesId, T(0.0));
        this->custConverges.try_emplace(speciesId, false);
    }
}

template<typename T>
void lbmMixingSimulator<T>::storeConcentrations(std::unordered_map<int, std::unordered_map<int, T>> concentrations_) {
    this->concentrations = concentrations_;
}

template<typename T>
std::unordered_map<int, std::unordered_map<int, T>> lbmMixingSimulator<T>::getConcentrations() const {
    return this->concentrations;
}

template<typename T>
bool lbmMixingSimulator<T>::hasAdConverged() const {
    bool c = true;
    for (auto& [key, converge] : custConverges) {
        if (!converge) {
            c = false;
        }
    }
    return c;
};

}   // namespace arch