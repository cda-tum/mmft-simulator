#include "OrganModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "CFDModule.h"

namespace arch {

template<typename T>
OrganModule<T>::OrganModule (
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, 
        T epsilon_, T relaxationTime_) : 
        CFDModule<T>(id_, name_, pos_, size_, nodes_, openings_, stlFile_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_)
    { 
        this->moduleType = ModuleType::ORGAN;
    } 

template<typename T>
void OrganModule<T>::lbmInit(T dynViscosity, T density, T physDiffusivity) {

    // Initialize the unit converters for the organ module
    T kinViscosity = dynViscosity/density;

    std::cout << "[OrganModule] Setting Navier-Stokes converter " << this->name << ":" <<std::endl;
    this->converterNS = std::make_shared<const olb::UnitConverterFromRelaxationTimeAndLatticeVelocity<T,NSDESCRIPTOR>> (
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        kinViscosity,
        density
    )
    this->converter.print();

    std::cout << "[OrganModule] Setting Advection Diffusion converter " << this->name << ":" << std::endl;
    olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR> tempAD = std::make_shared<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> (
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        physDiffusivity,
        density
    )
    tempAd.print();

    this->converterAD.try_emplace(0, tempAD);
    this->conversionFactorLength = converterNS->getConversionFactorLength();
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, this->epsilon);

    this->defineOrgan();

    std::cout << "[OrganModule] lbmInit " << this->name << "... OK" << std::endl;
}

template<typename T>
void OrganModule<T>::initIntegrals() {

    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX = Opening.node->getPosition()[0] - this->getPosition()[0] + 0.5*converter->getConversionFactorLength()*Opening.normal[0];
        T posY = Opening.node->getPosition()[1] - this->getPosition()[1] + 0.5*converter->getConversionFactorLength()*Opening.normal[1];

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        // Prepare the plane integrals to measure pressure and velocity values at openings.
        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D> (getLatticeNS(), getConverterNS(), this->getGeometry(),
                position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared<olb::SuperPlaneIntegralFluxVelocity2D<T>> (getLatticeNS(), getConverter(), this->getGeometry(),
                position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
        }

        // Prepare the plane integrals to measure concentration values at openings.
        for (auto& [adKey, LatticeAD] : this->getLatticesAD()) {
            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>  meanConcentration;
            concentration = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D<T>> (getLatticeAD(adKey), getConverter(adKey), this->getGeometry(),
                position, Opening.tangent, materials);
            meanConcentration.try_emplace(concKey, concentration);
            this->meanConcentrations.try_emplace(key, meanConcentration);
        }
    }
}

template<typename T>
void OrganModule<T>::prepareLattice() {
    
    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    /**
     * Prepare the NS lattice
    */
    const T omega = converter->getLatticeRelaxationFrequency();
    latticeNS = std::make_shared<olb::SuperLattice<T, NSDESCRIPTOR>>(this->getGeometry());

    // Set lattice dynamics
    latticeNS->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    latticeNS->template defineDynamics<BGKDynamics>(this->getGeometry(), 1);
    latticeNS->template defineDynamics<BounceBack>(this->getGeometry(), 2);

    // Set initial conditions
    latticeNS->defineRhoU(this->getGeometry(), 1, rhoF, uF);
    latticeNS->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

    // Set lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(getLattice(), omega, this->getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(getLattice(), omega, this->getGeometry(), key+3);
        }
    }
    
    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();

    /**
     * Prepare the AD lattices
    */
    for (auto& [adKey, lattice] : this->getLatticesAD()) {
        const T AdOmega = getConverterAD(adKey).getLatticeRelaxationFrequency();
        std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> latticeAD = std::make_shared<olb::SuperLattice<T,ADDESCRIPTOR>>(this->getGeometry());
        
        // Set lattice dynamics
        latticeAD->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
        latticeAD->template defineDynamics<ADDynamics>(this->getGeometry(), 1);
        latticeAD->template defineDynamics<ADDynamics>(this->getGeometry(), 2);

        // Set initial conditions
        latticeAD->defineRhoU(this->getGeometry(), 1, rhoF, uF);
        latticeAD->defineField<olb::descriptors::VELOCITY>(1, zeroVelocity);
        latticeAD->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

        // Add wall boundary
        setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(lattice, AdOmega, this->getGeometry(), 2, fluxWall.at(adKey), fluxWall.at(adKey));
        // Add inlet boundary
        // TODO
        // Add outlet boundary
        // TODO

        // Add Tissue boundary
        setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(lattice, AdOmega, this->getGeometry(), 3, Vmax.at(adKey), Km.at(adKey));
    }

    this->initIntegrals();

    std::cout << "[lbmModule] prepare lattice " << this->name << "... OK" << std::endl;

}

template<typename T>
void OrganModule<T>::prepareCoupling() {

    for  (auto& [adKey, lattice] : this->getLatticeAD()) {
        T velFactor = this->converterNS->getConversionFactorVelocity() / this->converterAD->getConversionFactorVelocity();
    }
    
    NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,NSDESCRIPTOR> coupling(0, this->converterNS->getLatticeLength(this->size[0]), 0, this->converterNS->getLatticeLength(this->size[0]), converterAD, velFactor);
    std::cout << "[OrganModule] prepare coupling " << this->name << "... OK" << std::endl;
}

template<typename T>
void OrganModule<T>::setBoundaryValues(int iT) {

}

template<typename T>
void OrganModule<T>::solve() {
    
}

template<typename T>
void OrganModule<T>::getResults() {
    
}

template<typename T>
void OrganModule<T>::writeVTK(int iT_) {
    
}

template<typename T>
void OrganModule<T>::evaluateShearStress() {
    
}

} // namespace arch

