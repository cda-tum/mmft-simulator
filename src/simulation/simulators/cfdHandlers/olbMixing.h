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
   
// Forward declared dependencies
template<typename T>
class HybridConcentration;
template<typename T>
class CfdConcentration;

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
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;          ///< Advection diffusion dynamics
// using ADDynamics = olb::AdvectionDiffusionTRTdynamics<T,ADDESCRIPTOR>;       ///< Optional alternative to advection diffusion dynamics Needs a 'magical number', set to 1/12 in initAdLattice()
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

private:
    bool diffusiveBC = false;
    std::unordered_map<size_t, std::unordered_map<size_t, T>> concentrations;   ///< Vector of concentration values at module nodes. <nodeId, <speciId, conc>>
    std::unordered_map<size_t, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>>> concentrationProfiles;   ///< Vector of concentration profiles at module nodes. <nodeId, <specieId, profile>>

    std::unordered_map<size_t, const Specie<T>*> species;

    std::unordered_map<size_t, T> bulkConcentrations;

    T adRelaxationTime;                         ///< Relaxation time (tau) for the OLB solver.

    std::unordered_map<size_t, T> averageDensities;
    std::unordered_map<size_t, bool> custConverges;

    std::unordered_map<size_t, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>>> adLattices;      ///< The LBM lattice on the geometry.
    std::unordered_map<size_t, std::unique_ptr<olb::util::ValueTracer<T>>> adConverges;            ///< Value tracer to track convergence.
    std::unordered_map<size_t, std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T, ADDESCRIPTOR>>> adConverters;      ///< Object that stores conversion factors from phyical to lattice parameters.

    std::unordered_map<size_t, T*> fluxWall;
    T zeroFlux = 0.0;

    std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<olb::AnalyticalConst2D<T,T>>>> lbmConcentrationProfiles;
    std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>> meanConcentrations;       ///< Map of mean pressure values at module nodes.

    [[nodiscard]] std::string getDefaultName(size_t id);

protected:

    auto& getAdConverter(size_t key) {
        return *adConverters.at(key);
    }

    auto& getAdLattice(size_t key) {
        return *adLattices.at(key);
    }

    void initValueContainers() override;

    void initAdConverters(T density);

    void initAdConvergenceTracker();

    void prepareAdLattice(const T omega, size_t speciesId);

    void initConcentrationIntegralPlane(size_t adKey);

    void initAdLattice(size_t adKey);

    /**
     * @brief Define and prepare the coupling of the NS lattice with the AD lattices.
    */
    void prepareCoupling();

    /**
     * @brief Maps the velocity field of the NS lattice to the AD lattices.
    */
    void executeCoupling() override;

    /**
     * @brief Defines and sets the concentration boundary conditions depending on flow direction.
     * @note The concentration values are taken from the concentrations map. This map, which acts as a buffer, 
     * is used by the abstract solver to set the boundary conditions for the advection-diffusion equations.
     */
    void setConcentration2D(int key);

    void setConcentrationProfiles2D(int key);

    /**
     * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
     * @param[in] iT Iteration step.
    */
    void storeCfdResults(int iT) override;

    /**
     * @brief Store the average concentration values at the module outflow nodes.
     * @param[in] iT Iteration step.
     */
    void storeConstantConcentrationCfdResults(int iT);

    /**
     * @brief Store the concentration profile values at the module outflow nodes.
     * @param[in] iT Iteration step.
     */
    void storeConcentrationProfileCfdResults(int iT);

    /**
     * @brief Construct the concentration profile for a given set of concentrations.
     * @param[in] concentrations The concentrations to use for the profile.
     * @param[in] spectralResolution The spectral resolution to use for the profile.
     * @return A tuple containing the profile function, the Fourier coefficients, and the zeroth coefficient.
     */
    std::tuple<std::function<T(T)>, std::vector<T>, T> constructProfile(std::vector<std::pair<std::array<int, 2>, T>>& concentrations, size_t spectralResolution=100);

    /**
     * @brief Constructor of an lbm module.
     * @param[in] id Id of the module.
     * @param[in] name Name of the module.
     * @param[in] cfdModule Module on which the simulation is conducted.
     * @param[in] species The species that can appear in the simulation domain and should be modeled.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] alpha Relaxation factor for the iterative updates between the 1D and CFD solvers.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
     * @param[in] adRelaxationTime Relaxation time tau for the advection-diffusion LBM solver.
    */
    lbmMixingSimulator(size_t id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule, std::unordered_map<size_t, const Specie<T>*> species,
        T resolution, T charPhysLenth, T charPhysVelocity, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

    /**
     * @brief Constructor of an lbm module.
     * @param[in] id Id of the module.
     * @param[in] name Name of the module.
     * @param[in] cfdModule Module on which the simulation is conducted.
     * @param[in] species The species that can appear in the simulation domain and should be modeled.
     * @param[in] updateScheme The update scheme that is used for LBM-Abstract coupling.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] alpha Relaxation factor for the iterative updates between the 1D and CFD solvers.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
     * @param[in] adRelaxationTime Relaxation time tau for the advection-diffusion LBM solver.
    */
    lbmMixingSimulator(size_t id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule, std::unordered_map<size_t, const Specie<T>*> species,
        std::shared_ptr<mmft::Scheme<T>> updateScheme, T resolution, T charPhysLenth, T charPhysVelocity, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

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
     * @brief Set the pressure and flow rate boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    void setBoundaryValues(int iT) override;

    /**
     * @brief Set the concentration boundary values on the lattice at the module nodes.
     * @brief Adds a specie after initialization of simulator.
     * @param[in] id Id of the specie.
     * @param[in] specie Pointer to the specie.
     * @param[in] bulkConcentration Initial bulk concentration of the specie.
     */
    bool addSpecie(size_t id, const Specie<T>* specie, T bulkConcentration=0.0);

    bool setBulkConcentration(size_t specieId, T bulkConcentration);

    /**
     * @brief Removes a specie after initialization of simulator.
     */
    bool removeSpecie(size_t id);

    /**
     * @brief Set the concentration boundary values on the lattice at all the module nodes.
     * @param[in] iT Iteration step.
    */
    void setConcBoundaryValues(int iT);

    /**
     * @brief Conducts the collide and stream operations of the lattice.
    */
    void solve() override;

    /**
     * @brief Conducts the collide and stream operations of the lattice for a pure CFD simulation.
     * @param[in] maxIter Maximum number of iterations for the CFD solving.
    */
    void solveCFD(size_t maxIter);

    /**
     * @brief Conducts the collide and stream operations of the NS lattice.
    */
    void nsSolve();

    /**
     * @brief Conducts the collide and stream operations of the AD lattice(s).
     * @param[in] maxIter Maximum number of iterations for the CFD solving.
    */
    void adSolve(size_t maxIter);

    /**
     * @brief Initialize the simulator with the given resistance and mixing models.
     * @param[in] resistanceModel The resistance model to use.
     * @param[in] mixingModel The mixing model to use.
     */
    void initialize(const ResistanceModel<T>* resistanceModel, const MixingModel<T>* mixingModel) override;

    /**
     * @brief Construct the boundary condition profiles for a given species and lattice.
     * @param[in] speciesId The ID of the species.
     * @param[in] adLattice The advection-diffusion lattice.
     * @param[in] key The key for the boundary condition.
     */
    void constructBCProfiles(size_t speciesId, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>> adLattice, int key);

    /**
     * @brief Store the abstract concentrations at the nodes on the module boundary in the simulator.
     * @param[in] concentrations Map of concentrations and node ids.
     */
    void storeConcentrations(std::unordered_map<size_t, std::unordered_map<size_t, T>> concentrations) override;

    /**
     * @brief Get the concentrations at the boundary nodes.
     * @returns Concentrations
     */
    const std::unordered_map<size_t, std::unordered_map<size_t, T>>& getConcentrations() const override;

    /**
     * @brief Store the abstract concentration profiles at the nodes on the module boundary in the simulator.
     * @param[in] concentrationProfiles Map of concentration profiles and node ids.
     */
    void storeConcentrationProfiles(std::unordered_map<size_t, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>>> concentrationProfiles) override;

    /**
     * @brief Get the concentration profiles at the boundary nodes.
     * @returns Concentrations
     */
    const std::unordered_map<size_t, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>>>& getConcentrationProfiles() const override;

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    bool hasAdConverged() const override;

public:

    /**
     * @brief Get the relaxation time of the advection-diffusion simulator.
     * @returns The relaxation time
    */
    [[nodiscard]] inline T getAdTau() const { return adRelaxationTime; }

    /**
     * @brief Set the relaxation time for the advection-diffusion simulator.
     * @param[in] tau The new relaxation time.
    */
    void setAdTau(T tau) { this->adRelaxationTime = tau; this->unsetIsInitialized(); }

    /**
     * @brief Get the concentration bounds for a specific advection-diffusion key.
     * @param[in] adKey The advection-diffusion key.
     * @returns A tuple containing the minimum and maximum concentration values.
     */
    [[nodiscard]] std::tuple<T, T> getConcentrationBounds(size_t adKey) override;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    void writeVTK(int iT) override;

    /**
     * @brief Write the concentration values to a ppm file.
     */
    void writeConcentrationPpm(size_t adKey, T min, T max, int imgResolution) override;

    friend class HybridConcentration<T>;
    friend class CfdConcentration<T>;
};

}   // namespace arch
