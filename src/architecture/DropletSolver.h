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

    template<typename T>
    struct Opening;

    /**
     * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
    */
    template<typename T>
    class DropletSolver : public lbmModule<T> {

        using DESCRIPTOR = olb::descriptors::D2Q9<>;
        using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
        using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
        using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

        private:

            std::vector<T> bufferZones;     ///< For each opening we need a buffer zone to generate droplet


        public:

            DropletSolver(int id, std::string name, std::string stlFile, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
                std::unordered_map<int, Opening<T>> openings, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932);

            /**
             * @brief Generate a droplet in the LBM solver
             * @return Id of the generated droplet
            */
           int generateDroplet(T density, T viscosity, T volume);

           /**
            * @brief Creates a virtual droplet in the 1D solver, that was created from a split operation
            * @param[in] Id 
            * @param[in] volume 
            * @param[in] Fluid
            * @return Droplet instance
           */
           sim::Droplet<T> addDroplet();

           /**
            * @brief Purge the droplet with Id in the buffer zone from the LBM solver
            * @return Opening id, where the droplet exits LBM domain.
           */
           int purgeDroplet(int dropletId);

           /**
            * @brief Removes a droplet in the 1D solver, that got consumed in a merge operation
           */
           void removeDroplet(int dropletId);

    };

}
