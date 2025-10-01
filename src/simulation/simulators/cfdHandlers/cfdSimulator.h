/**
 * @file cfdSimulator.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Module;

template<typename T>
class Network;

template<typename T>
struct Opening;

}

namespace test::definitions {
template<typename T>
class GlobalTest;
}

namespace mmft {

template<typename T>
class Scheme;

}

namespace nodal {

template<typename T>
class NodalAnalysis;

}

namespace sim {

/**
 * @brief Class to specify a module, which is a functional component in a network.
*/
template<typename T>
class CFDSimulator {
protected:
    inline static size_t simulatorCounter = 0;  ///< Global counter for amount of created CFD simulator objects.
    size_t const id;                            ///< Id of the simulator.
    std::string name;                           ///< Name of the simulator.
    std::string vtkFolder = "./tmp/";           ///< Folder in which vtk files will be saved.
    std::string vtkFile = ".";                  ///< File in which last file was saved.
    bool initialized = false;                   ///< Is the simulator initialized.    

    std::shared_ptr<arch::CfdModule<T>> cfdModule = nullptr;        ///< A pointer to the module, upon which this simulator acts.
    std::unordered_map<size_t, bool> groundNodes;                   ///< Map of nodes that communicate the pressure to the 1D solver. <nodeId, bool>
    mmft::Scheme<T>* updateScheme = nullptr;                        ///< The update scheme for Abstract-CFD coupling

    /**
     * @brief A static member function that resets the CFD simulator object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetSimulatorCounter() { simulatorCounter = 0; }

    /**
     * @brief A static member function that returns the amount of CFD simulator objects that have been created.
     * Is used in (friend) HybridContinuous<T>::add(Lbm)Simulator() to create a LBM simulators object and add it to the simulation.
     * @returns The number of created CFD simulator objects: simulatorCounter.
     */
    [[nodiscard]] static size_t getSimulatorCounter() { return simulatorCounter; }

    /**
     * @brief Constructor of a CFDSimulator, which acts as a base definition for other simulators.
     * @param[in] id Id of the simulator.
     * @param[in] name The name of the simulator.
     * @param[in] cfdModule Shared pointer to the module on which this solver acts.
     */
    CFDSimulator(int id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule);

    /**
     * @brief Constructor of a CFDSimulator, which acts as a base definition for other simulators.
     * @param[in] id Id of the simulator.
     * @param[in] name The name of the simulator.
     * @param[in] cfdModule Shared pointer to the module on which this solver acts.
     * @param[in] updateScheme Shared pointer to the update scheme for Abstract-CFD coupling.
     * @param[in] ResistanceModel The resistance model used for the simulation, necessary to set the initial condition.
     */
    CFDSimulator(int id, std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule, std::shared_ptr<mmft::Scheme<T>> updateScheme);

    /**
     * @brief Define and prepare the coupling of the NS lattice with the AD lattices.
    */
    virtual void executeCoupling() 
    {
        throw std::runtime_error("The function executeCoupling is undefined for this CFD simulator.");
    };

    void initialize(const ResistanceModel<T>* resistanceModel);

    /**
     * @brief Initialize an instance of the LBM solver for this simulator.
     * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
     * @param[in] density Density of the simulated fluid in _kg / m^3_.
    */
    virtual void lbmInit(T dynViscosity, T density) = 0;

    /**
     * @brief Conducts the collide and stream operations of the lattice.
    */
    virtual void solve() = 0;

    /** TODO: Miscellaneous */
    virtual void storePressures(std::unordered_map<size_t, T> pressure) = 0;

    /**
     * @brief Store the abstract pressures at the nodes on the module boundary in the simulator.
     * @param[in] pressure Map of pressures and node ids.
     */
    virtual const std::unordered_map<size_t, T>& getPressures() const = 0;

    /**
     * @brief Store the abstract flow rates at the nodes on the module boundary in the simulator.
     * @param[in] flowRate Map of flow rates and node ids.
     */
    virtual void storeFlowRates(std::unordered_map<size_t, T> flowRate) = 0;

    /**
     * @brief Get the flow rates at the boundary nodes.
     * @returns Flow rates in m^3/s.
     */
    virtual const std::unordered_map<size_t, T>& getFlowRates() const = 0;

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    virtual bool hasConverged() const = 0;

    /**
     * @brief Set the nodes of the module that communicate the pressure to the abstract solver.
     * @param[in] groundNodes Map of nodes.
     */
    inline void setGroundNodes(std::unordered_map<size_t, bool> groundNodes) { this->groundNodes = groundNodes; }

    /**
     * @brief Prepare the LBM geometry of this simulator.
    */
    virtual void prepareGeometry() 
    {
        throw std::runtime_error("The function prepareGeomtry is undefined for this CFD simulator.");
    }

    /**
     * @brief Prepare the LBM lattice on the LBM geometry.
    */
    virtual void prepareLattice() 
    {
        throw std::runtime_error("The function prepareLattice is undefined for this CFD simulator.");
    }

    /**
     * @brief Conducts the collide and stream operations of the NS lattice.
    */
    virtual void nsSolve() 
    {
        throw std::runtime_error("The function nsSolve is undefined for this CFD simulator.");
    }

    /**
     * @brief Conducts the collide and stream operations of the AD lattice(s).
    */
    virtual void adSolve() 
    {
        throw std::runtime_error("The function adSolve is undefined for this CFD simulator.");
    }

    /**
     * @brief Returns whether the AD lattices have converged or not.
     * @returns Boolean for AD convergence.
    */
    inline virtual bool hasAdConverged() const { return false; }

    /**
     * @brief Set the update scheme for Abstract-CFD coupling for this simulator.
     */
    inline void setUpdateScheme(mmft::Scheme<T>* updateScheme) { this->updateScheme = updateScheme; }

public:
    
    virtual ~CFDSimulator() = default;

    /**
     * @brief Get id of the simulator.
     * @returns id.
    */
    [[nodiscard]] inline int getId() const { return id; }

    /**
     * @brief Get a shared ptr to the module, upon which this simulator acts.
     * @return Network of the fully connected graph.
    */
    [[nodiscard]] inline std::shared_ptr<arch::CfdModule<T>> getModule() const { return cfdModule; }

    /**
     * @brief Get the flow direction at a node.
     * @param[in] key The id of the node for which the flow direction is requested.
     * @returns The flow direction at the node: -1 for inflow, 1 for outflow, 0 for no flow.
     */
    virtual int getFlowDirection(size_t key) = 0;

    /**
     * @brief Get the ground nodes of the module.
     * @returns Ground nodes.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, bool>& getGroundNodes() { return groundNodes; }

    /**
     * @brief Returns whether the module is initialized or not.
     * @returns Boolean for initialization.
    */
    [[nodiscard]] inline bool getInitialized() const { return initialized; }

    /**
     * @brief Set the initialized status for this module.
     * @param[in] initialization Boolean for initialization status.
    */
    inline void setInitialized(bool initialization) { this->initialized = initialization; }

    /**
     * @brief Set the path, where vtk output from the simulator should be stored.
     * @param[in] vtkFolder A string containing the path to the vtk folder.
     */
    inline void setVtkFolder(std::string vtkFolder) { this->vtkFolder = vtkFolder; }

    /**
     * @brief Get the location of the last created vtk file.
     * @returns vtkFile.
     */
    [[nodiscard]] inline const std::string& getVtkFile() const { return this->vtkFile; }

    /**
     * @brief Get the relaxation factor for pressure update values, alpha.
     * @returns alpha.
    */
    [[nodiscard]] inline T getAlpha(size_t nodeId) const { return updateScheme->getAlpha(nodeId); }

    /**
     * @brief Get the relaxation factor for flow rate update values, beta.
     * @returns beta.
    */
    [[nodiscard]] inline T getBeta(size_t nodeId) const {return updateScheme->getBeta(nodeId);}

    /**
     * @brief Store the abstract concentrations at the nodes on the module boundary in the simulator.
     * @param[in] concentrations Map of concentrations and node ids.
     */
    virtual void storeConcentrations(std::unordered_map<size_t, std::unordered_map<size_t, T>> concentrations) 
    {
        throw std::runtime_error("The function storeConcentrations is undefined for this CFD simulator.");
    }

    /**
     * @brief Get the concentrations at the boundary nodes.
     * @returns Concentrations
     */
    virtual std::unordered_map<size_t, std::unordered_map<size_t, T>> getConcentrations() const 
    { 
        throw std::runtime_error("The function storeConcentrations is undefined for this CFD simulator.");
        return std::unordered_map<size_t, std::unordered_map<size_t, T>>(); 
    }

    /**
     * @brief Set the boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    virtual void setBoundaryValues(int iT) = 0;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    virtual void writeVTK (int iT) 
    {
        throw std::runtime_error("The function writeVTK is undefined for this CFD simulator.");
    }
    
    /**
     * @brief Write the .ppm image file with the pressure results of the CFD simulation to file system.
     * @param[in] min Minimal bound for colormap.
     * @param[in] max Maximal bound for colormap.
     * @param[in] imgResolution Resolution of the .ppm image.
    */
    virtual void writePressurePpm (T min, T max, int imgResolution=600)
    {
        throw std::runtime_error("The function writePressurePpm is undefined for this CFD simulator.");
    }

    /**
     * @brief Write the .ppm image file with the velocity results of the CFD simulation to file system.
     * @param[in] min Minimal bound for colormap.
     * @param[in] max Maximal bound for colormap.
     * @param[in] imgResolution Resolution of the .ppm image.
    */
    virtual void writeVelocityPpm (T min, T max, int imgResolution=600)
    {
        throw std::runtime_error("The function writeVelocityPpm is undefined for this CFD simulator.");
    }

    /**
     * @brief Write the .ppm image file with the concentration results of the CFD simulation to file system.
     * @param[in] speciesId The id of the species for which the concentration should be written.
     * @param[in] min Minimal bound for colormap.
     * @param[in] max Maximal bound for colormap.
     * @param[in] imgResolution Resolution of the .ppm image.
    */
    virtual void writeConcentrationPpm (size_t speciesId, T min, T max, int imgResolution=600)
    {
        throw std::runtime_error("The function writeConcentrationPpm is undefined for this CFD simulator.");
    }


    /**
     * @brief Returns the local pressure bounds of this cfdSimulator 
     * @returns A tuple with the pressure bounds <pMin, pMax>
     */
    virtual std::tuple<T, T> getPressureBounds()
    {
        throw std::runtime_error("The function getPressureBounds is undefined for this CFD simulator.");
    }

    /**
     * @brief Returns the local velocity bounds of this cfdSimulator 
     * @returns A tuple with the velocity bounds <pMin, pMax>
     */
    virtual std::tuple<T, T> getVelocityBounds()
    {
        throw std::runtime_error("The function getVelocityBounds is undefined for this CFD simulator.");
    }

    /**
     * @brief Returns the local velocity bounds of this cfdSimulator 
     * @param[in] specieId The id of the species for which the concentration bounds should be returned.
     * @returns A tuple with the velocity bounds <pMin, pMax>
     */
    virtual std::tuple<T, T> getConcentrationBounds(size_t specieId)
    {
        throw std::runtime_error("The function getConcentrationBounds is undefined for this CFD simulator.");
    }

    /**
     * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
     * @param[in] iT Iteration step.
    */
    virtual void storeCfdResults (int iT) 
    {
        throw std::runtime_error("The function storeCfdResults is undefined for this CFD simulator.");
    }

    friend bool conductCFDSimulation<T>(const std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>>& cfdSimulators);
    friend void coupleNsAdLattices<T>(const std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>>& cfdSimulators);
    friend bool conductADSimulation<T>(const std::unordered_map<int, std::shared_ptr<CFDSimulator<T>>>& cfdSimulators);
    friend class HybridContinuous<T>;
    friend class HybridMixing<T>;
    friend class nodal::NodalAnalysis<T>;
    friend class test::definitions::GlobalTest<T>;

};

}   // namespace sim
