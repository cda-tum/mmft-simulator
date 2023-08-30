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

#include "CFDModule.h"
#include "../Node.h"
#include "../Channel.h"

namespace arch {

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class ContinuousModule : public CFDModule<T,olb::descriptors::D2Q9<>> {

    using DESCRIPTOR = olb::descriptors::D2Q9<>;
    using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
    using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
    using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

    private:
        int step = 0;                           ///< Iteration step of this module.
        int stepIter = 1000;                    ///< Number of iterations for the value tracer.
        int maxIter = 1e7;                      ///< Maximum total iterations.
        bool isConverged = false;               ///< Has the module converged?
        T alpha;                                ///< Relaxation factor.
        T epsilon;                              ///< Convergence criterion.

        std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry.
        std::unique_ptr<olb::util::ValueTracer<T>> converge;            ///< Value tracer to track convergence.

        /// Object that stores conversion factors from phyical to lattice parameters.
        std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;                   

        auto& getConverter() {
            return *converter;
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
        ContinuousModule(int id, std::string name, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, Opening<T>> openings, std::string stlFile, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932);

        /**
         * @brief Initialize an instance of the LBM solver for this module.
         * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
         * @param[in] density Density of the simulated fluid in _kg / m^3_.
        */
        void lbmInit(T dynViscosity, T density) override;

        /**
         * @brief Initialize the integral fluxes for the in- and outlets
        */
        void initIntegrals() override;

        /**
         * @brief Prepare the LBM lattice on the LBM geometry.
        */
        void prepareLattice() override;

        /**
         * @brief Set the boundary values on the lattice at the module nodes.
         * @param[in] iT Iteration step.
        */
        void setBoundaryValues(int iT) override;

        /**
         * @brief Conducts the collide and stream operations of the lattice.
        */
        void solve() override;

        /**
         * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
         * @param[in] iT Iteration step.
        */
        void getResults(int iT) override;

        /**
         * @brief Write the vtk file with results of the CFD simulation to file system.
         * @param[in] iT Iteration step.
        */
        void writeVTK(int iT) override;

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
         * @brief Get the relaxation factor alpha.
         * @returns alpha.
        */
        T getAlpha() const { 
            return alpha; 
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