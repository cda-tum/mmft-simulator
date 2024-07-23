#include "olbContinuous.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmOocSimulator<T>::lbmOocSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<Tissue<T>> tissue_, std::string organStlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, 
    std::unordered_map<int, arch::Opening<T>> openings_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, 
    T alpha_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_), 
        tissue(tissue_), organStlFile(organStlFile_), adRelaxationTime(adRelaxationTime_)
    { 
        this->cfdModule->setModuleTypeLbm();
        fluxWall.try_emplace(int(0), &zeroFlux);
    } 

template<typename T>
void lbmOocSimulator<T>::lbmInit (T dynViscosity, T density) {

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
void lbmOocSimulator<T>::prepareGeometry () {

    bool print = false;

    #ifdef VERBOSE
        print = true;
    #endif

    this->readGeometryStl(print);
    this->readOpenings();
    readOrganStl();

    this->geometry->clean(print);
    this->geometry->checkForErrors(print);

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] prepare geometry " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmOocSimulator<T>::prepareLattice () {

    /**
     * Prepare the NS lattice
    */
    const T omega = this->converter->getLatticeRelaxationFrequency();
    prepareNsLattice(omega);
    
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
    for (auto& [adKey, LatticeAD] : adLattices) {
        initConcentrationIntegralPlane(adKey);
    }

    /**
     * Initialize the lattices
     */
    this->initNsLattice(omega);
    for (auto& [speciesId, converter] : adConverges) {
        initAdLattice(speciesId);
    }

    std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;
}

template<typename T>
void lbmOocSimulator<T>::prepareCoupling() {
    
    std::vector<olb::SuperLattice<T,ADDESCRIPTOR>*> adLatticesVec;
    std::vector<T> velFactors;
    for (auto& [speciesId, adLattice] : adLattices) {
        adLatticesVec.emplace_back(adLattices[speciesId].get());
        //velFactors.emplace_back(this->converter->getConversionFactorVelocity() / this->adConverters[speciesId]->getConversionFactorVelocity());
        velFactors.emplace_back(1.0);
    }
    olb::NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR> coupling(0, this->converter->getLatticeLength(this->cfdModule->getSize()[0]), 0, this->converter->getLatticeLength(this->cfdModule->getSize()[1]), velFactors);
    this->lattice->addLatticeCoupling(coupling, adLatticesVec);
    std::cout << "[lbmSimulator] prepare coupling " << this->name << "... OK" << std::endl;
}

template<typename T>
void lbmOocSimulator<T>::setBoundaryValues (int iT) {

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
            T distance2Wall = this->getConverter().getConversionFactorLength()/2.;
            this->flowProfiles.at(key) = std::make_shared<olb::Poiseuille2D<T>>(this->getGeometry(), key+3, this->getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
            this->getLattice().defineU(this->getGeometry(), key+3, *this->flowProfiles.at(key));
        } else {
            T rhoV = this->getConverter().getLatticeDensityFromPhysPressure((this->pressures[key]));
            this->densities.at(key) = std::make_shared<olb::AnalyticalConst2D<T,T>>(rhoV);
            this->getLattice().defineRho(this->getGeometry(), key+3, *this->densities.at(key));
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
            for (auto& [speciesId, adLattice] : adLattices) {
                olb::setAdvectionDiffusionTemperatureBoundary<T,ADDESCRIPTOR>(*adLattice, getAdConverter(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3);
                olb::AnalyticalConst2D<T,T> one( 1. );
                olb::AnalyticalConst2D<T,T> inConc(0.1);
                //olb::AnalyticalConst2D<T,T> inConc(concentrations.at(key).at(speciesId));
                adLattice->defineRho(this->getGeometry(), key+3, one + inConc);
            }
        } else if (outflowNodes.count(key)) {
            for (auto& [speciesId, adLattice] : adLattices) {
                olb::setRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, getAdConverter(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3, &zeroFlux);
            }
        } else {
            std::cerr << "Error: Invalid Flow Type Boundary Node." << std::endl;
            exit(1);
        }
    }
    
}

template<typename T>
void lbmOocSimulator<T>::getResults (int iT) {
    int input[1] = { };
    T output[10];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            this->meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    this->meanPressures.at(key)->print();
                #endif
            }
        } else {
            this->fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    this->fluxes.at(key)->print();
                #endif
            }
        }
    }
    /*
    for (auto& [key, Opening] : this->moduleOpenings) {
        // If the node is an outflow, write the concentration value
        if (this->flowRates.at(key) < 0.0) {
            for (auto& [speciesId, adLattice] : adLattices) {
                this->meanConcentrations.at(key).at(speciesId)->operator()(output,input);
                this->concentrations.at(key).at(speciesId) = output[0];
                if (iT % 1000 == 0) {
                    this->meanConcentrations.at(key).at(speciesId)->print();
                }
            }
        }
    }
    */
}

template<typename T>
void lbmOocSimulator<T>::writeVTK (int iT) {

    bool print = false;
    #ifdef VERBOSE
        print = true;
    #endif

    olb::SuperVTMwriter2D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry2D<T,DESCRIPTOR> writeGeometry (this->getLattice(), this->getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + olb::createFileName( this->name ) + ".pvd";
    }

    if (iT % 1000 == 0) {
        
        olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(this->getLattice(), this->getConverter());
        olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(this->getLattice(), this->getConverter());
        olb::SuperLatticeDensity2D<T,DESCRIPTOR> latDensity(this->getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);

        vtmWriter.write(iT);
        
        // write all concentrations
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::SuperLatticeDensity2D<T,ADDESCRIPTOR> concentration( getAdLattice(speciesId) );
            concentration.getName() = "concentration " + std::to_string(speciesId);
            vtmWriter.write(concentration, iT);
        }

        // write vtk to file system
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + "data/" + olb::createFileName( this->name, iT ) + ".vtm";
        this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);
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
void lbmOocSimulator<T>::solve() {
    // theta = 10
    for (int iT = 0; iT < 10; ++iT){
        this->setBoundaryValues(this->step);
        writeVTK(this->step);
        this->lattice->collideAndStream();
        for (auto& [speciesId, adLattice] : adLattices) {
            adLattice->collideAndStream();
        }
        this->step += 1;
    }
    this->lattice->executeCoupling();
    getResults(this->step);
}

template<typename T>
void lbmOocSimulator<T>::initValueContainers () {
    // Initialize pressure, flowRate and concentration value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        this->pressures.try_emplace(key, (T) 0.0);
        this->flowRates.try_emplace(key, (T) 0.0);
        this->concentrations.try_emplace(key, std::vector<T>(tissue->getSpecies().size(), 0.0));
    }
}

template<typename T>
void lbmOocSimulator<T>::initAdConverters (T density) {
    for (auto& [speciesId, specie] : tissue->getSpecies()) {
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

        Vmax = (*tissue->getVmax(0))*tempAD->getPhysDeltaT();
    }
}

template<typename T>
void lbmOocSimulator<T>::initAdConvergenceTracker () {
    // Initialize a convergence tracker for concentrations
    for (auto& [speciesId, specie] : tissue->getSpecies()) {
        this->adConverges.try_emplace(speciesId, std::make_unique<olb::util::ValueTracer<T>> (this->stepIter, this->epsilon));
    }
}

template<typename T>
void lbmOocSimulator<T>::readOrganStl () {
    // Define Organ area
    organStlReader = std::make_shared<olb::STLreader<T>>(organStlFile, this->converter->getConversionFactorLength());
    organStl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*organStlReader);
    this->geometry->rename(1, 3, *organStl2Dindicator);
}

template<typename T>
void lbmOocSimulator<T>::prepareNsLattice (const T omega) {

    this->lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    // Set lattice dynamics
    this->lattice->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    this->lattice->template defineDynamics<BGKdynamics>(this->getGeometry(), 1);
    this->lattice->template defineDynamics<BounceBack>(this->getGeometry(), 2);
    this->lattice->template defineDynamics<BounceBack>(this->getGeometry(), 3);

    // Set initial conditions
    this->lattice->defineRhoU(this->getGeometry(), 1, rhoF, uF);
    this->lattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

    // Set lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(this->getLattice(), omega, this->getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(this->getLattice(), omega, this->getGeometry(), key+3);
        }
    }
}

template<typename T>
void lbmOocSimulator<T>::prepareAdLattice (const T adOmega, int speciesId) {

    std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> adLattice = std::make_shared<olb::SuperLattice<T,ADDESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> zeroVelocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> rhoZero(0);
    olb::AnalyticalConst2D<T,T> uZero(zeroVelocity);

    // Set AD lattice dynamics
    adLattice->template defineDynamics<NoADDynamics>(this->getGeometry(), 0);
    adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 1);
    adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 2);
    adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 3);

    // Set initial conditions
    adLattice->defineRhoU(this->getGeometry(), 0, rhoZero, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 0, rhoZero, uZero);
    adLattice->defineRhoU(this->getGeometry(), 1, rhoF, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uZero);
    adLattice->defineRhoU(this->getGeometry(), 2, rhoF, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 2, rhoF, uZero);
    adLattice->defineRhoU(this->getGeometry(), 3, rhoF, uZero);
    adLattice->iniEquilibrium(this->getGeometry(), 3, rhoF, uZero);
    
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 0, uZero);
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 1, uZero);
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 2, uZero);
    adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 3, uZero);

    for (auto& [key, Opening] : this->moduleOpenings) {
        adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), key+3);
        adLattice->iniEquilibrium(this->getGeometry(), key+3, rhoF, uZero);
        adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), key+3, uZero);
    }

    // Add wall boundary
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 2, fluxWall.at(0), fluxWall.at(0));

    // Add Tissue boundary
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 3, &Vmax, tissue->getkM(speciesId));

    adLattices.try_emplace(speciesId, adLattice);

}

template<typename T>
void lbmOocSimulator<T>::initAdLattice(int adKey) {
    const T adOmega = getAdConverter(adKey).getLatticeRelaxationFrequency();
    getAdLattice(adKey).template setParameter<olb::descriptors::OMEGA>(adOmega);
    getAdLattice(adKey).template setParameter<olb::collision::TRT::MAGIC>(1./12.);
    getAdLattice(adKey).initialize();
}

template<typename T>
void lbmOocSimulator<T>::initConcentrationIntegralPlane(int adKey) {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>  meanConcentration;
        std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> concentration;
        concentration = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D<T>> (getAdLattice(adKey), getAdConverter(adKey), this->getGeometry(),
            position, Opening.tangent, materials);
        meanConcentration.try_emplace(adKey, concentration);
        this->meanConcentrations.try_emplace(key, meanConcentration);
    }
}

}   // namespace arch