#include "olbOoc.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmOocSimulator<T>::lbmOocSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<Tissue<T>> tissue_, std::string organStlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, 
    std::unordered_map<int, Specie<T>*> species_, std::unordered_map<int, arch::Opening<T>> openings_,
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmMixingSimulator<T>(id_, name_, stlFile_, cfdModule_, species_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, 
                              resolution_, epsilon_, relaxationTime_, adRelaxationTime_), 
        tissue(tissue_), organStlFile(organStlFile_) 
{
    this->cfdModule->setModuleTypeLbm();
    this->fluxWall.try_emplace(int(0), &this->zeroFlux);
} 

template<typename T>
lbmOocSimulator<T>::lbmOocSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<Tissue<T>> tissue_, std::string organStlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, 
    std::unordered_map<int, Specie<T>*> species_, std::unordered_map<int, arch::Opening<T>> openings_, std::shared_ptr<mmft::Scheme<T>> updateScheme_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmOocSimulator<T>(id_, name_, stlFile_, tissue_, organStlFile_, cfdModule_, species_, openings_, updateScheme_, resistanceModel_, charPhysLength_, charPhysVelocity_, 
                              resolution_, epsilon_, relaxationTime_, adRelaxationTime_)
{
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmOocSimulator<T>::lbmInit (T dynViscosity, T density) {

    this->setOutputDir();
    this->initValueContainers();
    this->initNsConverter(dynViscosity, density);
    this->initAdConverters(density);
    this->initNsConvergeTracker();
    this->initAdConvergenceTracker();

    Vmax = (*tissue->getVmax(0))*this->getAdConverter(0).getPhysDeltaT();

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmOocSimulator<T>::prepareGeometry () {

    bool print = false;
    T dx = this->getConverter().getConversionFactorLength();

    #ifdef VERBOSE
        print = true;
    #endif

    this->readGeometryStl(dx, print);
    this->readOpenings(dx);
    readOrganStl(dx);
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
    for (auto& [speciesId, converter] : this->adConverges) {
        const T adOmega = this->getAdConverter(speciesId).getLatticeRelaxationFrequency();
        prepareAdLattice(adOmega, speciesId);
    }
    
    /**
     * Initialize the integral fluxes for the in- and outlets
     */
    this->initPressureIntegralPlane();
    this->initFlowRateIntegralPlane();
    for (auto& [adKey, LatticeAD] : this->adLattices) {
        this->initConcentrationIntegralPlane(adKey);
    }

    /**
     * Initialize the lattices
     */
    this->initNsLattice(omega);
    for (auto& [speciesId, converter] : this->adConverges) {
        this->initAdLattice(speciesId);
    }

    std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;

    this->prepareCoupling();

    std::cout << "[lbmSimulator] prepare coupling " << this->name << "... OK" << std::endl;
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
        for (auto& [speciesId, adLattice] : this->adLattices) {
            olb::SuperLatticeDensity2D<T,ADDESCRIPTOR> concentration( this->getAdLattice(speciesId) );
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
void lbmOocSimulator<T>::readOrganStl (const T dx) {
    // Define Organ area
    organStlReader = std::make_shared<olb::STLreader<T>>(organStlFile, dx);
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
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 2, this->fluxWall.at(0), this->fluxWall.at(0));

    // Add Tissue boundary
    olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 3, &Vmax, tissue->getkM(speciesId));

    this->adLattices.try_emplace(speciesId, adLattice);

}

}   // namespace arch