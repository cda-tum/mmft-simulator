#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Fluid.h"
#include "Mixture.h"
#include "Specie.h"

#include "../architecture/Network.h"

namespace sim { 

// Forward declared dependencies
template<typename T>
class Simulation;

template<typename T>
struct MixtureInFlow {

    int mixtureId;
    T inflowVolume;

};

template<typename T>
struct MixtureDistributionInFlow {
    
    int mixtureId;
    int gridResolution;
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
};

template<typename T>
class MixingModel {
protected:

    T minimalTimeStep = 0.0;

public:

    MixingModel();

    virtual void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) = 0;

    T getMinimalTimeStep();
};

template<typename T>
class InstantaneousMixingModel : public MixingModel<T> {

private:

    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>
    std::unordered_map<int, std::vector<MixtureInFlow<T>>> mixtureInflowAtNode;    // <nodeId <mixtureId, inflowVolume>>
    std::unordered_map<int, int> mixtureOutflowAtNode;
    std::unordered_map<int, T> totalInflowVolumeAtNode;
    std::unordered_map<int, int> filledEdges;                                   ///<  Which edges are currently filled with a single mixture <EdgeID, MixtureID>
    std::unordered_map<int, bool> createMixture;

    int generateInflows(int nodeId, T timeStep, arch::Network<T>* network);

public:

    InstantaneousMixingModel();

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
     * @brief Update the minimal timestep for a mixture to 'outflow' their channel.
     * @param[in] network
    */
    void updateMinimalTimeStep(arch::Network<T>* network);

    void injectMixtureInEdge(int mixtureId, int channelId);

    void printMixturesInNetwork();

    const std::deque<std::pair<int,T>>& getMixturesInEdge(int channelId) const;

    const std::unordered_map<int, std::deque<std::pair<int,T>>>& getMixturesInEdges() const;

    const std::unordered_map<int, int>& getFilledEdges() const;

//    void initialize(arch::Network<T>*);
};

template<typename T>
class DiffusionMixingModel : public MixingModel<T> {

private:
    std::vector<std::vector<RadialPosition<T>>> concatenatedFlows;
    std::unordered_map<int, std::vector<FlowSection<T>>> outflowDistributions;
    void generateInflows();

public:

    DiffusionMixingModel();

    void topologyAnalysis(arch::Network<T>* network);
    
    void printTopology();

    void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    void updateMixtureDistributions(T timeStep, arch::Network<T>* network, Simulation<T>* sim);

    void updateNodeInflow();

    void generateNodeOutflow();

    void updateChannelInflow();

};

}   // namespace sim