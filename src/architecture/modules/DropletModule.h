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
 * @brief Class that handles the OLB solver for a droplet-based module.
*/
template<typename T>
class DropletModule : public CFDModule<T,olb::descriptors::D2Q9<CHEM_POTENTIAL, FORCE>> {

    using DESCRIPTOR = olb::descriptors::D2Q9<CHEM_POTENTIAL, FORCE>;
    using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
    using ForcedBGKdynamics = olb::ForcedBGKdynamics<T,DESCRIPTOR>;
    using FreeEnergyBGKdynamics = olb::FreeEnergyBGKdynamics<T,DESCRIPTOR>;

    private: 
        T alpha;
        T kappa1;
        T kappa2;
        T h1;
        T h2;
        T gama;
        T omega;

        std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice1;
        std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice2;

        std::unique_ptr<olb::util::ValueTracer<T>> converge;
        std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;

        auto& getConverter() {
            return *converter;
        }

        auto& getLattice1() {
            return *lattice1;
        }

        auto& getLattice2() {
            return *lattice2;
        }

    public:

        DropletModule(int id, std::string name, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, Opening<T>> openings, std::string stlFile, T charPhysLenth, T charPhysVelocity, T resolution, T relaxationTime=0.932);

        void lbmInit() override;

        void initIntegrals() override;

        void prepareLattice() override;
        
        void prepareCoupling();

        void setInletBoundaries(int material);

        void setOutletBoundaries(int material);

        void setBoundaryValues(int iT) override;

        void addDroplet(T pos[2], T vol);

        void addDroplet(T x[2], T y[2], T theta=0);

        void scanDroplets();

        void delDroplet(int Id);

        void solve() override;

        void getResults(int iT);

        void writeVTK(int iT);


}

}   /// namespace arch