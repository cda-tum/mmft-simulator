#include "olbContinuous3D.h"

namespace sim{

template<typename T>
lbmSimulator3D<T>::lbmSimulator3D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        olbSim<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, relaxationTime_)
{ 
    this->cfdModule->setModuleTypeLbm();
} 

template<typename T>
lbmSimulator3D<T>::lbmSimulator3D (
    int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
    std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_) : 
        lbmSimulator3D(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, resolution_, epsilon_, relaxationTime_)
{ 
    this->updateScheme = updateScheme_;
} 

template<typename T>
void lbmSimulator3D<T>::lbmInit (T dynViscosity, T density) {

    this->setOutputDir();
    initValueContainers();
    initNsConverter(dynViscosity, density);
    this->initNsConvergeTracker();    

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] lbmInit " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator3D<T>::prepareGeometry () {

    bool print = false;
    T dx = this->getDx();

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
void lbmSimulator3D<T>::prepareLattice () {

    const T omega = this->getOmega();

    prepareNsLattice(omega);
    initPressureIntegralPlane();
    initFlowRateIntegralPlane();
    initNsLattice(omega);

    #ifdef VERBOSE
        std::cout << "[lbmSimulator] prepare lattice " << this->name << "... OK" << std::endl;
    #endif
}

template<typename T>
void lbmSimulator3D<T>::setBoundaryValues (int iT) {

    for (auto& [key, Opening] : this->moduleOpenings) {
        if (this->groundNodes.at(key)) {
            setFlowProfile(key, Opening.width);
        } else {
            setPressure(key);
        }
    }
}

template<typename T>
void lbmSimulator3D<T>::solve() {
    // theta = 10
    this->setBoundaryValues(this->step);
    for (int iT = 0; iT < 10; ++iT){    
        this->writeVTK(this->step);            
        collideAndStream();
        this->step += 1;
    }
    storeCfdResults(this->step);
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
        
        // write vtk to file system
        vtmWriter.write(iT);
        this->vtkFile = olb::singleton::directories().getVtkOutDir() + "data/" + olb::createFileName( this->name, iT ) + ".vtm";
        this->converge->takeValue(this->getLattice().getStatistics().getAverageEnergy(), print);
    }
    if (iT %1000 == 0) {
        #ifdef VERBOSE
            std::cout << "[writeVTK] " << this->name << " currently at timestep " << iT << std::endl;
            for (auto& [key, Opening] : this->moduleOpenings) {
                if (this->groundNodes.at(key)) {
                    this->meanPressures.at(key)->print();
                } else {
                    this->fluxes.at(key)->print();
                }
            } 
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
void lbmSimulator3D<T>::readGeometryStl(const T dx, const bool print) 
{
    /** TODO:
     * 
     */
}

template<typename T>
void lbmSimulator3D<T>::readOpenings(const T dx, const bool print) 
{
    /** TODO:
     * 
     */
}

template<typename T>
void lbmSimulator3D<T>::initValueContainers () {

    // Initialize pressure and flow rate value-containers
    for (auto& [key, node] : this->moduleOpenings) {
        this->pressures.try_emplace(key, (T) 0.0);
        this->flowRates.try_emplace(key, (T) 0.0);
    }
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
void lbmSimulator3D<T>::prepareNsLattice (const T omega) {

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
}

template<typename T>
void lbmSimulator3D<T>::initPressureIntegralPlane() {

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
            this->flowProfiles.try_emplace(key, std::make_shared<olb::RectanglePoiseuille3D<T>>(this->getGeometry(), 0, 0.0, 0.0, 0.0, 0.0));
        }
    }
}

template<typename T>
void lbmSimulator3D<T>::initFlowRateIntegralPlane() {

    // Initialize the integral fluxes for the in- and outlets
    for (auto& [key, Opening] : this->moduleOpenings) {

        T posX =  Opening.node->getPosition()[0] - this->cfdModule->getPosition()[0];
        T posY =  Opening.node->getPosition()[1] - this->cfdModule->getPosition()[1];          

        std::vector<T> position = {posX, posY};
        std::vector<int> materials = {1, key+3};

        if (!this->groundNodes.at(key)) {
            std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity3D<T>> flux;
            flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity3D<T> > (this->getLattice(), this->getConverter(), this->getGeometry(),
            position, Opening.tangent, materials);
            this->fluxes.try_emplace(key, flux);
            this->densities.try_emplace(key, std::make_shared<olb::AnalyticalConst3D<T,T>>((T) 0.0));
        }
    }

}

template<typename T>
void lbmSimulator3D<T>::initNsLattice (const T omega) {
    // Initialize lattice with relaxation frequency omega
    this->lattice->template setParameter<olb::descriptors::OMEGA>(omega);
    this->lattice->initialize();
}

template<typename T>
void lbmSimulator3D<T>::setFlowProfile (int openingKey, T openingWidth)  {
    T maxVelocity = (3./2.)*(this->flowRates[openingKey]/(openingWidth));
    T distance2Wall = this->getConverter().getConversionFactorLength()/2.;
    this->flowProfiles.at(openingKey) = std::make_shared<olb::RectanglePoiseuille3D<T>>(this->getGeometry(), openingKey+3, maxVelocity, distance2Wall, distance2Wall, distance2Wall);
    this->getLattice().defineU(this->getGeometry(), openingKey+3, *this->flowProfiles.at(openingKey));
}

template<typename T>
void lbmSimulator3D<T>::setPressure (int openingKey)  {
    T rhoV = this->getConverter().getLatticeDensityFromPhysPressure(this->pressures[openingKey]);
    this->densities.at(openingKey) = std::make_shared<olb::AnalyticalConst3D<T,T>>(rhoV);
    this->getLattice().defineRho(this->getGeometry(), openingKey+3, *this->densities.at(openingKey));
}

template<typename T>
void lbmSimulator3D<T>::storeCfdResults (int iT) {
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
}

}   // namespace arch