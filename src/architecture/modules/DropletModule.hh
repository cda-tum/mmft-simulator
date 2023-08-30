#include "DropletModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "CFDModule.h"

namespace arch {

template<typename T>
DropletModule<T>::DropletModule(
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T resolution_, T relaxationTime_) : 
        CFDModule<T,olb::descriptors::D2Q9<>>(id_, pos_, size_, nodes_, openings_, stlFile_, charPhysLength_, charPhysVelocity_, resolution_, relaxationTime_)
    {
        this->moduleType = ModuleType::DROPLET;
    }

template<typename T>
void DropletModule<T>::lbmInit() {

    const T omega = converter->getLatticeRelaxationFrequency();
}

template<typename T>
void DropletModule<T>::initIntegrals() {
    
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
void DropletModule<T>::prepareLattice() {

    lattice1 = std::make_shared<olb::SuperLattice<T,DESCRIPTOR>>(getGeometry());
    lattice2 = std::make_shared<olb::SuperLattice<T,DESCRIPTOR>>(getGeometry());

    // Set lattice dynamics
    lattice1->defineDynamics<NoDynamics>(getGeometry(), 0);
    lattice2->defineDynamics<NoDynamics>(getGeometry(), 0);

    lattice1->defineDynamics<ForcedBGKdynamics>(getGeometry(), 1);
    lattice2->defineDynamics<FreeEnergyBGKdynamics>(getGeometry(), 1);

    lattice1->defineDynamics<NoDynamics>(getGeometry(), 2);
    lattice2->defineDynamics<NoDynamics>(getGeometry(), 2);

    lattice1->defineDynamics<NoDynamics>(getGeometry(), 3);
    lattice2->defineDynamics<NoDynamics>(getGeometry(), 3);

    lattice1->defineDynamics<NoDynamics>(getGeometry(), 4);
    lattice2->defineDynamics<NoDynamics>(getGeometry(), 4);

    lattice1->defineDynamics<NoDynamics>(getGeometry(), 5);
    lattice2->defineDynamics<NoDynamics>(getGeometry(), 5);

    // Add wall boundary
    setFreeEnergyWallBoundary<T,DESCRIPTOR>(getLattice1(), getGeometry(), 2, alpha, kappa1, kappa2, h1, h2, 1);
    setFreeEnergyWallBoundary<T,DESCRIPTOR>(getLattice2(), getGeometry(), 2, alpha, kappa1, kappa2, h1, h2, 2);

    // Bulk initial conditions
    std::vector<T> u(2, T());
    olb::AnalyticalConst2D<T,T> zeroVelocity( u );
    olb::AnalyticalConst2D<T,T> zero ( 0. );
    olb::AnalyticalConst2D<T,T> one ( 1. );
    olb::AnalyticalIdentity2D<T,T> rho(one);
    olb::AnalyticalIdentity2D<T,T> phi(one);

    auto allIndicator = geometry->getMaterialIndicator({1, 2, 3, 4, 5}); 
    lattice1->iniEquilibrium( allIndicator, rho, zeroVelocity );
    lattice2->iniEquilibrium( allIndicator, phi, zeroVelocity );

    lattice1->setParameter<olb::descriptors::OMEGA>(omega);
    lattice2->setParameter<olb::descriptors::OMEGA>(omega);
    lattice2->setParameter<olb::collision::FreeEnergy::GAMMA>(gama);

    // Initialize lattices
    lattice1->initialize();
    lattice2->initialize();

    lattice1->communicate();
    lattice2->communicate();

    {
    auto& communicator = lattice1->getCommunicator(olb::PostPostProcess());
    communicator.requestField<olb::POPULATION>();
    communicator.requestOverlap(lattice1->getOverlap());
    communicator.exchangeRequests();
    }
    {
    auto& communicator = lattice2->getCommunicator(olb::PostPostProcess());
    communicator.requestField<olb::POPULATION>();
    communicator.requestOverlap(lattice2->getOverlap());
    communicator.exchangeRequests();
    }

    std::cout << "[DropletModule] prepare lattice " << name << " ... OK" << std::endl;
}

template<typename T>
void DropletModule<T>::prepareCoupling() {
    
    // Bulk couplings
    olb::FreeEnergyChemicalPotentialGenerator2D<T,DESCRIPTOR> coupling1( alpha, kappa1, kappa2 );
    olb::FreeEnergyForceGenerator2D<T,DESCRIPTOR> coupling2;

    lattice1->addLatticeCoupling<DESCRIPTOR>(getGeometry(), 1, coupling1, lattice2);
    lattice2->addLatticeCoupling<DESCRIPTOR>(getGeometry(), 1, coupling2, lattice1);

    {
        auto& communicator = lattice1->getCommunicator(PostCoupling());
        communicator.requestField<olb::CHEM_POTENTIAL>();
        communicator.requestOverlap(lattice1->getOverlap());
        communicator.exchangeRequests();
    }
    {
        auto& communicator = sLattice2.getCommunicator(PreCoupling());
        communicator.requestField<olb::CHEM_POTENTIAL>();
        communicator.requestOverlap(lattice2->getOverlap());
        communicator.exchangeRequests();
    }

    std::cout << "[DropletModule] prepare lattice coupling " << name << " ... OK" << std::endl;
}

template<typename T>
void DropletModule<T>::setInletBoundaries(int material) {

    setFreeEnergyInletBoundary<T,DESCRIPTOR>(getLattice1(), omega, material, "velocity", 1);
    setFreeEnergyInletBoundary<T,DESCRIPTOR>(getLattice2(), omega, material, "velocity", 2);

    // Inlet coupling
    olb::FreeEnergyInletOutletGenerator2D<T,DESCRIPTOR> coupling;
    lattice2->addLatticeCoupling<DESCRIPTOR>(getGeometry(), material, coupling, lattice1);
}

template<typename T>
void DropletModule<T>::setOutletBoundaries(int material) {

    setFreeEnergyOutletBoundary<T,DESCRIPTOR>(getLattice1(), omega, material, "density", 1);
    setFreeEnergyOutletBoundary<T,DESCRIPTOR>(getLattice2(), omega, material, "density", 2);

    // Outlet coupling
    olb::FreeEnergyDensityOutletGenerator2D<T,DESCRIPTOR> coupling( outletDensity );
    lattice1->addLatticeCoupling<DESCRIPTOR>(getGeometry(), material, coupling, lattice2);

}

template<typename T>
void DropletModule<T>::setBoundaryValues(int iT_) {

}

template<typename T>
void DropletModule<T>::addDroplet(T pos_[2], T radius_) { 

    olb::IndicatorCircle2D<T> indCircle ( pos_, radius_ );
    olb::SmoothIndicatorCircle2D<T,T> droplet (indCircle, converter.getPhysLength(alpha));

    lattice2->defineRho(droplet, T(-1.0));
}

template<typename T>
void DropletModule<T>::addDroplet(T origin_[2], T extend_[2], T theta) { 

    olb::Vector<T,2> origin = {origin_[0], origin_[1]};
    olb::Vector<T,2> extend = {extend_[0], extend_[1]};

    olb::IndicatorCuboid2D<T> indCuboid(origin, extend, theta);
    olb::SmoothIndicatorCuboid2D<T,T> droplet( indCuboid, converter.getPhysLength(alpha));

    lattice2->defineRho(droplet, T(-1.0));
}

template<typename T>
void DropletModule<T>::scanDroplets() {

}

template<typename T>
void DropletModule<T>::delDroplet(int Id_) {

}

template<typename T>
void DropletModule<T>::solve() {

    this->setBoundaryValues(step);

    for (int iT = 0; iT < theta; ++iT){

        writeVTK(step);

        // Collide and stream execution
        lattice1->collideAndStream();
        lattice2->collideAndStream();

        // Execute coupling between the two lattices
        lattice1->executeCoupling();
        lattice2->executeCoupling();

        step += 1;
    }

    getResults(step);
}

template<typename T>
void DropletModule<T>::getResults(int iT_) {

}

template<typename T>
void DropletModule<T>::writeVTK(int iT_) {

    if ( iT == 0 ) {
        // Writes the geometry, cuboid no. and rank no. as vti file for visualization
        olb::SuperLatticeGeometry2D<T, DESCRIPTOR> geometry( getLattice1(), getGeometry() );
        vtmWriter.write( geometry );
        vtmWriter.createMasterFile();
    }

    if (iT % statIter == 0) {
        std::cout << "[writeVTK] " << name << " currently at timestep " << iT << std::endl;
        lattice1->getStatistics().print(iT, converter->getPhysTime(iT));
        lattice2->getStatistics().print(iT, converter->getPhysTime(iT));
    }

    if (iT % vtkIter == 0) {

        olb::SuperLatticePhysVelocity2D<T, DESCRIPTOR> velocity(getLattice1());
        olb::SuperLatticePhysPressure2D<T, DESCRIPTOR> pressure(getLattice1());
        olb::SuperLatticeDensity2D<T, DESCRIPTOR> density1(getLattice1());
        density1.getName() = "rho";
        olb::SuperLatticeDensity2D<T, DESCRIPTOR> density2(getLattice2());
        density2.getName() = "phi";

        olb::AnalyticalConst2D<T,T> half_( 0.5 );
        olb::SuperLatticeFfromAnalyticalF2D<T, DESCRIPTOR> half(half_, getLattice1());

        olb::SuperIdentity2D<T,T> c1 (half*(density1+density2));
        c1.getName() = "density-fluid-1";
        olb::SuperIdentity2D<T,T> c2 (half*(density1-density2));
        c2.getName() = "density-fluid-2";

        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(density1);
        vtmWriter.addFunctor(density2);
        vtmWriter.addFunctor(c1);
        vtmWriter.addFunctor(c2);
        vtmWriter.write(iT);

    }

}

}   /// namespace arch