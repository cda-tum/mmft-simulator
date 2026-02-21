#include "olbMixing.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmMixingSimulator<T>::lbmMixingSimulator (
    size_t id_, std::string name_, std::shared_ptr<arch::CfdModule<T>> cfdModule_, std::unordered_map<size_t, const Specie<T>*> species_,
    T resolution_, T charPhysLength_, T charPhysVelocity_, T epsilon_, T relaxationTime_, T adRelaxationTime_) :
        lbmSimulator<T>(id_, name_, cfdModule_, resolution_, charPhysLength_, charPhysVelocity_, epsilon_, relaxationTime_),
        species(species_), adRelaxationTime(adRelaxationTime_) 
{ 
    fluxWall.try_emplace(int(0), &zeroFlux);
}

template<typename T>
lbmMixingSimulator<T>::lbmMixingSimulator (
    size_t id_, std::string name_, std::shared_ptr<arch::CfdModule<T>> cfdModule_, std::unordered_map<size_t, const Specie<T>*> species_,
    std::shared_ptr<mmft::Scheme<T>> updateScheme_, T resolution_, T charPhysLength_, T charPhysVelocity_, T epsilon_, T relaxationTime_, T adRelaxationTime_) :
        lbmSimulator<T>(id_, name_, cfdModule_, updateScheme_, resolution_, charPhysLength_, charPhysVelocity_, epsilon_, relaxationTime_),
        species(species_), adRelaxationTime(adRelaxationTime_) 
{ 
    fluxWall.try_emplace(int(0), &zeroFlux);
}

template<typename T>
std::string lbmMixingSimulator<T>::getDefaultName(size_t id) {
    return std::string("olbMixing-" + std::to_string(id));
}

template<typename T>
void lbmMixingSimulator<T>::lbmInit (T dynViscosity, T density) {

    this->setOutputDir();
    initValueContainers();
    this->initNsConverter(dynViscosity, density);
    initAdConverters(density);
    this->initNsConvergeTracker();
    initAdConvergenceTracker();
    this->setIsInitialized();

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmMixingSimulator<T>::prepareLattice () {

    /**
     * Prepare the NS lattice
    */
    const T omega = this->getConverter().getLatticeRelaxationFrequency();
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

    prepareCoupling();

    std::cout << "[lbmSimulator] prepare coupling " << this->name << "... OK" << std::endl;
}

template<typename T>
void lbmMixingSimulator<T>::setBoundaryValues (int iT) {
    lbmSimulator<T>::setBoundaryValues(iT);
}

template<typename T>
void lbmMixingSimulator<T>::setConcBoundaryValues (int iT) {
    for (auto& [key, Opening] : this->cfdModule->getOpenings()) {
        setConcentration2D(key);
    }
}

template<typename T>
void lbmMixingSimulator<T>::storeCfdResults (int iT) {
    // Store the pressure and flow rate values at the module nodes
    lbmSimulator<T>::storeCfdResults(iT);

    // Store the concentration values at the module nodes
    int input[1] = { };
    T output[10];
    
    for (auto& [key, Opening] : this->cfdModule->getOpenings()) {
        // If the node is an outflow, write the concentration value
        if (this->getFlowDirection(key) < 0.0) {
            for (auto& [speciesId, adLattice] : adLattices) {
                this->meanConcentrations.at(key).at(speciesId)->operator()(output,input);
                this->concentrations.at(key).at(speciesId) = output[0];
                if (iT % 1000 == 0) {
                    this->meanConcentrations.at(key).at(speciesId)->print();
                }
            }
        }
    }
    
}

template<typename T>
void lbmMixingSimulator<T>::writeVTK (int iT) {

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
        this->getConverge().takeValue(this->getLattice().getStatistics().getAverageEnergy(), !print);
        for (auto& [key, adConverge] : adConverges) {
            //adConverge->takeValue(getAdLattice(key).getStatistics().getAverageRho(), print);
            T newRho = getAdLattice(key).getStatistics().getAverageRho();
            if (std::abs(averageDensities.at(key) - newRho) < 1e-5) {
                custConverges.at(key) = true;
            }
            averageDensities.at(key) = newRho;
        }
    }
    if (iT %1000 == 0) {
        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
            for (auto& [key, Opening] : this->cfdModule->getOpenings()) {
                for (auto& [speciesId, adLattice] : adLattices) {
                    meanConcentrations.at(key).at(speciesId)->print();
                }
            }
        #endif
    }

    this->getConverge().takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (this->getConverge().hasConverged()) {
                this->getIsConverged() = true;
        }
    }

}

template<typename T>
std::tuple<T, T> lbmMixingSimulator<T>::getConcentrationBounds(size_t adKey) {
    olb::SuperLatticePhysPressure2D<T,ADDESCRIPTOR> pressure(getAdLattice(adKey), getAdConverter(adKey));
    olb::SuperMin2D<T> minPressureF( pressure, this->getGeometry(), 1);
    olb::SuperMax2D<T> maxPressureF( pressure, this->getGeometry(), 1);
    int input[0];
    T minPressure[1];
    T maxPressure[1];
    minPressureF( minPressure, input );
    maxPressureF( maxPressure, input );
    return std::tuple<T, T> {minPressure[0], maxPressure[0]};
}

template<typename T>
void lbmMixingSimulator<T>::writeConcentrationPpm(size_t adKey, T min, T max, int imgResolution) {
    // Color map options are 'earth'|'water'|'air'|'fire'|'leeloo'
    std::string colorMap = "leeloo";
    olb::SuperLatticePhysPressure2D<T,ADDESCRIPTOR> pressure(getAdLattice(adKey), getAdConverter(adKey));
    olb::BlockReduction2D2D<T> planeReductionP(pressure, imgResolution);
    olb::BlockGifWriter<T> ppmWriter(colorMap);
    ppmWriter.write(planeReductionP, min, max, this->getStep(), this->name+"_concentration-"+std::to_string(adKey)+"_");
}

template<typename T>
void lbmMixingSimulator<T>::solve() {
    // this->checkInitialized();
    // int theta = this->updateScheme->getTheta();
    // this->setBoundaryValues(this->getStep());
    // for (int iT = 0; iT < theta; ++iT){
    //     writeVTK(this->getStep());
    //     this->getLattice().collideAndStream();
    //     this->getLattice().executeCoupling();
    //     for (auto& [speciesId, adLattice] : adLattices) {
    //         adLattice->collideAndStream();
    //     }
    //     this->getStep() += 1;
    // }
    // storeCfdResults(this->getStep());
    nsSolve();
}

template<typename T>
void lbmMixingSimulator<T>::solveCFD(size_t maxIter) {
    std::cout << "Solving the NS" << std::endl;

    // Check if initialized and set boundary conditions
    this->checkInitialized();
    this->setBoundaryValues(this->getStep());

    // Main simulation loop
    for (int iT = 0; iT < int(maxIter); ++iT){    
        writeVTK(this->getStep());       
        this->getLattice().collideAndStream();
        this->getStep() += 1;
        // Check convergence
        if (this->getIsConverged()) { break; }
    }

    std::cout << "Storing the NS results" << std::endl;
    lbmSimulator<T>::storeCfdResults(this->getStep());

    std::cout << "Solving the AD" << std::endl;
    // Advection Diffusion Solver
    std::cout << "Setting concentration BC" << std::endl;
    this->setConcBoundaryValues(this->getStep());
    std::cout << "Starting AD solve loop" << std::endl;
    for (int iT = 0; iT < int(maxIter); ++iT) {
        writeVTK(this->getStep());
        for (auto& [speciesId, adLattice] : adLattices) {
            // this->getLattice().executeCoupling();
            adLattice->collideAndStream();
        }
        this->getStep() += 1;
    }
    std::cout << "Finished AD solve loop" << std::endl;
    storeCfdResults(this->getStep());
}

template<typename T>
void lbmMixingSimulator<T>::executeCoupling() {
    this->getLattice().executeCoupling();
    std::cout << "[lbmSimulator] Execute NS-AD coupling " << this->name << "... OK" << std::endl;
}


template<typename T>
void lbmMixingSimulator<T>::nsSolve() {
    this->checkInitialized();
    int theta = this->updateScheme->getTheta();
    this->setBoundaryValues(this->getStep());
    for (int iT = 0; iT < theta; ++iT){
        writeVTK(this->getStep());
        this->getLattice().collideAndStream();
        this->getStep() += 1;
    }
    storeCfdResults(this->getStep());
}

template<typename T>
void lbmMixingSimulator<T>::adSolve(size_t maxIter) {
    // theta = 10
    this->setConcBoundaryValues(this->getStep());
    for (int iT = 0; iT < int(maxIter); ++iT){
        writeVTK(this->getStep());
        for (auto& [speciesId, adLattice] : adLattices) {
            adLattice->collideAndStream();
        }
        this->getStep() += 1;
    }
    storeCfdResults(this->getStep());
}

template<typename T>
void lbmMixingSimulator<T>::initValueContainers () {
    // Initialize pressure and flowRate value-containers
    lbmSimulator<T>::initValueContainers();

    // Initialize concentration value-containers
    for (auto& [key, node] : this->cfdModule->getOpenings()) {
        std::unordered_map<size_t, T> tmpConcentrations;
        for (auto& [speciesId, speciesPtr] : species) {
            tmpConcentrations.try_emplace(speciesId, 0.0);
        }
        this->concentrations.try_emplace(key, tmpConcentrations);
    }
}

template<typename T>
void lbmMixingSimulator<T>::initAdConverters (T density) {
    for (auto& [speciesId, specie] : species) {
        std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> tempAD = std::make_shared<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> (
            this->getResolution(),
            this->getTau(),
            this->getCharPhysLength(),
            this->getCharPhysVelocity(),
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
void lbmMixingSimulator<T>::initAdConvergenceTracker () {
    // Initialize a convergence tracker for concentrations
    for (auto& [speciesId, specie] : species) {
        this->adConverges.try_emplace(speciesId, std::make_unique<olb::util::ValueTracer<T>> (1000, 1e-1));
        this->averageDensities.try_emplace(speciesId, T(0.0));
        this->custConverges.try_emplace(speciesId, false);
    }
}

template<typename T>
void lbmMixingSimulator<T>::prepareAdLattice (const T adOmega, size_t speciesId) {

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

    for (auto& [key, Opening] : this->cfdModule->getOpenings()) {
        adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), key+3);
        adLattice->iniEquilibrium(this->getGeometry(), key+3, rhoF, uZero);
        adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), key+3, uZero);
    }

    // Add wall boundary
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 2, fluxWall.at(0), fluxWall.at(0));

    adLattices.try_emplace(speciesId, adLattice);

}

template<typename T>
void lbmMixingSimulator<T>::initAdLattice(size_t adKey) {
    const T adOmega = getAdConverter(adKey).getLatticeRelaxationFrequency();
    getAdLattice(adKey).template setParameter<olb::descriptors::OMEGA>(adOmega);
    getAdLattice(adKey).template setParameter<olb::collision::TRT::MAGIC>(1./12.);
    getAdLattice(adKey).initialize();
}

template<typename T>
void lbmMixingSimulator<T>::initConcentrationIntegralPlane(size_t adKey) {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->cfdModule->getOpenings()) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, int(key)+3};

        std::unordered_map<size_t, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>  meanConcentration;
        std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> concentration;
        concentration = std::make_shared<olb::SuperPlaneIntegralFluxPressure2D<T>> (getAdLattice(adKey), getAdConverter(adKey), this->getGeometry(),
            position, Opening.tangent, materials);
        meanConcentration.try_emplace(adKey, concentration);
        this->meanConcentrations.try_emplace(key, meanConcentration);
    }
}

template<typename T>
void lbmMixingSimulator<T>::prepareCoupling() {
    
    std::vector<olb::SuperLattice<T,ADDESCRIPTOR>*> adLatticesVec;
    std::vector<T> velFactors;
    for (auto& [speciesId, adLattice] : adLattices) {
        adLatticesVec.emplace_back(adLattices[speciesId].get());
        //velFactors.emplace_back(this->converter->getConversionFactorVelocity() / this->adConverters[speciesId]->getConversionFactorVelocity());
        velFactors.emplace_back(1.0);
    }
    olb::NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR> coupling(0, this->getConverter().getLatticeLength(this->cfdModule->getSize()[0]), 0, this->getConverter().getLatticeLength(this->cfdModule->getSize()[1]), velFactors);
    this->getLattice().addLatticeCoupling(coupling, adLatticesVec);
}

template<typename T>
void lbmMixingSimulator<T>::setConcentration2D (int key) {
    size_t setBC = 0;
    // Set the boundary concentrations for inflows and outflows
    // If the boundary is an inflow
    if (this->getFlowDirection(key) > 0.0) {
        std::cout << "Setting conc BC for inflow" << std::endl;
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::setAdvectionDiffusionTemperatureBoundary<T,ADDESCRIPTOR>(*adLattice, this->getGeometry(), key+3);
            olb::AnalyticalConst2D<T,T> one( 1. );
            olb::AnalyticalConst2D<T,T> inConc(concentrations.at(key).at(speciesId));
            adLattice->defineRho(this->getGeometry(), key+3, one + inConc);
            setBC++;
        }
    }
    // If the boundary is an outflow or there is no flow
    else if (this->getFlowDirection(key) <= 0.0) {
        std::cout << "Setting conc BC for outflow" << std::endl;
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::setRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, getAdConverter(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3, &zeroFlux);
            setBC++;
        }
    }
    std::cout << "Done setting Conc BC" << std::endl;
    std::cout << "Set total of " << setBC << " concentration BCs" << std::endl;
}

template<typename T>
void lbmMixingSimulator<T>::storeConcentrations(std::unordered_map<size_t, std::unordered_map<size_t, T>> concentrations_) {
    assert(concentrations_.size() <= this->concentrations.size());
    for (auto& [key, value] : concentrations_) {
        auto it = this->concentrations.find(key);
        if (it == this->concentrations.end()) {
            throw std::logic_error("Cannot store concentration for node" + std::to_string(key) + ". Not a BC.");
        }
        it->second = value;
    }
}

template<typename T>
const std::unordered_map<size_t, std::unordered_map<size_t, T>>& lbmMixingSimulator<T>::getConcentrations() const {
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