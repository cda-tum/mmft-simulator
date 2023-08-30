#include "ContinuousModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "CFDModule.h"

namespace arch{

template<typename T>
ContinuousModule<T>::ContinuousModule (
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDModule<T,olb::descriptors::D2Q9<>>(id_, pos_, size_, nodes_, openings_, stlFile_, charPhysLength_, charPhysVelocity_, resolution_, relaxationTime_), 
        alpha(alpha_), epsilon(epsilon_)
    { 
        this->moduleType = ModuleType::CONTINUOUS;
    } 

template<typename T>
void ContinuousModule<T>::initIntegrals() {
    
    for (auto& [key, Opening] : moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->getPosition()[0] + 0.5*converter->getConversionFactorLength()*Opening.normal[0];
        T posY =  Opening.node->getPosition()[1] - this->getPosition()[1] + 0.5*converter->getConversionFactorLength()*Opening.normal[1];

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure2D<T>> (getLattice(), getConverter(), getGeometry(),
            position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity2D<T> > (getLattice(), getConverter(), getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
        }
    }
}

template<typename T>
void ContinuousModule<T>::prepareLattice () {
    const T omega = converter->getLatticeRelaxationFrequency();

    lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    // Set lattice dynamics
    lattice->template defineDynamics<NoDynamics>(getGeometry(), 0);
    lattice->template defineDynamics<BGKdynamics>(getGeometry(), 1);
    lattice->template defineDynamics<BounceBack>(getGeometry(), 2);

    // Set initial conditions
    lattice->defineRhoU(getGeometry(), 1, rhoF, uF);
    lattice->iniEquilibrium(getGeometry(), 1, rhoF, uF);

    // Set lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : moduleOpenings) {
        if (groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(getLattice(), omega, getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(getLattice(), omega, getGeometry(), key+3);
        }
    }

    this->initIntegrals();

    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();

    std::cout << "[lbmModule] prepare lattice " << name << "... OK" << std::endl;
}

template<typename T>
void ContinuousModule<T>::setBoundaryValues (int iT) {

    T pressureLow = -1.0;       
    for (auto& [key, pressure] : pressures) {
        if ( pressureLow < 0.0 ) {
            pressureLow = pressure;
        }
        if ( pressure < pressureLow ) {
            pressureLow = pressure;
        }
    }

    for (auto& [key, Opening] : moduleOpenings) {
        if (groundNodes.at(key)) {
            T maxVelocity = (3./2.)*(flowRates[key]/(Opening.width));
            T distance2Wall = 0.0*getConverter().getConversionFactorLength()/2.;
            olb::Poiseuille2D<T> poiseuilleU(getGeometry(), key+3, getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
            getLattice().defineU(getGeometry(), key+3, poiseuilleU);
        } else {
            T rhoV = getConverter().getLatticeDensityFromPhysPressure((pressures[key]));
            olb::AnalyticalConst2D<T,T> rho(rhoV);
            getLattice().defineRho(getGeometry(), key+3, rho);
        }
    }

}

template<typename T>
void ContinuousModule<T>::lbmInit (T dynViscosity, T density) {

    // Initialize the unit converter for the continuous platform
    T kinViscosity = dynViscosity/density;
    
    this->converter = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>>(
        resolution,
        relaxationTime,
        charPhysLength,
        charPhysVelocity,
        kinViscosity,
        density
    );
    this->converter->print();

    // Initialize a convergence tracker for pressure
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);

    std::cout << "[lbmModule] lbmInit " << name << "... OK" << std::endl;
}

template<typename T>
void ContinuousModule<T>::writeVTK (int iT) {

    olb::SuperVTMwriter2D<T> vtmWriter( name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry2D<T,DESCRIPTOR> writeGeometry (getLattice(), getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
    }

    if (iT % vtkIter == 0) {
        
        olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(getLattice(), getConverter());
        olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(getLattice(), getConverter());
        olb::SuperLatticeDensity2D<T,DESCRIPTOR> latDensity(getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);
        
        // write vtk to file system
        vtmWriter.write(iT);
        converge->takeValue(getLattice().getStatistics().getAverageEnergy(), true);
    }
    if (iT % statIter == 0) {
        std::cout << "[writeVTK] " << name << " currently at timestep " << iT << std::endl;
    }

    converge->takeValue(getLattice().getStatistics().getAverageEnergy(), true);

    if (iT % 100 == 0) {
        if (converge->hasConverged()) {
                isConverged = true;
        }
    }

}

template<typename T>
void ContinuousModule<T>::solve() {

    for (int iT = 0; iT < theta; ++iT){      
        this->setBoundaryValues(step);
        writeVTK(step);          
        lattice->collideAndStream();
        step += 1;
    }
    getResults(step);
}

}   // namespace arch