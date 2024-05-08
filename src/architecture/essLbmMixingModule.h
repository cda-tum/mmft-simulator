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

#include <ESSLbmSolver.h>

namespace arch {

    /**
     * @brief Class that defines the droplet module
    */
    template<typename T>
    class essLbmMixingModule : public essLbmModule<T> {

        private:

            std::unordered_map<int, T> concentrations;       ///< Map of concentration values at module nodes.


        public:

            essLbmMixingModule(int id, std::string name, std::string stlFile, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
                                std::unordered_map<int, Opening<T>> openings, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932)
                    : essLbmModule<T>::essLbmModule(id, name, stlFile, pos, size, nodes, openings, charPhysLenth, charPhysVelocity, alpha, resolution, epsilon, relaxationTime) { }

            /**
             * @brief Set the concentrations at the nodes on the module boundary.
             * @param[in] concentrations Map of concentrations and node ids.
             */
            void setConcentrations(std::unordered_map<int, T> concentrations_) {
                for(auto& [key, value] : flowRate_) {
                    concentrations.try_emplace(key, 1.0);
                }
                solver_->setConcentrations(concentrations);
            }

            // TODO: Agree on concentration unit, mol/l ok?
            /**
             * @brief Get the concentrations at the boundary nodes.
             * @returns Concentrations in mol/l.
             */
            std::unordered_map<int, T> getConcentrations() const {
                return concentrations;
            }

    };

}
