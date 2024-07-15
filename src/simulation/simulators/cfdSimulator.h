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

namespace sim {

/**
 * @brief Class to specify a module, which is a functional component in a network.
*/
template<typename T>
class CFDSimulator {
protected:
    int const id;                   ///< Id of the simulator.

    std::string name;                       ///< Name of the module.
    std::string vtkFolder = "./tmp/";       ///< Folder in which vtk files will be saved.
    bool initialized = false;               ///< Is the module initialized?
    std::string stlFile;                    ///< The STL file of the CFD domain.

    std::shared_ptr<arch::Module<T>> cfdModule;
    std::shared_ptr<arch::Network<T>> moduleNetwork;                      ///< Fully connected graph as network for the initial approximation.
    std::unordered_map<int, arch::Opening<T>> moduleOpenings;             ///< Map of openings.
    std::unordered_map<int, bool> groundNodes;                      ///< Map of nodes that communicate the pressure to the 1D solver.

    T alpha;                                ///< Relaxation factor for convergence between 1D and CFD simulation.

public:

    CFDSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, arch::Opening<T>> openings, T alpha, ResistanceModel<T>* ResistanceModel);

    /**
     * @brief Get id of the simulator.
     * @returns id.
    */
    int getId() const;

    /**
     * @brief Get the fully connected graph of this module, that is used for the initial approximation.
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

    void setVtkFolder(std::string vtkFolder_);

    /**
     * @brief Set the relaxation factor alpha.
    */
    void setAlpha(T alpha);

    /**
     * @brief Get the relaxation factor alpha.
     * @returns alpha.
    */
    T getAlpha();

    // fully virtual functions

    virtual void lbmInit(T dynViscosity, T density) = 0;

    virtual void solve() = 0;

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    virtual bool hasConverged() const = 0;

    virtual void setPressures(std::unordered_map<int, T> pressure) = 0;

    virtual std::unordered_map<int, T> getPressures() const = 0;

    virtual void setFlowRates(std::unordered_map<int, T> flowRate) = 0;

    virtual std::unordered_map<int, T> getFlowRates() const = 0;

    virtual void setBoundaryValues(int iT) = 0;

    // virtual functions
    
    virtual void prepareGeometry() {}

    virtual void prepareLattice() {}

    virtual void writeVTK (int iT) {}; 
    
    virtual void getResults (int iT) {}; 

};

}   // namespace sim
