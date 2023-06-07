#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <Network.h>
#include <Fluid.h>
#include <ResistanceModels.h>
#include <NodalAnalysis.h>
#include <CFDSim.h>

namespace sim {

    /**
     * @brief Class that conducts the simulation and owns all parameters necessary for it.
     */
    template<typename T>
    class Simulation {
        private:
            // TODO: Add static member variable that keeps track of total memory allocated for lbm sim
            arch::Network<T>* network;                                     ///< Network for which the simulation should be conducted.
            ResistanceModel2DPoiseuille<T>* resistanceModel;               ///< The resistance model used for te simulation.
            Fluid<T>* continuousPhase = nullptr;                           ///< Fluid of the continuous phase.

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
             * @param[in] fluidId Unique identifier of the fluid the continuous phase consists of.
             */
            void setContinuousPhase(Fluid<T>* fluid);

            /**
             * @brief Get the continuous phase.
             * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
             */
            Fluid<T>* getContinuousPhase();

            /**
             * @brief Define which resistance model should be used for the channel and droplet resistance calculations.
             * @param[in] modelName Name of the resistance model to be used.
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
    };
}   // namespace sim