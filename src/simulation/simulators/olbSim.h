/**
 * @file olbContinuous.h
 */

#pragma once

#define M_PI 3.14159265358979323846

#include <filesystem>
#include <memory>
#include <math.h>
#include <iostream>
#include <vector>
#include <unordered_map>

#if DIMENSION == 2
#include <olb2D.h>
#include <olb2D.hh>
#endif

#include <olb3D.h>
#include <olb3D.hh>

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

/**
 * @brief Struct that defines an opening, which is an in-/outlet of the CFD domain.
*/
template<typename T>
struct lbmParameters {
    T charPhysLength;
    T charPhysVelocity;
    T resolution;
    T epsilon;
    T tau;

    /**
     * @brief Constructor of struct containing lbmParameters.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
    */
    lbmParameters(T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T tau_) :
        charPhysLength(charPhysLength_), 
        charPhysVelocity(charPhysVelocity_), 
        resolution(resolution_), 
        epsilon(epsilon_),
        tau(tau_) { }
};

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class olbSim : public CFDSimulator<T> {

protected:
    int stlMargin = 1;
    int step = 0;                           ///< Iteration step of this module.
    int stepIter = 1000;                    ///< Number of iterations for the value tracer.
    int maxIter = 1e7;                      ///< Maximum total iterations.
    std::unordered_map<int, T> pressures;   ///< Vector of pressure values at module nodes.
    std::unordered_map<int, T> flowRates;   ///< Vector of flowRate values at module nodes.
    
    bool isConverged = false;               ///< Has the module converged?

    T charPhysLength;                       ///< Characteristic physical length (= width, usually).
    T charPhysVelocity;                     ///< Characteristic physical velocity (expected maximal velocity).

    T resolution;                           ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
    T epsilon;                              ///< Convergence criterion.
    T relaxationTime;                       ///< Relaxation time (tau) for the OLB solver.

    std::shared_ptr<olb::STLreader<T>> stlReader;
    std::unique_ptr<olb::util::ValueTracer<T>> converge;            ///< Value tracer to track convergence.=

    void setOutputDir() {
        if (!std::filesystem::is_directory(this->vtkFolder) || !std::filesystem::exists(this->vtkFolder)) {
        std::filesystem::create_directory(this->vtkFolder);
        }
        olb::singleton::directories().setOutputDir( this->vtkFolder+"/" );  // set output directory     
    }

    void initNsConvergeTracker() {
        converge = std::make_unique<olb::util::ValueTracer<T>> (stepIter, epsilon);
    }

    virtual void initValueContainers() = 0;

    virtual void collideAndStream() = 0;

    /**
     * @brief Constructor of an lbm module.
     * @param[in] id Id of the module.
     * @param[in] name Name of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] nodes Map of nodes that are on the boundary of the module.
     * @param[in] openings Map of the in-/outlets of the module.
     * @param[in] stlFile STL file that describes the geometry of the CFD domain.
     * @param[in] charPhysLength Characteristic physical length of the geometry of the module in _m_.
     * @param[in] charPhysVelocity Characteristic physical velocity of the flow in the module in _m/s_.
     * @param[in] alpha Relaxation factor for the iterative updates between the 1D and CFD solvers.
     * @param[in] resolution Resolution of the CFD mesh in gridpoints per charPhysLength.
     * @param[in] epsilon Convergence criterion for the pressure values at nodes on the boundary of the module.
     * @param[in] relaxationTime Relaxation time tau for the LBM solver.
    */
    olbSim(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, 
        ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_=0.932) : 
        CFDSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_), 
        charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), epsilon(epsilon_), relaxationTime(relaxationTime_) { };

public:

    /**
     * @brief Store the abstract pressures at the nodes on the module boundary in the simulator.
     * @param[in] pressure Map of pressures and node ids.
     */
    void storePressures(std::unordered_map<int, T> pressure_) {
        this->pressures = pressure_;
    }

    /**
     * @brief Store the abstract flow rates at the nodes on the module boundary in the simulator.
     * @param[in] flowRate Map of flow rates and node ids.
     */
    void storeFlowRates(std::unordered_map<int, T> flowRate_) {
        this->flowRates = flowRate_;
    }

    /**
     * @brief Get the pressures at the boundary nodes.
     * @returns Pressures in Pa.
     */
    std::unordered_map<int, T> getPressures() const {
        return pressures;
    };

    /**
     * @brief Get the flow rates at the boundary nodes.
     * @returns Flow rates in m^3/s.
     */
    std::unordered_map<int, T> getFlowRates() const {
        return flowRates;
    };

    /**
     * @brief Get the number of iterations for the value tracer.
     * @returns Number of iterations for the value tracer.
    */
    int getStepIter() const {
        return stepIter;
    };

    /**
     * @brief Returns whether the module has converged or not.
     * @returns Boolean for module convergence.
    */
    bool hasConverged() const {
        return converge->hasConverged();
    };

    /**
     * @brief Get the characteristic physical length.
     * @returns Characteristic physical length.
    */
    T getCharPhysLength() const { 
        return charPhysLength; 
    };

    /**
     * @brief Get the characteristic physical velocity.
     * @returns Characteristic physical velocity.
    */
    T getCharPhysVelocity() const { 
        return charPhysVelocity; 
    };

    /**
     * @brief Get the resolution.
     * @returns resolution.
    */
    T getResolution() const { 
        return resolution; 
    };

    /**
     * @brief Get the convergence criterion.
     * @returns epsilon.
    */
    T getEpsilon() const { 
        return epsilon; 
    };

};

}   // namespace arch
