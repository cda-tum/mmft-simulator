#include "olbContinuous.h"
#include <filesystem>

namespace sim{

//=====================================================================================
//==========================  lbmSimulatorBase ========================================
//=====================================================================================

template<typename T, int DIM>
lbmSimulatorBase<T,DIM>::lbmSimulatorBase (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDSimulator<T,DIM>(id_, name_, stlFile_, cfdModule_, openings_, alpha_, resistanceModel_), 
        charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), 
        epsilon(epsilon_), relaxationTime(relaxationTime_)
    { 
        this->cfdModule->setModuleTypeLbm();
    } 

template<typename T, int DIM>
void lbmSimulatorBase<T,DIM>::lbmInit (T dynViscosity, 
                                    T density) {
    // Create network with fully connected graph and set initial resistances

    if (!std::filesystem::is_directory(this->vtkFolder) || !std::filesystem::exists(this->vtkFolder)) {
        std::filesystem::create_directory(this->vtkFolder);
    }

    olb::singleton::directories().setOutputDir( this->vtkFolder+"/" );  // set output directory     

    T kinViscosity = dynViscosity/density;

    // There must be more than 1 node to have meaningful flow in the module domain
    assert(this->moduleOpenings.size() > 1);
    // We must have exactly one opening assigned to each boundaryNode
    assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());

    this->converter = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>>(
        resolution,
        relaxationTime,
        charPhysLength,
        charPhysVelocity,
        kinViscosity,
        density
    );

    #ifdef VERBOSE
        this->converter->print();
    #endif

    // Initialize pressure, flowRate and resistance value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        pressures.try_emplace(key, (T) 0.0);
        flowRates.try_emplace(key, (T) 0.0);
    }

    // Initialize a convergence tracker for pressure
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);

    #ifdef VERBOSE
        std::cout << "[lbmModule] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T, int DIM>
void lbmSimulatorBase<T,DIM>::solve() {
    // theta = 10
    for (int iT = 0; iT < 10; ++iT){      
        this->setBoundaryValues(step);
        this->writeVTK(step);          
        lattice->collideAndStream();
        step += 1;
    }
    this->getResults(step);
}

template<typename T, int DIM>
void lbmSimulatorBase<T,DIM>::setPressures(std::unordered_map<int, T> pressure_) {
    this->pressures = pressure_;
}

template<typename T, int DIM>
void lbmSimulatorBase<T,DIM>::setFlowRates(std::unordered_map<int, T> flowRate_) {
    this->flowRates = flowRate_;
}

//=====================================================================================
//===========================  lbmSimulator2D =========================================
//=====================================================================================

template<typename T>
lbmSimulator<T, 2>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulatorBase<T, 2>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_,
        resolution_, epsilon_, relaxationTime_) { } 

template<typename T>
void lbmSimulator<T, 2>::prepareGeometry () {

    bool print = false;

    T conversionFactorLength = this->getConverter().getConversionFactorLength();

    this->stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, this->converter->getConversionFactorLength());
    #ifdef VERBOSE
        print = true;
        std::cout << "[lbmModule] reading STL file " << this->name << "... OK" << std::endl;
    #endif
    stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*this->stlReader);
    #ifdef VERBOSE
        std::cout << "[lbmModule] create 2D indicator " << this->name << "... OK" << std::endl;
    #endif

    olb::Vector<T, 2> origin(-0.5*conversionFactorLength, -0.5*conversionFactorLength);
    olb::Vector<T, 2> extend(this->cfdModule->getSize()[0] + conversionFactorLength, this->cfdModule->getSize()[1] + conversionFactorLength);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    this->cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, conversionFactorLength, 1);
    this->loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*this->cuboidGeometry);
    this->geometry = std::make_shared<olb::SuperGeometry<T,2>> (
        *this->cuboidGeometry, 
        *this->loadBalancer);

    #ifdef VERBOSE
        std::cout << "[lbmModule] generate geometry " << this->name << "... OK" << std::endl;   
    #endif 

    this->geometry->rename(0, 2);
    this->geometry->rename(2, 1, *stl2Dindicator);

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
        T x_extend = Opening.width*Opening.tangent[0] - conversionFactorLength*Opening.normal[0];
        T y_extend = Opening.width*Opening.tangent[1] - conversionFactorLength*Opening.normal[1];

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
        
        this->geometry->rename(2, key+3, opening);
    }

    this->geometry->clean(print);
    this->geometry->checkForErrors(print);

    #ifdef VERBOSE
        std::cout << "[lbmModule] prepare geometry " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T, 2>::prepareLattice () {
    const T omega = this->converter->getLatticeRelaxationFrequency();

    this->lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    // Set lattice dynamics
    this->lattice->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    this->lattice->template defineDynamics<BGKdynamics>(this->getGeometry(), 1);
    this->lattice->template defineDynamics<BounceBack>(this->getGeometry(), 2);

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

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure2D<T>> (this->getLattice(), this->getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
            flowProfiles.try_emplace(key, std::make_shared<olb::Poiseuille2D<T>>(this->getGeometry(), 0, (T) 0.0, (T) 0.0));
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity2D<T> > (this->getLattice(), this->getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
            densities.try_emplace(key, std::make_shared<olb::AnalyticalConst2D<T,T>>((T) 0.0));
        }
    }

    // Initialize lattice with relaxation frequency omega
    this->lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    this->lattice->initialize();

    #ifdef VERBOSE
        std::cout << "[lbmModule] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T, 2>::setBoundaryValues (int iT) {

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
}

template<typename T>
void lbmSimulator<T, 2>::getResults (int iT) {
    int input[1] = { };
    T output[3];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    meanPressures.at(key)->print();
                #endif
            }
        } else {
            fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    fluxes.at(key)->print();
                #endif
            }
        }
    }
}

template<typename T>
void lbmSimulator<T, 2>::writeVTK (int iT) {

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
    }

    if (iT % 1000 == 0) {

        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
        #endif
        
        olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(this->getLattice(), this->getConverter());
        olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(this->getLattice(), this->getConverter());
        olb::SuperLatticeDensity2D<T,DESCRIPTOR> latDensity(this->getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);
        
        // write vtk to file system
        vtmWriter.write(iT);
        this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);
    }

    this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (this->converge->hasConverged()) {
            this->isConverged = true;
        }
    }

}

//=====================================================================================
//===========================  lbmSimulator3D =========================================
//=====================================================================================

template<typename T>
lbmSimulator<T, 3>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulatorBase<T, 3>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_,
        resolution_, epsilon_, relaxationTime_) { } 

template<typename T>
void lbmSimulator<T, 3>::prepareGeometry () {

    bool print = false;

    T conversionFactorLength = this->getConverter().getConversionFactorLength();

    this->stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, conversionFactorLength);
    #ifdef VERBOSE
        print = true;
        std::cout << "[lbmModule] reading STL file " << this->name << "... OK" << std::endl;
    #endif
    stl3Dindicator = std::make_shared<olb::IndicatorF3D<T>>(*this->stlReader);
    #ifdef VERBOSE
        std::cout << "[lbmModule] create 3D indicator " << this->name << "... OK" << std::endl;
    #endif

    olb::Vector<T,3> origin(-0.5*conversionFactorLength, -0.5*conversionFactorLength, -0.5*conversionFactorLength);
    olb::Vector<T,3> extend(this->cfdModule->getSize()[0] + conversionFactorLength, this->cfdModule->getSize()[1] + conversionFactorLength);
    olb::IndicatorCuboid3D<T> cuboid(extend, origin);
    this->cuboidGeometry = std::make_shared<olb::CuboidGeometry3D<T>> (cuboid, conversionFactorLength, 1);
    this->loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*this->cuboidGeometry);
    this->geometry = std::make_shared<olb::SuperGeometry<T,3>> (
        *this->cuboidGeometry, 
        *this->loadBalancer);

    #ifdef VERBOSE
        std::cout << "[lbmModule] generate geometry " << this->name << "... OK" << std::endl;   
    #endif 

    this->geometry->rename(0, 2);
    this->geometry->rename(2, 1, *stl3Dindicator);

    #ifdef VERBOSE
        std::cout << "[lbmModule] generate 2D geometry from STL  " << this->name << "... OK" << std::endl;
    #endif

    for (auto& [key, Opening] : this->moduleOpenings ) {
        // The unit vector pointing to the extend (opposite origin) of the opening
        T x_origin =    Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0]
                        - 0.5*Opening.width*Opening.tangent[0];
        T y_origin =   Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1]
                        - 0.5*Opening.width*Opening.tangent[1];
        T z_origin = 0.0;
        
        // The unit vector pointing to the extend
        T x_extend = Opening.width*Opening.tangent[0] - conversionFactorLength*Opening.normal[0];
        T y_extend = Opening.width*Opening.tangent[1] - conversionFactorLength*Opening.normal[1];
        T z_extend = 0.0;

        // Extend can only have positive values, hence the following transformation
        if (x_extend < 0 ){
            x_extend *= -1;
            x_origin -= x_extend;
        }
        if (y_extend < 0 ){
            y_extend *= -1;
            y_origin -= y_extend;
        }

        olb::Vector<T,3> originO (x_origin, y_origin, z_origin);
        olb::Vector<T,3> extendO (x_extend, y_extend, z_extend);
        olb::IndicatorCuboid3D<T> opening(extendO, originO);
        
        this->geometry->rename(2, key+3, opening);
    }

    this->geometry->clean(print);
    this->geometry->checkForErrors(print);

    #ifdef VERBOSE
        std::cout << "[lbmModule] prepare geometry " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T, 3>::prepareLattice () {
    const T omega = this->converter->getLatticeRelaxationFrequency();

    this->lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(this->getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst3D<T,T> rhoF(1);
    olb::AnalyticalConst3D<T,T> uF(velocity);

    // Set lattice dynamics
    this->lattice->template defineDynamics<NoDynamics>(this->getGeometry(), 0);
    this->lattice->template defineDynamics<BGKdynamics>(this->getGeometry(), 1);
    this->lattice->template defineDynamics<BounceBack>(this->getGeometry(), 2);

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

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure3D<T>> meanPressure;
            meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure3D<T>> (this->getLattice(), this->getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
            this->flowProfiles.try_emplace(key, std::make_shared<olb::Poiseuille2D<T>>(this->getGeometry(), 0, (T) 0.0, (T) 0.0));
        } else {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity3D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity3D<T> > (this->getLattice(), this->getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
            densities.try_emplace(key, std::make_shared<olb::AnalyticalConst3D<T,T>>((T) 0.0));
        }
    }

    // Initialize lattice with relaxation frequency omega
    this->lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    this->lattice->initialize();

    #ifdef VERBOSE
        std::cout << "[lbmModule] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T, 3>::setBoundaryValues (int iT) {

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
            this->densities.at(key) = std::make_shared<olb::AnalyticalConst3D<T,T>>(rhoV);
            this->getLattice().defineRho(this->getGeometry(), key+3, *this->densities.at(key));
        }
    }

}

template<typename T>
void lbmSimulator<T, 3>::getResults (int iT) {
    int input[1] = { };
    T output[3];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    meanPressures.at(key)->print();
                #endif
            }
        } else {
            fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
            if (iT % 1000 == 0) {
                #ifdef VERBOSE
                    fluxes.at(key)->print();
                #endif
            }
        }
    }
}

template<typename T>
void lbmSimulator<T, 3>::writeVTK (int iT) {

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
    }

    if (iT % 1000 == 0) {
        
        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
        #endif

        olb::SuperLatticePhysVelocity3D<T,DESCRIPTOR> velocity(this->getLattice(), this->getConverter());
        olb::SuperLatticePhysPressure3D<T,DESCRIPTOR> pressure(this->getLattice(), this->getConverter());
        olb::SuperLatticeDensity3D<T,DESCRIPTOR> latDensity(this->getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);
        
        // write vtk to file system
        vtmWriter.write(iT);
        this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);
    }

    this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (this->converge->hasConverged()) {
            this->isConverged = true;
        }
    }

}

}   // namespace arch