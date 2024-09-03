/**
 * @file olbContinuous.h
 */

#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

namespace arch {

// Forward declared dependencies
template<typename T>
class Module;
template<typename T>
class Network;
template<typename T>
class Node;
template<typename T>
class Opening;

}

namespace sim {

template<typename T>
class Sim2D {

using DESCRIPTOR = olb::descriptors::D2Q9<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

using ADDESCRIPTOR = olb::descriptors::D2Q5<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

protected:

    std::shared_ptr<olb::IndicatorF2DfromIndicatorF3D<T>> stl2Dindicator;

    std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids.
    std::shared_ptr<olb::CuboidGeometry<T,2>> cuboidGeometry;       ///< The geometry in a single cuboid.
    std::shared_ptr<olb::SuperGeometry<T,2>> geometry;              ///< The final geometry of the channels.
    std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
    
    std::unordered_map<int, std::shared_ptr<olb::Poiseuille2D<T>>> flowProfiles;
    std::unordered_map<int, std::shared_ptr<olb::AnalyticalConst2D<T,T>>> densities;

    std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;      ///< Object that stores conversion factors from phyical to lattice parameters.
    std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>>> fluxes;              ///< Map of fluxes at module nodes. 
    std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>> meanPressures;       ///< Map of mean pressure values at module nodes.

    auto& getConverter() {
        return *converter;
    }

    auto& getGeometry() {
        return *geometry;
    }

    auto& getLattice() {
        return *lattice;
    }

public:

    Sim2D()

    auto& readGeometry() const {
        return *geometry;
    }

    void readGeometryStl2D(const T dx, const bool print) 
    {
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

    void readOpenings2D(const T dx, const bool print) 
    {
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

    T getDx() { return converter->getConversionFactorLength(); }

    T getOmega() { return converter->getLatticeRelaxationFrequency(); }

};

}   // namespace arch
