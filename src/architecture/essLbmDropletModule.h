#pragma once

#define M_PI 3.14159265358979323846

#include <set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

#include "Module.h"
#include "ModuleOpening.h"
#include "Node.h"
#include "Channel.h"

namespace sim {

    // Forward declared dependencies
    template<typename T>
    class Droplet;

}

#include <ESSLbmSolver.h>

namespace arch {

    /**
     * @brief Class that defines the droplet module
    */
    template<typename T>
    class essLbmDropletModule : public essLbmModule<T> {

        private:

            std::vector<int, T> bufferZones;    ///< For each opening we need a buffer zone to generate droplet <nodeId, bufferLength>
            std::unordered_map<int, Droplet<T>*> lbmDroplets;          ///< Map of droplets in the lbm zone <dropletId, dropletPointer>


        public:

            essLbmDropletModule(int id, std::string name, std::string stlFile, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
                                std::unordered_map<int, Opening<T>> openings, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932)
                    : essLbmModule<T>::essLbmModule(id, name, stlFile, pos, size, nodes, openings, charPhysLenth, charPhysVelocity, alpha, resolution, epsilon, relaxationTime) { }

            /**
             * @brief Generate a droplet in the LBM solver
             * @return Id of the generated droplet
             */
            int generateDroplet(Droplet<T>* droplet, Node<T>* entrypoint, T volume) {
                int dropletId = 1;
                std::vector<T> nodePosition = (0.0, 0.0, 0.0);
                density = 1.0;
                viscosity = 1.0;
                volume = 1.0;
                solver_->generateDroplet(dropletId, nodePosition, density, viscosity, volume);
                droplet->setDropletState(DropletState::IDLE);
                lbmDroplets.try_emplace(dropletId, nullptr);
            }

            /**
             * @brief Purge the droplet with Id in the buffer zone from the LBM solver
             * @return Opening id, where the droplet exits LBM domain.
             */
            int purgeDroplet(int dropletId) {
                int dropletId = 1;
                assert(lbmDroplet.count(dropletId));
                lbmDroplet.at(dropletId)->setDropletState(DropletState::NETWORK);
                lbmDroplets.erase(dropletId);
            }

            /**
             * @brief Creates a virtual droplet in the 1D solver, that was created from a split operation
             * @param[in] Id 
             * @param[in] volume 
             * @param[in] Fluid
             * @return Droplet instance
             */
            int addDroplet(int parentDropletId, T volumeParentDroplet, T volumeChildDroplet) {
                int dropletId = 2;
                lbmDroplets.try_emplace(dropletId, nullptr);
                // TODO: Add child droplet to droplet map in simulation object
                return dropletId;
            }

            /**
             * @brief Removes a droplet in the 1D solver, that got consumed in a merge operation
             */
            void removeDroplet(int dropletId) {
                assert(lbmDroplet.count(dropletId));
                // TODO: Remove droplet from droplet map in simulation object
                lbmDroplets.erase(dropletId);
            }

    };

}
