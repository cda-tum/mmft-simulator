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

    size_t maxIter = 100000;                    ///< Maximum number of iterations for the CFD solver.
    size_t resolution = 20;                     ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
    T charPhysLength = 1e-4;                    ///< Characteristic physical length (= width, usually).
    T charPhysVelocity = 1e-1;                  ///< Characteristic physical velocity (expected maximal velocity).
    T epsilon = 0.1;                            ///< Convergence criterion.
    T relaxationTime = 0.932;                   ///< Relaxation time (tau) for the OLB solver.

    std::shared_ptr<stl::Network> network_stl = nullptr;            ///< Network definition as in stl namespace
    std::unique_ptr<stl::NetworkSTL> stlNetwork = nullptr;          ///< STL shape of a network
    std::string fName = "";                                         ///< Location of the stored STL file
    std::set<std::shared_ptr<arch::Node<T>>> danglingNodes;         ///< Set of nodes that are dangling and should have a BC.
    std::set<size_t> idleNodes;                                     ///< Set of nodes that are idle. I.e., dangling but not assigned a BC.
    std::unordered_map<size_t, T> pressureBCs;                      ///< Map of pressure BCs at dangling nodes. <nodeId, pressure>
    std::unordered_map<size_t, T> flowRateBCs;                      ///< Map of flowrate BCs at dangling nodes. <nodeId, flowrate>
    int radialResolution = 25;                                      ///< The resolution of radial objects for the STL definition of the network.
    std::shared_ptr<arch::CfdModule<T>> cfdModule = nullptr;        ///< A pointer to the module, upon which this simulator acts.
    std::shared_ptr<lbmSimulator<T>> simulator = nullptr;           ///< The set of CFD simulator, that conducts the CFD simulations on <arch::Network>.
    bool writePpm = false;                                          ///< Whether to write ppm files for pressure and velocity fields.

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
     * @brief Updates the idle nodes from the dangling nodes
     */
    void updateIdleNodes();

    /**
     * @brief Updates the idle nodes from the given openings
     * @param[in] openings The map of openings that define the idle nodes
     */
    void updateIdleNodes(const std::unordered_map<size_t, arch::Opening<T>>& openings);

    /**
     * @brief Returns the position of the CFD domain.
     * @returns The position of the CFD domain as vector<T>
     */
    std::vector<T> getPosition() const;

    /**
     * @brief Returns the size of the CFD domain.
     * @returns The size of the CFD domain as vector<T>
     */
    std::vector<T> getSize() const;

    /**
     * @brief Returns the cfdModule pointer.
     * @returns The cfdModule pointer.
     */
    [[nodiscard]] inline std::shared_ptr<arch::CfdModule<T>> getCfdModule() const { return cfdModule; }

    /**
     * @brief Returns the openings of the CFD domain.
     * @returns The openings of the CFD domain as unordered_map<size_t, arch::Opening<T>>
     */
    std::unordered_map<size_t, arch::Opening<T>> getOpenings() const;

    /**
     * @brief Constructor of the hybrid continuous simulator object using the network as basis for the STL definition.
     * @param[in] platform The platform of the derived simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     * @param[in] radialResolution The resolution of radial objects in the STL definition of the network
     * @note This constructor constructs the simulation STL shape from a given network
     */
    CfdContinuous(Platform platform, std::shared_ptr<arch::Network<T>> network, int radialResolution=25);

    /**
     * @brief Constructor of the hybrid continuous simulator object using an stlFile and openings as basis for the STL definition.
     * @param[in] platform The platform of the derived simulator object
     * @param[in] position The position of the CFD domain
     * @param[in] size The size of the CFD domain
     * @param[in] stlFile The STL file that defines the CFD domain shape
     * @param[in] openings The map of openings that define the idle nodes
     * @note This constructor uses an stlFile and openings to define the CFD domain
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
    [[nodiscard]] inline std::shared_ptr<lbmSimulator<T>> getCFDSimulator() { return simulator; }

    /**
     * @brief Get injection
     * @param simulatorId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    [[nodiscard]] inline const lbmSimulator<T>* readCFDSimulator() const { return simulator.get(); }

    /** TODO:
     * 
     */
    bool getWritePpm() { return writePpm; }

    /** TODO:
     * 
     */
    virtual void setBoundaryConditions();

    inline void setSimulator(std::shared_ptr<lbmSimulator<T>> simulator) { this->simulator = simulator; }

    /**
     * @brief Get dangling nodes
     * @return Set of dangling nodes
     */
    [[nodiscard]] inline const std::set<std::shared_ptr<arch::Node<T>>>& getDanglingNodes() const { return danglingNodes; }

    /**
     * @brief Get idle nodes
     * @return Set of idle node ids
     */
    [[nodiscard]] inline const std::set<size_t>& getIdleNodes() const { return idleNodes; }

public:

    /**
     * @brief Constructor of the CFD continuous simulator object using the network as basis for the STL definition.
     * @param[in] network Pointer to the network object, in which the simulation takes place
     * @param[in] radialResolution The resolution of radial objects in the STL definition of the network
     * @note This constructor constructs the simulation STL shape from a given network
     */
    CfdContinuous(std::shared_ptr<arch::Network<T>> network, int radialResolution=25);

    /**
     * @brief Constructor of the CFD continuous simulator object using an stlFile and openings as basis for the STL definition.
     * @param[in] position The position of the CFD domain
     * @param[in] size The size of the CFD domain
     * @param[in] stlFile The STL file that defines the CFD domain shape
     * @param[in] openings The map of openings that define the idle nodes
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

    /**
     * @note Adding a BC is only possible for domains that have been constructed using a network.
     * @throws logic_error if adding a BC is not possible.
     */
    void addPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure);

    /**
     * @note Adding a BC is only possible for domains that have been constructed using a network.
     * @throws logic_error if adding a BC is not possible.
     */
    void addFlowRateBC(std::shared_ptr<arch::Node<T>>, T velocity);

    /**
     * @brief Sets a pressure boundary condition at a given node.
     * @param[in] node The node at which the pressure BC is set.
     * @param[in] pressure The pressure value at the boundary condition.
     * @throws logic_error if setting the BC value is not possible.
     */
    void setPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure);

    /**
     * @brief Sets a flowrate boundary condition at a given node.
     * @param[in] node The node at which the velocity BC is set.
     * @param[in] flowRate The fowrate value at the boundary condition.
     * @throws logic_error if setting the BC value is not possible.
     */
    void setFlowRateBC(std::shared_ptr<arch::Node<T>>, T flowrate);

    /**
     * @note Removing a BC is only possible for domains that have been constructed using a network. 
     * If it was constructed using an stlFile and openings, this BC will default to a pressure BC with 0 Pa.
     */
    void removePressureBC(std::shared_ptr<arch::Node<T>> node);

    /** TODO:
     * @note Removing a BC is only possible for domains that have been constructed using a network. 
     * If it was constructed using an stlFile and openings, this BC will default to a pressure BC with 0 Pa.
     */
    void removeFlowRateBC(std::shared_ptr<arch::Node<T>> node);

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
     * @brief Returns the maximum number of iterations for the CFD solver.
     * @returns The maximum number of iterations for the CFD solver.
     */
    [[nodiscard]] inline size_t getMaxIter() const { return maxIter; }

    /**
     * @brief Sets the maximum number of iterations for the CFD solver.
     * @param[in] maxIter The maximum number of iterations for the CFD solver.
     */
    inline void setMaxIter(size_t maxIter) { this->maxIter = maxIter; }

    /**
     * @brief Returns the resolution of the CFD domain.
     * @returns The resolution of the CFD domain.
     */
    [[nodiscard]] inline size_t getResolution() const { return resolution; }

    /**
     * @brief Sets the resolution of the CFD domain.
     * @param[in] resolution The resolution of the CFD domain.
     */
    inline void setResolution(size_t resolution) { this->resolution = resolution; }

    /**
     * @brief Returns the epsilon for the simulator.
     * @returns The epsilon for the simulator.
     */
    [[nodiscard]] inline T getEpsilon() const { return epsilon; }

    /**
     * @brief Sets the epsilon for the simulator.
     * @param[in] epsilon The epsilon for the simulator.
     */
    inline void setEpsilon(T epsilon) { this->epsilon = epsilon; }

    /**
     * @brief Returns the relaxation time for the simulator.
     * @returns The relaxation time for the simulator.
     */
    [[nodiscard]] inline T getTau() const { return relaxationTime; }

    /**
     * @brief Sets the relaxation time for the simulator.
     * @param[in] relaxationTime The relaxation time for the simulator.
     */
    inline void setTau(T relaxationTime) { this->relaxationTime = relaxationTime; }

    /**
     * @brief Returns the characteristic physical length for the simulation.
     * @returns The characteristic physical length
     */
    [[nodiscard]] inline T getCharPhysLength() const { return charPhysLength; }

    /**
     * @brief Sets the characteristic physical length for the simulation.
     * @param[in] charPhysLength The characteristic physical length
     */
    inline void setCharPhysLength(T charPhysLength) { this->charPhysLength = charPhysLength; }

    /**
     * @brief Returns the characteristic physical velocity for the simulation.
     * @returns The characteristic physical velocity
     */
    [[nodiscard]] inline T getCharPhysVelocity() const { return charPhysVelocity; }

    /**
     * @brief Sets the characteristic physical velocity for the simulation.
     * @param[in] charPhysVelocity The characteristic physical velocity
     */
    inline void setCharPhysVelocity(T charPhysVelocity) { this->charPhysVelocity = charPhysVelocity; }

    /**
     * @brief Sets whether to write ppm files for pressure and velocity fields.
     * @param[in] writePpm Whether to write ppm files for pressure and velocity fields
     */
    inline void setWritePpm(bool writePpm) { this->writePpm = writePpm; }

    /**
     * @brief Returns whether ppm files for pressure and velocity fields are written.
     * @returns Whether ppm files for pressure and velocity fields are written
     */
    [[nodiscard]] inline bool isWritePpm() const { return writePpm; }

    /**
     * @brief Returns the current global characteristic length for the simulation.
     * @returns The global characteristic length
     */
    [[nodiscard]] inline T getCharacteristicLength() { return charPhysLength; }

    /**
     * @brief Returns the current global characteristic velocity for the simulation.
     * @returns The global characteristic velocity
     */
    [[nodiscard]] inline T getCharacteristicVelocity() { return charPhysVelocity; }

    /**
     * @brief Get the global bounds of pressure values in the CFD simulators.
     * @return A tuple with the global bounds for pressure values <pMin, pMax>
     */
    std::pair<T,T> getGlobalPressureBounds();
    
    /**
     * @brief Get the global bounds of velocity magnitude values in the CFD simulators.
     * @return A tuple with the global bounds for velocity magnitude values <velMin, velMax>
     */
    std::pair<T,T> getGlobalVelocityBounds();

    /**
     * @brief Write the pressure field in .ppm image format for all cfdSimulators
     */
    void writePressurePpm(std::pair<T,T> bounds, int resolution=600);

    /**
     * @brief Write the velocity field in .ppm image format for all cfdSimulators
     */
    void writeVelocityPpm(std::pair<T,T> bounds, int resolution=600);

    void simulate() override;
};

}   // namespace sim
