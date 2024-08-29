/**
 * @file olbOoc.h
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

// Forward declared dependencies
template<typename T>
class Tissue;

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class lbmOocSimulator : public lbmMixingSimulator<T> {

using DESCRIPTOR = olb::descriptors::D2Q9<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

using ADDESCRIPTOR = olb::descriptors::D2Q5<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

private:

    std::shared_ptr<Tissue<T>> tissue;
    std::string organStlFile;                   ///< The STL file of the CFD domain.

    std::shared_ptr<olb::STLreader<T>> organStlReader;
    std::shared_ptr<olb::IndicatorF2DfromIndicatorF3D<T>> organStl2Dindicator;

    T Vmax;

    void readOrganStl(const T dx);

    void prepareNsLattice(const T omega) override;

    void prepareAdLattice(const T omega, int speciesId);

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
    lbmOocSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<Tissue<T>> tissue, std::string organStlFile, std::shared_ptr<arch::Module<T>> cfdModule, 
        std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, arch::Opening<T>> openings, ResistanceModel<T>* resistanceModel, 
        T charPhysLenth, T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

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
    lbmOocSimulator(int id, std::string name, std::string stlFile, std::shared_ptr<Tissue<T>> tissue, std::string organStlFile, std::shared_ptr<arch::Module<T>> cfdModule, 
        std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, arch::Opening<T>> openings, std::shared_ptr<mmft::Scheme<T>> updateScheme, ResistanceModel<T>* resistanceModel, 
        T charPhysLenth, T charPhysVelocity, T resolution, T epsilon, T relaxationTime=0.932, T adRelaxationTime=0.932);

    /**
     * @brief Initialize an instance of the LBM solver for this module.
     * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
     * @param[in] density Density of the simulated fluid in _kg / m^3_.
    */
    void lbmInit(T dynViscosity, T density) override;

    /**
     * @brief Prepare the LBM geometry of this instance.
    */
    void prepareGeometry() override;

    /**
     * @brief Prepare the LBM lattice on the LBM geometry.
    */
    void prepareLattice() override;

    /**
     * @brief Write the vtk file with results of the CFD simulation to file system.
     * @param[in] iT Iteration step.
    */
    void writeVTK(int iT);

};

}   // namespace arch
