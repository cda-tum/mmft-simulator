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

#include <architecture/pNetwork.h>
#include <stlHandler/NetworkSTL.h>

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

template<typename T>
class RectangularChannel;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

template<typename T>
class lbmSimulator;

/**
 * @brief Class that conducts a CFD continuous simulation
 */
template<typename T>
class CfdContinuous : public Simulation<T> {
private:
    std::shared_ptr<stl::Network> network_stl = nullptr;            ///< Network definition as in stl namespace
    std::unique_ptr<stl::NetworkSTL> stlNetwork = nullptr;          ///< STL shape of a network
    std::string fName = "";                                         ///< Location of the stored STL file
    std::set<std::shared_ptr<arch::Node<T>>> danglingNodes;         ///< Set of nodes that are dangling and should have a BC.
    std::set<size_t> idleNodes;                                     ///< Set of nodes that are idle. I.e., dangling but not assigned a BC.
    int radialResolution = 25;                                      ///< The resolution of radial objects for the STL definition of the network.
    std::shared_ptr<arch::CfdModule<T>> cfdModule = nullptr;        ///< A pointer to the module, upon which this simulator acts.
    std::shared_ptr<CFDSimulator<T>> cfdSimulator = nullptr;        ///< The set of CFD simulator, that conducts the CFD simulations on <arch::Network>.
    bool writePpm = true;

protected:

    void assertInitialized() const override;

    void initialize() override;

    void saveState() override;       

    /** 
     * @brief Updates the network_stl object for the currently set network definition
     */
    void updateNetworkSTL();

    /**
     * @brief Generates the stl definition from network_stl and stores it in stlNetwork.
     */
    void generateSTL();

    /**
     * @brief Writes the STL definition stored in stlNetwork into the location in fName.
     */
    void updateSTL();

    /**
     * @brief Constructor of the hybrid continuous simulator object
     * @param[in] platform The platform of the derived simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     * @param[in] radialResolution The resolution of radial objects in the STL definition of the network
     * @note This constructor constructs the simulation STL shape from a given network
     */
    CfdContinuous(Platform platform, std::shared_ptr<arch::Network<T>> network, int radialResolution=25);

    /** TODO:
     * 
     */
    CfdContinuous(Platform platform,
                  std::vector<T> position,
                  std::vector<T> size,
                  std::string stlFile,
                  std::unordered_map<size_t, arch::Opening<T>> openings);

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    [[nodiscard]] inline std::shared_ptr<CFDSimulator<T>>& getCFDSimulator() { return cfdSimulator; }

    /**
     * @brief Get injection
     * @param simulatorId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    [[nodiscard]] inline const CFDSimulator<T>* readCFDSimulator() const { return cfdSimulator.get(); }

    /** TODO:
     * 
     */
    bool getWritePpm() { return writePpm; }

public:

    /**
     * @brief Constructor of the CFD continuous simulator object using the network as basis for the STL definition.
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    CfdContinuous(std::shared_ptr<arch::Network<T>> network, int radialResolution=25);

    /** TODO:
     * 
     */
    CfdContinuous(std::vector<T> position,
                  std::vector<T> size,
                  std::string stlFile,
                  std::unordered_map<size_t, arch::Opening<T>> openings);

    /**
     * @brief Set the network for which the simulation should be conducted. Invoking this function will 
     * automatically update the STL domain of the simulation.
     * @param[in] network Network on which the simulation will be conducted.
     * @note Setting a network is only possible for simulation objects that were created using a network.
     */
    void setNetwork(std::shared_ptr<arch::Network<T>> network) override;

    /** TODO:
     * @note Adding a BC is only possible for domains that have been constructed using a network.
     * @throws logic_error if the simulation object was constructed using an stlFile and openings.
     */
    void addPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure);

    /** TODO:
     * 
     */
    void setPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure);

    /** TODO:
     * @note Adding a BC is only possible for domains that have been constructed using a network.
     * @throws logic_error if the simulation object was constructed using an stlFile and openings.
     */
    void addVelocityBC(std::shared_ptr<arch::Node<T>>, T velocity);

    /** TODO:
     * 
     */
    void setVelocityBC(std::shared_ptr<arch::Node<T>>, T velocity);

    /**
     * @note Removing a BC is only possible for domains that have been constructed using a network. 
     * If it was constructed using an stlFile and openings, this BC will default to a pressure BC with 0 Pa.
     */
    void removePressureBC(std::shared_ptr<arch::Node<T>> node);

    /** TODO:
     * @note Removing a BC is only possible for domains that have been constructed using a network. 
     * If it was constructed using an stlFile and openings, this BC will default to a pressure BC with 0 Pa.
     */
    void removeVelocityBC(std::shared_ptr<arch::Node<T>> node);

    /** TODO:
     * 
     */
    void generateSTL();

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
};

}   // namespace sim
