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

    Sim2D() { };

    auto& readGeometry() const {
        return *geometry;
    }

    T getDx() { return converter->getConversionFactorLength(); }

    T getOmega() { return converter->getLatticeRelaxationFrequency(); }

};

}   // namespace arch
