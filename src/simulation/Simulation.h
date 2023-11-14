#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "CFDSim.h"
#include "Fluid.h"
#include "ResistanceModels.h"

#include "../architecture/Network.h"
#include "../nodalAnalysis/NodalAnalysis.h"

namespace sim {

    enum class Platform {
        NONE,
        CONTINUOUS,     ///< A simulation with a single continuous fluid.
        DROPLET,        ///< A simulation with droplets filling a channel cross-section
        MIXING          ///< A simulation wit multiple miscible fluids.
    };

    enum class Type {
        NONE,
        _1D,            ///< A simulation in the 1D abstraction level
        HYBRID,         ///< A simulation combining the 1D and CFD abstraction levels
        CFD             ///< A simulation in the CFD abstraction level
    };

    /**
     * @brief Class that conducts the simulation and owns all parameters necessary for it.
     */
    template<typename T>
    class Simulation {
        private:
            // TODO: Add static member variable that keeps track of total memory allocated for lbm sim
            Platform platform = Platform::NONE;
            Type simType = Type::NONE;
            arch::Network<T>* network;                                     ///< Network for which the simulation should be conducted.
            ResistanceModel2DPoiseuille<T>* resistanceModel;               ///< The resistance model used for te simulation.
            std::vector<std::unique_ptr<Fluid<T>>> fluids;
            int continuousPhase = 0;                                  ///< Fluid of the continuous phase.

            /**
             * @brief Initializes the resistance model and the channel resistances of the empty channels.
             */
            void initialize();

        public:
            Simulation();

            /**
             * @brief Set the network for which the simulation should be conducted.
             * @param[in] network Network on which the simulation will be conducted.
             */
            void setNetwork(arch::Network<T>* network);

            /**
             * @brief Get the network.
             * @return Network or nullptr if no network is specified.
             */
            arch::Network<T>* getNetwork();

            /**
             * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
             * @param[in] fluid The fluid the continuous phase consists of.
             */
            void setContinuousPhase(int fluidId);

            /**
             * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
             * @param[in] fluid The fluid the continuous phase consists of.
             */
            void setFluids(std::vector<std::unique_ptr<Fluid<T>>>& fluids);

            /**
             * @brief Get the continuous phase.
             * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
             */
            Fluid<T>* getContinuousPhase();

            /**
             * @brief Define which resistance model should be used for the channel and droplet resistance calculations.
             * @param[in] model The resistance model to be used.
             */
            void setResistanceModel(ResistanceModel2DPoiseuille<T>* model);

            /**
             * @brief Conduct the simulation.
             * @return The result of the simulation containing all intermediate simulation steps and calculated parameters.
             */
            void simulate();

            /**
             * @brief Print the results as pressure at the nodes and flow rates at the channels
            */
           void printResults();

           /**
            * @brief Set the platform of the simulation.
            * @param[in] platform
           */
            void setPlatform(Platform platform);

            /**
            * @brief Set the type of the simulation.
            * @param[in] type
           */
            void setType(Type type);
    };
}   // namespace sim