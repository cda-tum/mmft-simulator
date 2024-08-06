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

#include <ESSDropletSolver.h>

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
    class essLbmDropletSimulator : public essLbmSimulator<T> {
        private:

            std::unordered_map<int, T> bufferZones;                 ///< For each opening we need a buffer zone to generate droplet <nodeId, bufferLength>
            std::unordered_map<int, arch::RectangularChannel<T>> virtualChannels;
            std::unordered_map<int, sim::DropletInjection<T>> dropletInjections; ///< <nodeId/bufferId, vector of pending dropletInjections >
            std::unordered_map<int, std::vector<sim::Droplet<T>*>> lbmDroplets;  ///< Map of droplets in the lbm zone <dropletId, dropletPointer>

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
            essLbmDropletSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule,  std::unordered_map<int, arch::Opening<T>> openings_,
                            ResistanceModel<T>* resistanceModel, T charPhysLength_, T charPhysVelocity_, T alpha, T resolution_, T epsilon_, T relaxationTime_);
            /**
             * @brief Initialize an instance of the LBM solver for this module.
             * @param[in] dynViscosity Dynamic viscosity of the simulated fluid in _kg / m s_.
             * @param[in] density Density of the simulated fluid in _kg / m^3_.
            */
            void lbmInit(T dynViscosity, T density) override;

            /**
             * @brief Generate a droplet in the LBM solver
             * @return Id of the generated droplet
             */
            int generateDroplet(sim::Droplet<T>* droplet, Node<T>* entrypoint);

            /**
             * @brief Purge the droplet with Id in the buffer zone from the LBM solver
             * @return Opening id, where the droplet exits LBM domain.
             */
            int purgeDroplet(int dropletId);

            /**
             * @brief Create a droplet in the 1D solver, that is exiting the CFD domain.
             * @param[in] Id 
             * @param[in] volume 
             * @param[in] Fluid
             * @return Droplet instance
             */
            int addDroplet(T dropletVolume, T viscosity, T density, T timeStamp, int channelId, T position);

            /**
             * @brief A droplet in the 1D solver was removed and needs to be injected at specified time at bufferzone with nodeId
             * @param[in] time
             * @param[in] nodeId
             * @param[in] dropletPtr
             */
            void addDropletInjection(T time, int nodeId, Droplet<T>* dropletPtr);

            /** TODO:
             * 
             */
            std::unordered_map<int, std::vector<sim::Droplet<T>*>> getPendingDroplets();
    };
}
