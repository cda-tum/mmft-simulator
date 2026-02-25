/**
 * @file HybridConcentration.h
 */

#pragma once

#include <memory>

namespace arch {

// Forward declared dependencies
template<typename T>
class CfdModule;

template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class ConcentrationSemantics;

template<typename T>
class CFDSimulator;

template<typename T>
class lbmSimulator;

template<typename T>
class lbmMixingSimulator;

/**
 * @brief Class that conducts an abstract mixing simulation
 */
template<typename T>
class HybridConcentration : public HybridContinuous<T>, public ConcentrationSemantics<T> {
private:
    std::unordered_map<size_t, T> setInitialConcentrations;
    
    void assertInitialized() const override;

    /** TODO: HybridConcentration */
    // void initialize() override;

    void saveState() override;

    void saveMixtures();

public:
    /**
     * @brief Constructor of the hybrid mixing simulator object
     */
    HybridConcentration(std::shared_ptr<arch::Network<T>> network);

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note This is the cheapest definition for add an lbmSimulator, and many parameters, such as epsilon, tau and resolution
     * are defaulted.
    */
    [[maybe_unused]] std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, std::string name="") override;

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] resolution The number of grid cells across the characteristic length.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note Besides the resolution, all other simulation parameters are defaulted.
    */
    [[maybe_unused]] std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, size_t resolution, std::string name="") override;

    /**
     * @brief Create and add an LBM Simulator for a CFD Module to the Hybrid simulation
     * @param[in] module A pointer to the CfdModule on which this simulator instance will conduct LBM simulations.
     * @param[in] resolution The number of grid cells across the characteristic length.
     * @param[in] epsilon The allowed error margin between this hybrid domain and the abstract domain in TODO.
     * @param[in] tau The relaxation number of this LBM solver.
     * @param[in] charPhysLength The physical characteristic length of this LBM solver.
     * @param[in] charPhysVelocity The physical characteristic velocity of this LBM solver.
     * @param[in] name The name of this simulator
     * @return A shared pointer to the created lbmSimulator instance
     * @note No simulation parameters are defaulted in this function overload.
    */
    [[maybe_unused]] std::shared_ptr<lbmSimulator<T>> addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, 
        size_t resolution, T epsilon, T tau, T adTau, T charPhysLength, T charPhysVelocity, std::string name="");    
        
    /**
     * @brief Create and add a specie to the simulation.
     * @param[in] diffusivity Diffusion coefficient of the specie in the carrier medium in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in the carrier medium in g/m^3.
     * @param[in] initialConcentration Initial concentration of the specie in the carrier medium in g/m^3.
     * @return Pointer to created specie.
     */
    [[maybe_unused]] std::shared_ptr<Specie<T>> addSpecie(T diffusivity, T satConc, T initialConcentration);

    /**
     * @brief Remove specie from the simulator. If a mixture contains the specie, it is removed from the mixture as well.
     * A mixture consisting of a single specie is removed from the simulation.
     * @param[in] specie The specie to be removed.
     * @throws std::logic_error if the specie is not present in the simulation.
     */
    void removeSpecie(const std::shared_ptr<Specie<T>>& specie) override;
        
    /**
     * @brief Sets an instantaneous mixing model for the simulation.
     */
    void setInstantaneousMixingModel() override;

    /**
     * @brief Sets a diffusive mixing model for the simulation.
     */
    void setDiffusiveMixingModel() override;

    /**
     * @brief Get the global bounds of velocity magnitude values in the CFD simulators.
     * @return A tuple with the global bounds for velocity magnitude values <velMin, velMax>
     */
    std::pair<T,T> getGlobalConcentrationBounds(size_t adKey) const;

    /**
     * @brief Write the velocity field in .ppm image format for all cfdSimulators
     */
    void writeConcentrationPpm(size_t adKey, std::pair<T,T> bounds, int resolution=600) const;

    void simulate() override;

};

}   // namespace sim