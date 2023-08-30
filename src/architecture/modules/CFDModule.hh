#include "CFDModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "Module.h"

namespace arch{

template<typename T, typename DESCRIPTOR>
CFDModule<T,DESCRIPTOR>::CFDModule(
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, 
    std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, 
    T charPhysLength_, T charPhysVelocity_, T resolution_, T relaxationTime_) : Module<T>(id_, pos_, size_, nodes_), 
    moduleOpenings(openings_), stlFile(stlFile_), name(name_), charPhysLength(charPhysLength_), 
    charPhysVelocity(charPhysVelocity_), resolution(resolution_), relaxationTime(relaxationTime_) 
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

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::prepareGeometry() {

    olb::STLreader<T> stlReader(stlFile, converter->getConversionFactorLength());
    std::cout << "[lbmModule] reading STL file " << name << "... OK" << std::endl;
    olb::IndicatorF2DfromIndicatorF3D<T> stl2Dindicator(stlReader);
    std::cout << "[lbmModule] create 2D indicator " << name << "... OK" << std::endl;

    olb::Vector<T,2> origin(-1.0*converter->getConversionFactorLength(), -1.0*converter->getConversionFactorLength());
    olb::Vector<T,2> extend(this->size[0] + 2.0*converter->getConversionFactorLength(), this->size[1] + 2.0*converter->getConversionFactorLength());
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
                        - 0.5*Opening.width*Opening.tangent[0] + 0.5*converter->getConversionFactorLength()*Opening.normal[0] - 0.5*converter->getConversionFactorLength()*abs(Opening.normal[1]);
        T y_origin =   Opening.node->getPosition()[1] - this->getPosition()[1]
                        - 0.5*Opening.width*Opening.tangent[1] + 0.5*converter->getConversionFactorLength()*Opening.normal[1] - 0.5*converter->getConversionFactorLength()*abs(Opening.normal[0]);

        std::cout << "The origin is: " << x_origin << ", " << y_origin << std::endl;
        
        // The unit vector pointing to the extend
        T x_extend = Opening.width*Opening.tangent[0] - converter->getConversionFactorLength()*Opening.normal[0];// + 0.5*converter->getConversionFactorLength()*Opening.normal[1];
        T y_extend = Opening.width*Opening.tangent[1] - converter->getConversionFactorLength()*Opening.normal[1];// - 0.5*converter->getConversionFactorLength()*Opening.normal[0];
        
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

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::getResults(int iT_) {
    
    int input[1] = { };
    T output[3];
    
    for (auto& [key, Opening] : moduleOpenings) {
        if (groundNodes.at(key)) {
            meanPressures.at(key)->operator()(output, input);
            T newPressure =  output[0]/output[1];
            pressures.at(key) = newPressure;
            if (iT % statIter == 0) {
                meanPressures.at(key)->print();
            }
        } else {
            fluxes.at(key)->operator()(output,input);
            flowRates.at(key) = output[0];
            if (iT % statIter == 0) {
                fluxes.at(key)->print();
            }
        }
    }
}

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::setInitialized(bool initialization_) {
    this->initialized = initialization_;
}

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::setPressures(std::unordered_map<int, T> pressures_) {
    this->pressures = pressure_;
}

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::setFlowRates(std::unordered_map<int, T> flowRates_) {
    this->flowRates = flowRate_;
}

template<typename T, typename DESCRIPTOR>
void CFDModule<T,DESCRIPTOR>::setGroundNodes(std::unordered_map<int, bool> groundNodes_) {
    this->groundNodes = groundNodes_;
}

}   /// namespace arch