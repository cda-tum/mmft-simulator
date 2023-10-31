#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "BolusInjection.h"
#include "CFDSim.h"
#include "Fluid.h"
#include "MixingModels.h"
#include "ResistanceModels.h"

#include "../architecture/Network.h"
#include "../architecture/Platform.h"
#include "../nodalAnalysis/NodalAnalysis.h"

namespace sim {

    /**
     * @brief Class that conducts the simulation and owns all parameters necessary for it.
     */
    template<typename T>
    class Simulation {
        private:
            arch::Network<T>* network;                                                      ///< Network for which the simulation should be conducted.
            std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids;                     ///< Fluids specified for the simulation.
            std::vector<Mixture<T>> mixtures;                                              ///< Mixtures present in the simulation.
            std::unordered_map<int, std::unique_ptr<BolusInjection<T>>> bolusInjections;   ///< Injections of fluids that should take place during the simulation.
            // TODO: Add static member variable that keeps track of total memory allocated for lbm sim
            bool transient = false;
            T globalTime = 0.0;
            ResistanceModel2DPoiseuille<T>* resistanceModel;                ///< The resistance model used for te simulation.
            InstantaneousMixingModel<T>* mixingModel;                       ///< The resistance model used for te simulation.
            Fluid<T>* continuousPhase = nullptr;                            ///< Fluid of the continuous phase.

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
             * @brief Add new fluid to the simulation.
             * 
             * @param viscosity Dynamic viscosity of the fluid in Pas.
             * @param density Density of the fluid in kg/m^3.
             * @param concentration Concentration of the fluid in percent (between 0.0 and 1.0).
             * @param molecularSize Molecular size in m^3.
             * @param diffusionCoefficient Diffusion coefficient of the fluid in m^2/s.
             * @param saturation Saturation value to translate the concentration in an actual concentration value [mol/m^3].
             * @return Pointer to the fluid.
             */
            Fluid<T>* addFluid(T viscosity, T density, T concentration, T molecularSize, T diffusionCoefficient, T saturation);

            /**
             * @brief Get fluid.
             * @param[in] fluidId Id of the fluid
             * @return Pointer to fluid with the corresponding id
             */
            Fluid<T>* getFluid(int fluidId);

            /**
             * @brief Get mixture.
             * 
             * @param mixtureId Id of the mixture
             * @return Pointer to mixture with the correspondig id
             */
            Mixture<T>* getMixture(int mixtureId);

            /**
             * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
             * @param[in] fluid The fluid the continuous phase consists of.
             */
            void setContinuousPhase(Fluid<T>* fluid);

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
             * @brief Define which mixing model should be used for the concentrations.
             * @param[in] model The mixing model to be used.
             */
            void setMixingModel(InstantaneousMixingModel<T>* model);

            /**
             * @brief Conduct the simulation.
             * @return The result of the simulation containing all intermediate simulation steps and calculated parameters.
             */
            void simulate();

            /**
             * @brief Calculate and set new state of the continuous fluid simulation. Move mixture positions and create new mixtures if necessary.
             * 
             * @param timeStep Time step in s for which the new mixtures state should be calculated.
             */
            void calculateNewMixtures(double timeStep);

            /**
             * @brief Print the results as pressure at the nodes and flow rates at the channels
            */
           void printResults();
    };
}   // namespace sim