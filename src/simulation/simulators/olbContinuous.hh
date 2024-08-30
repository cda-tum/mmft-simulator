#include "olbContinuous.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmSimulator<T>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_), 
        charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), 
        epsilon(epsilon_), relaxationTime(relaxationTime_)
{ 
    this->cfdModule->setModuleTypeLbm();
} 

template<typename T>
lbmSimulator<T>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulator(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, relaxationTime_)
{ 
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmSimulator<T>::lbmInit (T dynViscosity, T density) {

    setOutputDir();
    initValueContainers();
    initNsConverter(dynViscosity, density);
    initNsConvergeTracker();    

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T>::prepareGeometry () {

    bool print = false;
    T dx = getDx();

    #ifdef VERBOSE
        print = true;
    #endif

    readGeometryStl(dx, print);
    readOpenings(dx, print);

    if (print) {
        std::cout << "[lbmSimulator] prepare geometry " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator<T>::prepareLattice () {

    const T omega = getOmega();

    prepareNsLattice(omega);
    initPressureIntegralPlane();
    initFlowRateIntegralPlane();
    initNsLattice(omega);

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T>::solve() {
    // theta = 10
    this->setBoundaryValues(step);
    for (int iT = 0; iT < 10; ++iT){    
        this->writeVTK(step);            
        collideAndStream();
        step += 1;
    }
    storeCfdResults(step);
}

template<typename T>
void lbmSimulator<T>::setOutputDir () {
    if (!std::filesystem::is_directory(this->vtkFolder) || !std::filesystem::exists(this->vtkFolder)) {
        std::filesystem::create_directory(this->vtkFolder);
    }

    olb::singleton::directories().setOutputDir( this->vtkFolder+"/" );  // set output directory     
}   

template<typename T>
void lbmSimulator<T>::initValueContainers () {

    // Initialize pressure and flow rate value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        pressures.try_emplace(key, (T) 0.0);
        flowRates.try_emplace(key, (T) 0.0);
    }

}

template<typename T>
void lbmSimulator<T>::initNsConvergeTracker () {
    // Initialize a convergence tracker for pressure
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);
}

template<typename T>
void lbmSimulator<T>::storePressures(std::unordered_map<int, T> pressure_) {
    this->pressures = pressure_;
}

template<typename T>
void lbmSimulator<T>::storeFlowRates(std::unordered_map<int, T> flowRate_) {
    this->flowRates = flowRate_;
}

}   // namespace arch