/**
 * @file MixingModels.h
 */

#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <set>
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

template<typename T>
class lbmSimulator;

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
    std::unordered_map<int, int> filledEdges;                                   ///<  Which edges are currently filled with a single mixture <EdgeID, MixtureID>

public:

    /**
     * @brief Constructor of a mixing model.
    */
    MixingModel();

    /**
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    virtual void propagateSpecies(arch::Network<T>* network, Simulation<T>* sim) = 0;

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
     * @brief Insert a mixture at the back of the mixtures (deque) for a channel.
     * @param[in] mixtureId Id of the mixture.
     * @param[in] channelId Id of the channel.
    */
    void injectMixtureInEdge(int mixtureId, int channelId);

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
    void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) override;

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
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    void propagateSpecies(arch::Network<T>* network, Simulation<T>* sim) override;

    /**
     * @brief From the mixtureInjections and CFD simulators, generate temporary mxtures that 
     * flow into the network at correspondingnode entry points.
     * @param[in] sim Pointer to the simulation.
     */
    void initNodeOutflow(Simulation<T>* sim, std::vector<Mixture<T>>& tmpMixtures);

    /**
     * @brief Propagate the mixtures through the corresponding channel entirely, without considering time steps
     */
    void channelPropagation(arch::Network<T>* network);

    /**
     * @brief From the node's inflows, generate the node outflow
     */
    bool updateNodeOutflow(Simulation<T>* sim, std::vector<Mixture<T>>& tmpMixtures);

    void storeConcentrations(Simulation<T>* sim, const std::vector<Mixture<T>>& tmpMixtures);

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
    int noFourierTerms = 100;
    std::set<int> mixingNodes;
    std::vector<std::vector<RadialPosition<T>>> concatenatedFlows;
    std::unordered_map<int, std::vector<FlowSection<T>>> outflowDistributions;
    std::unordered_map<int, int> filledEdges;                                   ///< Which edges are currently filled and what mixture is at the front <EdgeID, MixtureID>
    std::unordered_map<int, std::unordered_map<int, std::vector<T>>>  concentrationFieldsOut; ///< Defines which concentration fields are defined at nodes at the interface between 1D into CFD <channelId, <specieId, concentrationField>>
    void generateInflows();

    std::shared_ptr<lbmSimulator<T>> simulator; // Pointer to the lbm simulator

public:

    DiffusionMixingModel();

    /**
     * @brief Create and/or propagate mixtures into channels downstream.
     * @param[in] timeStep the current timestep size.
     * @param[in] network pointer to the network.
     * @param[in] sim pointer to the simulation.
     * @param[in] mixtures reference to the unordered map of mixtures.
    */
    void updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) override;

    /**
     * @brief Propagate the mixtures and check if a mixtures reaches channel end.
    */
    void updateNodeInflow(T timeStep, arch::Network<T>* network) override;

    /**
     * @brief Generate a new inflow in case a mixture has reached channel end. Invoked by updateNodeInflow.
    */
    void generateInflows(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures);

    void topologyAnalysis(arch::Network<T>* network, int nodeId);

    /**
     * @brief Propagate all the species through a network for a steady-state simulation
     */
    void propagateSpecies(arch::Network<T>* network, Simulation<T>* sim) override;
    
    void printTopology();

    /**
     * @brief Calculate the species concentration distribution across the channel width (at the end of the channel) for constant concentration flow sections. To get the analytical solution, a fourier series is employed. 
     */
    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalFunction(T channelLength, T channelWidth, int noFourierTerms, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters);

    /**
     * @brief Calculate the species concentration distribution across the channel width (at the end of the channel) for non-constant concentration inputs flow sections. To get the analytical solution, a fourier series is employed. 
     */
    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalFunction(T channelLength, T channelWidth, int noFourierTerms, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters, std::function<T(T)> fConstant);

    /**
     * @brief Calculate the species concentration distribution in a channel by calculating the get analytical function for constant and non-constant flow sections.
     */
    std::tuple<std::function<T(T)>,std::vector<T>, T> getAnalyticalSolution(T channelLength, T currChannelFlowRate, T channelWidth, int noFourierTerms, int specieId, T pecletNr, 
        const std::vector<FlowSection<T>>& flowSections, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& diffusiveMixtures);

    /**
     * @brief Calculate the species concentration across the channel width (at the end of the channel) for concentation fields flowing out of a CFD module. For this the constant concentration values at each lattice point of the CFD module are translated into linear segments.
     * This way a more accurate connection, while adhering to the law of mass conservation is achieved. For the concentration distribution function at the end of a channel the analytical solution is solved by employing a fourier series. 
     */
    std::tuple<std::function<T(T)>,std::vector<T>,T> getAnalyticalSolutionHybrid(T channelLength, T currChannelFlowRate, T channelWidth, int resolution, T pecletNr, std::vector<T> concentrationField, T dx);

    /**
     * @brief Use piecewise linerar interpolation to translate the analytical solution of the species concentration across the channel width (at the end of the channel) into the concentration values in the lattice of the CFD module. This considers the conservation of mass. 
     */
    std::vector<T> defineConcentrationNodeFieldForCfdInput(int resolutionIntoCfd, int specieId, int channelId, T channelWidth, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& Mixtures, int noFourierTerms);
    
    /**
     * @brief Use a fifth order polynomial function to translate the analytical solution of the species concentration across the channel width (at the end of the channel) into the concentration values in the lattice of the CFD module.
     */
    std::tuple<std::function<T(T)>, std::vector<T>, T> getAnalyticalSolutionHybridInput(T channelLength, T channelWidth, int noFourierTerms, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters);

    void clean(arch::Network<T>* network);

    void printMixturesInNetwork();

    std::vector<std::vector<RadialPosition<T>>>& getConcatenatedFlows() { return concatenatedFlows; }

    std::unordered_map<int, std::vector<FlowSection<T>>>& getOutflowDistributions() { return outflowDistributions; }

    bool isInstantaneous() override { return 0; };

    bool isDiffusive() override { return 1; };

    /**
     * @brief Reset the number of Fourier terms used in the analytical solution. The higher the number the higher the precision, although this is negligible at some point.
     * @param[in] newnoFourierTerms The new number of Fourier terms.
     */
    void resetnoFourierTerms(int newnoFourierTerms) {
        noFourierTerms = newnoFourierTerms;
    }

    /**
     * @brief Return the number of Fourier terms used in the analytical solution.
     * @param[out] noFourierTerms The current number of Fourier terms.
     */
    int getnoFourierTerms() const {
        return noFourierTerms;
    }

};

}   // namespace sim