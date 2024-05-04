/**
 * @file MixingModels.h
 */

#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

namespace arch { 

// Forward declared dependencies
template<typename T>
class Network;

}

namespace sim { 

// Forward declared dependencies
template<typename T>
class Fluid;

template<typename T>
class Mixture;

template<typename T>
class Simulation;

template<typename T>
class Specie;

// Structure to define the mixture inflow into a node
template<typename T>
struct MixtureInFlow {

    int mixtureId;
    T inflowVolume;

};

/**
 * @brief Virtual class that describes the basic functionality for mixing models.
*/
template<typename T>
class MixingModel {
protected:

    T minimalTimeStep = 0.0;                                                    ///< Required minimal timestep for a mixture to reach a node.
    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>
    std::unordered_map<int, int> filledEdges;                                   ///<  Which edges are currently filled with a single mixture <EdgeID, MixtureID>

public:

    /**
     * @brief Constructor of a mixing model.
    */
    MixingModel();

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
    virtual void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) = 0;

    /**
     * @brief Insert a mixture at the back of the mixtures (deque) for a channel.
     * @param[in] mixtureId Id of the mixture.
     * @param[in] channelId Id of the channel.
    */
    virtual void injectMixtureInEdge(int mixtureId, int channelId) = 0;
};

/**
 * @brief Class that describes the instantaneous mixing model. This mixing model, assumes that mixtures become fully mixed inside nodes.
*/
template<typename T>
class InstantaneousMixingModel : public MixingModel<T> {

private:

    std::unordered_map<int, std::vector<MixtureInFlow<T>>> mixtureInflowAtNode;     ///< Unordered map to track mixtures flowing into nodes <nodeId <mixtureId, inflowVolume>>
    std::unordered_map<int, int> mixtureOutflowAtNode;                              ///< Unordered map to track mixtures flowing out of nodes.
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
    void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Calculate and store the mixtures flowing into all nodes
     * @param[in] timeStep The timeStep size of the current iteration.
     * @param[in] network Pointer to the network.
    */
    void updateNodeInflow(T timeStep, arch::Network<T>* network);

    /**
     * @brief Calculate the mixture outflow at each node from all inflows and, when necessary, create new mixtures.
     * @param[in] sim Pointer to the simulation.
     * @param[in] mixtures Unordered map of the mixtures in the system.
    */
    void generateNodeOutflow(Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Add the node outflow as inflow to the channels
     * @param[in] timeStep The timeStep size of the current iteration.
     * @param[in] network Pointer to the network.
     * @param[in] mixtures Unordered map of the mixtures in the system.
    */
    void updateChannelInflow(T timeStep, arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    /**
     * @brief Remove mixtures that have 'outflowed' their channel
     * @param[in] network Pointer to the network.
    */
    void clean(arch::Network<T>* network);

    /**
     * @brief Insert a mixture at the back of the mixtures (deque) for a channel.
     * @param[in] mixtureId Id of the mixture.
     * @param[in] channelId Id of the channel.
    */
    void injectMixtureInEdge(int mixtureId, int channelId);

    /**
     * @brief Print all mixtures and their positions in the network.
    */
    void printMixturesInNetwork();

};


}   // namespace sim