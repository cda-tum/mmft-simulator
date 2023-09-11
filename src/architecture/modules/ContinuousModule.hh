#include "ContinuousModule.h"

#include <iostream>

#include <olb2D.h>

#include "CFDModule.h"

namespace arch{

template<typename T>
ContinuousModule<T>::ContinuousModule (
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDModule<T>(id_, name_, pos_, size_, nodes_, openings_, stlFile_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_)
    { 
        this->moduleType = ModuleType::CONTINUOUS;
    } 

template<typename T>
void ContinuousModule<T>::initIntegrals() {
    
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->getPosition()[0] + 0.5*converter->getConversionFactorLength()*Opening.normal[0];
        T posY =  Opening.node->getPosition()[1] - this->getPosition()[1] + 0.5*converter->getConversionFactorLength()*Opening.normal[1];

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure2D<T>> (getLattice(), getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity2D<T> > (getLattice(), getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
        }
    }
}

template<typename T>
void ContinuousModule<T>::prepareLattice () {
    const T omega = converter->getLatticeRelaxationFrequency();

    lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    // Set lattice dynamics
    lattice->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    lattice->template defineDynamics<BGKdynamics>(this->getGeometry(), 1);
    lattice->template defineDynamics<BounceBack>(this->getGeometry(), 2);

    // Set initial conditions
    lattice->defineRhoU(this->getGeometry(), 1, rhoF, uF);
    lattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

    // Set lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(getLattice(), omega, this->getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(getLattice(), omega, this->getGeometry(), key+3);
        }
    }

    this->initIntegrals();

    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();

    std::cout << "[lbmModule] prepare lattice " << this->name << "... OK" << std::endl;
}

template<typename T>
void ContinuousModule<T>::setBoundaryValues (int iT) {

    T pressureLow = -1.0;       
    for (auto& [key, pressure] : this->pressures) {
        if ( pressureLow < 0.0 ) {
            pressureLow = pressure;
        }
        if ( pressure < pressureLow ) {
            pressureLow = pressure;
        }
    }

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            T maxVelocity = (3./2.)*(this->flowRates[key]/(Opening.width));
            T distance2Wall = 0.0*getConverter().getConversionFactorLength()/2.;
            olb::Poiseuille2D<T> poiseuilleU(this->getGeometry(), key+3, getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
            getLattice().defineU(this->getGeometry(), key+3, poiseuilleU);
        } else {
            T rhoV = getConverter().getLatticeDensityFromPhysPressure((this->pressures[key]));
            olb::AnalyticalConst2D<T,T> rho(rhoV);
            getLattice().defineRho(this->getGeometry(), key+3, rho);
        }
    }

}

template<typename T>
void ContinuousModule<T>::lbmInit (T dynViscosity, T density) {

    // Initialize the unit converter for the continuous platform
    T kinViscosity = dynViscosity/density;
    
    this->converter = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>>(
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        kinViscosity,
        density
    );
    this->converter->print();

    this->conversionFactorLength = converter->getConversionFactorLength();
    // Initialize a convergence tracker for pressure
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, this->epsilon);

    std::cout << "[lbmModule] lbmInit " << this->name << "... OK" << std::endl;
}

template<typename T>
void ContinuousModule<T>::writeVTK (int iT) {

    olb::SuperVTMwriter2D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry2D<T,DESCRIPTOR> writeGeometry (getLattice(), this->getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
    }

    if (iT % this->vtkIter == 0) {
        
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
    if (iT % this->statIter == 0) {
        std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
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

    for (int iT = 0; iT < this->theta; ++iT){      
        this->setBoundaryValues(step);
        writeVTK(step);          
        lattice->collideAndStream();
        step += 1;
    }
    getResults(step);
}

template<typename T>
void ContinuousModule<T>::getResults(int iT) {
    //TODO
}

}   // namespace arch