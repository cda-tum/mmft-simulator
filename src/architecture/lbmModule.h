#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>
#include <olb3D.h>
#include <olb3D.hh>

#include "Module.h"
#include "Node.h"
#include "Channel.h"

namespace arch {

    // Forward declared dependencies
    template<typename T>
    class Network;

    /**
     * @brief Struct that defines an opening, which is an in-/outlet of the CFD domain.
    */
    template<typename T>
    struct Opening {
        std::shared_ptr<Node<T>> node;
        std::vector<T> normal;
        std::vector<T> tangent;
        T width;
        T height;

        /**
         * @brief Constructor of an opening.
         * @param[in] node The module node in the network that corresponds to this opening.
         * @param[in] normal The normal direction of the opening, pointing into the CFD domain.
         * @param[in] width The width of the opening.
         * @param[in] height The height of the opening.
        */
        Opening(std::shared_ptr<Node<T>> node_, std::vector<T> normal_, T width_, T height_=1e-4) :
            node(node_), normal(normal_), width(width_), height(height_) {

                // Rotate the normal vector 90 degrees counterclockwise to get the tangent
                T theta = 0.5*M_PI;
                tangent = { cos(theta)*normal_[0] - sin(theta)*normal_[1],
                            sin(theta)*normal_[0] + cos(theta)*normal_[1]};

            }
    };

    /**
     * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
    */
    template<typename T>
    class lbmModule : public Module<T> {

        using DESCRIPTOR = olb::descriptors::D2Q9<>;
        using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
        using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
        using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

        private:
            int step = 0;                           ///< Iteration step of this module.
            int stepIter = 1000;                    ///< Number of iterations for the value tracer.
            int maxIter = 1e7;                      ///< Maximum total iterations.
            int theta = 10;                         ///< Number of OLB iterations per communication iteration.
            std::unordered_map<int, T> pressures;   ///< Vector of pressure values at module nodes.
            std::unordered_map<int, T> flowRates;   ///< Vector of flowRate values at module nodes.
            std::string name;                       ///< Name of the module.
            std::string stlFile;                    ///< The STL file of the CFD domain.
            bool initialized = false;               ///< Is the module initialized?
            bool isConverged = false;               ///< Has the module converged?
            T charPhysLength;                       ///< Characteristic physical length (= width, usually).
            T charPhysVelocity;                     ///< Characteristic physical velocity (expected maximal velocity).
            T alpha;                                ///< Relaxation factor.
            T resolution;                           ///< Resolution of the CFD domain. Gridpoints in charPhysLength.
            T epsilon;                              ///< Convergence criterion.
            T relaxationTime;                       ///< Relaxation time (tau) for the OLB solver.

            std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids.
            std::shared_ptr<olb::CuboidGeometry<T,2>> cuboidGeometry;       ///< The geometry in a single cuboid.
            std::shared_ptr<olb::SuperGeometry<T,2>> geometry;              ///< The final geometry of the channels.
            std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
            std::unique_ptr<olb::util::ValueTracer<T>> converge;            ///< Value tracer to track convergence.

            std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;      ///< Object that stores conversion factors from phyical to lattice parameters.
            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>>> fluxes;              ///< Map of fluxes at module nodes. 
            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>> meanPressures;       ///< Map of mean pressure values at module nodes.

            std::shared_ptr<Network<T>> moduleNetwork;                      ///< Fully connected graph as network for the initial approximation.
            std::unordered_map<int, Opening<T>> moduleOpenings;             ///< Map of openings.
            std::unordered_map<int, bool> groundNodes;                      ///< Map of nodes that communicate the pressure to the 1D solver.

            auto& getConverter() {
                return *converter;
            }

            auto& getGeometry() {
                return *geometry;
            }

            auto& getLattice() {
                return *lattice;
            }

        public:
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
            lbmModule(int id, std::string name, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
                std::unordered_map<int, Opening<T>> openings, std::string stlFile, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932);

            /**
             * @brief Initialize an instance of the LBM solver for this module.
             * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
             * @param[in] density Density of the simulated fluid in _kg / m^3_.
            */
            void lbmInit(T dynViscosity, T density);

            /**
             * @brief Prepare the LBM geometry of this instance.
            */
            void prepareGeometry();

            /**
             * @brief Prepare the LBM lattice on the LBM geometry.
            */
            void prepareLattice();

            /**
             * @brief Set the boundary values on the lattice at the module nodes.
             * @param[in] iT Iteration step.
            */
            void setBoundaryValues(int iT);

            /**
             * @brief Conducts the collide and stream operations of the lattice.
            */
            void solve();

            /**
             * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
             * @param[in] iT Iteration step.
            */
            void getResults(int iT);

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
             * @brief Get the openings of the module.
             * @returns Module openings.
             */
            std::unordered_map<int, Opening<T>> getOpenings() const {
                return moduleOpenings;
             };

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
            };

            /**
             * @brief Returns whether the module is initialized or not.
             * @returns Boolean for initialization.
            */
            bool getInitialized() const { 
                return initialized; 
            };

            /**
             * @brief Returns whether the module has converged or not.
             * @returns Boolean for module convergence.
            */
            bool hasConverged() const {
                return converge->hasConverged();
            };

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
             * @brief Get the relaxation factor alpha.
             * @returns alpha.
            */
            T getAlpha() const { 
                return alpha; 
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
}