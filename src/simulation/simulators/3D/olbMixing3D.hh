#include "olbMixing3D.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmMixingSimulator3D<T>::lbmMixingSimulator3D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, Specie<T>*> species_,
    std::unordered_map<int, arch::Opening<T>> openings_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, 
    T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmSimulator3D<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, 
                            relaxationTime_), species(species_), adRelaxationTime(adRelaxationTime_)
{   
    std::cout << "Creating module and setting its type to lbm" << std::endl;
    this->cfdModule->setModuleTypeLbm();
    fluxWall.try_emplace(int(0), &zeroFlux);
} 

template<typename T>
lbmMixingSimulator3D<T>::lbmMixingSimulator3D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, Specie<T>*> species_,
    std::unordered_map<int, arch::Opening<T>> openings_, std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, 
    T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmMixingSimulator3D<T>(id_, name_, stlFile_, cfdModule_, openings_, updateScheme_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, 
                            relaxationTime_, species_, adRelaxationTime_)
{   
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmMixingSimulator3D<T>::lbmInit (T dynViscosity, T density) {

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
void lbmMixingSimulator3D<T>::prepareGeometry () {

    bool print = false;
    T dx = this->getDx();

    #ifdef VERBOSE
        print = true;
    #endif

    this->readGeometryStl(dx, print);
    this->readOpenings(dx, print);

    if (print) {
        std::cout << "[lbmSimulator] prepare geometry " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::prepareLattice () {

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
void lbmMixingSimulator3D<T>::setBoundaryValues (int iT) {

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            this->setFlowProfile(key, Opening.width);
        } else {
            this->setPressure(key);
        }
        setConcentration3D(key);
    }    
}

template<typename T>
void lbmMixingSimulator3D<T>::solve() {
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
void lbmMixingSimulator3D<T>::nsSolve() {
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
void lbmMixingSimulator3D<T>::adSolve() {
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
bool lbmMixingSimulator3D<T>::hasAdConverged() const {
    bool c = true;
    for (auto& [key, converge] : custConverges) {
        if (!converge) {
            c = false;
        }
    }
    return c;
};


template<typename T>
void lbmMixingSimulator3D<T>::writeVTK (int iT) {

    bool print = false;
    #ifdef VERBOSE
        print = true;
    #endif

    olb::SuperVTMwriter3D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry3D<T,DESCRIPTOR> writeGeometry (this->getLattice(), this->getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + olb::createFileName( this->name ) + ".pvd";
    }

    if (iT % 1000 == 0) {
        
        olb::SuperLatticePhysVelocity3D<T,DESCRIPTOR> velocity(this->getLattice(), this->getConverter());
        olb::SuperLatticePhysPressure3D<T,DESCRIPTOR> pressure(this->getLattice(), this->getConverter());
        olb::SuperLatticeDensity3D<T,DESCRIPTOR> latDensity(this->getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);

        vtmWriter.write(iT);
        
        // write all concentrations
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::SuperLatticeDensity3D<T,ADDESCRIPTOR> concentration( getAdLattice(speciesId) );
            concentration.getName() = "concentration " + std::to_string(speciesId);
            vtmWriter.write(concentration, iT);
        }

        // write vtk to file system
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + "data/" + olb::createFileName( this->name, iT ) + ".vtm";
        this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), !print);
        for (auto& [key, adConverge] : this->adConverges) {
            //adConverge->takeValue(getAdLattice(key).getStatistics().getAverageRho(), print);
            T newRho = getAdLattice(key).getStatistics().getAverageRho();
            if (std::abs(this->averageDensities.at(key) - newRho) < 1e-5) {
                this->custConverges.at(key) = true;
            }
            this->averageDensities.at(key) = newRho;
        }
    }
    if (iT %1000 == 0) {
        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
        #endif
    }

    this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (this->converge->hasConverged()) {
                this->isConverged = true;
        }
    }

}

template<typename T>
std::unordered_map<int, T> lbmMixingSimulator3D<T>::getAdOmegas() {
    std::unordered_map<int, T> adOmegas;
    for (auto& [key, adConverter] : adConverters) {
        adOmegas.try_emplace(key, adConverter->getLatticeRelaxationFrequency());
    }
    return adOmegas;
}

template<typename T>
void lbmMixingSimulator3D<T>::initValueContainers () {
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
void lbmMixingSimulator3D<T>::initAdConvergenceTracker () {
    // Initialize a convergence tracker for concentrations
    for (auto& [speciesId, specie] : species) {
        this->adConverges.try_emplace(speciesId, std::make_unique<olb::util::ValueTracer<T>> (1000, 1e-1));
        this->averageDensities.try_emplace(speciesId, T(0.0));
        this->custConverges.try_emplace(speciesId, false);
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::initAdConverters (T density) {
    for (auto& [speciesId, specie] : this->species) {
        std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> tempAD = std::make_shared<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> (
            this->resolution,
            this->relaxationTime,
            this->charPhysLength,
            this->charPhysVelocity,
            specie->getDiffusivity(),
            density
        );
        #ifdef VERBOSE
            tempAD->print();
        #endif

        this->adConverters.try_emplace(speciesId, tempAD);
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::prepareAdLattice (const T adOmega, int speciesId) {

    std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> adLattice = std::make_shared<olb::SuperLattice<T,ADDESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> zeroVelocity(T(0), T(0));
    olb::AnalyticalConst3D<T,T> rhoF(1);
    olb::AnalyticalConst3D<T,T> rhoZero(0);
    olb::AnalyticalConst3D<T,T> uZero(zeroVelocity);

    // Set AD lattice dynamics
    adLattice->template defineDynamics<NoADDynamics>(this->getGeometry(), 0);
    adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 1);
    adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 2);

    // Set initial conditions
    adLattice->defineRhoU(this->getGeometry(), 0, rhoZero, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 0, rhoZero, uZero);
    adLattice->defineRhoU(this->getGeometry(), 1, rhoF, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uZero);
    adLattice->defineRhoU(this->getGeometry(), 2, rhoF, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 2, rhoF, uZero);
    
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 0, uZero);
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 1, uZero);
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 2, uZero);

    for (auto& [key, Opening] : this->moduleOpenings) {
        adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), key+3);
        adLattice->iniEquilibrium(this->getGeometry(), key+3, rhoF, uZero);
        adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), key+3, uZero);
    }

    // Add wall boundary
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 2, this->fluxWall.at(0), this->fluxWall.at(0));

    adLattices.try_emplace(speciesId, adLattice);

}

template<typename T>
void lbmMixingSimulator3D<T>::initConcentrationIntegralPlane() {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [adKey, LatticeAD] : adLattices) {
        for (auto& [key, Opening] : this->moduleOpenings) {

            T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
            T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

            std::vector<T> position = {posX, posY};
            std::vector<int> materials = {1, key+3};

            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure3D<T>>>  meanConcentration;
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure3D<T>> concentration;
            concentration = std::make_shared<olb::SuperPlaneIntegralFluxPressure3D<T>> (getAdLattice(adKey), getAdConverter(adKey), this->getGeometry(),
                position, Opening.tangent, materials);
            meanConcentration.try_emplace(adKey, concentration);
            this->meanConcentrations.try_emplace(key, meanConcentration);
        }
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::initAdLattice(int adKey) {
    const T adOmega = getAdConverter(adKey).getLatticeRelaxationFrequency();
    getAdLattice(adKey).template setParameter<olb::descriptors::OMEGA>(adOmega);
    getAdLattice(adKey).template setParameter<olb::collision::TRT::MAGIC>(1./12.);
    getAdLattice(adKey).initialize();
}

template<typename T>
void lbmMixingSimulator3D<T>::prepareCoupling() {
    
    std::vector<olb::SuperLattice<T,ADDESCRIPTOR>*> adLatticesVec;
    std::vector<T> velFactors;
    for (auto& [speciesId, adLattice] : adLattices) {
        adLatticesVec.emplace_back(adLattices[speciesId].get());
        //velFactors.emplace_back(this->converter->getConversionFactorVelocity() / this->adConverters[speciesId]->getConversionFactorVelocity());
        velFactors.emplace_back(1.0);
    }
    olb::NavierStokesAdvectionDiffusionSingleCouplingGenerator3D<T,DESCRIPTOR> coupling(0, this->converter->getLatticeLength(this->cfdModule->getSize()[0]), 0, this->converter->getLatticeLength(this->cfdModule->getSize()[1]), velFactors);
    this->lattice->addLatticeCoupling(coupling, adLatticesVec);
}

template<typename T>
void lbmMixingSimulator3D<T>::executeCoupling() {
    this->lattice->executeCoupling();
}

template<typename T>
void lbmMixingSimulator3D<T>::collideAndStreamAD() {
    for (auto& [speciesId, adLattice] : adLattices) {
        adLattice->collideAndStream();
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::setConcentration3D (int key) {
    // Set the boundary concentrations for inflows and outflows
    // If the boundary is an inflow
    if (this->flowRates.at(key) >= 0.0) {
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::setAdvectionDiffusionTemperatureBoundary<T,ADDESCRIPTOR>(*adLattice, getAdConverter(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3);
            olb::AnalyticalConst3D<T,T> one( 1. );
            olb::AnalyticalConst3D<T,T> inConc(this->concentrations.at(key).at(speciesId));
            adLattice->defineRho(this->getGeometry(), key+3, one + inConc);
        }
    }
    // If the boundary is an outflow
    else if (this->flowRates.at(key) < 0.0) {
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::setRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, getAdConverter(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3, &this->zeroFlux);
        }
    }
    else {
        std::cerr << "Error: Invalid Flow Type Boundary Node." << std::endl;
        exit(1);
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::storeCfdResults (int iT) {
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
            storeAdResults(key, iT);
        }
    }
}

template<typename T>
void lbmMixingSimulator3D<T>::storeAdResults (int nodeId, int iT) {
    int input[1] = { };
    T output[10];

    for (auto& [speciesId, adLattice] : adLattices) {
        this->meanConcentrations.at(nodeId).at(speciesId)->operator()(output,input);
        this->concentrations.at(nodeId).at(speciesId) = output[0];
        if (iT % 1000 == 0) {
            this->meanConcentrations.at(nodeId).at(speciesId)->print();
        }
    }
}

}   // namespace arch