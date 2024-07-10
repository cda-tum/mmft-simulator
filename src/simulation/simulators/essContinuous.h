/**
 * @file essContinuous.h
*/

#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

#include "Module.h"
#include "ModuleOpening.h"
#include "Node.h"
#include "Channel.h"

#include <ESSLbmSolver.h>

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
     * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
    */
    template<typename T>
    class essLbmSimulator : public CFDSimulator<T> {
        private:
            int step = 0;                           ///< Iteration step of this module.
            int stepIter = 1000;                    ///< Number of iterations for the value tracer.
            int maxIter = 1e7;                      ///< Maximum total iterations.
            int theta = 10;                         ///< Number of OLB iterations per communication iteration.

            bool isConverged = false;               ///< Has the module converged?

            T charPhysLength;                       ///< Characteristic physical length (= width, usually).
            T charPhysVelocity;                     ///< Characteristic physical velocity (expected maximal velocity).
            T alpha;                                ///< Relaxation factor.
            T resolution;                           ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
            T epsilon;                              ///< Convergence criterion.
            T relaxationTime;

            std::unordered_map<int, T> flowRates;              ///< Map of fluxes at module nodes.
            std::unordered_map<int, T> pressures;       ///< Map of mean pressure values at module nodes.

            std::shared_ptr<ess::lbmSolver> solver_;

        public:
            /**
             * @brief Constructor of an lbm module.
             * @param[in] id Id of the module.
             * @param[in] name Name of the module.
             * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
             * @param[in] size Size of the module in _m_.
             * @param[in] nodes Map of nodes that are on the boundary of the module.
             * @param[in] openings Map of the in-/outlets of the module.
            */
            essLbmSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule,  std::unordered_map<int, arch::Opening<T>> openings_,
                            ResistanceModel<T>* resistanceModel, T charPhysLength_, T charPhysVelocity_, T alpha, T resolution_, T epsilon_, T relaxationTime_);
            /**
             * @brief Initialize an instance of the LBM solver for this module.
             * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
             * @param[in] density Density of the simulated fluid in _kg / m^3_.
            */
            void lbmInit(T dynViscosity, T density) override;

            /**
             * @brief Set the boundary values on the lattice at the module nodes.
             * @param[in] iT Iteration step.
            */
            void setBoundaryValues(int iT);

            /**
             * @brief Conducts the collide and stream operations of the lattice.
            */
            void solve() override;

            /**
             * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
             * @param[in] iT Iteration step.
            */
            void storeCfdResults();

            /**
             * @brief Set the pressures at the nodes on the module boundary.
             * @param[in] pressure Map of pressures and node ids.
             */
            void setPressures(std::unordered_map<int, T> pressure) override;

            /**
             * @brief Set the flow rates at the nodes on the module boundary.
             * @param[in] flowRate Map of flow rates and node ids.
             */
            void setFlowRates(std::unordered_map<int, T> flowRate) override;

            /**
             * @brief Get the pressures at the boundary nodes.
             * @returns Pressures in Pa.
             */
            std::unordered_map<int, T> getPressures() const override;

            /**
             * @brief Get the flow rates at the boundary nodes.
             * @returns Flow rates in m^3/s.
             */
            std::unordered_map<int, T> getFlowRates() const override;

            /**
             * @brief Get the ground nodes of the module.
             * @returns Ground nodes.
            */
            std::unordered_map<int, bool> getGroundNodes() {
                return this->groundNodes;
            }

            /**
             * @brief Get the number of iterations for the value tracer.
             * @returns Number of iterations for the value tracer.
            */
            int getStepIter() const {
                return stepIter;
            }

            /**
             * @brief Returns whether the module has converged or not.
             * @returns Boolean for module convergence.
            */
            bool hasConverged() const override;

            void setVtkFolder(std::string vtkFolder);

    };
}
