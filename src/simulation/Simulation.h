/**
 * @file Simulation.h
 */

#pragma once

#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class ChannelPosition;

template<typename T>
class Module;

template<typename T>
class Network;

template<typename T>
class Opening;

}

namespace mmft {

// Forward declared dependencies
template<typename T>
class Scheme;

template<typename T>
class NaiveScheme;

}

namespace nodal {

// Forward declared dependencies
template<typename T>
class NodalAnalysis;

}

namespace result {

// Forward declared dependencies
template<typename T>
class SimulationResult;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class CFDSimulator;

template<typename T>
class Droplet;

template<typename T>
class DropletInjection;

template<typename T>
class Event;

template<typename T>
class Fluid;

template<typename T>
class lbmSimulator;

template<typename T>
class lbmMixingSimulator;

template<typename T>
class lbmOocSimulator;

template<typename T>
class essLbmSimulator;

template<typename T>
class MembraneModel;

template<typename T>
class MixingModel;

template<typename T>
class Mixture;

template<typename T>
class MixtureInjection;

template<typename T>
class ResistanceModel;

template<typename T>
class Specie;

template<typename T>
class Tissue;

enum class Type {
    Abstract,       ///< A simulation in the 1D abstraction level
    Hybrid,         ///< A simulation combining the 1D and CFD abstraction levels
    CFD             ///< A simulation in the CFD abstraction level
};

enum class Platform {
    Continuous,     ///< A simulation with a single continuous fluid.
    BigDroplet,     ///< A simulation with droplets filling a channel cross-section
    Mixing,         ///< A simulation with multiple miscible fluids.
    Ooc             ///< A simulation with organic tissue
};

/**
 * @brief Class that conducts the simulation and owns all parameters necessary for it.
 */
template<typename T>
class Simulation {
private:
    int fixtureId = 0;
    Type simType = Type::Abstract;                                                      ///< The type of simulation that is being done.                                      
    Platform platform = Platform::Continuous;                                           ///< The microfluidic platform that is simulated in this simulation.
    arch::Network<T>* network;                                                          ///< Network for which the simulation should be conducted.
    std::shared_ptr<nodal::NodalAnalysis<T>> nodalAnalysis;                             ///< The nodal analysis object, used to conduct abstract simulation.
    std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids;                          ///< Fluids specified for the simulation.
    std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets;                      ///< Droplets which are simulated in droplet simulation.
    std::unordered_map<int, std::unique_ptr<Specie<T>>> species;                        ///< Species specified for the simulation.
    std::unordered_map<int, std::shared_ptr<Tissue<T>>> tissues;                        ///< Tissues specified for the simulation.
    std::unordered_map<int, std::unique_ptr<DropletInjection<T>>> dropletInjections;    ///< Injections of droplets that should take place during a droplet simulation.
    std::unordered_map<int, std::unique_ptr<Mixture<T>>> mixtures;                      ///< Mixtures present in the simulation.
    std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>> mixtureInjections;    ///< Injections of fluids that should take place during the simulation.
    std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>> cfdSimulators;            ///< The set of CFD simulators, that conduct CFD simulations on <arch::Module>.
    ResistanceModel<T>* resistanceModel;                                                ///< The resistance model used for the simulation.
    MembraneModel<T>* membraneModel;                                                    ///< The membrane model used for an OoC simulation.
    MixingModel<T>* mixingModel;                                                        ///< The mixing model used for a mixing simulation.
    std::unordered_map<int, std::shared_ptr<mmft::Scheme<T>>> updateSchemes;            ///< The update scheme for Abstract-CFD coupling
    int continuousPhase = 0;                                                            ///< Fluid of the continuous phase.
    int iteration = 0;
    int maxIterations = 1e5;
    T maximalAdaptiveTimeStep = 0;                                                      ///< Maximal adaptive time step that is applied when droplets change the channel.
    T time = 0.0;                                                                       ///< Current time of the simulation.
    T dt = 0.01;
    T writeInterval = 0.1;
    T tMax = 100;
    bool writePpm = true;
    bool eventBasedWriting = false;
    bool dropletsAtBifurcation = false;                                  ///< If one or more droplets are currently at a bifurcation. Triggers the usage of the maximal adaptive time step.
    std::unique_ptr<result::SimulationResult<T>> simulationResult = nullptr;

    /**
     * @brief Initializes the resistance model and the channel resistances of the empty channels.
     */
    void initialize();

    /**
     * @brief Update the droplet resistances of the channels based on the current positions of the droplets.
     */
    void updateDropletResistances();

    /**
     * @brief Compute all possible next events.
     */
    std::vector<std::unique_ptr<Event<T>>> computeEvents();

    /**
     * @brief Compute all possible next events for mixing simulation.
     */
    std::vector<std::unique_ptr<Event<T>>> computeMixingEvents();

    /**
     * @brief Moves all droplets according to the given time step.
     * @param[in] timeStep to which the droplets should be moved to.
     */
    void moveDroplets(T timeStep);

    /**
     * @brief Store simulation parameters to the result.
     * @param[in, out] result Reference to the simulation result in which all current parameters of the simulation should be stored.
     */
    void storeSimulationParameters(result::SimulationResult<T>& result);

    /**
     * @brief Store all simulation results to the result.
     * @param[in, out] result Reference to the simulation result in which all current parameters of the simulation should be stored.
     */
    void storeSimulationResults(result::SimulationResult<T>& result);

    /**
     * @brief Store the current simulation state in simulationResult.
    */
    void saveState();

    /**
     * @brief Store the mixtures in this simulation in simulationResult.
    */
    void saveMixtures();

public:
    /**
     * @brief Creates simulation.
     */
    Simulation();

    /**
     * @brief Create fluid.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] concentration Concentration of the fluid in % (between 0.0 and 1.0).
     * @return Pointer to created fluid.
     */
    Fluid<T>* addFluid(T viscosity, T density, T concentration);

    /**
     * @brief Create droplet.
     * @param[in] fluidId Unique identifier of the fluid the droplet consists of.
     * @param[in] volume Volume of the fluid in m^3.
     * @return Pointer to created droplet.
     */
    Droplet<T>* addDroplet(int fluidId, T volume);

    /**
     * @brief Create specie.
     * @param[in] diffusivity Diffusion coefficient of the specie in the carrier medium in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in the carrier medium in g/m^3.
     * @return Pointer to created specie.
     */
    Specie<T>* addSpecie(T diffusivity, T satConc);

    /**
     * @brief Create tissue.
     * @param[in] species Map of Species that interacts with this tissue.
     * @param[in] Vmax
     * @param[in] kM
     * @return Pointer to created tissue.
     */
    Tissue<T>* addTissue(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> Vmax, std::unordered_map<int, T> kM);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] species Unordered map of specie ids and pointer to that specie.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);
    
    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for all modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(int moduleId, T alpha, T beta, int theta);

    /**
     * @brief Define and set the naive update scheme for a hybrid simulation.
     * @param[in] alpha The relaxation value for the pressure value update for all nodes of the module.
     * @param[in] beta The relaxation value for the flow rate value update for all nodes of the module.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for the modules.
     * @returns A shared_ptr to the created naive update scheme.
     */
    std::shared_ptr<mmft::NaiveScheme<T>> setNaiveHybridScheme(int moduleId, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Create injection.
     * @param[in] dropletId Id of the droplet that should be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s.
     * @param[in] channelId Id of the channel, where droplet should be injected.
     * @param[in] injectionPosition Position inside the channel at which the droplet should be injected (relative to the channel length between 0.0 and 1.0).
     * @return Pointer to created injection.
     */
    DropletInjection<T>* addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition);

    /**
     * @brief Create mixture injection. The injection is always performed at the beginning (position 0.0) of the channel.
     * @param[in] mixtureId Id of the mixture that should be injected.
     * @param[in] channelId Id of the channel, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @return Pointer to created injection.
     */
    MixtureInjection<T>* addMixtureInjection(int mixtureId, int channelId, T injectionTime);

    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
     * @return Pointer to the newly created simulator.
    */
    lbmSimulator<T>* addLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings, 
                                    T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] species Map of specieIds and speciePtrs of the species simulated in the AD fields of this simulator.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
     * @return Pointer to the newly created simulator.
    */
    lbmMixingSimulator<T>* addLbmMixingSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
                                            std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);


    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] tissueId The Id of the tissue that the organ in this nodule consists of.
     * @param[in] organStlFile The location of the stl file describing the geometry of the organ.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] species Map of specieIds and speciePtrs of the species simulated in the AD fields of this simulator.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
     * @return Pointer to the newly created simulator.
    */
    lbmOocSimulator<T>* addLbmOocSimulator(std::string name, std::string stlFile, int tissueId, std::string organStlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, Specie<T>*> species,
                                            std::unordered_map<int, arch::Opening<T>> openings, T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

    /**
     * @brief Adds a new simulator to the network.
     * @param[in] name Name of the simulator.
     * @param[in] module Shared pointer to the module on which this solver acts.
     * @param[in] openings Map of openings corresponding to the nodes.
    */
    essLbmSimulator<T>* addEssLbmSimulator(std::string name, std::string stlFile, std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, arch::Opening<T>> openings,
                                        T charPhysLength, T charPhysVelocity, T resolution, T epsilon, T tau);

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

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    void setFixtureId(int fixtureId);

    /**
     * @brief Set the network for which the simulation should be conducted.
     * @param[in] network Network on which the simulation will be conducted.
     */
    void setNetwork(arch::Network<T>* network);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setFluids(std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setDroplets(std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluidId The ID of the fluid the continuous phase consists of.
     */
    void setContinuousPhase(int fluidId);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    void setContinuousPhase(Fluid<T>* fluid);

    /**
     * @brief Define which resistance model should be used for the channel and droplet resistance calculations.
     * @param[in] model The resistance model to be used.
     */
    void setResistanceModel(ResistanceModel<T>* model);

    /**
     * @brief Define which mixing model should be used for the concentrations.
     * @param[in] model The mixing model to be used.
     */
    void setMixingModel(MixingModel<T>* model);

    /**
     * @brief Calculate and set new state of the continuous fluid simulation. Move mixture positions and create new mixtures if necessary.
     * @param[in] timeStep Time step in s for which the new mixtures state should be calculated.
     */
    void calculateNewMixtures(double timeStep);

    /**
     * @brief Get the platform of the simulation.
     * @return platform of the simulation
     */
    Platform getPlatform();

    /**
     * @brief Get the type of the simulation.
     * @return type of the simulation
     */
    Type getType();

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    int getFixtureId();

    /**
     * @brief Get the network.
     * @return Network or nullptr if no network is specified.
     */
    arch::Network<T>* getNetwork();

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    Fluid<T>* getFluid(int fluidId);

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    std::unordered_map<int, std::unique_ptr<Fluid<T>>>& getFluids();

    /**
     * @brief Get droplet
     * @param dropletId Id of the droplet
     * @return Pointer to droplet with the corresponding id
     */
    Droplet<T>* getDroplet(int dropletId);

    /**
     * @brief Gets droplet that is present at the corresponding node (i.e., the droplet spans over this node).
     * @param nodeId The id of the node
     * @return Pointer to droplet or nullptr if no droplet was found
     */
    Droplet<T>* getDropletAtNode(int nodeId);

    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    DropletInjection<T>* getDropletInjection(int injectionId);

    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    MixtureInjection<T>* getMixtureInjection(int injectionId);

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>>& getMixtureInjections();

    /**
     * @brief Get injection
     * @param simulatorId The id of the injection
     * @return Pointer to injection with the corresponding id.
     */
    CFDSimulator<T>* getCFDSimulator(int simulatorId);

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& getCFDSimulators();

    /**
     * @brief Get the continuous phase.
     * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
     */
    Fluid<T>* getContinuousPhase();

    /**
     * @brief Get the mixing model that is used in the simulation.
     * @return The mixing model of the simulation.
     */
    MixingModel<T>* getMixingModel();

    /**
     * @brief Get the resistance model that is used in the simulation.
     * @return The resistance model of the simulation.
     */
    ResistanceModel<T>* getResistanceModel();

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture
     * @return Pointer to mixture with the correspondig id
     */
    Mixture<T>* getMixture(int mixtureId);

    /**
     * @brief Get mixtures.
     * @return Reference to the unordered map of mixtures
     */
    std::unordered_map<int, std::unique_ptr<Mixture<T>>>& getMixtures();

    /**
     * @brief Get specie.
     * @param specieId Id of the specie.
     * @return Pointer to specie with the correspondig id
     */
    Specie<T>* getSpecie(int specieId);

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture
     * @return Pointer to mixture with the correspondig id
     */
    std::unordered_map<int, std::unique_ptr<Specie<T>>>& getSpecies();

    /**
     * @brief Get the results of the simulation.
     * @return Pointer to the result of the simulation.
     */
    result::SimulationResult<T>* getSimulationResults();

    /**
     * @brief Get the global bounds of pressure values in the CFD simulators.
     * @return A tuple with the global bounds for pressure values <pMin, pMax>
     */
    std::tuple<T, T> getGlobalPressureBounds();
    
    /**
     * @brief Get the global bounds of velocity magnitude values in the CFD simulators.
     * @return A tuple with the global bounds for velocity magnitude values <velMin, velMax>
     */
    std::tuple<T, T> getGlobalVelocityBounds();

    /**
     * @brief Creates a new fluid out of two existing fluids.
     * @param fluid0Id Id of the first fluid.
     * @param volume0 The volume of the first fluid.
     * @param fluid1Id Id of the second fluid.
     * @param volume1 The volume of the second fluid.
     * @return Pointer to new fluid.
     */
    Fluid<T>* mixFluids(int fluid0Id, T volume0, int fluid1Id, T volume1);

    /**
     * @brief Creates a new droplet from two existing droplets. Please note that this only creates a new 
     * droplet inside the simulation, but the actual boundaries have to be set separately, which is usually 
     * done inside the corresponding merge events.
     * @param droplet0Id Id of the first droplet.
     * @param droplet1Id Id of the second droplet.
     * @return Pointer to new droplet.
     */
    Droplet<T>* mergeDroplets(int droplet0Id, int droplet1Id);

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
     * @brief Write the pressure field in .ppm image format for all cfdSimulators
     */
    void writePressurePpm(std::tuple<T, T> bounds, int resolution=600);

    /**
     * @brief Write the velocity field in .ppm image format for all cfdSimulators
     */
    void writeVelocityPpm(std::tuple<T, T> bounds, int resolution=600);
};

}   // namespace sim
