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
class Sim3D {

using DESCRIPTOR = olb::descriptors::D3Q19<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

using ADDESCRIPTOR = olb::descriptors::D3Q7<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

protected:

    std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids.
    std::shared_ptr<olb::CuboidGeometry<T,3>> cuboidGeometry;       ///< The geometry in a single cuboid.
    std::shared_ptr<olb::SuperGeometry<T,3>> geometry;              ///< The final geometry of the channels.
    std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
    
    std::unordered_map<int, std::shared_ptr<olb::RectanglePoiseuille3D<T>>> flowProfiles;
    std::unordered_map<int, std::shared_ptr<olb::AnalyticalConst3D<T,T>>> densities;

    std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;      ///< Object that stores conversion factors from phyical to lattice parameters.
    std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity3D<T>>> fluxes;              ///< Map of fluxes at module nodes. 
    std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure3D<T>>> meanPressures;       ///< Map of mean pressure values at module nodes.

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


    Sim3D()

    auto& readGeometry() const {
        return *geometry;
    }

    void readGeometryStl3D(const T dx, const bool print) 
    {
        /**
         * TODO: Thought I already had defined this somewhere?
         */
    }

    void readOpenings2D(const T dx, const bool print) 
    {
        /**
         * TODO: Thought I already had defined this somewhere?
         */
    }

    T getDx() { return converter->getConversionFactorLength(); }

    T getOmega() { return converter->getLatticeRelaxationFrequency(); }

};

}   // namespace arch
