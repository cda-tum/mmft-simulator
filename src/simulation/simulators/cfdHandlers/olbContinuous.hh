#include "olbContinuous.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmSimulator<T>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_), 
        charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), 
        epsilon(epsilon_), relaxationTime(relaxationTime_)
{ 
    this->cfdModule->setModuleTypeLbm();
} 

template<typename T>
lbmSimulator<T>::lbmSimulator (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulator(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, relaxationTime_)
{ 
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmSimulator<T>::lbmInit (T dynViscosity, T density) {

    setOutputDir();
    initValueContainers();
    initNsConverter(dynViscosity, density);
    initNsConvergeTracker();    

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T>::prepareGeometry () {

    bool print = false;
    T dx = getConverter().getConversionFactorLength();

    #ifdef VERBOSE
        print = true;
    #endif

    readGeometryStl(dx, print);
    readOpenings(dx);
    this->geometry->checkForErrors(print);

    if (print) {
        std::cout << "[lbmSimulator] prepare geometry " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator<T>::prepareLattice () {

    const T omega = converter->getLatticeRelaxationFrequency();

    prepareNsLattice(omega);
    initPressureIntegralPlane();
    initFlowRateIntegralPlane();
    initNsLattice(omega);

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator<T>::setBoundaryValues (int iT) {

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setFlowProfile2D(key, Opening.width);
        } else {
            setPressure2D(key);
        }
    }
}

template<typename T>
void lbmSimulator<T>::writeVTK (int iT) {

    bool print = false;
    #ifdef VERBOSE
        print = true;
    #endif

    olb::SuperVTMwriter2D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry2D<T,DESCRIPTOR> writeGeometry (getLattice(), getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + olb::createFileName( this->name ) + ".pvd";
    }

    if (iT % 1000 == 0) {
        
        olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(getLattice(), getConverter());
        olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(getLattice(), getConverter());
        olb::SuperLatticeDensity2D<T,DESCRIPTOR> latDensity(getLattice());
        vtmWriter.addFunctor(velocity);
        vtmWriter.addFunctor(pressure);
        vtmWriter.addFunctor(latDensity);
        
        // write vtk to file system
        vtmWriter.write(iT);
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + "data/" + olb::createFileName( this->name, iT ) + ".vtm";
        converge->takeValue(getLattice().getStatistics().getAverageEnergy(), print);
    }
    if (iT %1000 == 0) {
        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
            for (auto& [key, Opening] : this->moduleOpenings) {
                if (this->groundNodes.at(key)) {
                    meanPressures.at(key)->print();
                } else {
                    fluxes.at(key)->print();
                }
            } 
        #endif
    }

    converge->takeValue(getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (converge->hasConverged()) {
            isConverged = true;
        }
    }

}

template<typename T>
void lbmSimulator<T>::writePressurePpm (T min, T max, int imgResolution) {
    // Color map options are 'earth'|'water'|'air'|'fire'|'leeloo'
    std::string colorMap = "leeloo";
    olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(getLattice(), getConverter());
    olb::BlockReduction2D2D<T> planeReductionP(pressure, imgResolution);
    olb::BlockGifWriter<T> ppmWriter(colorMap);
    ppmWriter.write(planeReductionP, min, max, step, this->name+"_pressure_");
}

template<typename T>
void lbmSimulator<T>::writeVelocityPpm (T min, T max, int imgResolution) {
    // Color map options are 'earth'|'water'|'air'|'fire'|'leeloo'
    std::string colorMap = "leeloo";
    olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(getLattice(), getConverter());
    olb::SuperEuklidNorm2D<T, DESCRIPTOR> normVel( velocity );   
    olb::BlockReduction2D2D<T> planeReductionVel(normVel, imgResolution);
    olb::BlockGifWriter<T> ppmWriter(colorMap);
    ppmWriter.write(planeReductionVel, min, max, step, this->name+"_velocity_");
}

template<typename T>
void lbmSimulator<T>::solve() {
    int theta = this->updateScheme->getTheta(this->cfdModule->getId());
    this->setBoundaryValues(step);
    for (int iT = 0; iT < theta; ++iT){    
        writeVTK(step);       
        lattice->collideAndStream();
        step += 1;
    }
    storeCfdResults(step);
}

template<typename T>
void lbmSimulator<T>::setOutputDir () {
    if (!std::filesystem::is_directory(this->vtkFolder) || !std::filesystem::exists(this->vtkFolder)) {
        std::filesystem::create_directory(this->vtkFolder);
    }

    olb::singleton::directories().setOutputDir( this->vtkFolder+"/" );  // set output directory     
}   

template<typename T>
void lbmSimulator<T>::initValueContainers () {

    // Initialize pressure and flow rate value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        pressures.try_emplace(key, (T) 0.0);
        flowRates.try_emplace(key, (T) 0.0);
    }

}

template<typename T>
void lbmSimulator<T>::initNsConverter (T dynViscosity, T density) {

    T kinViscosity = dynViscosity/density;

    #ifdef DEBUG
    // There must be more than 1 node to have meaningful flow in the module domain
    assert(this->moduleOpenings.size() > 1);
    // We must have exactly one opening assigned to each boundaryNode
    assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());
    #endif

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
    
}

template<typename T>
void lbmSimulator<T>::initNsConvergeTracker () {
    // Initialize a convergence tracker for pressure
    this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);
}

template<typename T>
void lbmSimulator<T>::prepareNsLattice (const T omega) {

    lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst2D<T,T> rhoF(1);
    olb::AnalyticalConst2D<T,T> uF(velocity);

    // Set lattice dynamics
    lattice->template defineDynamics<NoDynamics>(getGeometry(), 0);
    lattice->template defineDynamics<BGKdynamics>(getGeometry(), 1);
    lattice->template defineDynamics<BounceBack>(getGeometry(), 2);

    // Set initial conditions
    lattice->defineRhoU(getGeometry(), 1, rhoF, uF);
    lattice->iniEquilibrium(getGeometry(), 1, rhoF, uF);

    // Set lattice dynamics and initial condition for in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setInterpolatedVelocityBoundary(getLattice(), omega, getGeometry(), key+3);
        } else {
            setInterpolatedPressureBoundary(getLattice(), omega, getGeometry(), key+3);
        }
    }

}

template<typename T>
void lbmSimulator<T>::initPressureIntegralPlane() {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
            meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure2D<T>> (getLattice(), getConverter(), getGeometry(),
            position, Opening.tangent, materials);
            this->meanPressures.try_emplace(key, meanPressure);
            flowProfiles.try_emplace(key, std::make_shared<olb::Poiseuille2D<T>>(getGeometry(), 0, (T) 0.0, (T) 0.0));
        }
    }
}

template<typename T>
void lbmSimulator<T>::initFlowRateIntegralPlane() {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (!this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity2D<T> > (getLattice(), getConverter(), getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
            densities.try_emplace(key, std::make_shared<olb::AnalyticalConst2D<T,T>>((T) 0.0));
        }
    }

}

template<typename T>
void lbmSimulator<T>::initNsLattice (const T omega) {
    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();
}


template<typename T>
void lbmSimulator<T>::readGeometryStl (const T dx, const bool print) {

    T correction[2]= {0.0, 0.0};

    stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();

    if (max[0] - min[0] > this->cfdModule->getSize()[0] + 1e-9 ||
        max[1] - min[1] > this->cfdModule->getSize()[1] + 1e-9) 
    {
        std::string sizeMessage;
        sizeMessage =   "\nModule size:\t[" + std::to_string(this->cfdModule->getSize()[0]) + 
                        ", " + std::to_string(this->cfdModule->getSize()[1]) + "]" +
                        "\nSTL size:\t[" + std::to_string(max[0] - min[0]) + 
                        ", " + std::to_string(max[1] - min[1]) + "]";

        throw std::runtime_error("The module size is too small for the STL geometry." + sizeMessage);
    }

    for (unsigned char d : {0, 1}) {
        if (fmod(min[d], dx) < 1e-12) {
            if (fmod(max[d] + 0.5*dx, dx) < 1e-12) {
                correction[d] = 0.25;
            } else {
                correction[d] = 0.5;
            }
        }
    }

    if (print) {
        std::cout << "[lbmSimulator] reading STL file " << this->name << "... OK" << std::endl;
    }
        
    stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    if (print) {
        std::cout << "[lbmSimulator] create 2D indicator " << this->name << "... OK" << std::endl;
    }

    olb::Vector<T,2> origin(min[0]-stlMargin*dx-correction[0]*dx, min[1]-stlMargin*dx-correction[1]*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*stlMargin*dx+2*correction[0]*dx, max[1]-min[1]+2*stlMargin*dx+2*correction[1]*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    if (print) {
        std::cout << "[lbmSimulator] generate geometry " << this->name << "... OK" << std::endl;   
    }

    this->geometry->rename(0, 2);
    this->geometry->rename(2, 1, *stl2Dindicator);
    this->geometry->clean(print);

    if (print) {
        std::cout << "[lbmSimulator] generate 2D geometry from STL  " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator<T>::readOpenings (const T dx) {

    int extendMargin = 4;
    auto min = stlReader->getMesh().getMin();

    T stlShift[2];
    stlShift[0] = this->cfdModule->getPosition()[0] - min[0];
    stlShift[1] = this->cfdModule->getPosition()[1] - min[1];

    for (auto& [key, Opening] : this->moduleOpenings ) {
        // The unit vector pointing to the extend (opposite origin) of the opening
        T x_origin = Opening.node->getPosition()[0] -stlShift[0] -0.5*extendMargin*dx;
        T y_origin = Opening.node->getPosition()[1] -stlShift[1] -0.5*Opening.width;
        
        // The unit vector pointing to the extend
        T x_extend = extendMargin*dx;
        T y_extend = Opening.width;

        olb::Vector<T,2> originO (x_origin, y_origin);
        olb::Vector<T,2> extendO (x_extend, y_extend);
        olb::IndicatorCuboid2D<T> opening(extendO, originO, Opening.radial);
        
        this->geometry->rename(2, key+3, opening);
    }
}

template<typename T>
void lbmSimulator<T>::storePressures(std::unordered_map<int, T> pressure_) {
    this->pressures = pressure_;
}

template<typename T>
void lbmSimulator<T>::storeFlowRates(std::unordered_map<int, T> flowRate_) {
    this->flowRates = flowRate_;
}

template<typename T>
void lbmSimulator<T>::setFlowProfile2D (int openingKey, T openingWidth)  {
    T maxVelocity = (3./2.)*(flowRates[openingKey]/(openingWidth));
    T distance2Wall = getConverter().getConversionFactorLength()/2.;
    this->flowProfiles.at(openingKey) = std::make_shared<olb::Poiseuille2D<T>>(getGeometry(), openingKey+3, getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
    getLattice().defineU(getGeometry(), openingKey+3, *this->flowProfiles.at(openingKey));
}

template<typename T>
void lbmSimulator<T>::setPressure2D (int openingKey)  {
    T rhoV = getConverter().getLatticeDensityFromPhysPressure((pressures[openingKey]));
    this->densities.at(openingKey) = std::make_shared<olb::AnalyticalConst2D<T,T>>(rhoV);
    getLattice().defineRho(getGeometry(), openingKey+3, *this->densities.at(openingKey));
}

template<typename T>
void lbmSimulator<T>::storeCfdResults (int iT) {
    int input[1] = { };
    T output[10];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            pressures.at(key) = newPressure;
        } else {
            fluxes.at(key)->operator()(output,input);
            flowRates.at(key) = output[0];
        }
    }
}

template<typename T>
std::tuple<T, T> lbmSimulator<T>::getPressureBounds() {
    olb::SuperLatticePhysPressure2D<T,DESCRIPTOR> pressure(getLattice(), getConverter());
    olb::SuperMin2D<T> minPressureF( pressure, getGeometry(), 1);
    olb::SuperMax2D<T> maxPressureF( pressure, getGeometry(), 1);
    int input[0];
    T minPressure[1];
    T maxPressure[1];
    minPressureF( minPressure, input );
    maxPressureF( maxPressure, input );
    return std::tuple<T, T> {minPressure[0], maxPressure[0]};
}

template<typename T>
std::tuple<T, T> lbmSimulator<T>::getVelocityBounds() {
    olb::SuperLatticePhysVelocity2D<T,DESCRIPTOR> velocity(getLattice(), getConverter());
    olb::SuperEuklidNorm2D<T, DESCRIPTOR> normVel( velocity );   
    olb::SuperMin2D<T> minVelF( normVel, getGeometry(), 1);
    olb::SuperMax2D<T> maxVelF( normVel, getGeometry(), 1);
    int input[0];
    T minVel[1];
    T maxVel[1];
    minVelF( minVel, input );
    maxVelF( maxVel, input );
    return std::tuple<T, T> {minVel[0], maxVel[0]};
}

}   // namespace arch