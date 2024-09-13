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

#include <ESSMixingSolver.h>

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
    class essLbmMixingSimulator : public essLbmSimulator<T> {
        private:

            std::unordered_map<int, std::unordered_map<int, T>> concentrations;   ///< Vector of concentration values at module nodes. <specieId, <nodeId, conc>> !! Inverse of OLB !!

            std::unordered_map<int, Specie<T>*> species;

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
            essLbmMixingSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule, std::unordered_map<int, Specie<T>*> species_,
                                  std::unordered_map<int, arch::Opening<T>> openings_, ResistanceModel<T>* resistanceModel, T charPhysLength_, T charPhysVelocity_, T alpha, T resolution_, T epsilon_, T relaxationTime_);
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
            void setBoundaryValues(int iT) override;

            /**
             * @brief Conducts the collide and stream operations of the lattice.
            */
            void solve() override;

            /**
             * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
            */
            void storeCfdResults() override;

            /**
             * @brief Set the pressures at the nodes on the module boundary.
             * @param[in] pressure Map of pressures and node ids.
             */
            void storeConcentrations(std::unordered_map<int, std::unordered_map<int, T>> concentrations) override;

            /**
             * @brief Get the pressures at the boundary nodes.
             * @returns Pressures in Pa.
             */
            std::unordered_map<int, std::unordered_map<int, T>> getConcentrations() const override;

    };
}
