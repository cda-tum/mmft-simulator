/**
 * @file HybridContinuous.h
 */

#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
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
class Node;

template<typename T>
class Opening;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

template<typename T>
class lbmMixingSimulator;

template<typename T>
class Specie;

/**
 * @brief Class that conducts a CFD continuous simulation
 */
template<typename T>
class CfdMixing : public CfdContinuous<T>, public ConcentrationSemantics<T> {
private:

    T adRelaxationTime = 0.932;                 ///< Relaxation time (tau) for the advection-diffusion LBM solver.
    std::unordered_map<size_t, std::unordered_map<size_t, T>> concentrationBCs;     ///< Map of concentration BCs at dangling nodes. <nodeId, <specieId, concentration>>
    std::shared_ptr<lbmMixingSimulator<T>> mixingSimulator = nullptr;           ///< The set of CFD simulator, that conducts the CFD simulations on <arch::Network>.

protected:

    /** TODO:
     * 
     */
    void setBoundaryConditions() override;

    void assertInitialized() const override;

public:

    /**
     * @brief Constructor of the CFD mixing simulator object using the network as basis for the STL definition.
     * @param[in] network Pointer to the network object, in which the simulation takes place
     * @param[in] radialResolution The resolution of radial objects in the STL definition of the network
     * @note This constructor constructs the simulation STL shape from a given network
     */
    CfdMixing(std::shared_ptr<arch::Network<T>> network, int radialResolution=25);

    /**
     * @brief Constructor of the CFD mixing simulator object using an stlFile and openings as basis for the STL definition.
     * @param[in] position The position of the CFD domain
     * @param[in] size The size of the CFD domain
     * @param[in] stlFile The STL file that defines the CFD domain shape
     * @param[in] openings The map of openings that define the idle nodes
     */
    CfdMixing(std::vector<T> position,
                std::vector<T> size,
                std::string stlFile,
                std::unordered_map<size_t, arch::Opening<T>> openings);

    /**
     * @note Adding a BC is only possible for domains that have been constructed using a network.
     * @throws logic_error if adding a BC is not possible.
     */
    void addConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie,  T concentration);

    /**
     * @brief Sets a concentration boundary condition at a given node.
     * @param[in] node The node at which the pressure BC is set.
     * @param[in] concentration The pressure value at the boundary condition.
     * @throws logic_error if setting the BC value is not possible.
     */
    void setConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie, T pressure);

    /**
     * @note Removing a BC is only possible for domains that have been constructed using a network. 
     */
    void removeConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie);

    /**
     * @brief Get the global bounds of concentration values in the CFD simulators.
     * @return A tuple with the global bounds for concentration values <concMin, concMax>
     */
    std::pair<T,T> getGlobalConcentrationBounds(const std::shared_ptr<Specie<T>>& specie);

    /**
     * @brief Write the concentration field in .ppm image format for all cfdSimulators
     */
    void writeConcentrationPpm(const std::shared_ptr<Specie<T>>& specie, std::pair<T,T> bounds, int resolution=600);

    void simulate() override;
};

}   // namespace sim
