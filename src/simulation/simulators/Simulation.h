/**
 * @file Simulation.h
 */

#pragma once

#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <optional>
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
    std::shared_ptr<arch::Network<T>> network = nullptr;                                ///< Network for which the simulation should be conducted.
    std::unique_ptr<ResistanceModel<T>> resistanceModel = nullptr;                      ///< The resistance model used for the simulation.
    std::shared_ptr<nodal::NodalAnalysis<T>> nodalAnalysis = nullptr;                   ///< The nodal analysis object, used to conduct abstract simulation.
    std::unordered_map<size_t, std::shared_ptr<Fluid<T>>> fluids;                       ///< Fluids specified for the simulation.
    int fixtureId = 0;
    int continuousPhase = 0;                                                            ///< Fluid of the continuous phase.
    size_t iteration = 0;
    size_t maxIterations = 1e5;
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
     * @param[in] simType_ Type of the simulation.
     * @param[in] platform_ Platform of the simulation.
     * @param[in] network_ Pointer to the network on which the simulation is conducted.
     * @throws std::logic_error if the network is null.
     * @note network must not be null.
     */
    Simulation(Type simType, Platform platform, std::shared_ptr<arch::Network<T>> network);

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
    virtual std::optional<bool> conductNodalAnalysis() { nodalAnalysis->conductNodalAnalysis(); return std::nullopt; }

    /**
     * @brief Returns a reference to the nodalAnalysis shared_ptr.
     */
    inline nodal::NodalAnalysis<T>* getNodalAnalysis() const { return nodalAnalysis.get(); }

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
     * @brief Returns the maximal adaptive timestep size.
     */
    inline double getMaximalAdaptiveTimeStep() { return maximalAdaptiveTimeStep; }

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     */
    inline void setFluids(std::unordered_map<size_t, std::shared_ptr<Fluid<T>>> fluids) { this->fluids = std::move(fluids); }

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     */
    const std::unordered_map<size_t, std::shared_ptr<Fluid<T>>>& getFluids() const;

    /**
     * @brief Get the resistance model that is used in the simulation.
     * @return The resistance model of the simulation.
     */
    inline const ResistanceModel<T>* getResistanceModel() const { return resistanceModel.get(); }

    /**
     * @brief Get the results of the simulation.
     * @return Pointer to the result of the simulation.a
     */
    inline result::SimulationResult<T>* getSimulationResults() const { return simulationResult.get(); }

    /**
     * @brief Get the current iteration number.
     */
    inline size_t& getIterations() { return iteration; }

    /**
     * @brief Removes a fluid from the simulation, based on the fluid id
     * @param[in] fluidId The id of the fluid that is to be removed
     * @throws logic_error if the fluidId is the continuous phase or is not listed in the simulator.
    */
    void removeFluid(int fluidId);

    /**
     * @brief Get the hash of the simulation.
     * @return Hash of the simulation.
     */
    [[nodiscard]] inline size_t getHash() const { return std::hash<const Simulation<T>*>{}(this); }

public:

    /**
     * @brief Get the platform (read-only value) of the simulation.
     * @return platform of the simulation
     */
    [[nodiscard]] inline const Platform& getPlatform() const noexcept { return this->platform; }

    /**
     * @brief Get the type (read-only value) of the simulation.
     * @return type of the simulation
     */
    [[nodiscard]] inline const Type& getType() const noexcept { return this->simType; }

    /**
     * @brief Get the network.
     * @return Network or nullptr if no network is specified.
     */
    [[nodiscard]] inline std::shared_ptr<arch::Network<T>> getNetwork() const { return this->network; }

    /**
     * @brief Set the network for which the simulation should be conducted.
     * @param[in] network Network on which the simulation will be conducted.
     */
    inline void setNetwork(std::shared_ptr<arch::Network<T>> network) { this->network = network; }

    /**
     * @brief Sets the resistance model for abstract simulation components to the 1D resistance model
     */
    virtual void set1DResistanceModel();

    /**
     * @brief Sets the resistance model for abstract simulation components to the poiseuille resistance model
     */
    virtual void setPoiseuilleResistanceModel();

    /**
     * @brief Create fluid and add to the simulation.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     * @param[in] density Density of the fluid in kg/m^3.
     * @return Pointer to created fluid.
     */
    [[maybe_unused]] std::shared_ptr<Fluid<T>> addFluid(T viscosity, T density);

    /**
     * @brief Creates and adds a new fluid out of two existing fluids.
     * @param fluid0Id Id of the first fluid.
     * @param volume0 The volume of the first fluid.
     * @param fluid1Id Id of the second fluid.
     * @param volume1 The volume of the second fluid.
     * @return Pointer to new fluid.
     */
    std::shared_ptr<Fluid<T>> addMixedFluid(int fluid0Id, T volume0, int fluid1Id, T volume1);

    /**
     * @brief Creates and adds a new fluid out of two existing fluids.
     * @param fluid0 Pointer to the first fluid.
     * @param volume0 The volume of the first fluid.
     * @param fluid1 Pointer to the second fluid.
     * @param volume1 The volume of the second fluid.
     * @return Pointer to new fluid.
     */
    std::shared_ptr<Fluid<T>> addMixedFluid(const std::shared_ptr<Fluid<T>>& fluid0, T volume0, const std::shared_ptr<Fluid<T>>& fluid1, T volume1);

    /**
     * @brief Get fluid.
     * @param[in] fluidId Id of the fluid
     * @return Pointer to fluid with the corresponding id
     * @throws logic error if the fluidId is not found
     */
    [[nodiscard]] std::shared_ptr<Fluid<T>> getFluid(int fluidId) const;

    /**
     * @brief Return a read-only map of fluids currently stored in the simulation
     * @return Unordered map of fluid ids and const pointers to the fluids
     */
    [[nodiscard]] const std::unordered_map<size_t, const Fluid<T>*> readFluids() const;

    /** TODO: Miscellaneous
     * reset the simHash
     */
    /**
     * @brief Removes a fluid from the simulation, based using the fluid ptr. 
     * @param[in] fluid Pointer to the fluid that is to be removed
     * @throws logic_error if the fluidId is the continuous phase or is not listed in the simulator.
    */
    virtual void removeFluid(const std::shared_ptr<Fluid<T>>& fluid);

    /**
     * @brief Set the type of the simulation.
     * @param[in] type
     */
    [[nodiscard]] inline int getFixtureId() const { return this->fixtureId; }

    /**
     * @brief Set the fixtureId.
     * @param[in] fixtureId
     */
    inline void setFixtureId(int fixtureId) { this->fixtureId = fixtureId; }

    /**
     * @brief Get the continuous phase.
     * @return Fluid if the continuous phase or nullptr if no continuous phase is specified.
     */
    [[nodiscard]] std::shared_ptr<Fluid<T>> getContinuousPhase() const;

    /** TODO: AbstractDroplet
     * If a new continuous phase is set, the droplets should be adapted to not be the new continuous phase.
     ** TODO: AbstractMixing 
     * If a new continuous phase is set, the continuous phase of the Mixtures should be adapted to the new continuous phase.
     */

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluidId The ID of the fluid the continuous phase consists of.
     * @throws logic error if the fluidId is not found
     */
    void setContinuousPhase(int fluidId);

    /**
     * @brief Define which fluid should act as continuous phase, i.e., as carrier fluid for the droplets.
     * @param[in] fluid The fluid the continuous phase consists of.
     * @throws logic error if the fluid is not listed
     */
    void setContinuousPhase(const std::shared_ptr<Fluid<T>>& fluid);

    /**
     * @brief Get the current iteration number.
     */
    [[nodiscard]] inline size_t getCurrentIteration() { return iteration; }

    /**
     * @brief Returns the maxmimum allowed simulation iterations.
     */
    [[nodiscard]] inline size_t getMaxIterations() { return maxIterations; }

    /**
     * @brief Set the maximum amount of iterations for the simulation.
     * The simulation ends once it reaches the maximum amount of iterations.
     * @param[in] maxIterations The number of maximum iterations
     */
    inline void setMaxIterations(size_t maxIterations) { this->maxIterations = maxIterations; }

    /**
     * @brief Set interval in which the state is saved to the SimulationResult.
     * @param[in] Interval in [s].
     */
    void inline setWriteInterval(T interval) { this->writeInterval = interval; }

    /**
     * @brief Returns the maximal allowed time, tMax.
     */
    [[nodiscard]] inline double getTMax() { return tMax; }

    /**
     * @brief Set maximal time after which the simulation should end.
     * @param[in] Maximal end time in [s].
     */
    inline void setMaxEndTime(T maxTime) { this->tMax = maxTime; }

    /**
     * @brief Get the results of the simulation.
     * @return Pointer to the result of the simulation.
     */
    [[nodiscard]] inline const result::SimulationResult<T>* getResults() const { return simulationResult.get(); }

    /**
     * @brief Print the results as pressure at the nodes and flow rates at the channels
     */
    virtual void printResults() const;

    /**
     * @brief Conduct the simulation.
     * @return The result of the simulation containing all intermediate simulation steps and calculated parameters.
     */
    virtual void simulate() = 0;

    /**
     * @brief Mandatory virtual destructor is set to default.
     */
    virtual ~Simulation() = default;

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

};

}   // namespace sim
