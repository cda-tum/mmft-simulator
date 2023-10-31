#include "OrganModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "CFDModule.h"
#include "../../postProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"
#include "../../postProcessors/setFunctionalRegularizedHeatFlux.h"

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
void OrganModule<T>::lbmInit(T dynViscosity, T density, std::unordered_map<int, T> physDiffusivities) {

    // Initialize the unit converters for the organ module
    T kinViscosity = dynViscosity/density;

    std::cout << "[OrganModule] Setting Navier-Stokes converter " << this->name << ":" <<std::endl;
    this->converterNS = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,NSDESCRIPTOR>> (
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        kinViscosity,
        density
    );
    this->conversionFactorLength = converterNS->getConversionFactorLength();
    this->converterNS->print();

    std::cout << "[OrganModule] Setting Advection Diffusion converter " << this->name << ":" << std::endl;
    for (int c=0; c<concentrationCount; c++) {
        std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> tempAD = std::make_shared<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> (
            this->resolution,
            this->relaxationTime,
            this->charPhysLength,
            this->charPhysVelocity,
            physDiffusivities.at(c),
            density
        );
        tempAD->print();

        this->converterAD.try_emplace(c, tempAD);
        this->convergesAD.try_emplace(c, std::make_unique<olb::util::ValueTracer<T>> (this->stepIter, this->epsilon));
    }

    std::cout << "[OrganModule] lbmInit " << this->name << "... OK" << std::endl;
}

template<typename T>
void OrganModule<T>::initIntegrals() {

    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX = Opening.node->getPosition()[0] - this->getPosition()[0] + 0.5*converterNS->getConversionFactorLength()*Opening.normal[0];
        T posY = Opening.node->getPosition()[1] - this->getPosition()[1] + 0.5*converterNS->getConversionFactorLength()*Opening.normal[1];

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        // Prepare the plane integrals to measure pressure and velocity values at openings.
        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D<T>> (getLatticeNS(), getConverterNS(), this->getGeometry(),
                position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared<olb::SuperPlaneIntegralFluxVelocity2D<T>> (getLatticeNS(), getConverterNS(), this->getGeometry(),
                position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
        }

        // Prepare the plane integrals to measure concentration values at openings.
        for (auto& [adKey, LatticeAD] : this->latticesAD) {
            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>  meanConcentration;
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> concentration;
            concentration = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D<T>> (getLatticeAD(adKey), getConverterAD(adKey), this->getGeometry(),
                position, Opening.tangent, materials);
            meanConcentration.try_emplace(adKey, concentration);
            this->meanConcentrations.try_emplace(key, meanConcentration);
        }
    }
}

template<typename T>
void OrganModule<T>::prepareLattice() {
    
    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    std::vector<T> zeroVelocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    /**
     * Prepare the NS lattice
    */
    const T omega = converterNS->getLatticeRelaxationFrequency();
    latticeNS = std::make_shared<olb::SuperLattice<T, NSDESCRIPTOR>>(this->getGeometry());

    // Set NS lattice dynamics
    latticeNS->template defineDynamics<NoNSDynamics>(this->getGeometry(), 0);
    latticeNS->template defineDynamics<BGKDynamics>(this->getGeometry(), 1);
    latticeNS->template defineDynamics<BounceBack>(this->getGeometry(), 2);

    // Set initial conditions
    latticeNS->defineRhoU(this->getGeometry(), 1, rhoF, uF);
    latticeNS->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

    // Set NS lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(this->getLatticeNS(), omega, this->getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(this->getLatticeNS(), omega, this->getGeometry(), key+3);
        }
    }
    
    // Initialize NS lattice with relaxation frequency omega
    latticeNS->template setParameter<olb::descriptors::OMEGA>(omega);
    latticeNS->initialize();

    /**
     * Prepare the AD lattices
    */
    for (auto& [adKey, converter] : this->converterAD) {
        const T AdOmega = this->getConverterAD(adKey).getLatticeRelaxationFrequency();
        std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> latticeAD = std::make_shared<olb::SuperLattice<T,ADDESCRIPTOR>>(this->getGeometry());
        
        // Set AD lattice dynamics
        latticeAD->template defineDynamics<NoADDynamics>(this->getGeometry(), 0);
        latticeAD->template defineDynamics<ADDynamics>(this->getGeometry(), 1);
        latticeAD->template defineDynamics<ADDynamics>(this->getGeometry(), 2);

        // Set initial conditions
        latticeAD->defineRhoU(this->getGeometry(), 1, rhoF, uF);
        latticeAD->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 1, uF);
        latticeAD->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

        // Add wall boundary
        olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*latticeAD, AdOmega, this->getGeometry(), 2, fluxWall.at(adKey), fluxWall.at(adKey));

        // In-/ and Outlets boundaries depend on flow direction, and are handled in setBoundaryValues()

        // Add Tissue boundary
        olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*latticeAD, AdOmega, this->getGeometry(), 3, Vmax.at(adKey), Km.at(adKey));

        latticesAD.try_emplace(adKey, latticeAD);
    }

    this->initIntegrals();

    std::cout << "[lbmModule] prepare lattice " << this->name << "... OK" << std::endl;

}

template<typename T>
void OrganModule<T>::prepareCoupling() {

    std::vector<olb::SuperLattice<T,ADDESCRIPTOR>*> adLattices;
    std::vector<T> velFactors;
    for (auto& [adKey, lattice] : this->getLatticeAD()) {
        adLattices.emplace_back(this->getLatticeAD(adKey).get());
        velFactors[adKey] = this->converterNS->getConversionFactorVelocity() / this->converterAD->getConversionFactorVelocity();
    }

    olb::NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,NSDESCRIPTOR> coupling(0, this->converterNS->getLatticeLength(this->size[0]), 0, this->converterNS->getLatticeLength(this->size[0]), converterAD, velFactors);
    latticeNS.addLatticeCoupling(coupling, adLattices);
    std::cout << "[OrganModule] prepare coupling " << this->name << "... OK" << std::endl;
}

template<typename T>
void OrganModule<T>::setBoundaryValues(int iT) {

    // Set Boundary Continuous Flow
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
            T distance2Wall = 0.0*getConverterNS().getConversionFactorLength()/2.;
            olb::Poiseuille2D<T> poiseuilleU(this->getGeometry(), key+3, getConverterNS().getLatticeVelocity(maxVelocity), distance2Wall);
            getLatticeNS().defineU(this->getGeometry(), key+3, poiseuilleU);
        } else {
            T rhoV = getConverterNS().getLatticeDensityFromPhysPressure((this->pressures[key]));
            olb::AnalyticalConst2D<T,T> rho(rhoV);
            getLatticeNS().defineRho(this->getGeometry(), key+3, rho);
        }
    }

    std::set<int> inflowNodes;
    std::set<int> outflowNodes;

    // Check In-/Outflow
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->flowRates.at(key) >= 0.0) {
            inflowNodes.emplace(key);
        } else if (this->flowRates.at(key) < 0.0) {
            outflowNodes.emplace(key);
        }
    }

    // Set Boundary Concentrations
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (inflowNodes.count(key)) {
            for (auto& [adKey, lattice] : latticesAD) {
                olb::setAdvectionDiffusionTemperatureBoundary<T,ADDESCRIPTOR>(*lattice, this->getConverterAD(adKey).getLatticeRelaxationFrequency(), this->getGeometry(), key+3);
                olb::AnalyticalConst2D<T,T> one( 1. );
                olb::AnalyticalConst2D<T,T> inConc(concentrations.at(key).at(adKey));
                lattice->defineRho(this->getGeometry(), key+3, one + inConc);
            }
        } else if (outflowNodes.count(key)) {
            for (auto& [adKey, lattice] : latticesAD) {
                olb::setRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*lattice, this->getConverterAD(adKey).getLatticeRelaxationFrequency(), this->getGeometry(), key+3, &zeroFlux);
            }
        } else {
            std::cerr << "Error: Invalid Flow Type Boundary Node." << std::endl;
            exit(1);
        }
    }

}

template<typename T>
void OrganModule<T>::solve() {
    for (int iT = 0; iT < this->theta; ++iT){      
        this->setBoundaryValues(this->step);
        writeVTK(this->step);        
        latticeNS->collideAndStream();
        for (auto& [adKey, lattice] : latticesAD) {
            lattice->collideAndStream();
        }
        this->step += 1;
    }
    getResults();
}

template<typename T>
void OrganModule<T>::getResults() {

    int input[1] = { };
    T output[3];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            this->meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
            if (this->step % this->statIter == 0) {
                this->meanPressures.at(key)->print();
            }
        } else {
            this->fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
            if (this->step % this->statIter == 0) {
                this->fluxes.at(key)->print();
            }
        }
    }

    for (auto& [key, Opening] : this->moduleOpenings) {
        // If the node is an outflow, write the concentration value
        if (this->flowRates.at(key) < 0.0) {
            for (auto& [adKey, lattice] : latticesAD) {
                this->meanConcentrations.at(key).at(adKey)->operator()(output,input);
                this->concentrations.at(key).at(adKey) = output[0];
                if (this->step % this->statIter == 0) {
                    this->meanConcentrations.at(key).at(adKey)->print();
                }
            }
        }
    }
}

template<typename T>
void OrganModule<T>::writeVTK(int iT) {
    olb::SuperVTMwriter2D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry2D<T,NSDESCRIPTOR> writeGeometry (getLatticeNS(), this->getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
    }

    if (iT % this->vtkIter == 0) {
        
        olb::SuperLatticePhysVelocity2D<T,NSDESCRIPTOR> velocity(getLatticeNS(), getConverterNS());
        olb::SuperLatticePhysPressure2D<T,NSDESCRIPTOR> pressure(getLatticeNS(), getConverterNS());
        olb::SuperLatticeDensity2D<T,NSDESCRIPTOR> latDensity(getLatticeNS());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);

        // write all concentrations
        for (auto& [adKey, lattice] : latticesAD) {
            olb::SuperLatticeDensity2D<T,ADDESCRIPTOR> concentration( getLatticeAD(adKey) );
            concentration.getName() = "concentration " + std::to_string(adKey);
            vtmWriter.addFunctor(concentration);
        }
        
        // write vtk to file system
        vtmWriter.write(iT);
        convergeNS->takeValue(getLatticeNS().getStatistics().getAverageEnergy(), true);
    }
    if (iT % this->statIter == 0) {
        std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
    }

    convergeNS->takeValue(getLatticeNS().getStatistics().getAverageEnergy(), true);

    if (iT % 100 == 0) {
        if (convergeNS->hasConverged()) {
                this->isConverged = true;
        }
    }
}

template<typename T>
void OrganModule<T>::evaluateShearStress() {
    // TODO
}

} // namespace arch

