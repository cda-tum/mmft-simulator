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
    T flowRate;
};

template<typename T>
struct FlowSectionInput {
    T startWidth;
    T endWidth;
    T stretchFactor; // this is technically redundant for constant flow sections
    T startWidthIfFunctionWasSplit;
    T concentrationAtChannelEnd; // concentration if it is a constant flow section
    std::function<T(T)> concentrationAtChannelEndFunction;
    std::vector<T> segmentedResult; // this is technically redundant for constant flow sections
    T a_0_old;

    // T getA_0() {
    //     return a_0_old;
    // }

    // void setA_0(T a_0_old) {
    //     this->a_0_old = a_0_old;
    // }
};

template<typename T>
class MixingModel {
protected:

    T minimalTimeStep = 0.0;
    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>

public:

    MixingModel();

    //virtual void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) = 0;

    T getMinimalTimeStep();

    /**
     * @brief Update the minimal timestep for a mixture to 'outflow' their channel.
     * @param[in] network
    */
    void updateMinimalTimeStep(arch::Network<T>* network);


    const std::deque<std::pair<int,T>>& getMixturesInEdge(int channelId) const;

    const std::unordered_map<int, std::deque<std::pair<int,T>>>& getMixturesInEdges() const;

    virtual bool getDiffusive() = 0;
};

template<typename T>
class InstantaneousMixingModel : public MixingModel<T> {

private:

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

    void injectMixtureInEdge(int mixtureId, int channelId);

    void printMixturesInNetwork();

    const std::unordered_map<int, int>& getFilledEdges() const;

    bool getDiffusive() { return false; }

//    void initialize(arch::Network<T>*);
};

template<typename T>
class DiffusionMixingModel : public MixingModel<T> {

private:
    int resolution = 10;
    std::vector<std::vector<RadialPosition<T>>> concatenatedFlows;
    std::unordered_map<int, std::vector<FlowSection<T>>> outflowDistributions;
    std::unordered_map<int, int> filledEdges;                                   ///< Which edges are currently filled and what mixture is at the front <EdgeID, MixtureID>
    void generateInflows();

public:

    DiffusionMixingModel();

    /**
     * @brief Propagate the mixtures and check if a mixtures reaches channel end.
    */
    void updateNodeInflow(T timeStep, arch::Network<T>* network, Simulation<T>*, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures);

    /**
     * @brief Generate a new inflow in case a mixture has reached channel end. Invoked by updateNodeInflow.
    */
    void generateInflows(int nodeId, T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures);

    void topologyAnalysis(arch::Network<T>* network, int nodeId);
    
    void printTopology();

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionConstant(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters);

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionFunction(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters, std::function<T(T)> fConstant);

    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolutionTotal(T channelLength, T currChannelFlowRate, T channelWidth, int resolution, int speciesId, T pecletNr, 
        const std::vector<FlowSection<T>>& flowSections, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& diffusiveMixtures);

    void updateDiffusiveMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures);

    void generateNodeOutflow();

    void updateChannelInflow();

    void injectMixtureInEdge(int mixtureId, int channelId);

    void clean(arch::Network<T>* network);

    bool getDiffusive() { return true; }

};

}   // namespace sim