#include "olbContinuous2D.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmSimulator2D<T>::lbmSimulator2D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        CFDSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_), 
        charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), 
        epsilon(epsilon_), relaxationTime(relaxationTime_)
{ 
    this->cfdModule->setModuleTypeLbm();
} 

template<typename T>
lbmSimulator2D<T>::lbmSimulator2D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulator(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, relaxationTime_)
{ 
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmSimulator2D<T>::lbmInit (T dynViscosity, T density) {

    setOutputDir();
    initValueContainers();
    initNsConverter(dynViscosity, density);
    initNsConvergeTracker();    

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator2D<T>::prepareGeometry () {

    bool print = false;
    T dx = getDx();

    #ifdef VERBOSE
        print = true;
    #endif

    readGeometryStl(dx, print);
    readOpenings(dx, print);

    if (print) {
        std::cout << "[lbmSimulator] prepare geometry " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator2D<T>::prepareLattice () {

    const T omega = getOmega();

    prepareNsLattice(omega);
    initPressureIntegralPlane();
    initFlowRateIntegralPlane();
    initNsLattice(omega);

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator2D<T>::solve() {
    // theta = 10
    this->setBoundaryValues(step);
    for (int iT = 0; iT < 10; ++iT){    
        this->writeVTK(step);            
        collideAndStream();
        step += 1;
    }
    storeCfdResults(step);
}

template<typename T>
void lbmSimulator2D<T>::initValueContainers () {

    // Initialize pressure and flow rate value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        pressures.try_emplace(key, (T) 0.0);
        flowRates.try_emplace(key, (T) 0.0);
    }

}

template<typename T>
void lbmSimulator2D<T>::setBoundaryValues (int iT) {

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setFlowProfile2D(key, Opening.width);
        } else {
            setPressure2D(key);
        }
    }
}

template<typename T>
void lbmSimulator2D<T>::setFlowProfile (int openingKey, T openingWidth)  {
    T maxVelocity = (3./2.)*(this->flowRates[openingKey]/(openingWidth));
    T distance2Wall = getConverter().getConversionFactorLength()/2.;
    this->flowProfiles.at(openingKey) = std::make_shared<olb::Poiseuille2D<T>>(getGeometry(), openingKey+3, getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
    getLattice().defineU(getGeometry(), openingKey+3, *this->flowProfiles.at(openingKey));
}

template<typename T>
void lbmSimulator2D<T>::setPressure (int openingKey)  {
    T rhoV = getConverter().getLatticeDensityFromPhysPressure(this->pressures[openingKey]);
    this->densities.at(openingKey) = std::make_shared<olb::AnalyticalConst2D<T,T>>(rhoV);
    getLattice().defineRho(getGeometry(), openingKey+3, *this->densities.at(openingKey));
}

template<typename T>
void lbmSimulator2D<T>::writeVTK (int iT) {

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
        this->converge->takeValue(getLattice().getStatistics().getAverageEnergy(), print);
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

    this->converge->takeValue(getLattice().getStatistics().getAverageEnergy(), print);

    if (iT%100 == 0) {
        if (this->converge->hasConverged()) {
            this->isConverged = true;
        }
    }
}

template<typename T>
void lbmSimulator2D<T>::initPressureIntegralPlane() {

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
void lbmSimulator2D<T>::initFlowRateIntegralPlane() {

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
void lbmSimulator2D<T>::readGeometryStl (const T dx, const bool print) {

    T correction[2]= {0.0, 0.0};

    this->stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, dx);
    auto min = this->stlReader->getMesh().getMin();
    auto max = this->stlReader->getMesh().getMax();

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
        
    stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*(this->stlReader));

    if (print) {
        std::cout << "[lbmSimulator] create 2D indicator " << this->name << "... OK" << std::endl;
    }

    olb::Vector<T,2> origin(min[0]-this->stlMargin*dx-correction[0]*dx, min[1]-this->stlMargin*dx-correction[1]*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*this->stlMargin*dx+2*correction[0]*dx, max[1]-min[1]+2*this->stlMargin*dx+2*correction[1]*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    if (print) {
        std::cout << "[lbmSimulator] generate geometry " << this->name << "... OK" << std::endl;   
    }

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);
    geometry->clean(print);

    if (print) {
        std::cout << "[lbmSimulator] generate 2D geometry from STL  " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator2D<T>::readOpenings (const T dx, const bool print) {

    int extendMargin = 4;
    auto min = this->stlReader->getMesh().getMin();

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
        
        geometry->rename(2, key+3, opening);
    }

    geometry->checkForErrors(print);
}

template<typename T>
void lbmSimulator2D<T>::initNsConverter (T dynViscosity, T density) {

    T kinViscosity = dynViscosity/density;

    #ifdef DEBUG
    // There must be more than 1 node to have meaningful flow in the module domain
    assert(this->moduleOpenings.size() > 1);
    // We must have exactly one opening assigned to each boundaryNode
    assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());
    #endif

    this->converter = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>>(
        this->resolution,
        this->relaxationTime,
        this->charPhysLength,
        this->charPhysVelocity,
        kinViscosity,
        density
    );

    #ifdef VERBOSE
        this->converter->print();
    #endif
    
}

template<typename T>
void lbmSimulator2D<T>::initNsLattice (const T omega) {
    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();
}

template<typename T>
void lbmSimulator2D<T>::storeCfdResults (int iT) {
    int input[1] = { };
    T output[10];
    
    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            this->pressures.at(key) = newPressure;
        } else {
            fluxes.at(key)->operator()(output,input);
            this->flowRates.at(key) = output[0];
        }
    }
}

}   // namespace arch