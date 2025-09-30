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
class CfdModule;
template<typename T>
class Module;
template<typename T>
class Network;
template<typename T>
class Node;
template<typename T>
class Opening;

}

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GeometryTest;

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

private:

    size_t resolution = 0;                  ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
    T charPhysLength = 0.0;                 ///< Characteristic physical length (= width, usually).
    T charPhysVelocity = 0.0;               ///< Characteristic physical velocity (expected maximal velocity).
    T epsilon = 0.0;                        ///< Convergence criterion.
    T relaxationTime = 0.0;                 ///< Relaxation time (tau) for the OLB solver.

    int stlMargin = 1;
    int step = 0;                           ///< Iteration step of this module.
    int stepIter = 1000;                    ///< Number of iterations for the value tracer.
    int maxIter = 1e7;                      ///< Maximum total iterations.
    std::unordered_map<size_t, T> pressures;    ///< Vector of pressure values at module nodes.
    std::unordered_map<size_t, T> flowRates;    ///< Vector of flowRate values at module nodes.
    
    bool isInitialized = false;             ///< Has lbmInit taken place?
    bool isConverged = false;               ///< Has the module converged?
    
    std::shared_ptr<olb::STLreader<T>> stlReader;
    std::shared_ptr<olb::IndicatorF2DfromIndicatorF3D<T>> stl2Dindicator;
    std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids.
    std::shared_ptr<olb::CuboidGeometry<T,2>> cuboidGeometry;       ///< The geometry in a single cuboid.
    std::shared_ptr<olb::SuperGeometry<T,2>> geometry;              ///< The final geometry of the channels.
    std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
    std::unique_ptr<olb::util::ValueTracer<T>> converge;            ///< Value tracer to track convergence.

    std::unordered_map<size_t, std::shared_ptr<olb::Poiseuille2D<T>>> flowProfiles;
    std::unordered_map<size_t, std::shared_ptr<olb::AnalyticalConst2D<T,T>>> densities;
    std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;      ///< Object that stores conversion factors from phyical to lattice parameters.
    std::unordered_map<size_t, std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>>> fluxes;           ///< Map of fluxes at module nodes. 
    std::unordered_map<size_t, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>> meanPressures;    ///< Map of mean pressure values at module nodes.

    [[nodiscard]] std::string getDefaultName(int id);

protected:

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

    /**
     * @brief Constructor of an lbm simulator.
     * @param[in] id Id of the simulator.
     * @param[in] name Name of the simulator.
     * @param[in] cfdModule Module on which the simulation is conducted.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
    */
    lbmSimulator(int id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule,
        size_t resolution, T charPhysLenth, T charPhysVelocity, T epsilon, T relaxationTime=0.932);
    
        /**
     * @brief Constructor of an lbm simulator.
     * @param[in] id Id of the simulator.
     * @param[in] name Name of the simulator.
     * @param[in] cfdModule Module on which the simulation is conducted.
     * @param[in] updateScheme The update scheme that is used for LBM-Abstract coupling.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
    */
    lbmSimulator(int id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule, std::shared_ptr<mmft::Scheme<T>> updateScheme, 
        size_t resolution, T charPhysLenth, T charPhysVelocity, T epsilon, T relaxationTime=0.932);

    /**
     * @brief Initialize an instance of the LBM solver for this simulator.
     * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
     * @param[in] density Density of the simulated fluid in _kg / m^3_.
    */
    void lbmInit(T dynViscosity, T density) override;

    /**
     * @brief Checks if all simulation parameters have been initialized correctly before simulation.
     * @note The LBM converter object needs to be reconfigured if simulation parameters were changed.
    */
    void checkInitialized();

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
     * @brief Store the abstract pressures at the nodes on the module boundary in the simulator.
     * @param[in] pressure Map of pressures and node ids.
     */
    inline void storePressures(std::unordered_map<size_t, T> pressure) { this->pressures = pressure; }

    /**
     * @brief Store the abstract flow rates at the nodes on the module boundary in the simulator.
     * @param[in] flowRate Map of flow rates and node ids.
     */
    inline void storeFlowRates(std::unordered_map<size_t, T> flowRate) { this->flowRates = flowRate; }

    auto& readGeometry() const {
        return *geometry;
    }

    void readGeometryStl(const T dx, const bool print);

    void readOpenings(const T dx);

    /**
     * @brief Get the pressures at the boundary nodes.
     * @returns Pressures in Pa.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, T>& getPressures() const { return pressures; }

    /**
     * @brief Get the flow rates at the boundary nodes.
     * @returns Flow rates in m^3/s.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, T>& getFlowRates() const { return flowRates; }

    /**
     * @brief Sets a new characteristic length for the simulator.
     * @param[in] charPhysLength the new characteristic physical length.
     * @note Since this is a global parameter, only the HybridContinuous object can set a new characteristic length.
     */
    void setCharPhysLength(T charPhysLength) { this->charPhysLength = charPhysLength; isInitialized = false; }

    /**
     * @brief Sets a new characteristic velocity for the simulator.
     * @param[in] charPhysVelocity the new characteristic physical velocity.
     * @note Since this is a global parameter, only the HybridContinuous object can set a new characteristic velocity.
     */
    void setCharPhysVelocity(T charPhysVelocity) { this->charPhysVelocity = charPhysVelocity; isInitialized = false; }

    void setIsInitialized() { isInitialized = true; }

    void unsetIsInitialized() { isInitialized = false; }

public:

    /**
     * @brief Get the characteristic physical length.
     * @returns Characteristic physical length.
    */
    [[nodiscard]] inline T getCharPhysLength() const { return charPhysLength; }

    /**
     * @brief Get the characteristic physical velocity.
     * @returns Characteristic physical velocity.
    */
    [[nodiscard]] inline T getCharPhysVelocity() const { return charPhysVelocity; }

    /**
     * @brief Get the resolution.
     * @returns resolution.
    */
    [[nodiscard]] inline T getResolution() const { return resolution; }

    /**
     * @brief Set the resolution for the simulator.
     * @param[in] resolution The new resolution.
     */
    inline void setResolution(size_t resolution) { this->resolution = resolution; isInitialized = false; }

    /**
     * @brief Get the convergence criterion.
     * @returns epsilon.
    */
    [[nodiscard]] inline T getEpsilon() const { return epsilon; }
    
    /**
     * @brief Set the epsilon for the simulator.
     * @param[in] epsilon The new epsilon.
     */
    void setEpsilon(T epsilon) { this->epsilon = epsilon; isInitialized = false; }

    /**
     * @brief Get the relaxation time of the simulator.
     * @returns The relaxation time
    */
    [[nodiscard]] inline T getTau() const { return relaxationTime; }

    /**
     * @brief Set the relaxation time for the simulator.
     * @param[in] tau The new relaxation time.
    */
    void setTau(T tau) { this->relaxationTime = tau; isInitialized = false; }

    /**
     * @brief Returns the local pressure bounds of this cfdSimulator 
     * @returns A tuple with the pressure bounds <pMin, pMax>
     */
    [[nodiscard]] std::tuple<T, T> getPressureBounds() override;

    /**
     * @brief Returns the local velocity bounds of this cfdSimulator 
     * @returns A tuple with the velocity bounds <pMin, pMax>
     */
    [[nodiscard]] std::tuple<T, T> getVelocityBounds() override;

    /**
     * @brief Get the number of iterations for the value tracer.
     * @returns Number of iterations for the value tracer.
    */
    [[nodiscard]] inline int getStepIter() const { return stepIter; }

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    [[nodiscard]] inline bool hasConverged() const { return converge->hasConverged(); }

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

    friend class HybridContinuous<T>;
    friend class test::definitions::GeometryTest<T>;
};

}   // namespace arch
