/**
 * @file MixingModels.h
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace arch { 

// Forward declared dependencies
template<typename T>
class Network;

template<typename T>
class Membrane;

template<typename T>
class Tank;

}

namespace sim { 

// Forward declared dependencies
template<typename T>
class AbstractMixing;

template<typename T>
class Fluid;

template<typename T>
class Mixture;

template<typename T>
class Specie;

// Structure to define the mixture inflow into a node
template<typename T>
struct MixtureInFlow {

    int mixtureId;
    T inflowVolume;

};

template<typename T>
struct RadialPosition {
    T radialAngle;
    int channelId;
    bool inFlow;
};

template<typename T>
struct FlowSection {
    int channelId;  // Channel of this flow coming into the node
    T sectionStart; // Start of the relevant section of this inflow (relative, 0.0-1.0)
    T sectionEnd;   // End of the relevant section of this inflow (relative, 0.0-1.0)
    T flowRate;
    T width;
};

template<typename T>
struct FlowSectionInput {
    T startWidth;
    T endWidth;
    T scaleFactor; // this is technically redundant for constant flow sections
    T translateFactor;
    T concentrationAtChannelEnd; // concentration if it is a constant flow section
    std::function<T(T)> concentrationAtChannelEndFunction;
    std::vector<T> segmentedResult; // this is technically redundant for constant flow sections
    T a_0_old;
};

/**
 * @brief Virtual class that describes the basic functionality for mixing models.
*/
template<typename T>
class MixingModel {
protected:

    T minimalTimeStep = 0.0;                                                    ///< Required minimal timestep for a mixture to reach a node.
    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>
    std::unordered_map<int, int> filledEdges;                                   ///< Which edges are currently filled with a single mixture <EdgeID, MixtureID>
    std::unordered_multimap<int, int> permanentMixtureInjections;               ///< Permanent mixture injections which are currently active, <ChannelID, MixtureIDs>

public:

    /**
     * @brief Constructor of a mixing model.
    */
    MixingModel();

    virtual ~MixingModel() = default;

    /**
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    virtual void propagateSpecies(arch::Network<T>* network, AbstractMixing<T>* sim) = 0;

    /**
     * @brief Returns the current minimal timestep.
     * @return The minimal timestep in s.
    */
    T getMinimalTimeStep();

    /**
     * @brief Update the minimal timestep for a mixture to 'outflow' their channel.
     * @param[in] network
    */
    void updateMinimalTimeStep(arch::Network<T>* network);

    /**
     * @brief Calculate and set fixed minimal timestep according to shortest channel.
     * This can cause multiple mixtures to outflow the channel in a timestep and should
     * only be used for simulations that can handle this correctly.
     * @param[in] network
     */
    void fixedMinimalTimeStep(arch::Network<T>* network);

    /**
     * @brief Limit current minimal timestep to given value.
     * This can be used to ensure that the simulation can be saved at a specific simulation time.
     * @param[in] minMinimalTimeStep Lower bound for current timestep.
     * @param[in] maxMinimalTimeStep Upper bound for current timestep.
     */
    void limitMinimalTimeStep(T minMinimalTimeStep, T maxMinimalTimeStep);

    /**
     * @brief Retrieve the mixtures that are present in a specific channel.
     * @param[in] channelId The channel id.
     * @return A reference to the deque containing the mixtures and their location in the channel.
    */
    const std::deque<std::pair<int,T>>& getMixturesInEdge(int channelId) const;

    /**
     * @brief Retrieve all mixtures in all edges.
     * @return The unordered map of channel ids and deques containing the mixtures and their location per channel.
    */
    const std::unordered_map<int, std::deque<std::pair<int,T>>>& getMixturesInEdges() const;

    /**
     * @brief Retrieve the edges that are filled (one mixture has end position 1.0) and the mixture that is in the front of the channel.
     * @return The unordered map of channel ids and the mixture ids.
    */
    const std::unordered_map<int, int>& getFilledEdges() const;

    /**
     * @brief Insert a mixture at the back of the mixtures (deque) for a channel.
     * @param[in] mixtureId Id of the mixture.
     * @param[in] channelId Id of the channel.
     * @param[in] endPos Injection position of the mixture.
    */
    void injectMixtureInEdge(int mixtureId, int channelId, T endPos = 0.0);

    /**
     * @brief Add permanent mixture injection to current simulation run.
     * @param[in] mixtureId Id of the mixture that should be injected continuously from now on.
     * @param[in] channelId Id of the channel into which the injection is leading.
     */
    void addPermanentMixtureInjection(int mixtureId, int channelId) { permanentMixtureInjections.insert({channelId, mixtureId}); }

    /**
     * @brief Update the position of all mixtures in the network and update the inflow into all nodes.
     * @param[in] timeStep the current timestep size.
     * @param[in] network pointer to the network.
    */
    virtual void updateNodeInflow(T timeStep, arch::Network<T>* network) = 0;

    /**
     * @brief Create and/or propagate mixtures into channels downstream.
     * @param[in] timeStep the current timestep size.
     * @param[in] network pointer to the network.
     * @param[in] sim pointer to the simulation.
     * @param[in] mixtures reference to the unordered map of mixtures.
    */
    virtual void updateMixtures(T timeStep, arch::Network<T>* network, AbstractMixing<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) = 0;

    virtual bool isInstantaneous() = 0;

    virtual bool isDiffusive() = 0;
};

/**
 * @brief Class that describes the instantaneous mixing model. This mixing model, assumes that mixtures become fully mixed inside nodes.
*/
template<typename T>
class InstantaneousMixingModel : public MixingModel<T> {

private:

    std::unordered_map<int, std::vector<MixtureInFlow<T>>> mixtureInflowAtNode;     ///< Unordered map to track mixtures flowing into nodes <nodeId <mixtureId, inflowVolume>>
    std::unordered_map<int, int> mixtureOutflowAtNode;                              ///< Unordered map to track mixtures flowing out of nodes <nodeId, mixtureId>.
    std::unordered_map<int, T> totalInflowVolumeAtNode;                             ///< Unordered map to track the total volumetric flow entering a node.
    std::unordered_map<int, bool> createMixture;                                    ///< Unordered map to track whether a new mixture is created at a node.

    int generateInflows(int nodeId, T timeStep, arch::Network<T>* network);

public:

    /**
     * @brief Constructor of an instantaneous mixing model.
    */
    InstantaneousMixingModel();

    /**
     * @brief Create and/or propagate mixtures into channels downstream.
     * @param[in] timeStep the current timestep size.
     * @param[in] network pointer to the network.
     * @param[in] sim pointer to the simulation.
     * @param[in] mixtures reference to the unordered map of mixtures.
    */
    void updateMixtures(T timeStep, arch::Network<T>* network, AbstractMixing<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) override;

    /**
     * @brief Move mixtures according to the timestep
     * @param[in] timeStep Current time step in [s].
     * @param[in] network Network of the simulation.
     */
    void moveMixtures(T timeStep, arch::Network<T>* network);

    /**
     * @brief Calculate exchange between tank and channel through membranes and change mixtures accordingly
     * @param[in] timeStep Current time step in [s].
     * @param[in] sim Pointer to the simulation.
     * @param[in] network Pointer to the network of the simulation.
     * @param[in] mixtures Reference to collection containing all mixtures in the simulation
     */
    void calculateMembraneExchange(T timeStep, AbstractMixing<T>* sim, arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Calculate and store the mixtures flowing into all nodes
     * @param[in] timeStep The timeStep size of the current iteration.
     * @param[in] network Pointer to the network.
    */
    void updateNodeInflow(T timeStep, arch::Network<T>* network) override;

    /**
     * @brief Calculate the mixture outflow at each node from all inflows and, when necessary, create new mixtures.
     * @param[in] sim Pointer to the simulation.
     * @param[in] mixtures Unordered map of the mixtures in the system.
    */
    void generateNodeOutflow(AbstractMixing<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Add the node outflow as inflow to the channels
     * @param[in] timeStep The timeStep size of the current iteration.
     * @param[in] sim Pointer to the simulation.
     * @param[in] network Pointer to the network.
     * @param[in] mixtures Unordered map of the mixtures in the system.
    */
    void updateChannelInflow(T timeStep, AbstractMixing<T>* sim, arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Remove mixtures that have 'outflowed' their channel
     * @param[in] network Pointer to the network.
    */
    void clean(arch::Network<T>* network);

    /**
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    void propagateSpecies(arch::Network<T>* network, AbstractMixing<T>* sim) override;

    /**
     * @brief From the mixtureInjections and CFD simulators, generate temporary mxtures that 
     * flow into the network at correspondingnode entry points.
     * @param[in] sim Pointer to the simulation.
     */
    void initNodeOutflow(AbstractMixing<T>* sim, std::vector<Mixture<T>>& tmpMixtures);

    /**
     * @brief Propagate the mixtures through the corresponding channel entirely, without considering time steps
     */
    void channelPropagation(arch::Network<T>* network);

    /**
     * @brief From the node's inflows, generate the node outflow
     */
    bool updateNodeOutflow(AbstractMixing<T>* sim, std::vector<Mixture<T>>& tmpMixtures);

    void storeConcentrations(AbstractMixing<T>* sim, const std::vector<Mixture<T>>& tmpMixtures);

    /**
     * @brief Print all mixtures and their positions in the network.
    */
    void printMixturesInNetwork();

    bool isInstantaneous() override { return 1; };

    bool isDiffusive() override { return 0; };

};

template<typename T>
class DiffusionMixingModel : public MixingModel<T> {

private:
    int resolution = 10;
    std::set<int> mixingNodes;
    std::vector<std::vector<RadialPosition<T>>> concatenatedFlows;
    std::unordered_map<int, std::vector<FlowSection<T>>> outflowDistributions;
    std::unordered_map<int, int> filledEdges;                                   ///< Which edges are currently filled and what mixture is at the front <EdgeID, MixtureID>
    void generateInflows();

public:

    DiffusionMixingModel();

    /**
     * @brief Create and/or propagate mixtures into channels downstream.
     * @param[in] timeStep the current timestep size.
     * @param[in] network pointer to the network.
     * @param[in] sim pointer to the simulation.
     * @param[in] mixtures reference to the unordered map of mixtures.
    */
    void updateMixtures(T timeStep, arch::Network<T>* network, AbstractMixing<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) override;

    /**
     * @brief Propagate the mixtures and check if a mixtures reaches channel end.
    */
    void updateNodeInflow(T timeStep, arch::Network<T>* network) override;

    /**
     * @brief Generate a new inflow in case a mixture has reached channel end. Invoked by updateNodeInflow.
    */
    void generateInflows(T timeStep, arch::Network<T>* network, AbstractMixing<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    void topologyAnalysis(arch::Network<T>* network, int nodeId);

    /**
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    void propagateSpecies(arch::Network<T>* network, AbstractMixing<T>* sim) override;
    
    void printTopology();

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionConstant(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters);

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionFunction(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters, std::function<T(T)> fConstant);

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionTotal(T channelLength, T currChannelFlowRate, T channelWidth, int resolution, int speciesId, T pecletNr, 
        const std::vector<FlowSection<T>>& flowSections, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& diffusiveMixtures);

    void clean(arch::Network<T>* network);

    void printMixturesInNetwork();

    std::vector<std::vector<RadialPosition<T>>>& getConcatenatedFlows() { return concatenatedFlows; }

    std::unordered_map<int, std::vector<FlowSection<T>>>& getOutflowDistributions() { return outflowDistributions; }

    bool isInstantaneous() override { return 0; };

    bool isDiffusive() override { return 1; };

};

}   // namespace sim