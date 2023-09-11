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

        T physDiffusivity;

        std::shared_ptr<olb::SuperLattice<T, NSDESCRIPTOR>> latticeNS;
        std::unordered_map<int, std::shared_ptr<olb::SuperLattice<T, ADDESCRIPTOR>>> latticesAD;

        std::unique_ptr<olb::util::ValueTracer<T>> convergeNS;
        std::unordered_map<int, std::unique_ptr<olb::util::ValueTracer<T>>> convergesAD;

        std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, NSDESCRIPTOR>> converterNS;
        std::unordered_map<int, std::shared_ptr<const olb::AdeUnitConverter<T, ADDESCRIPTOR>>> converterAD;

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

        void lbmInit();

        void initIntegrals() override;

        void prepareLattice() override;

        void prepareCoupling();

        void setBoundaryValues(int iT) override;

        void solve() override;

        void getResults(int iT) override;

        void writeVTK(int iT) override;

        void evaluateShearStress();

        bool hasConverged() const;


};

} // namespace arch

