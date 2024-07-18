#include "olbContinuous.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmOocSimulator<T>::lbmOocSimulator (
    int id_, std::string name_, std::string stlFile_, Tissue<T>* tissue_, std::string organStlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, 
    std::unordered_map<int, arch::Opening<T>> openings_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, 
    T alpha_, T resolution_, T epsilon_, T relaxationTime_, T adRelaxationTime_) : 
        lbmSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_), 
        tissue(tissue_), organStlFile(organStlFile_), adRelaxationTime(adRelaxationTime_)
    { 
        this->cfdModule->setModuleTypeLbm();
    } 

template<typename T>
void lbmOocSimulator<T>::prepareGeometry () {

    bool print = false;

    this->stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, this->converter->getConversionFactorLength());
    #ifdef VERBOSE
        print = true;
        std::cout << "[lbmModule] reading STL file " << this->name << "... OK" << std::endl;
    #endif
    this->stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*this->stlReader);
    #ifdef VERBOSE
        std::cout << "[lbmModule] create 2D indicator " << this->name << "... OK" << std::endl;
    #endif

    olb::Vector<T,2> origin(-0.5*this->converter->getConversionFactorLength(), -0.5*this->converter->getConversionFactorLength());
    olb::Vector<T,2> extend(this->cfdModule->getSize()[0] + this->converter->getConversionFactorLength(), this->cfdModule->getSize()[1] + this->converter->getConversionFactorLength());
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    this->cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, this->converter->getConversionFactorLength(), 1);
    this->loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*this->cuboidGeometry);
    this->geometry = std::make_shared<olb::SuperGeometry<T,2>> (
        *this->cuboidGeometry, 
        *this->loadBalancer);

    #ifdef VERBOSE
        std::cout << "[lbmModule] generate geometry " << this->name << "... OK" << std::endl;   
    #endif 

    this->geometry->rename(0, 2);
    this->geometry->rename(2, 1, *this->stl2Dindicator);

    #ifdef VERBOSE
        std::cout << "[lbmModule] generate 2D geometry from STL  " << this->name << "... OK" << std::endl;
    #endif

    for (auto& [key, Opening] : this->moduleOpenings ) {
        // The unit vector pointing to the extend (opposite origin) of the opening
        T x_origin =    Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0]
                        - 0.5*Opening.width*Opening.tangent[0];
        T y_origin =   Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1]
                        - 0.5*Opening.width*Opening.tangent[1];
        
        // The unit vector pointing to the extend
        T x_extend = Opening.width*Opening.tangent[0] - this->converter->getConversionFactorLength()*Opening.normal[0];
        T y_extend = Opening.width*Opening.tangent[1] - this->converter->getConversionFactorLength()*Opening.normal[1];

        // Extend can only have positive values, hence the following transformation
        if (x_extend < 0 ){
            x_extend *= -1;
            x_origin -= x_extend;
        }
        if (y_extend < 0 ){
            y_extend *= -1;
            y_origin -= y_extend;
        }

        olb::Vector<T,2> originO (x_origin, y_origin);
        olb::Vector<T,2> extendO (x_extend, y_extend);
        olb::IndicatorCuboid2D<T> opening(extendO, originO);
        
        this->geometry->rename(2, key+3, 1, opening);
    }

    this->geometry->clean(print);
    this->geometry->checkForErrors(print);

    #ifdef VERBOSE
        std::cout << "[lbmModule] prepare geometry " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmOocSimulator<T>::prepareLattice () {
   
    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    std::vector<T> zeroVelocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    /**
     * Prepare the NS lattice
    */
    const T omega = this->converter->getLatticeRelaxationFrequency();
    this->lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(this->getGeometry());

    // Set NS lattice dynamics
    this->lattice->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    this->lattice->template defineDynamics<BGKdynamics>(this->getGeometry(), 1);
    this->lattice->template defineDynamics<BounceBack>(this->getGeometry(), 2);

    // Set initial conditions
    this->lattice->defineRhoU(this->getGeometry(), 1, rhoF, uF);
    this->lattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

    // Set NS lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(this->getLatticeNS(), omega, this->getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(this->getLatticeNS(), omega, this->getGeometry(), key+3);
        }
    }
    
    // Initialize NS lattice with relaxation frequency omega
    this->lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    this->lattice->initialize();

    /**
     * Prepare the AD lattices
    */
    for (auto& [speciesId, converter] : this->converterAD) {
        const T adOmega = this->getConverterAD(speciesId).getLatticeRelaxationFrequency();
        std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> adLattice = std::make_shared<olb::SuperLattice<T,ADDESCRIPTOR>>(this->getGeometry());
        
        // Set AD lattice dynamics
        adLattice->template defineDynamics<NoADDynamics>(this->getGeometry(), 0);
        adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 1);
        adLattice->template defineDynamics<ADDynamics>(this->getGeometry(), 2);

        // Set initial conditions
        adLattice->defineRhoU(this->getGeometry(), 1, rhoF, uF);
        adLattice->template defineField<olb::descriptors::VELOCITY>(this->getGeometry(), 1, uF);
        adLattice->iniEquilibrium(this->getGeometry(), 1, rhoF, uF);

        // Add wall boundary
        olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 2, fluxWall.at(speciesId), fluxWall.at(speciesId));

        // In-/ and Outlets boundaries depend on flow direction, and are handled in setBoundaryValues()

        // Add Tissue boundary
        olb::setFunctionalRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, adOmega, this->getGeometry(), 3, tissue->getVmax(speciesId), tissue->getkM(speciesId));

        adLattices.try_emplace(speciesId, adLattice);
    }

    this->initIntegrals();

    std::cout << "[OocLbmModule] prepare lattice " << this->name << "... OK" << std::endl;
}

template<typename T>
void lbmOocSimulator<T>::prepareCoupling() {

    std::vector<olb::SuperLattice<T,ADDESCRIPTOR>*> adLattices;
    std::vector<T> velFactors;
    for (auto& [speciesId, lattice] : this->getLatticeAD()) {
        adLattices.emplace_back(this->getLatticeAD(speciesId).get());
        velFactors[speciesId] = this->converterNS->getConversionFactorVelocity() / this->converterAD->getConversionFactorVelocity();
    }

    olb::NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR> coupling(0, this->converterNS->getLatticeLength(this->size[0]), 0, this->converterNS->getLatticeLength(this->size[0]), adConverters, velFactors);
    this->lattice.addLatticeCoupling(coupling, adLattices);
    std::cout << "[OocLbmModule] prepare coupling " << this->name << "... OK" << std::endl;
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
                olb::setAdvectionDiffusionTemperatureBoundary<T,ADDESCRIPTOR>(*adLattice, this->getConverterAD(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3);
                olb::AnalyticalConst2D<T,T> one( 1. );
                olb::AnalyticalConst2D<T,T> inConc(concentrations.at(key).at(speciesId));
                this->lattice->defineRho(this->getGeometry(), key+3, one + inConc);
            }
        } else if (outflowNodes.count(key)) {
            for (auto& [speciesId, adLattice] : adLattices) {
                olb::setRegularizedHeatFluxBoundary<T,ADDESCRIPTOR>(*adLattice, this->getConverterAD(speciesId).getLatticeRelaxationFrequency(), this->getGeometry(), key+3, &zeroFlux);
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

    for (auto& [key, Opening] : this->moduleOpenings) {
        // If the node is an outflow, write the concentration value
        if (this->flowRates.at(key) < 0.0) {
            for (auto& [speciesId, adLattice] : adLattices) {
                this->meanConcentrations.at(key).at(speciesId)->operator()(output,input);
                this->concentrations.at(key).at(speciesId) = output[0];
                if (this->step % this->statIter == 0) {
                    this->meanConcentrations.at(key).at(speciesId)->print();
                }
            }
        }
    }
}

template<typename T>
void lbmOocSimulator<T>::lbmInit (T dynViscosity, 
                            T density) {

    // Initialize the unit converters for the organ module
    T kinViscosity = dynViscosity/density;

    std::cout << "[OocLbmModule] Setting Navier-Stokes converter " << this->name << ":" <<std::endl;
    this->converterNS = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>> (
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        kinViscosity,
        density
    );
    this->conversionFactorLength = this->converter->getConversionFactorLength();
    this->converterNS->print();

    std::cout << "[OocLbmModule] Setting Advection Diffusion converter " << this->name << ":" << std::endl;
    for (auto& [speciesId, specie] : tissue->getSpecies()) {
        std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> tempAD = std::make_shared<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T,ADDESCRIPTOR>> (
            this->resolution,
            this->relaxationTime,
            this->charPhysLength,
            this->charPhysVelocity,
            specie->getDiffusivity(),
            density
        );
        tempAD->print();

        this->adConverters.try_emplace(speciesId, tempAD);
        this->adConverges.try_emplace(speciesId, std::make_unique<olb::util::ValueTracer<T>> (this->stepIter, this->epsilon));
    }

    std::cout << "[OocLbmModule] lbmInit " << this->name << "... OK" << std::endl;
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
        
        // write all concentrations
        for (auto& [speciesId, adLattice] : adLattices) {
            olb::SuperLatticeDensity2D<T,ADDESCRIPTOR> concentration( getLatticeAD(speciesId) );
            concentration.getName() = "concentration " + std::to_string(speciesId);
        }

        // write vtk to file system
        vtmWriter.write(iT);
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
    getResults(this->step);
}


}   // namespace arch