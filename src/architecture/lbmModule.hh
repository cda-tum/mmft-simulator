#include "lbmModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "Module.h"

namespace arch{

    template<typename T>
    lbmModule<T>::lbmModule (
        int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
        std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) : 
            Module<T>(id_, pos_, size_, nodes_), 
            moduleOpenings(openings_), stlFile(stlFile_), name(name_), charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_),
             alpha(alpha_), resolution(resolution_), epsilon(epsilon_), relaxationTime(relaxationTime_)
        { 
            this->moduleType = ModuleType::LBM;
        } 

    template<typename T>
    void lbmModule<T>::prepareGeometry () {

        olb::STLreader<T> stlReader(stlFile, converter->getConversionFactorLength());
        std::cout << "[lbmModule] reading STL file " << name << "... OK" << std::endl;
        olb::IndicatorF2DfromIndicatorF3D<T> stl2Dindicator(stlReader);
        std::cout << "[lbmModule] create 2D indicator " << name << "... OK" << std::endl;

        olb::Vector<T,2> origin(-0.5*converter->getConversionFactorLength(), -0.5*converter->getConversionFactorLength());
        olb::Vector<T,2> extend(this->size[0] + converter->getConversionFactorLength(), this->size[1] + converter->getConversionFactorLength());
        olb::IndicatorCuboid2D<T> cuboid(extend, origin);
        cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, converter->getConversionFactorLength(), 1);
        loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
        geometry = std::make_shared<olb::SuperGeometry<T,2>> (
            *cuboidGeometry, 
            *loadBalancer);

        std::cout << "[lbmModule] generate geometry " << name << "... OK" << std::endl;    

        this->geometry->rename(0, 2);
        this->geometry->rename(2, 1, stl2Dindicator);

        std::cout << "[lbmModule] generate 2D geometry from STL  " << name << "... OK" << std::endl;

        for (auto& [key, Opening] : moduleOpenings ) {
            // The unit vector pointing to the extend (opposite origin) of the opening
            T x_origin =    Opening.node->getPosition()[0] - this->getPosition()[0]
                            - 0.5*Opening.width*Opening.tangent[0];
            T y_origin =   Opening.node->getPosition()[1] - this->getPosition()[1]
                            - 0.5*Opening.width*Opening.tangent[1];
            
            // The unit vector pointing to the extend
            T x_extend = Opening.width*Opening.tangent[0] - converter->getConversionFactorLength()*Opening.normal[0];
            T y_extend = Opening.width*Opening.tangent[1] - converter->getConversionFactorLength()*Opening.normal[1];

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

        this->geometry->clean();
        this->geometry->checkForErrors();

        std::cout << "[lbmModule] prepare geometry " << name << "... OK" << std::endl;
    }

    template<typename T>
    void lbmModule<T>::prepareLattice () {
        const T omega = converter->getLatticeRelaxationFrequency();

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
        for (auto& [key, Opening] : moduleOpenings) {
            if (groundNodes.at(key)) {
                setInterpolatedVelocityBoundary(getLattice(), omega, getGeometry(), key+3);
            } else {
                setInterpolatedPressureBoundary(getLattice(), omega, getGeometry(), key+3);
            }
        }

        // Initialize the integral fluxes for the in- and outlets
        for (auto& [key, Opening] : moduleOpenings) {

            T posX =  Opening.node->getPosition()[0] - this->getPosition()[0];
            T posY =  Opening.node->getPosition()[1] - this->getPosition()[1];          

            std::vector<T> position = {posX, posY};
            std::vector<int> materials = {1, key+3};

            if (groundNodes.at(key)) {
                std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>> meanPressure;
                meanPressure = std::make_shared< olb::SuperPlaneIntegralFluxPressure2D<T>> (getLattice(), getConverter(), getGeometry(),
                position, Opening.tangent, materials);
                this->meanPressures.try_emplace(key, meanPressure);
            } else {
                std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>> flux;
                flux = std::make_shared< olb::SuperPlaneIntegralFluxVelocity2D<T> > (getLattice(), getConverter(), getGeometry(),
                position, Opening.tangent, materials);
                this->fluxes.try_emplace(key, flux);
            }
        }

        // Initialize lattice with relaxation frequency omega
        lattice->template setParameter<olb::descriptors::OMEGA>(omega);
        lattice->initialize();

        std::cout << "[lbmModule] prepare lattice " << name << "... OK" << std::endl;
    }

    template<typename T>
    void lbmModule<T>::setBoundaryValues (int iT) {

        T pressureLow = -1.0;       
        for (auto& [key, pressure] : pressures) {
            if ( pressureLow < 0.0 ) {
                pressureLow = pressure;
            }
            if ( pressure < pressureLow ) {
                pressureLow = pressure;
            }
        }

        for (auto& [key, Opening] : moduleOpenings) {
            if (groundNodes.at(key)) {
                T maxVelocity = (3./2.)*(flowRates[key]/(Opening.width));
                T distance2Wall = getConverter().getConversionFactorLength()/2.;
                olb::Poiseuille2D<T> poiseuilleU(getGeometry(), key+3, getConverter().getLatticeVelocity(maxVelocity), distance2Wall);
                getLattice().defineU(getGeometry(), key+3, poiseuilleU);
            } else {
                T rhoV = getConverter().getLatticeDensityFromPhysPressure((pressures[key]));
                olb::AnalyticalConst2D<T,T> rho(rhoV);
                getLattice().defineRho(getGeometry(), key+3, rho);
            }
        }

    }

    template<typename T>
    void lbmModule<T>::getResults (int iT) {
        int input[1] = { };
        T output[3];
        
        for (auto& [key, Opening] : moduleOpenings) {
            if (groundNodes.at(key)) {
                meanPressures.at(key)->operator()(output, input);
                T newPressure =  output[0]/output[1];
                pressures.at(key) = newPressure;
                if (iT % 1000 == 0) {
                    meanPressures.at(key)->print();
                }
            } else {
                fluxes.at(key)->operator()(output,input);
                flowRates.at(key) = output[0];
                if (iT % 1000 == 0) {
                    fluxes.at(key)->print();
                }
            }
        }
    }

    template<typename T>
    void lbmModule<T>::lbmInit (T dynViscosity, 
                                T density) {
        // Create network with fully connected graph and set initial resistances

        olb::singleton::directories().setOutputDir( "./tmp/" );  // set output directory     

        T kinViscosity = dynViscosity/density;

        this->moduleNetwork = std::make_shared<Network<T>> (this->boundaryNodes);

        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->boundaryNodes.size() > 1);
        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->boundaryNodes.size());
        
        this->converter = std::make_shared<const olb::UnitConverterFromResolutionAndRelaxationTime<T,DESCRIPTOR>>(
            resolution,
            relaxationTime,
            charPhysLength,
            charPhysVelocity,
            kinViscosity,
            density
        );

        this->converter->print();

        // Initialize pressure, flowRate and resistance value-containers
        for (auto& [key, node] : this->boundaryNodes) {
            pressures.try_emplace(key, (T) 0.0);
            flowRates.try_emplace(key, (T) 0.0);
        }

        // Initialize a convergence tracker for pressure
        this->converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);

        std::cout << "[lbmModule] lbmInit " << name << "... OK" << std::endl;
    }

    template<typename T>
    void lbmModule<T>::writeVTK (int iT) {

        olb::SuperVTMwriter2D<T> vtmWriter( name );
        // Writes geometry to file system
        if (iT == 0) {
            olb::SuperLatticeGeometry2D<T,DESCRIPTOR> writeGeometry (getLattice(), getGeometry());
            vtmWriter.write(writeGeometry);
            vtmWriter.createMasterFile();
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
            converge->takeValue(getLattice().getStatistics().getAverageEnergy(), true);
        }
        if (iT %1000 == 0) {
            std::cout << "[writeVTK] " << name << " currently at timestep " << iT << std::endl;
        }

        converge->takeValue(getLattice().getStatistics().getAverageEnergy(), true);

        if (iT%100 == 0) {
            if (converge->hasConverged()) {
                    isConverged = true;
            }
        }

    }

    template<typename T>
    void lbmModule<T>::solve() {
        // TODO: theta = 10
        for (int iT = 0; iT < 10; ++iT){      
            this->setBoundaryValues(step);
            writeVTK(step);          
            lattice->collideAndStream();
            step += 1;
        }
        getResults(step);
    }


    template<typename T>
    void lbmModule<T>::setPressures(std::unordered_map<int, T> pressure_) {
        this->pressures = pressure_;
    }

    template<typename T>
    void lbmModule<T>::setFlowRates(std::unordered_map<int, T> flowRate_) {
        this->flowRates = flowRate_;
    }

    template<typename T>
    void lbmModule<T>::setGroundNodes(std::unordered_map<int, bool> groundNodes_){
        this->groundNodes = groundNodes_;
    }

    template<typename T>
    void lbmModule<T>::setInitialized(bool initialization_) {
        this->initialized = initialization_;
    }

}   // namespace arch