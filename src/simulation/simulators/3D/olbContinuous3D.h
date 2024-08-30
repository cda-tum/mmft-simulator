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
class lbmSimulator3D : public lbmSimulator<T> {

using DESCRIPTOR = olb::descriptors::D3Q19<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

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

    void setFlowProfile3D(int key, T openingWidth);

    void setPressure3D(int key);

    void initNsConverter(T dynViscosity, T density) override;

    void prepareNsLattice(const T omega) override;

    void initNsLattice(const T omega) override;

    void initPressureIntegralPlane() override;

    void initFlowRateIntegralPlane() override;

    void collideAndStream() override { lattice->collideAndStream(); }

    void storeCfdResults(int iT);

public:
    
    lbmSimulator3D(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, arch::Opening<T>> openings, 
        ResistanceModel<T>* resistanceModel, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932);

    /**
     * @brief Set the boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    void setBoundaryValues(int iT) override;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    virtual void writeVTK(int iT) override;

    auto& readGeometry() const {
        return *geometry;
    }

    void readGeometryStl(const T dx, const bool print) override;

    void readOpenings(const T dx, const bool print) override;

    T getDx() override { return converter->getConversionFactorLength(); }

    T getOmega() override { return converter->getLatticeRelaxationFrequency(); }
};

}   // namespace arch
