/**
 * @file HybridContinuous.h
 */

#pragma once

#include <iostream>
#include <math.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Module;

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
class lbmMixingSimulator;

template<typename T>
class lbmOocSimulator;

template<typename T>
class essLbmSimulator;

/**
 * @brief Class that conducts a abstract continuous simulation
 */
template<typename T>
class HybridContinuous : public Simulation<T> {
private:
    std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>> cfdSimulators;            ///< The set of CFD simulators, that conduct CFD simulations on <arch::Module>.
    std::unordered_map<int, std::shared_ptr<mmft::Scheme<T>>> updateSchemes;            ///< The update scheme for Abstract-CFD coupling
    bool writePpm = true;
    bool eventBasedWriting = false;

    void initialize() override;

    void saveState() override;

    inline bool conductNodalAnalysis() { return this->getNodalAnalysis()->conductNodalAnalysis(cfdSimulators); }

public:

    /**
     * @brief Constructor of the hybrid continuous simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    HybridContinuous(std::shared_ptr<arch::Network<T>> network);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for all modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(int moduleId, T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(int moduleId, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
     * @return Pointer to the newly created simulator.
    */
    lbmSimulator<T>* addLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings, 
                                    T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

    /** TODO: HybridMixingSimulation
     * Enable hybrid mixing simulation and uncomment code below
     */
    // /**
    //  * @brief Adds a new simulator to the network.
    //  * @param[in] name Name of the simulator.
    //  * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
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
    // lbmMixingSimulator<T>* addLbmMixingSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
    //                                         std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);


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
     * @param simulatorId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    CFDSimulator<T>* getCFDSimulator(int simulatorId) const;

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& getCFDSimulators() const;

    /**
     * @brief Get the global bounds of pressure values in the CFD simulators.
     * @return A tuple with the global bounds for pressure values <pMin, pMax>
     */
    std::tuple<T, T> getGlobalPressureBounds() const;
    
    /**
     * @brief Get the global bounds of velocity magnitude values in the CFD simulators.
     * @return A tuple with the global bounds for velocity magnitude values <velMin, velMax>
     */
    std::tuple<T, T> getGlobalVelocityBounds() const;

    /**
     * @brief Write the pressure field in .ppm image format for all cfdSimulators
     */
    void writePressurePpm(std::tuple<T, T> bounds, int resolution=600);

    /**
     * @brief Write the velocity field in .ppm image format for all cfdSimulators
     */
    void writeVelocityPpm(std::tuple<T, T> bounds, int resolution=600);

    void simulate() override;

};

/** TODO: HybridMixingSimulation
 * Enable hybrid mixing simulation and uncomment code below
 */
// /**
//  * @brief Class that conducts a abstract continuous simulation
//  */
// template<typename T>
// class HybridMixing : public HybridContinuous<T> {
// private:
//     /**
//      * @brief Constructor of the hybrid mixing simulator object
//      */
//     HybridMixing();

//     void simulate() override;

// };

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

}   // namespace sim
