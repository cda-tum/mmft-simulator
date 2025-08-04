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

#include <olb2D.h>
#include <olb2D.hh>
#include <olb3D.h>
#include <olb3D.hh>

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

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class lbmSimulator : public CFDSimulator<T> {

using DESCRIPTOR = olb::descriptors::D2Q9<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

protected:
    int stlMargin = 1;
    int step = 0;                           ///< Iteration step of this module.
    int stepIter = 1000;                    ///< Number of iterations for the value tracer.
    int maxIter = 1e7;                      ///< Maximum total iterations.
    std::unordered_map<int, T> pressures;   ///< Vector of pressure values at module nodes.
    std::unordered_map<int, T> flowRates;   ///< Vector of flowRate values at module nodes.
    
    bool isConverged = false;               ///< Has the module converged?
    
    T charPhysLength;                       ///< Characteristic physical length (= width, usually).
    T charPhysVelocity;                     ///< Characteristic physical velocity (expected maximal velocity).

    T resolution;                           ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
    T epsilon;                              ///< Convergence criterion.
    T relaxationTime;                       ///< Relaxation time (tau) for the OLB solver.

    std::shared_ptr<olb::STLreader<T>> stlReader;
    std::shared_ptr<olb::IndicatorF2DfromIndicatorF3D<T>> stl2Dindicator;
    std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids.
    std::shared_ptr<olb::CuboidGeometry<T,2>> cuboidGeometry;       ///< The geometry in a single cuboid.
    std::shared_ptr<olb::SuperGeometry<T,2>> geometry;              ///< The final geometry of the channels.
    std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
    std::unique_ptr<olb::util::ValueTracer<T>> converge;            ///< Value tracer to track convergence.

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

    void setOutputDir();
    
    virtual void initValueContainers();

    void initNsConverter(T dynViscosity, T density);

    void initNsConvergeTracker();

    virtual void prepareNsLattice(const T omega);

    void initPressureIntegralPlane();

    void initFlowRateIntegralPlane();

    void initNsLattice(const T omega);

    void setFlowProfile2D(int key, T openingWidth);

    void setPressure2D(int key);

    /**
     * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
     * @param[in] iT Iteration step.
    */
    void storeCfdResults(int iT);

public:
    /**
     * @brief Constructor of an lbm module.
     * @param[in] id Id of the module.
     * @param[in] name Name of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] nodes Map of nodes that are on the boundary of the module.
     * @param[in] openings Map of the in-/outlets of the module.
     * @param[in] stlFile STL file that describes the geometry of the CFD domain.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] alpha Relaxation factor for the iterative updates between the 1D and CFD solvers.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
    */
    lbmSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, arch::Opening<T>> openings, 
        ResistanceModel<T>* resistanceModel, T charPhysLenth, T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932);

    /**
     * @brief Constructor of an lbm module.
     * @param[in] id Id of the module.
     * @param[in] name Name of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] nodes Map of nodes that are on the boundary of the module.
     * @param[in] openings Map of the in-/outlets of the module.
     * @param[in] stlFile STL file that describes the geometry of the CFD domain.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] alpha Relaxation factor for the iterative updates between the 1D and CFD solvers.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
    */
    lbmSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, arch::Opening<T>> openings, 
        std::shared_ptr<mmft::Scheme<T>> updateScheme, ResistanceModel<T>* resistanceModel, T charPhysLenth, T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932);

    /**
     * @brief Initialize an instance of the LBM solver for this simulator.
     * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
     * @param[in] density Density of the simulated fluid in _kg / m^3_.
    */
    void lbmInit(T dynViscosity, T density) override;

    /**
     * @brief Prepare the LBM geometry of this simulator.
    */
    void prepareGeometry() override;

    /**
     * @brief Prepare the LBM lattice on the LBM geometry.
    */
    void prepareLattice() override;

    /**
     * @brief Set the boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    void setBoundaryValues(int iT) override;

    /**
     * @brief Conducts the collide and stream operations of the lattice.
    */
    void solve();

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    void writeVTK(int iT) override;

    /**
     * @brief Write the .ppm image file with the pressure results of the CFD simulation to file system.
     * @param[in] min Minimal bound for colormap.
     * @param[in] max Maximal bound for colormap.
     * @param[in] imgResolution Resolution of the .ppm image.
    */
    void writePressurePpm (T min, T max, int imgResolution) override;

    /**
     * @brief Write the .ppm image file with the velocity results of the CFD simulation to file system.
     * @param[in] min Minimal bound for colormap.
     * @param[in] max Maximal bound for colormap.
     * @param[in] imgResolution Resolution of the .ppm image.
    */
    void writeVelocityPpm (T min, T max, int imgResolution) override;

    /**
     * @brief Store the abstract pressures at the nodes on the module boundary in the simulator.
     * @param[in] pressure Map of pressures and node ids.
     */
    void storePressures(std::unordered_map<int, T> pressure);

    /**
     * @brief Store the abstract flow rates at the nodes on the module boundary in the simulator.
     * @param[in] flowRate Map of flow rates and node ids.
     */
    void storeFlowRates(std::unordered_map<int, T> flowRate);

    auto& readGeometry() const {
        return *geometry;
    }

    void readGeometryStl(const T dx, const bool print);

    void readOpenings(const T dx);

    /**
     * @brief Get the pressures at the boundary nodes.
     * @returns Pressures in Pa.
     */
    std::unordered_map<int, T> getPressures() const {
        return pressures;
    };

    /**
     * @brief Get the flow rates at the boundary nodes.
     * @returns Flow rates in m^3/s.
     */
    std::unordered_map<int, T> getFlowRates() const {
        return flowRates;
    };

    /**
     * @brief Get the number of iterations for the value tracer.
     * @returns Number of iterations for the value tracer.
    */
    int getStepIter() const {
        return stepIter;
    };

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    bool hasConverged() const {
        return converge->hasConverged();
    };

    /**
     * @brief Get the characteristic physical length.
     * @returns Characteristic physical length.
    */
    T getCharPhysLength() const { 
        return charPhysLength; 
    };

    /**
     * @brief Get the characteristic physical velocity.
     * @returns Characteristic physical velocity.
    */
    T getCharPhysVelocity() const { 
        return charPhysVelocity; 
    };

    /**
     * @brief Get the resolution.
     * @returns resolution.
    */
    T getResolution() const { 
        return resolution; 
    };

    /**
     * @brief Get the convergence criterion.
     * @returns epsilon.
    */
    T getEpsilon() const { 
        return epsilon; 
    };

    /**
     * @brief Returns the local pressure bounds of this cfdSimulator 
     * @returns A tuple with the pressure bounds <pMin, pMax>
     */
    std::tuple<T, T> getPressureBounds() override;

    /**
     * @brief Returns the local velocity bounds of this cfdSimulator 
     * @returns A tuple with the velocity bounds <pMin, pMax>
     */
    std::tuple<T, T> getVelocityBounds() override;
};

}   // namespace arch
