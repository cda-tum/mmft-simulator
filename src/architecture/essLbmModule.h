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

    /**
     * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
    */
    template<typename T>
    class essLbmModule : public Module<T> {
        private:
            int step = 0;                           ///< Iteration step of this module.
            int stepIter = 1000;                    ///< Number of iterations for the value tracer.
            int maxIter = 1e7;                      ///< Maximum total iterations.
            int theta = 10;                         ///< Number of OLB iterations per communication iteration.

            std::string name;                       ///< Name of the module.
            std::string stlFile;                    ///< The STL file of the CFD domain.

            bool initialized = false;               ///< Is the module initialized?
            bool isConverged = false;               ///< Has the module converged?
            
            std::shared_ptr<Network<T>> moduleNetwork;                      ///< Fully connected graph as network for the initial approximation.
            std::unordered_map<int, Opening<T>> moduleOpenings;             ///< Map of openings.
            std::unordered_map<int, bool> groundNodes;                      ///< Map of nodes that communicate the pressure to the 1D solver.

            T charPhysLength;                       ///< Characteristic physical length (= width, usually).
            T charPhysVelocity;                     ///< Characteristic physical velocity (expected maximal velocity).
            T alpha;                                ///< Relaxation factor.
            T resolution;                           ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
            T epsilon;                              ///< Convergence criterion.
            T relaxationTime;

            std::unordered_map<int, float> flowRates;              ///< Map of fluxes at module nodes.
            std::unordered_map<int, float> pressures;       ///< Map of mean pressure values at module nodes.

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
            essLbmModule(int id_, std::string name_, std::string stlFile_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, std::unordered_map<int, Opening<T>> openings_,
                         T charPhysLenth_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_);
            /**
             * @brief Initialize an instance of the LBM solver for this module.
             * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
             * @param[in] density Density of the simulated fluid in _kg / m^3_.
            */
            void lbmInit(T dynViscosity, T density);

            /**
             * @brief Set the boundary values on the lattice at the module nodes.
             * @param[in] iT Iteration step.
            */
            void setBoundaryValues();

            /**
             * @brief Conducts the collide and stream operations of the lattice.
            */
            void solve();

            /**
             * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
             * @param[in] iT Iteration step.
            */
            void getResults();

            /**
             * @brief Write the vtk file with results of the CFD simulation to file system.
             * @param[in] iT Iteration step.
            */
            void writeVTK(int iT);

            /**
             * @brief Set the pressures at the nodes on the module boundary.
             * @param[in] pressure Map of pressures and node ids.
             */
            void setPressures(std::unordered_map<int, T> pressure);

            /**
             * @brief Set the flow rates at the nodes on the module boundary.
             * @param[in] flowRate Map of flow rates and node ids.
             */
            void setFlowRates(std::unordered_map<int, T> flowRate);

            /**
             * @brief Set the nodes of the module that communicate the pressure to the 1D solver.
             * @param[in] groundNodes Map of nodes.
             */
            void setGroundNodes(std::unordered_map<int, bool> groundNodes);

            /**
             * @brief Get the pressures at the boundary nodes.
             * @returns Pressures in Pa.
             */
            std::unordered_map<int, T> getPressures() const {
                return solver_->getPressures();
            }

            /**
             * @brief Get the flow rates at the boundary nodes.
             * @returns Flow rates in m^3/s.
             */
            std::unordered_map<int, T> getFlowRates() const {
                return solver_->getFlowRates();
            }

            /**
             * @brief Get the openings of the module.
             * @returns Module openings.
             */
            std::unordered_map<int, Opening<T>> getOpenings() const {
                return moduleOpenings;
             }

            /**
             * @brief Get the ground nodes of the module.
             * @returns Ground nodes.
            */
            std::unordered_map<int, bool> getGroundNodes() {
                return groundNodes;
            }

            /**
             * @brief Get the number of iterations for the value tracer.
             * @returns Number of iterations for the value tracer.
            */
            int getStepIter() const {
                return stepIter;
            }

            /**
             * @brief Returns whether the module is initialized or not.
             * @returns Boolean for initialization.
            */
            bool getInitialized() const { 
                return initialized; 
            }

            /**
             * @brief Returns whether the module has converged or not.
             * @returns Boolean for module convergence.
            */
            bool hasConverged() const {
                return solver_->hasConverged();
            }

            /**
             * @brief Set the initialized status for this module.
             * @param[in] initialization Boolean for initialization status.
            */

            void setInitialized(bool initialization);

            /**
             * @brief Get the fully connected graph of this module, that is used for the initial approximation.
             * @return Network of the fully connected graph.
            */
            std::shared_ptr<Network<T>> getNetwork() const {
                return moduleNetwork;
            }
    };
}
