#pragma once

#include <memory>

#include <olb2D.h>

namespace arch {
    
/**
 * @brief Class that handles the OLB solver for a organ module.
*/
template<typename T>
class OrganModule : public CFDModule<T> {

    using NSDESCRIPTOR = olb::descriptors::D2Q9<>;
    using ADDESCRIPTOR = olb::descriptors::D2Q5<olb::descriptors::VELOCITY>;
    using NoDynamics = olb::NoDynamics<T,NSDESCRIPTOR>;
    using BulkDynamics = olb::BGKdynamics<T,NSDESCRIPTOR>;
    using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;

    private: 

        T radiusOrgan;
        T lengthX;
        T lengthY;

        int concentrationCount;

        T relaxationTimeAD;

        T physDiffusivity;

        std::shared_ptr<olb::SuperLattice<T, NSDESCRIPTOR>> latticeNS;
        std::unordered_map<int, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>>> latticesAD;

        std::unique_ptr<olb::util::ValueTracer<T>> convergeNS;
        std::unordered_map<int, std::unique_ptr<olb::util::ValueTracer<T>>> convergesAD;

        std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, NSDESCRIPTOR>> converterNS;
        std::unordered_map<int, std::shared_ptr<const olb::AdeUnitConverter<T, ADDESCRIPTOR>>> converterAD;

        std::unordered_map<int, T> Vmax;
        std::unordered_map<int, T> Km;
        std::unordered_map<int, T*> fluxWall;

        // First loop over all openings of module, then loop over all set of concentrations
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxPressure2D<T>>>> meanConcentrations;

        auto& getConverterNS() {
            return *converterNS;
        }

        auto& getConverterAD( int Id ) {
            return *converterAD.at(Id);
        }

        auto& getLatticeNS() {
            return *latticeNS;
        }

        auto& getLatticeAD( int Id ) {
            return *latticesAD.at(Id);
        }

    public: 

        OrganModule(int id, std::string name, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, Opening<T>> openings, std::string stlFile, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, 
            T epsilon, T relaxationTime=0.932);

        /**
         * @brief Initialize an instance of the LBM solver for this module.
         * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
         * @param[in] density Density of the simulated fluid in _kg / m^3_.
        */
        void lbmInit(T dynViscosity, T density);

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
         * @brief Define and prepare the coupling of the NS lattice with the AD lattices.
        */
        void prepareCoupling();

        /**
         * @brief Conducts the collide and stream operations of the lattice.
        */
        void solve() override;

        /**
         * @brief Update the values at the module nodes based on the simulation result after stepIter iterations.
         * @param[in] iT Iteration step.
        */
        void getResults() override;

        /**
         * @brief Write the vtk file with results of the CFD simulation to file system.
         * @param[in] iT Iteration step.
        */
        void writeVTK(int iT) override;

        void evaluateShearStress();

        bool hasConverged() const;


};

} // namespace arch

