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
class Network;

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
class Fluid;

template<typename T>
class ResistanceModel;

enum class Type {
    Abstract,       ///< A simulation in the 1D abstraction level
    Hybrid,         ///< A simulation combining the 1D and CFD abstraction levels
    CFD             ///< A simulation in the CFD abstraction level
};

enum class Platform {
    Continuous,     ///< A simulation with a single continuous fluid.
    BigDroplet,     ///< A simulation with droplets filling a channel cross-section
    Mixing,         ///< A simulation with multiple miscible fluids.
    Ooc,            ///< A simulation with organic tissue
    Membrane,       ///< A simulation with membranes
};

/**
 * @brief Class that conducts the simulation and owns all parameters necessary for it.
 */
template<typename T>
class Simulation {
private:
    const Type simType;                                                                 ///< The type of simulation that is being done.                                      
    const Platform platform;                                                            ///< The microfluidic platform that is simulated in this simulation.
    arch::Network<T>* network = nullptr;                                                ///< Network for which the simulation should be conducted.
    ResistanceModel<T>* resistanceModel = nullptr;                                      ///< The resistance model used for the simulation.
    std::shared_ptr<nodal::NodalAnalysis<T>> nodalAnalysis = nullptr;                   ///< The nodal analysis object, used to conduct abstract simulation.
    std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids;                          ///< Fluids specified for the simulation.
    int fixtureId = 0;
    int continuousPhase = 0;                                                            ///< Fluid of the continuous phase.
    int iteration = 0;
    int maxIterations = 1e5;
    T maximalAdaptiveTimeStep = 0;                                                      ///< Maximal adaptive time step that is applied when droplets change the channel.
    T time = 0.0;                                                                       ///< Current time of the simulation.
    T dt = 0.01;
    T writeInterval = 0.1;
    T tMax = 100;
    std::unique_ptr<result::SimulationResult<T>> simulationResult = nullptr;

    // Disabled because no stable simulator uses tissues
    // std::unordered_map<int, std::shared_ptr<Tissue<T>>> tissues;                        ///< Tissues specified for the simulation.

protected:
    /**
     * @brief Creates simulation.
     */
    Simulation(Type simType, Platform platform, arch::Network<T>* network);

    /**
     * @brief Asserts that the simulation is initialized.
     * This means that at least the network and resistance model, and continuous phase are set.
     * @throws std::logic_error if the simulation is not initialized correctly.
     * @note This function is called at the start of the simulate() function.
     */
    virtual void assertInitialized() const;

    /**
     * @brief Initializes the resistance model and the channel resistances of the empty channels.
     */
    virtual void initialize();

    /**
     * @brief Store the current simulation state in simulationResult.
    */
    virtual void saveState() = 0;

    /**
     * @brief Wrapper function that conducts the nodal analysis of the nodalAnalysis object.
     */
    inline void conductNodalAnalysis() { nodalAnalysis->conductNodalAnalysis(); }

    /**
     * @brief Returns a reference to the nodalAnalysis shared_ptr.
     */
    inline std::shared_ptr<nodal::NodalAnalysis<T>>& getNodalAnalysis() { return nodalAnalysis; }

    /**
     * @brief Returns a reference to the simulation time.
     */
    inline double& getTime() { return time; }

    /**
     * @brief Returns the set write interval of the simulation.
     */
    inline double getWriteInterval() { return writeInterval; }

    /**
     * @brief Returns the timestep size dt.
     */
    inline double& getDt() { return dt; }

        /**
     * @brief Returns the maximal allowed time, tMax.
     */
    inline double getTMax() { return tMax; }

    /**
     * @brief Returns the maximal adaptive timestep size.
     */
    inline double getMaximalAdaptiveTimeStep() { return maximalAdaptiveTimeStep; }

    /**
     * @brief Get the current iteration number.
     */
    inline int& getIterations() { return iteration; }

    /**
     * @brief Returns the maxmimum allowed simulation iterations.
     */
    inline int getMaxIterations() { return maxIterations; }

public:


    /**
     * @brief Create fluid.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] concentration Concentration of the fluid in % (between 0.0 and 1.0).
     * @return Pointer to created fluid.
     */
    Fluid<T>* addFluid(T viscosity, T density, T concentration);

    /** TODO: HybridOocSimulation
     * Enable definition of Tissue objects for OoC simulation
     */
    // /**
    //  * @brief Create tissue.
    //  * @param[in] species Map of Species that interacts with this tissue.
    //  * @param[in] Vmax
    //  * @param[in] kM
    //  * @return Pointer to created tissue.
    //  */
    // Tissue<T>* addTissue(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> Vmax, std::unordered_map<int, T> kM);

    /**
     * @brief Set the fixtureId.
     * @param[in] fixtureId
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
     * @brief Set maximal time after which the simulation should end.
     * @param[in] Maximal end time in [s].
     */
    void setMaxEndTime(T maxTime);

    /**
     * @brief Set interval in which the state is saved to the SimulationResult.
     * @param[in] Interval in [s].
     */
    void setWriteInterval(T interval);

    /**
     * @brief Get the platform of the simulation.
     * @return platform of the simulation
     */
    Platform getPlatform() const;

    /**
     * @brief Get the type of the simulation.
     * @return type of the simulation
     */
    Type getType() const;

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    int getFixtureId() const;

    /**
     * @brief Get the network.
     * @return Network or nullptr if no network is specified.
     */
    arch::Network<T>* getNetwork() const;

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    Fluid<T>* getFluid(int fluidId) const;

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    const std::unordered_map<int, std::unique_ptr<Fluid<T>>>& getFluids() const;

    /**
     * @brief Get the continuous phase.
     * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
     */
    Fluid<T>* getContinuousPhase() const;

    /**
     * @brief Get the resistance model that is used in the simulation.
     * @return The resistance model of the simulation.
     */
    ResistanceModel<T>* getResistanceModel() const;

    /**
     * @brief Get the results of the simulation.
     * @return Pointer to the result of the simulation.
     */
    result::SimulationResult<T>* getSimulationResults() const;

    /**
     * @brief Conduct the simulation.
     * @return The result of the simulation containing all intermediate simulation steps and calculated parameters.
     */
    virtual void simulate() = 0;

    /**
     * @brief Print the results as pressure at the nodes and flow rates at the channels
     */
    virtual void printResults() const;

    /**
     * @brief Mandatory virtual destructor is set to default.
     */
    virtual ~Simulation() = default;
};

}   // namespace sim
