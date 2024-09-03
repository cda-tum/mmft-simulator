#include "olbContinuous3D.h"
#include <filesystem>

namespace sim{

template<typename T>
lbmSimulator3D<T>::lbmSimulator3D(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, arch::Opening<T>> openings, 
                                    ResistanceModel<T>* resistanceModel, T charPhysLength, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime) :
    lbmSimulator<T>(id, name, stlFile, cfdModule, openings, resistanceModel, charPhysLength, charPhysVelocity, alpha, resolution, epsilon, relaxationTime) { }

template<typename T>
void lbmSimulator3D<T>::setBoundaryValues (int iT) {

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setFlowProfile3D(key, Opening.width);
        } else {
            setPressure3D(key);
        }
    }
}

template<typename T>
void lbmSimulator3D<T>::setFlowProfile (int openingKey, T openingWidth)  {
    T maxVelocityFactor = getConverter().getLatticeVelocity((3./2.)*(this->flowRates[openingKey]/(openingWidth)));
    std::vector<T> maxVelocity = {  maxVelocityFactor*this->moduleOpenings.at(openingKey).normal[0],
                                    maxVelocityFactor*this->moduleOpenings.at(openingKey).normal[1],
                                    maxVelocityFactor*this->moduleOpenings.at(openingKey).normal[2]};
    T distance2Wall = getConverter().getConversionFactorLength()/2.;
    this->flowProfiles.at(openingKey) = std::make_shared<olb::RectanglePoiseuille3D<T>>(getGeometry(), openingKey+3, maxVelocity, distance2Wall, distance2Wall, distance2Wall);
    getLattice().defineU(getGeometry(), openingKey+3, *this->flowProfiles.at(openingKey));
}

template<typename T>
void lbmSimulator3D<T>::setPressure (int openingKey)  {
    T rhoV = getConverter().getLatticeDensityFromPhysPressure(this->pressures[openingKey]);
    this->densities.at(openingKey) = std::make_shared<olb::AnalyticalConst3D<T,T>>(rhoV);
    getLattice().defineRho(getGeometry(), openingKey+3, *this->densities.at(openingKey));
}

template<typename T>
void lbmSimulator3D<T>::writeVTK (int iT) {

    bool print = false;
    #ifdef VERBOSE
        print = true;
    #endif

    olb::SuperVTMwriter3D<T> vtmWriter( this->name );
    // Writes geometry to file system
    if (iT == 0) {
        olb::SuperLatticeGeometry3D<T,DESCRIPTOR> writeGeometry (getLattice(), getGeometry());
        vtmWriter.write(writeGeometry);
        vtmWriter.createMasterFile();
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + olb::createFileName( this->name ) + ".pvd";
    }

    if (iT % 1000 == 0) {
        
        olb::SuperLatticePhysVelocity3D<T,DESCRIPTOR> velocity(getLattice(), getConverter());
        olb::SuperLatticePhysPressure3D<T,DESCRIPTOR> pressure(getLattice(), getConverter());
        olb::SuperLatticeDensity3D<T,DESCRIPTOR> latDensity(getLattice());
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
void lbmSimulator3D<T>::prepareNsLattice (const T omega) {

    lattice = std::make_shared<olb::SuperLattice<T, DESCRIPTOR>>(getGeometry());

    // Initial conditions
    std::vector<T> velocity(T(0), T(0));
    olb::AnalyticalConst3D<T,T> rhoF(1);
    olb::AnalyticalConst3D<T,T> uF(velocity);

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
            olb::setInterpolatedVelocityBoundary<T,olb::descriptors::D3Q19<>>(getLattice(), omega, getGeometry(), key+3);
        } else {
            olb::setInterpolatedPressureBoundary<T,olb::descriptors::D3Q19<>>(getLattice(), omega, getGeometry(), key+3);
        }
    }
}

template<typename T>
void lbmSimulator3D<T>::initFlowRateIntegralPlane() {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];
        T posZ =  Opening.node->getPosition()[2] - this->cfdModule->getPosition()[2];

        std::vector<T> position = {posX, posY, posZ};
        std::vector<int> materials = {1, key+3};

        if (!this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity3D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity3D<T> > (getLattice(), getConverter(), getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
            densities.try_emplace(key, std::make_shared<olb::AnalyticalConst3D<T,T>>((T) 0.0));
        }
    }

}

template<typename T>
void lbmSimulator3D<T>::readGeometryStl (const T dx, const bool print) {

    T correction[3]= {0.0, 0.0, 0.0};

    this->stlReader = std::make_shared<olb::STLreader<T>>(this->stlFile, dx);
    auto min = this->stlReader->getMesh().getMin();
    auto max = this->stlReader->getMesh().getMax();

    if (max[0] - min[0] > this->cfdModule->getSize()[0] + 1e-9 ||
        max[1] - min[1] > this->cfdModule->getSize()[1] + 1e-9 ||
        max[2] - min[2] > this->cfdModule->getSize()[2] + 1e-9) 
    {
        std::string sizeMessage;
        sizeMessage =   "\nModule size:\t[" + std::to_string(this->cfdModule->getSize()[0]) + 
                        ", " + std::to_string(this->cfdModule->getSize()[1]) +
                        ", " + std::to_string(this->cfdModule->getSize()[2]) + "]" +
                        "\nSTL size:\t[" + std::to_string(max[0] - min[0]) + 
                        ", " + std::to_string(max[1] - min[1]) +
                        ", " + std::to_string(max[2] - min[2]) + "]";

        throw std::runtime_error("The module size is too small for the STL geometry." + sizeMessage);
    }

    for (unsigned char d : {0, 1, 2}) {
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

    olb::Vector<T,3> origin(min[0]-this->stlMargin*dx-correction[0]*dx, 
                            min[1]-this->stlMargin*dx-correction[1]*dx, 
                            min[2]-this->stlMargin*dx-correction[2]*dx);
    olb::Vector<T,3> extend(max[0]-min[0]+2*this->stlMargin*dx+2*correction[0]*dx, 
                            max[1]-min[1]+2*this->stlMargin*dx+2*correction[1]*dx,
                            max[2]-min[2]+2*this->stlMargin*dx+2*correction[2]*dx);
    olb::IndicatorCuboid3D<T> cuboid(extend, origin);
    cuboidGeometry = std::make_shared<olb::CuboidGeometry3D<T>> (cuboid, dx, 1);
    loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    geometry = std::make_shared<olb::SuperGeometry<T,3>> (*cuboidGeometry, *loadBalancer);

    if (print) {
        std::cout << "[lbmSimulator] generate geometry " << this->name << "... OK" << std::endl;   
    }

    geometry->rename(0, 2);
    geometry->rename(2, 1, *(this->stlReader));
    geometry->clean(print);

    if (print) {
        std::cout << "[lbmSimulator] generate 3D geometry from STL  " << this->name << "... OK" << std::endl;
    }
}

template<typename T>
void lbmSimulator3D<T>::readOpenings (const T dx, const bool print) {

    int extendMargin = 4;
    auto min = this->stlReader->getMesh().getMin();

    T stlShift[3];
    stlShift[0] = this->cfdModule->getPosition()[0] - min[0];
    stlShift[1] = this->cfdModule->getPosition()[1] - min[1];
    stlShift[2] = this->cfdModule->getPosition()[2] - min[2];

    for (auto& [key, Opening] : this->moduleOpenings ) {
        // The unit vector pointing to the extend (opposite origin) of the opening
        T x_origin = Opening.node->getPosition()[0] -stlShift[0] -0.5*extendMargin*dx;
        T y_origin = Opening.node->getPosition()[1] -stlShift[1] -0.5*Opening.width;
        T z_origin = Opening.node->getPosition()[2] -stlShift[2] -0.5*Opening.height;
        
        // The unit vector pointing to the extend
        T x_extend = extendMargin*dx;
        T y_extend = Opening.width;
        T z_extend = Opening.height;

        // The unit vector pointing to the center of rotation
        T x_center = x_origin + 0.5*x_extend;
        T y_center = y_origin + 0.5*y_extend;
        T z_center = z_origin + 0.5*z_extend;

        olb::Vector<T,3> originO (x_origin, y_origin, z_origin);
        olb::Vector<T,3> extendO (x_extend, y_extend, z_extend);
        olb::Vector<T,3> centerO (x_center, y_center, z_center);
        olb::IndicatorCuboidRotate3D<T> opening(extendO, originO, Opening.radial, 2, centerO);
        
        geometry->rename(2, key+3, opening);
    }
    geometry->checkForErrors(print);
}

template<typename T>
void lbmSimulator3D<T>::initNsConverter (T dynViscosity, T density) {

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
void lbmSimulator3D<T>::initNsLattice (const T omega) {
    // Initialize lattice with relaxation frequency omega
    lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    lattice->initialize();
}

template<typename T>
void lbmSimulator3D<T>::storeCfdResults (int iT) {
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