/**
 * @file olbMixing.h
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

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class lbmMixingSimulator : public lbmSimulator<T> {

using DESCRIPTOR = olb::descriptors::D2Q9<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

using ADDESCRIPTOR = olb::descriptors::D2Q5<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

protected:
    std::unordered_map<int, std::unordered_map<int, T>> concentrations;   ///< Vector of concentration values at module nodes. <nodeId, <speciId, conc>>

    std::unordered_map<int, Specie<T>*> species;

    T adRelaxationTime;                         ///< Relaxation time (tau) for the OLB solver.

    std::unordered_map<int, T> averageDensities;
    std::unordered_map<int, bool> custConverges;

    std::unordered_map<int, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>>> adLattices;      ///< The LBM lattice on the geometry.
    std::unordered_map<int, std::unique_ptr<olb::util::ValueTracer<T>>> adConverges;            ///< Value tracer to track convergence.
    std::unordered_map<int, std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T, ADDESCRIPTOR>>> adConverters;      ///< Object that stores conversion factors from phyical to lattice parameters.

    std::unordered_map<int, T*> fluxWall;
    T zeroFlux = 0.0;

    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<olb::AnalyticalConst2D<T,T>>>> concentrationProfiles;
    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>> meanConcentrations;       ///< Map of mean pressure values at module nodes.

    auto& getAdConverter(int key) {
        return *adConverters.at(key);
    }

    auto& getAdLattice(int key) {
        return *adLattices.at(key);
    }

    void initValueContainers() override;

    void initAdConverters(T density);

    void initAdConvergenceTracker();

    void prepareAdLattice(const T omega, int speciesId);

    void initConcentrationIntegralPlane(int adKey);

    void initAdLattice(int adKey);

    /**
     * @brief Define and prepare the coupling of the NS lattice with the AD lattices.
    */
    void prepareCoupling();

    /**
     * @brief Execute the coupling placed onto the NS lattice.
    */
    void executeCoupling() override;

    void setConcentration2D(int key);

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
    lbmMixingSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, Specie<T>*> species,
        std::unordered_map<int, arch::Opening<T>> openings, ResistanceModel<T>* resistanceModel, T charPhysLenth, 
        T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

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
    lbmMixingSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, Specie<T>*> species,
        std::unordered_map<int, arch::Opening<T>> openings, std::shared_ptr<mmft::Scheme<T>> updateScheme, ResistanceModel<T>* resistanceModel, T charPhysLenth, 
        T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

    /**
     * @brief Initialize an instance of the LBM solver for this module.
     * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
     * @param[in] density Density of the simulated fluid in _kg / m^3_.
    */
    void lbmInit(T dynViscosity, T density) override;

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
    void solve() override;

    /**
     * @brief Conducts the collide and stream operations of the NS lattice.
    */
    void nsSolve() override;

    /**
     * @brief Conducts the collide and stream operations of the AD lattice(s).
    */
    void adSolve() override;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    void writeVTK(int iT) override;

    /**
     * @brief Store the abstract concentrations at the nodes on the module boundary in the simulator.
     * @param[in] concentrations Map of concentrations and node ids.
     */
    void storeConcentrations(std::unordered_map<int, std::unordered_map<int, T>> concentrations) override;

    /**
     * @brief Get the concentrations at the boundary nodes.
     * @returns Concentrations
     */
    std::unordered_map<int, std::unordered_map<int, T>> getConcentrations() const override;

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    bool hasAdConverged() const override;

};

}   // namespace arch
