/**
 * @file olbMixing.h
 */

#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

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
class lbmMixingSimulator2D : public olbSim<T>, public Sim2D<T> {

using ADDESCRIPTOR = olb::descriptors::D2Q5<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

protected:

    std::unordered_map<int, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>>> adLattices;      ///< The LBM lattice on the geometry.
    std::unordered_map<int, std::shared_ptr<const olb::AdeUnitConverterFromResolutionAndRelaxationTime<T, ADDESCRIPTOR>>> adConverters;      ///< Object that stores conversion factors from phyical to lattice parameters.

    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<olb::AnalyticalConst2D<T,T>>>> concentrationProfiles;
    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>> meanConcentrations;       ///< Map of mean pressure values at module nodes.

    auto& getAdConverter(int key) {
        return *adConverters.at(key);
    }

    auto& getAdLattice(int key) {
        return *adLattices.at(key);
    }

    void setConcentration2D(int key);

    void initAdConverters(T density) final;

    void prepareAdLattice(const T omega, int speciesId) final;

    void initConcentrationIntegralPlane() final;

    void initAdLattice(int adKey) final;

    void prepareCoupling() final;

    void storeAdResults(int nodeId) final;

    void storeCfdResults(int nodeId) final;

    /**
     * @brief Execute the coupling placed onto the NS lattice.
    */
    void executeCoupling() final;

    /**
     * @brief Execute the coupling placed onto the NS lattice.
    */
    void collideAndStreamAD() final;

public:

    /**
     * @brief Set the boundary values on the lattice at the module nodes.
     * @param[in] iT Iteration step.
    */
    void setBoundaryValues(int iT) final;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    void writeVTK(int iT) final;

    T getAdOmega(int key) final { return adConverters.at(key)->getLatticeRelaxationFrequency(); }

    std::unordered_map<int, T> getAdOmegas() final;
};

}   // namespace arch
