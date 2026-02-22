/**
 * @file HybridContinuous.h
 */

#pragma once

#include <iostream>
#include <math.h>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class CfdModule;

template<typename T>
class Network;

template<typename T>
class Opening;

}

namespace mmft {

// Forward declared dependencies
template<typename T>
class Scheme;

template<typename T>
class NaiveScheme;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

template<typename T>
class lbmSimulator;

template<typename T>
class lbmOocSimulator;

template<typename T>
class essLbmSimulator;

template<typename T>
class InstantaneousMixingModel;

template<typename T>
class DiffusionMixingModel;

/**
 * @brief Class that conducts a abstract continuous simulation
 */
template<typename T>
class HybridContinuous : public Simulation<T> {
private:
    T characteristicLength = 1e-4;                                                      ///< Standard value (1e-4) or smallest opening width / height.
    T characteristicVelocity = 0.1;                                                     ///< Standard value (0.1) or Largest expected average velocity in the system.
    std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>> cfdSimulators;            ///< The set of CFD simulators, that conduct CFD simulations on <arch::Module>.
    std::unordered_map<int, std::unique_ptr<mmft::Scheme<T>>> updateSchemes;            ///< The update scheme for Abstract-CFD coupling
    bool writePpm = true;
    bool eventBasedWriting = false;

protected:

    /**
     * @brief Constructor of the hybrid continuous simulator object
     * @param[in] platform The platform of the derived simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    HybridContinuous(Platform platform, std::shared_ptr<arch::Network<T>> network);

    void assertInitialized() const override;

    void initialize() override;

    void saveState() override;                                           

    std::optional<bool> conductNodalAnalysis() override { return this->getNodalAnalysis()->conductNodalAnalysis(cfdSimulators); } 

    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
    */
    essLbmSimulator<T>* addEssLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings,
                                        T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    [[nodiscard]] inline std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>>& getCFDSimulators() { return cfdSimulators; }

    /**
     * @brief Get injection
     * @param simulatorId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    [[nodiscard]] inline const CFDSimulator<T>* readCFDSimulator(int simulatorId) const { return cfdSimulators.at(simulatorId).get(); }

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    [[nodiscard]] inline const std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>>& readCFDSimulators() const { return cfdSimulators; }

    /**
     * @brief Checks if the simulator has a valid resistance model for hybrid simulations.
     * @returns true/false
     * @note So far, only a poiseuille resistance model is valid for the 2-dimensional CFD simulations.
     */
    [[nodiscard]] bool hasValidResistanceModel();

    /** TODO:
     * 
     */
    bool getWritePpm() { return writePpm; }

public:

    /**
     * @brief Constructor of the hybrid continuous simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    HybridContinuous(std::shared_ptr<arch::Network<T>> network);

    /**
     * @brief Sets the resistance model for abstract simulation components to the 1D resistance model
     * @throws A logic_error because this resistance model is incompatible with Hybrid simulation.
     */
    void set1DResistanceModel() override;

    /**
     * @brief Sets the resistance model for abstract simulation components to the poiseuille resistance model
     */
    void setPoiseuilleResistanceModel() override;

    /**
     * @brief Returns the current global characteristic length for the simulation.
     * @returns The global characteristic length
     */
    [[nodiscard]] inline T getCharacteristicLength() { return characteristicLength; }

    /**
     * @brief Returns the current global characteristic velocity for the simulation.
     * @returns The global characteristic velocity
     */
    [[nodiscard]] inline T getCharacteristicVelocity() { return characteristicVelocity; }

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note This is the cheapest definition for add an lbmSimulator, and many parameters, such as epsilon, tau and resolution
     * are defaulted.
    */
    [[maybe_unused]] virtual std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, std::string name="");

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] resolution The number of grid cells across the characteristic length.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note Besides the resolution, all other simulation parameters are defaulted.
    */
    [[maybe_unused]] virtual std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, size_t resolution, std::string name="");

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] resolution The number of grid cells across the characteristic length.
     * @param[in] epsilon The allowed error margin between this hybrid domain and the surrounding abstract domain.
     * @param[in] tau The relaxation number of this LBM solver.
     * @param[in] charPhysLength The physical characteristic length of this LBM solver.
     * @param[in] charPhysVelocity The physical characteristic velocity of this LBM solver.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note No simulation parameters are defaulted in this function overload.
    */
    [[maybe_unused]] std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, 
        size_t resolution, T epsilon, T tau, T charPhysLength, T charPhysVelocity, std::string name="");

    /**
     * @brief Returns a pointer to the lbmSimulator for the given simulation id
     * @param[in] simulatorId Id of the lbmSimulator
     * @returns A pointer to the lbmSimulator
     * @throws logic_error if the simulator with given id is not listed
     */
    [[nodiscard]] inline std::shared_ptr<CFDSimulator<T>> getLbmSimulator(size_t simulatorId) const { return cfdSimulators.at(simulatorId); } 

    /**
     * @brief Removes a simulator from the hybrid simulation
     * @param[in] simulator A pointer to the simulator that is to be removed from this hybrid simulation
     * @throws logic_error if the simulator with given id is not listed
     * @note Leaves a 'dangling' module which needs to be reassigned to another simulator or removed from 
     * the network before the next simulation.
     */
    void removeLbmSimulator(const std::shared_ptr<CFDSimulator<T>>& simulator);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation on all nodes in all simulators.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for all simulators.
     */
    void setNaiveHybridScheme(T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation on all nodes of the module.
     * @param[in] simulator A pointer to the simulator for which the update scheme is set.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     */
    void setNaiveHybridScheme(const std::shared_ptr<CFDSimulator<T>>& simulator, T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    void setNaiveHybridScheme(const std::shared_ptr<CFDSimulator<T>>& simulator, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Get the global bounds of pressure values in the CFD simulators.
     * @return A tuple with the global bounds for pressure values <pMin, pMax>
     */
    std::pair<T,T> getGlobalPressureBounds() const;
    
    /**
     * @brief Get the global bounds of velocity magnitude values in the CFD simulators.
     * @return A tuple with the global bounds for velocity magnitude values <velMin, velMax>
     */
    std::pair<T,T> getGlobalVelocityBounds() const;

    /**
     * @brief Write the pressure field in .ppm image format for all cfdSimulators
     */
    void writePressurePpm(std::pair<T,T> bounds, int resolution=600) const;

    /**
     * @brief Write the velocity field in .ppm image format for all cfdSimulators
     */
    void writeVelocityPpm(std::pair<T,T> bounds, int resolution=600) const;

    void simulate() override;

    friend class InstantaneousMixingModel<T>;
    friend class DiffusionMixingModel<T>;
};

}   // namespace sim

/** TODO: HybridOocSimulation
 * Enable hybrid OoC simulation and uncomment code below
 */
// /**
//  * @brief Class that conducts a abstract continuous simulation
//  */
// template<typename T>
// class HybridOoc final : public HybridMixing<T> {
// private:
//     /**
//      * @brief Constructor of the hybrid mixing simulator object
//      */
//     HybridOoc();

//     void simulate() override;

// };

/** TODO: HybridOocSimulation
 * Enable hybrid OoC simulation and uncomment code below
 */
// /**
//  * @brief Adds a new simulator to the network.
//  * @param[in] name Name of the simulator.
//  * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
//  * @param[in] tissueId The Id of the tissue that the organ in this nodule consists of.
//  * @param[in] organStlFile The location of the stl file describing the geometry of the organ.
//  * @param[in] module Shared pointer to the module on which this solver acts.
//  * @param[in] species Map of specieIds and speciePtrs of the species simulated in the AD fields of this simulator.
//  * @param[in] openings Map of openings corresponding to the nodes.
//  * @param[in] charPhysLength Characteristic physical length of this simulator.
//  * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
//  * @param[in] resolution Resolution of this simulator.
//  * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
//  * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
//  * @return Pointer to the newly created simulator.
// */
// lbmOocSimulator<T>* addLbmOocSimulator(std::string name, std::string stlFile, int tissueId, std::string organStlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
//                                         std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

