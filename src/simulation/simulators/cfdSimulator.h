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

namespace mmft {

template<typename T>
class Scheme;

}

namespace sim {

/**
 * @brief Class to specify a module, which is a functional component in a network.
*/
template<typename T>
class CFDSimulator {
protected:
    int const id;                           ///< Id of the simulator.
    std::string name;                       ///< Name of the simulator.
    std::string vtkFolder = "./tmp/";       ///< Folder in which vtk files will be saved.
    std::string vtkFile = ".";              ///< File in which last file was saved.
    bool initialized = false;               ///< Is the simulator initialized.
    std::string stlFile;                    ///< The STL file of the CFD domain.

    std::shared_ptr<arch::Module<T>> cfdModule;                     ///< A pointer to the module, upon which this simulator acts.
    std::shared_ptr<arch::Network<T>> moduleNetwork;                ///< Fully connected graph as network for the initial approximation.
    std::unordered_map<int, arch::Opening<T>> moduleOpenings;       ///< Map of openings. <nodeId, arch::Opening>
    std::unordered_map<int, bool> groundNodes;                      ///< Map of nodes that communicate the pressure to the 1D solver. <nodeId, bool>
    std::shared_ptr<mmft::Scheme<T>> updateScheme;                  ///< The update scheme for Abstract-CFD coupling

    /**
     * @brief Constructor of a CFDSimulator, which acts as a base definition for other simulators.
     * @param[in] id Id of the simulator.
     * @param[in] name The name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] cfdModule Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] ResistanceModel The resistance model used for the simulation, necessary to set the initial condition.
     */
    CFDSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, 
                std::unordered_map<int, arch::Opening<T>> openings, ResistanceModel<T>* ResistanceModel);

    /**
     * @brief Constructor of a CFDSimulator, which acts as a base definition for other simulators.
     * @param[in] id Id of the simulator.
     * @param[in] name The name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] cfdModule Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] updateScheme Shared pointer to the update scheme for Abstract-CFD coupling.
     * @param[in] ResistanceModel The resistance model used for the simulation, necessary to set the initial condition.
     */
    CFDSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, 
                std::unordered_map<int, arch::Opening<T>> openings, std::shared_ptr<mmft::Scheme<T>> updateScheme, ResistanceModel<T>* ResistanceModel);


    /**
     * @brief Define and prepare the coupling of the NS lattice with the AD lattices.
    */
    virtual void executeCoupling() 
    {
        throw std::runtime_error("The function executeCoupling is undefined for this CFD simulator.");
    };

public:
    /**
     * @brief Get id of the simulator.
     * @returns id.
    */
    int getId() const;

    /**
     * @brief Get a shared ptr to the module, upon which this simulator acts.
     * @return Network of the fully connected graph.
    */
    std::shared_ptr<arch::Module<T>> getModule() const;

    /**
     * @brief Get the fully connected graph of this module, that is used for the initial approximation.
     * @return Network of the fully connected graph.
    */
    std::shared_ptr<arch::Network<T>> getNetwork() const;

    /**
     * @brief Get the ground nodes of the module.
     * @returns Ground nodes.
    */
    std::unordered_map<int, bool> getGroundNodes();

    /**
     * @brief Set the nodes of the module that communicate the pressure to the abstract solver.
     * @param[in] groundNodes Map of nodes.
     */
    void setGroundNodes(std::unordered_map<int, bool> groundNodes);

    /**
     * @brief Returns whether the module is initialized or not.
     * @returns Boolean for initialization.
    */
    bool getInitialized() const;

    /**
     * @brief Get the openings of the module.
     * @returns Module openings.
     */
    std::unordered_map<int, arch::Opening<T>> getOpenings() const;

    /**
     * @brief Set the initialized status for this module.
     * @param[in] initialization Boolean for initialization status.
    */
    void setInitialized(bool initialization);

    /**
     * @brief Set the update scheme for Abstract-CFD coupling for this simulator.
     */
    void setUpdateScheme(const std::shared_ptr<mmft::Scheme<T>>& updateScheme);

    /**
     * @brief Set the path, where vtk output from the simulator should be stored.
     * @param[in] vtkFolder A string containing the path to the vtk folder.
     */
    void setVtkFolder(std::string vtkFolder);

    /**
     * @brief Get the location of the last created vtk file.
     * @returns vtkFile.
     */
    std::string getVtkFile();

    /**
     * @brief Get the relaxation factor for pressure update values, alpha.
     * @returns alpha.
    */
    T getAlpha(int nodeId);

    /**
     * @brief Get the relaxation factor for flow rate update values, beta.
     * @returns beta.
    */
    T getBeta(int nodeId);

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

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    virtual bool hasConverged() const = 0;

    /** TODO:
     * 
     */
    virtual void storePressures(std::unordered_map<int, T> pressure) = 0;

    /**
     * @brief Store the abstract pressures at the nodes on the module boundary in the simulator.
     * @param[in] pressure Map of pressures and node ids.
     */
    virtual std::unordered_map<int, T> getPressures() const = 0;

    /**
     * @brief Store the abstract flow rates at the nodes on the module boundary in the simulator.
     * @param[in] flowRate Map of flow rates and node ids.
     */
    virtual void storeFlowRates(std::unordered_map<int, T> flowRate) = 0;

    /**
     * @brief Get the flow rates at the boundary nodes.
     * @returns Flow rates in m^3/s.
     */
    virtual std::unordered_map<int, T> getFlowRates() const = 0;

    /**
     * @brief Store the abstract concentrations at the nodes on the module boundary in the simulator.
     * @param[in] concentrations Map of concentrations and node ids.
     */
    virtual void storeConcentrations(std::unordered_map<int, std::unordered_map<int, T>> concentrations) 
    {
        throw std::runtime_error("The function storeConcentrations is undefined for this CFD simulator.");
    }

    /**
     * @brief Get the concentrations at the boundary nodes.
     * @returns Concentrations
     */
    virtual std::unordered_map<int, std::unordered_map<int, T>> getConcentrations() const 
    { 
        throw std::runtime_error("The function storeConcentrations is undefined for this CFD simulator.");
        return std::unordered_map<int, std::unordered_map<int, T>>(); 
    }

    /**
     * @brief Set the boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    virtual void setBoundaryValues(int iT) = 0;

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
     * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
     * @param[in] iT Iteration step.
    */
    virtual void storeCfdResults (int iT) 
    {
        throw std::runtime_error("The function storeCfdResults is undefined for this CFD simulator.");
    }

    /**
     * @brief Returns whether the AD lattices have converged or not.
     * @returns Boolean for AD convergence.
    */
    virtual bool hasAdConverged() const { return false; }

    friend void coupleNsAdLattices<T>(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators);

};

}   // namespace sim
