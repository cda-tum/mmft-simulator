#include "CFDModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>
#include <olb3D.h>
#include <olb3D.hh>

#include "Module.h"

namespace arch{

template<typename T>
CFDModule<T>::CFDModule(
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, 
    std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, 
    T charPhysLength_, T charPhysVelocity_,  T alpha_, T resolution_, T epsilon_, T relaxationTime_) : Module<T>(id_, pos_, size_, nodes_), 
    moduleOpenings(openings_), stlFile(stlFile_), name(name_), charPhysLength(charPhysLength_), 
    charPhysVelocity(charPhysVelocity_), alpha(alpha_), resolution(resolution_), epsilon(epsilon_), relaxationTime(relaxationTime_) 
    { 
        olb::singleton::directories().setOutputDir( "./tmp/" );  // set output directory

        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->boundaryNodes.size() > 1);

        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->boundaryNodes.size());

        // Create network with fully connected graph for initial approximation
        moduleNetwork = std::make_shared<Network<T>> (this->boundaryNodes);

        // Initialize pressure, flowRate and resistance value-containers
        for (auto& [key, node] : this->boundaryNodes) {
            pressures.try_emplace(key, (T) 0.0);
            flowRates.try_emplace(key, (T) 0.0);
        }
    } 

template<typename T>
void CFDModule<T>::prepareGeometry() {

    olb::STLreader<T> stlReader(stlFile, getConversionFactorLength());
    std::cout << "[lbmModule] reading STL file " << name << "... OK" << std::endl;
    olb::IndicatorF2DfromIndicatorF3D<T> stl2Dindicator(stlReader);
    std::cout << "[lbmModule] create 2D indicator " << name << "... OK" << std::endl;

    olb::Vector<T,2> origin(-1.0*getConversionFactorLength(), -1.0*getConversionFactorLength());
    olb::Vector<T,2> extend(this->size[0] + 2.0*getConversionFactorLength(), this->size[1] + 2.0*getConversionFactorLength());
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, getConversionFactorLength(), 1);
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
                        - 0.5*Opening.width*Opening.tangent[0] + 0.5*getConversionFactorLength()*Opening.normal[0] - 0.5*getConversionFactorLength()*abs(Opening.normal[1]);
        T y_origin =   Opening.node->getPosition()[1] - this->getPosition()[1]
                        - 0.5*Opening.width*Opening.tangent[1] + 0.5*getConversionFactorLength()*Opening.normal[1] - 0.5*getConversionFactorLength()*abs(Opening.normal[0]);

        std::cout << "The origin is: " << x_origin << ", " << y_origin << std::endl;
        
        // The unit vector pointing to the extend
        T x_extend = Opening.width*Opening.tangent[0] - getConversionFactorLength()*Opening.normal[0];// + 0.5*getConversionFactorLength()*Opening.normal[1];
        T y_extend = Opening.width*Opening.tangent[1] - getConversionFactorLength()*Opening.normal[1];// - 0.5*getConversionFactorLength()*Opening.normal[0];
        
        std::cout << "The extend is: " << x_extend << ", " << y_extend << std::endl;

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
        this->geometry->rename(1, key+3, opening);
        this->geometry->rename(2, key+3, opening);
    }

    this->geometry->clean();
    this->geometry->checkForErrors();

    std::cout << "[lbmModule] prepare geometry " << name << "... OK" << std::endl;
}

template<typename T>
void CFDModule<T>::setInitialized(bool initialization_) {
    this->initialized = initialization_;
}

template<typename T>
void CFDModule<T>::setPressures(std::unordered_map<int, T> pressures_) {
    this->pressures = pressures_;
}

template<typename T>
void CFDModule<T>::setFlowRates(std::unordered_map<int, T> flowRates_) {
    this->flowRates = flowRates_;
}

template<typename T>
void CFDModule<T>::setGroundNodes(std::unordered_map<int, bool> groundNodes_) {
    this->groundNodes = groundNodes_;
}

}   /// namespace arch