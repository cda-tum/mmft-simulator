#include "MixingModels.h"

#include <unordered_map>
#include <deque>
#include <iostream>
#include <cmath>

#define M_PI 3.14159265358979323846

namespace sim {

template<typename T>
MixingModel<T>::MixingModel() { }

template<typename T>
T MixingModel<T>::getMinimalTimeStep() {
    return this->minimalTimeStep;
}

template<typename T>
void MixingModel<T>::updateMinimalTimeStep(arch::Network<T>* network) {
    this->minimalTimeStep = 0.0;
    for (auto& [channelId, deque] : mixturesInEdge) {
        T channelVolume = network->getChannel(channelId)->getVolume();
        T channelFlowRate = std::abs(network->getChannel(channelId)->getFlowRate());
        for (auto& [mixtureId, endPos] : deque) {
            T flowTime = (1.0 - endPos)*channelVolume/channelFlowRate;
            if (this->minimalTimeStep < 1e-12) {
                this->minimalTimeStep = flowTime;
            } else if (flowTime < this->minimalTimeStep) {
                this->minimalTimeStep = flowTime;
            }
        }
    }
}

template<typename T>
const std::deque<std::pair<int,T>>& MixingModel<T>::getMixturesInEdge(int channelId) const {
    return mixturesInEdge.at(channelId);
}

template<typename T>
const std::unordered_map<int, std::deque<std::pair<int,T>>>& MixingModel<T>::getMixturesInEdges() const {
    return mixturesInEdge;
}

template<typename T>
const std::unordered_map<int, int>& MixingModel<T>::getFilledEdges() const {
    return filledEdges;
}

template<typename T>
void MixingModel<T>::injectMixtureInEdge(int mixtureId, int channelId) {
    if (this->mixturesInEdge.count(channelId)) {
        this->mixturesInEdge.at(channelId).push_back(std::make_pair(mixtureId, T(0.0)));
    } else {
        std::deque<std::pair<int,T>> newDeque;
        newDeque.push_back(std::make_pair(mixtureId, T(0.0)));
        this->mixturesInEdge.try_emplace(channelId, newDeque);
    }
}

template<typename T>
InstantaneousMixingModel<T>::InstantaneousMixingModel() : MixingModel<T>() { }

template<typename T>
void InstantaneousMixingModel<T>::propagateSpecies(arch::Network<T>* network, Simulation<T>* sim) {

    std::vector<Mixture<T>> tmpMixtures;

    // Define total inflow volume at nodes
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId) ) {
            // Check if the channel flows into the node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                T inflowVolume = std::abs(channel->getFlowRate());
                auto [iterator, inserted] = totalInflowVolumeAtNode.try_emplace(nodeId, inflowVolume);
                if (!inserted) {
                    iterator->second += inflowVolume;
                }
            }
        }
    }

    // Initial node outflow from mixtureInjections and CFD simulators, stored in mixtureOutflowAtNode
    initNodeOutflow(sim, tmpMixtures);

    // Propagate the mixtures through the entire channel, without considering time steps    
    channelPropagation(network);

    bool inflowUpdated = true;
    while (inflowUpdated) {
        // From node inflow, generate the node's outflow
        inflowUpdated = updateNodeOutflow(sim, tmpMixtures);
        // Propagate the mixtures through the entire channel
        channelPropagation(network);
    }

    // Store the concentrations of the final state in the concentration buffer of olbMixingSolver.
    storeConcentrations(sim, tmpMixtures);

    clean(network);

}

template<typename T>
void InstantaneousMixingModel<T>::initNodeOutflow(Simulation<T>* sim, std::vector<Mixture<T>>& tmpMixtures) {
    // Add mixture injections
    for (auto& [key, mixtureInjection] : sim->getMixtureInjections()) {
        int tmpMixtureIndex = tmpMixtures.size();
        int nodeId = mixtureInjection->getInjectionChannel()->getFlowRate() >= 0.0 ? 
                     mixtureInjection->getInjectionChannel()->getNodeB() : mixtureInjection->getInjectionChannel()->getNodeA();
        tmpMixtures.push_back(Mixture<T>(*sim->getMixture(mixtureInjection->getMixtureId())));
        mixtureOutflowAtNode.try_emplace(nodeId, tmpMixtureIndex);
    }
    // Add CFD Simulator outflows
    for (auto& [key, cfdSimulator] : sim->getCFDSimulators()) {
        for (auto& [nodeId, opening] : cfdSimulator->getOpenings()) {
            // If the node is an outflow
            if (cfdSimulator->getFlowRates().at(nodeId) < 0.0) {
                int tmpMixtureIndex = tmpMixtures.size();
                int tmpMixtureId = std::numeric_limits<int>::max();
                std::unordered_map<int, Specie<T>*> species;
                std::unordered_map<int, T> speciesConcentrations(cfdSimulator->getConcentrations().at(nodeId));
                for (auto& [speciesId, concentration] : cfdSimulator->getConcentrations().at(nodeId)) {
                    species.try_emplace(speciesId, sim->getSpecie(speciesId));
                }
                Mixture<T> tmpMixture = Mixture<T>(tmpMixtureId, species, speciesConcentrations, sim->getContinuousPhase());
                tmpMixtures.push_back(tmpMixture);
                mixtureOutflowAtNode.try_emplace(nodeId, tmpMixtureIndex);
            }
        }
    }
}

template<typename T>
void InstantaneousMixingModel<T>::channelPropagation(arch::Network<T>* network) {
    for (auto& [nodeId, mixtureId] : mixtureOutflowAtNode) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            // Find the nodeId that is across the channel
            int oppositeNode;
            if (channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) {
                oppositeNode = channel->getNodeB();
            } else if (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId) {
                oppositeNode = channel->getNodeA();
            } else {
                continue;
            }
            // Update the mixture inflow at the node across the channel
            MixtureInFlow<T> mixtureInflow = {mixtureId, std::abs(channel->getFlowRate())};
            auto [iterator, inserted] = mixtureInflowAtNode.try_emplace(oppositeNode, std::vector<MixtureInFlow<T>>(1, mixtureInflow));
            if (!inserted) {
                mixtureInflowAtNode.at(oppositeNode).push_back(mixtureInflow);
            }
        }
    }
}

template<typename T>
bool InstantaneousMixingModel<T>::updateNodeOutflow(Simulation<T>* sim, std::vector<Mixture<T>>& tmpMixtures) {
    bool updated = false;
    for (auto& [nodeId, mixtureInflowList] : mixtureInflowAtNode) {
        bool createMixture = false;
        std::unordered_map<int, Specie<T>*> speciePtrs;
        std::unordered_map<int, T> newConcentrations;
        for (auto& mixtureInflow : mixtureInflowList) {
            for (auto& [specieId, oldConcentration] : tmpMixtures[mixtureInflow.mixtureId].getSpecieConcentrations()) {
                speciePtrs.try_emplace(specieId, sim->getSpecie(specieId));
                T newConcentration = oldConcentration * mixtureInflow.inflowVolume / totalInflowVolumeAtNode.at(nodeId);
                auto [iterator, inserted] = newConcentrations.try_emplace(specieId, newConcentration);
                if (!inserted) {
                    iterator->second = iterator->second + newConcentration;
                }
            }
            if (mixtureInflow.mixtureId != mixtureInflowList[0].mixtureId) {
                createMixture = true;
            }
        }
        int outflowMixtureId;
        Mixture<T> newMixture (tmpMixtures.size(), speciePtrs, newConcentrations, sim->getContinuousPhase());
        if (createMixture) {
            outflowMixtureId = tmpMixtures.size();
        } else {
            outflowMixtureId = mixtureInflowList[0].mixtureId;
        }
        if (!mixtureOutflowAtNode.count(nodeId)) {
            if (createMixture) {
                tmpMixtures.push_back(newMixture);
            }
            mixtureOutflowAtNode.try_emplace(nodeId, outflowMixtureId);
            updated = true;

        } else {
            // Check if the ouflow gets updated or not
            if (tmpMixtures[mixtureOutflowAtNode.at(nodeId)] == tmpMixtures[outflowMixtureId]) {
                continue;
            } else {
                if (createMixture) {
                    tmpMixtures.push_back(newMixture);
                }
                mixtureOutflowAtNode.at(nodeId) = outflowMixtureId;
                updated = true;
            }
        }
    }
    return updated;
}

template<typename T>
void InstantaneousMixingModel<T>::storeConcentrations(Simulation<T>* sim, const std::vector<Mixture<T>>& tmpMixtures) {
    for (auto& [key, cfdSimulator] : sim->getCFDSimulators()) {
        std::unordered_map<int, std::unordered_map<int, T>> concentrations = cfdSimulator->getConcentrations();
        for (auto& [nodeId, opening] : cfdSimulator->getOpenings()) {
            // If the node is an inflow
            if (cfdSimulator->getFlowRates().at(nodeId) > 0.0) {
                if (mixtureOutflowAtNode.count(nodeId)) {
                    for (auto& [specieId, specieConcentration] : tmpMixtures[mixtureOutflowAtNode.at(nodeId)].getSpecieConcentrations()) {
                        concentrations.at(nodeId).at(specieId) = specieConcentration;
                    }
                }
            }
        }
        cfdSimulator->storeConcentrations(concentrations);
    }
}

template<typename T>
void InstantaneousMixingModel<T>::updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    generateNodeOutflow(sim, mixtures);
    updateChannelInflow(timeStep, network, mixtures);
    clean(network);
    this->updateMinimalTimeStep(network);
}

template<typename T>
void InstantaneousMixingModel<T>::updateNodeInflow(T timeStep, arch::Network<T>* network) {

    for (auto& [nodeId, node] : network->getNodes()) {
        bool generateInflow = false;
        int totalInflowCount = 0;
        int mixtureInflowCount = 0;
        if (createMixture.count(nodeId)) {
            createMixture.at(nodeId) = false;
        } else {
            createMixture.try_emplace(nodeId, false);
        }
        for (auto& [channelId, channel] : network->getChannels()) {
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                totalInflowCount++;
                T inflowVolume = std::abs(channel->getFlowRate()) * timeStep;
                T movedDistance = inflowVolume / channel->getVolume();
                auto [iterator, inserted] = totalInflowVolumeAtNode.try_emplace(nodeId, inflowVolume);
                if (!inserted) {
                    iterator->second += inflowVolume;
                }
                if (this->mixturesInEdge.count(channel->getId())){
                    for (auto& [mixtureId, endPos] : this->mixturesInEdge.at(channel->getId())) {
                        T newEndPos = std::min(endPos + movedDistance, (T) 1.0);
                        endPos = newEndPos;
                        if (newEndPos == 1.0) {
                            // if the mixture front left the channel, it's fully filled
                            if (this->filledEdges.count(channel->getId())) {
                                this->filledEdges.at(channel->getId()) = mixtureId;
                            } else {
                                this->filledEdges.try_emplace(channel->getId(), mixtureId);
                            }
                            generateInflow = true;
                        }   
                    }
                }
            }
        }
        // specie flows into node, add to mixture inflow
        if (generateInflow) {
            mixtureInflowCount = generateInflows(nodeId, timeStep, network);
        }
        if (totalInflowCount != mixtureInflowCount) {
            createMixture.at(nodeId) = true;
        }
    }
    for (auto& [nodeId, mixtureInflowList] : mixtureInflowAtNode) {
        for (auto& mixtureInflow : mixtureInflowList) {
            if (mixtureInflow.mixtureId != mixtureInflowList[0].mixtureId) {
                createMixture.at(nodeId) = true;
            }
        }
    }
}

template<typename T>
int InstantaneousMixingModel<T>::generateInflows(int nodeId, T timeStep, arch::Network<T>* network) {
    int mixtureInflowCount = 0;
    for (auto& [channelId, channel] : network->getChannels()) {
        T inflowVolume = std::abs(channel->getFlowRate()) * timeStep;
        if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
            if (this->filledEdges.count(channelId)  && !network->getNode(nodeId)->getSink()) {
                MixtureInFlow<T> mixtureInflow = {this->filledEdges.at(channelId), inflowVolume};
                mixtureInflowAtNode[nodeId].push_back(mixtureInflow);
                mixtureInflowCount++;
            }
        }
    }
    return mixtureInflowCount;
}

template<typename T>
void InstantaneousMixingModel<T>::generateNodeOutflow(Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    for (auto& [nodeId, mixtureInflowList] : mixtureInflowAtNode) {
        std::unordered_map<int, T> newConcentrations;
        for (auto& mixtureInflow : mixtureInflowList) {
            for (auto& [specieId, oldConcentration] : mixtures.at(mixtureInflow.mixtureId)->getSpecieConcentrations()) {
                T newConcentration = oldConcentration * mixtureInflow.inflowVolume / totalInflowVolumeAtNode.at(nodeId);
                auto [iterator, inserted] = newConcentrations.try_emplace(specieId, newConcentration);
                if (!inserted) {
                    iterator->second = iterator->second + newConcentration;
                }
            }
        }
        if ( !createMixture.at(nodeId)) {
            mixtureOutflowAtNode.try_emplace(nodeId, mixtureInflowList[0].mixtureId);
        } else {
            Mixture<T>* newMixture = sim->addMixture(newConcentrations);
            mixtureOutflowAtNode.try_emplace(nodeId, newMixture->getId());
            createMixture.at(nodeId) = false;
        }
    }
}

template<typename T>
void InstantaneousMixingModel<T>::updateChannelInflow(T timeStep, arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            // check if edge is an outflow edge to this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
                if (mixtureOutflowAtNode.count(nodeId)) {
                    this->injectMixtureInEdge(mixtureOutflowAtNode.at(nodeId), channel->getId());
                }
            }
        }
    }
}

template<typename T>
void InstantaneousMixingModel<T>::clean(arch::Network<T>* network) {
    
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            if (this->mixturesInEdge.count(channel->getId())){
                for (auto& [mixtureId, endPos] : this->mixturesInEdge.at(channel->getId())) {
                    if (endPos == 1.0) {
                        this->mixturesInEdge.at(channel->getId()).pop_front();
                    } else {
                        break;
                    }
                }
            }
        }
    }
    mixtureInflowAtNode.clear();
    mixtureOutflowAtNode.clear();
    totalInflowVolumeAtNode.clear();
}

template<typename T>
void InstantaneousMixingModel<T>::printMixturesInNetwork() {
    for (auto& [channelId, deque] : this->mixturesInEdge) {
        for (auto& [mixtureId, endPos] : deque) {
            std::cout << "Mixture " << mixtureId << " in channel " << channelId << 
            " at position " << endPos << std::endl;
        }
    }
}

//=========================================================================================
//================================  diffusive Mixing  =====================================
//=========================================================================================

template<typename T>
DiffusionMixingModel<T>::DiffusionMixingModel() : MixingModel<T>() { }

template<typename T>
void DiffusionMixingModel<T>::updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {
    updateNodeInflow(timeStep, network);
    generateInflows(timeStep, network, sim, mixtures);
    clean(network);
    this->updateMinimalTimeStep(network);
    //printMixturesInNetwork();
}

template<typename T>
void DiffusionMixingModel<T>::updateNodeInflow(T timeStep, arch::Network<T>* network) {
    mixingNodes.clear();
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& [channelId, channel] : network->getChannels()) {
            // If the channel flows into this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                T inflowVolume = std::abs(channel->getFlowRate()) * timeStep;
                T movedDistance = inflowVolume / channel->getVolume();
                if (this->mixturesInEdge.count(channel->getId())) {
                    for (auto& [mixtureId, endPos] : this->mixturesInEdge.at(channel->getId())) {
                        // Propagate the mixture positions in the channel with movedDistance
                        T newEndPos = std::min(endPos + movedDistance, (T) 1.0);
                        endPos = newEndPos;
                        if (newEndPos == 1.0) {
                            // if the mixture front left the channel, it's fully filled with the current mixture
                            if (this->filledEdges.count(channel->getId())) {
                                this->filledEdges.at(channel->getId()) = mixtureId;
                            } else {
                                this->filledEdges.try_emplace(channel->getId(), mixtureId);
                            }
                            // We must generate the outflow of this node, i.e., the inflow of the channels that flow out of the node
                            mixingNodes.emplace(nodeId);
                        }   
                    }
                }
            }
        }
    }
}

template<typename T>
void DiffusionMixingModel<T>::generateInflows(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {
    // Due to the nature of the diffusive mixing model, per definition a new mixture is created.
    // It is unlikely that this exact mixture, with same species and functions already exists
    for (auto& nodeId : mixingNodes) {
        // Perform topoology analysis at node, to know how the inflow sections and their relative order.
        topologyAnalysis(network, nodeId);
        for (auto& [channelId, channel] : network->getChannels()) {
            // If the channel flows out of this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
                // Loop over all species in the incoming mixtures and add to set of present species
                std::set<int> presentSpecies;
                for (auto& section : outflowDistributions.at(channel->getId())) {
                    if (this->filledEdges.count(section.channelId)) {
                        for (auto& [specieId, pair] : mixtures.at(this->filledEdges.at(section.channelId))->getSpecieDistributions()) {
                            if (presentSpecies.find(specieId) == presentSpecies.end()) {
                                presentSpecies.emplace(specieId);
                            }
                        }
                    }
                }
                // We should evaluate the final function of the mixture reaching the end of this channel
                std::unordered_map<int, std::tuple<std::function<T(T)>,std::vector<T>,T>> newDistributions;
                for (auto& specieId : presentSpecies) {
                    T pecletNr = (std::abs(channel->getFlowRate()) / channel->getHeight()) / (sim->getSpecie(specieId))->getDiffusivity();
                    std::tuple<std::function<T(T)>, std::vector<T>, T> analyticalResult = getAnalyticalSolutionTotal(
                        channel->getLength(), std::abs(channel->getFlowRate()), channel->getWidth(), 100, specieId, 
                        pecletNr, outflowDistributions.at(channel->getId()), mixtures);
                    newDistributions.try_emplace(specieId, analyticalResult);
                }
                //Create new DiffusiveMixture
                DiffusiveMixture<T>* newMixture = dynamic_cast<DiffusiveMixture<T>*>(sim->addDiffusiveMixture(newDistributions));
                newMixture->setNonConstant();
                this->injectMixtureInEdge(newMixture->getId(), channelId);
            }
        }
    }
} 

template<typename T>
void DiffusionMixingModel<T>::topologyAnalysis( arch::Network<T>* network, int nodeId) {
    /**
     * 1. List all connecting angles with angle and inflow
     * 2. Order connected channels according to radial angle
     * 3. Group connected channels into groups of 
     *      - Concatenated inflows
     *      - Concatenated outflows
     * 4. Inflow groups always (?) merge incoming channels
     * 5. Inflow group is always (?) split into the 1 or 2 neighbouring outflow groups
     * 6. Outflow groups always (?) merge the incoming 1 or 2 neighbouring inflow groups
     * 7. Outflow groups always (?) split the total income into channels.
    */
    arch::Node<T>* node = network->getNode(nodeId).get();
    concatenatedFlows.clear();
    outflowDistributions.clear();
    if (!node->getGround()){

        // 1. List all connecting channels with angle and inflow
        std::vector<RadialPosition<T>> channelOrder;
        for (auto& [channelId, channel] : network->getChannels()) {
            bool inflow;
            arch::Node<T>* nodeA = network->getNode(channel->getNodeA()).get();
            arch::Node<T>* nodeB = network->getNode(channel->getNodeB()).get();
            T dx = ( nodeId == channel->getNodeA() ) ? nodeB->getPosition()[0]-nodeA->getPosition()[0] : nodeA->getPosition()[0]-nodeB->getPosition()[0];
            T dy = ( nodeId == channel->getNodeA() ) ? nodeB->getPosition()[1]-nodeA->getPosition()[1] : nodeA->getPosition()[1]-nodeB->getPosition()[1];
            #ifdef DEBUG
            assert((dx*dx+dy*dy) > 1e-12);
            #endif
            T angle = atan2(dy,dx);
            angle = std::fmod(atan2(dy,dx)+2*M_PI,2*M_PI);
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                // Channel is inflow channel
                inflow = true;
            } else if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
                // Channel is outflow channel
                inflow = false;
            } else {
                // Channel has no flow rate: Do nothing.
                continue;
            }
            RadialPosition<T> newPosition = {angle, channel->getId(), inflow};
            channelOrder.push_back(newPosition);
        }

        // 2. Order connected channels according to radial angle
        std::sort(channelOrder.begin(), channelOrder.end(), [](auto& a, auto& b) {
            return a.radialAngle < b.radialAngle;  // ascending order
        });

        // 3. Group connected channels into groups of
        //     - Concatenated inflows
        //     - Concatenated outflows
        while (!channelOrder.empty()) {
            std::vector<RadialPosition<T>> concatenatedFlow;
            concatenatedFlow.emplace_back( std::move(channelOrder.front()) );
            channelOrder.erase(channelOrder.begin());
            int n = 0;
            for (auto& flow : channelOrder) {
                if (flow.inFlow == concatenatedFlow.back().inFlow) {
                    concatenatedFlow.emplace_back( std::move(flow) );
                    n++;
                } else {
                    break;
                }
            }

            channelOrder.erase(channelOrder.begin(), channelOrder.begin() + n);
            concatenatedFlows.push_back(concatenatedFlow);
        }

        if (concatenatedFlows.front().front().inFlow == concatenatedFlows.back().back().inFlow) {
            for (auto& flow : concatenatedFlows.front()) {
                concatenatedFlows.back().emplace_back( std::move(flow) );
            }
            concatenatedFlows.erase(concatenatedFlows.begin());
        }

        // 4. Calculate the flow connections between inflow/outflow channels
        // and generate outflowDistribution.
        if (concatenatedFlows.size() == 2) {
            int in = (concatenatedFlows[0][0].inFlow) ? 0 : 1;
            int out = (concatenatedFlows[0][0].inFlow) ? 1 : 0;
            T flowRateIn = 0.0;
            T flowRateOut = 0.0;
            for (auto& channel : concatenatedFlows[in]) {
                flowRateIn += std::abs(network->getChannel(channel.channelId)->getFlowRate());
            }
            for (auto& channel : concatenatedFlows[out]) {
                flowRateOut += std::abs(network->getChannel(channel.channelId)->getFlowRate());
            }

            std::vector<T> inflowCuts = {0.0};
            std::vector<int> channelInIDs;
            // Calculate inflow separations
            for (auto channelIn = concatenatedFlows[in].rbegin(); channelIn != concatenatedFlows[in].rend(); ++channelIn) {
                T newCut = inflowCuts.back() + std::abs(network->getChannel(channelIn->channelId)->getFlowRate())/flowRateIn;
                inflowCuts.push_back(std::min(newCut, (T) 1.0));
                channelInIDs.push_back(channelIn->channelId);
            }

            std::vector<T> outflowCuts = {0.0};
            std::vector<int> channelOutIDs;
            for (auto& channelOut : concatenatedFlows[out]) {
                T newCut = outflowCuts.back() + std::abs(network->getChannel(channelOut.channelId)->getFlowRate())/flowRateOut;
                outflowCuts.push_back(std::min(newCut, (T) 1.0));
                channelOutIDs.push_back(channelOut.channelId);
            }

            long unsigned int n_in = 0;
            long unsigned int n_out = 0;
            T start = 0.0;
            T end = 0.0;
            for (auto& channelInId : channelInIDs) {
                bool filled = false;
                while (!filled){
                    FlowSection<T> inFlowSection;
                    if (inflowCuts[n_in+1] <= outflowCuts[n_out+1] + 1e-15) {   ///< 1e-16 to account for machine precision. TODO: improve algorithm
                        // The cut is caused on the inflow side
                        T flowRate = (1.0 - start) * std::abs(network->getChannel(channelInId)->getFlowRate());
                        inFlowSection = FlowSection<T> {channelInId, start, 1.0, flowRate, network->getChannel(channelInId)->getWidth()};
                        if (outflowDistributions.count(channelOutIDs[n_out])) {
                            outflowDistributions.at(channelOutIDs[n_out]).push_back(inFlowSection);
                        } else {
                            std::vector<FlowSection<T>> newFlowSectionVector = {inFlowSection};
                            outflowDistributions.try_emplace(channelOutIDs[n_out], newFlowSectionVector);
                        }
                        #ifdef DEBUG
                        assert(n_out < channelOutIDs.size());
                        #endif
                        n_in++;
                        start = 0.0;
                        filled = true;
                    } else {
                        // The cut is caused on the outflow side
                        T flowRate = (outflowCuts[n_out + 1] - std::max(inflowCuts[n_in], outflowCuts[n_out]))*flowRateIn;
                        end = start + flowRate / std::abs(network->getChannel(channelInId)->getFlowRate());
                        inFlowSection = FlowSection<T> {channelInId, start, end, flowRate, network->getChannel(channelInId)->getWidth()};
                        if (outflowDistributions.count(channelOutIDs[n_out])) {
                            outflowDistributions.at(channelOutIDs[n_out]).push_back(inFlowSection);
                        } else {
                            std::vector<FlowSection<T>> newFlowSectionVector = {inFlowSection};
                            outflowDistributions.try_emplace(channelOutIDs[n_out], newFlowSectionVector);
                        }
                        #ifdef DEBUG
                        assert(n_out < channelOutIDs.size());
                        #endif
                        n_out++;
                        start = end;
                    }
                }                    
            }

        } else if (concatenatedFlows.size() == 4){
            // Assume case with 2 pairs of opposing channels
            int lowest = 0;
            int n = 0;
            for (auto flowIt : concatenatedFlows) {
                if (std::abs(network->getChannel(flowIt[0].channelId)->getFlowRate()) < abs(network->getChannel(concatenatedFlows.at(lowest)[0].channelId)->getFlowRate())) {
                    lowest = n;
                }
                n++;
            }
            auto flow = concatenatedFlows.at(lowest);
            auto clockWise = concatenatedFlows.at((lowest + 1) % 4);
            auto counterClockWise = concatenatedFlows.at((lowest + 3) % 4);
            auto opposed = concatenatedFlows.at((lowest + 2) % 4);
            if (flow.at(0).inFlow) {
                T ratio = (std::abs(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()) - 0.5*std::abs(network->getChannel(flow.at(0).channelId)->getFlowRate())) 
                            / std::abs(network->getChannel(opposed.at(0).channelId)->getFlowRate());
                FlowSection<T> flow1 {flow.at(0).channelId, 0.0, 0.5, 0.5*std::abs(network->getChannel(flow.at(0).channelId)->getFlowRate()), network->getChannel(flow.at(0).channelId)->getWidth()};
                std::vector<FlowSection<T>> newFlow1Vector = {flow1};
                outflowDistributions.try_emplace(clockWise.at(0).channelId, newFlow1Vector);
                FlowSection<T> flow2 {opposed.at(0).channelId, 0.0, ratio, ratio*std::abs(network->getChannel(opposed.at(0).channelId)->getFlowRate()), network->getChannel(opposed.at(0).channelId)->getWidth()};
                std::vector<FlowSection<T>> newFlow2Vector = {flow2};
                outflowDistributions.try_emplace(counterClockWise.at(0).channelId, newFlow2Vector);
                FlowSection<T> flow3 {flow.at(0).channelId, 0.5, 1.0, 0.5*std::abs(network->getChannel(flow.at(0).channelId)->getFlowRate()), network->getChannel(flow.at(0).channelId)->getWidth()};
                outflowDistributions.at(counterClockWise.at(0).channelId).push_back(flow3);
                FlowSection<T> flow4 {opposed.at(0).channelId, ratio, 1.0, (1.0 - ratio)*std::abs(network->getChannel(opposed.at(0).channelId)->getFlowRate()), network->getChannel(opposed.at(0).channelId)->getWidth()};
                outflowDistributions.at(clockWise.at(0).channelId).push_back(flow4);
            } else {
                T ratio = std::abs(network->getChannel(clockWise.at(0).channelId)->getFlowRate()) / 
                            (std::abs(network->getChannel(clockWise.at(0).channelId)->getFlowRate()) + std::abs(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()));
                T ratio1 = (ratio*std::abs(network->getChannel(flow.at(0).channelId)->getFlowRate()))/(std::abs(network->getChannel(clockWise.at(0).channelId)->getFlowRate()));
                T ratio2 = ((1.0-ratio)*std::abs(network->getChannel(flow.at(0).channelId)->getFlowRate()))/(std::abs(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()));
                FlowSection<T> flow1 {clockWise.at(0).channelId, 0.0, (1.0-ratio1), (1.0-ratio1)*std::abs(network->getChannel(clockWise.at(0).channelId)->getFlowRate()), network->getChannel(clockWise.at(0).channelId)->getWidth()};
                std::vector<FlowSection<T>> newFlow1Vector = {flow1};
                outflowDistributions.try_emplace(opposed.at(0).channelId, newFlow1Vector);
                FlowSection<T> flow2 {counterClockWise.at(0).channelId, 0.0, ratio2, ratio2*std::abs(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()), network->getChannel(counterClockWise.at(0).channelId)->getWidth()};
                std::vector<FlowSection<T>> newFlow2Vector = {flow2};
                outflowDistributions.try_emplace(flow.at(0).channelId, newFlow2Vector);
                FlowSection<T> flow3 {clockWise.at(0).channelId, (1.0-ratio1), 1.0, ratio1*std::abs(network->getChannel(clockWise.at(0).channelId)->getFlowRate()), network->getChannel(clockWise.at(0).channelId)->getWidth()};
                outflowDistributions.at(flow.at(0).channelId).push_back(flow3);
                FlowSection<T> flow4 {counterClockWise.at(0).channelId, ratio2, 1.0, (1.0 - ratio2)*std::abs(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()), network->getChannel(counterClockWise.at(0).channelId)->getWidth()};
                outflowDistributions.at(opposed.at(0).channelId).push_back(flow4);
            }
        } else {
            throw std::invalid_argument("Too many channels at node " + std::to_string(nodeId) + " for DiffusionMixingModel.");
        }
    }
}

template<typename T>
void DiffusionMixingModel<T>::propagateSpecies(arch::Network<T>* network, Simulation<T>* sim) {
    // TODO
}

template<typename T>
std::tuple<std::function<T(T)>, std::vector<T>, T> DiffusionMixingModel<T>::getAnalyticalSolutionConstant(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters) { 
    T a_0 = 0.0;
    std::vector<T> segmentedResult;

    for (const auto& parameter : parameters) {
        for (int n = 1; n < resolution; n++) {
            T a_n = (2/(n * M_PI))  * (parameter.concentrationAtChannelEnd) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth)); 
                segmentedResult.push_back(a_n * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * (channelLength/channelWidth)));
        }
    }

    for (const auto& parameter : parameters) { // iterates through all channels that flow into the current channel
            a_0 += 2 * parameter.concentrationAtChannelEnd  * (parameter.endWidth - parameter.startWidth);
        }

    auto f = [a_0, channelLength, channelWidth, resolution, pecletNr, parameters](T w) { // This returns C(w, l_1)
        T f_sum = 0.0;

        for (const auto& parameter : parameters) {
            for (int n = 1; n < resolution; n++) {
                T a_n = (2/(n * M_PI))  * (parameter.concentrationAtChannelEnd) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth));
                f_sum += a_n * std::cos(n * M_PI * (w)) * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * (channelLength/channelWidth));
            }
        }
        return 0.5 * a_0 + f_sum;
    };
    return {f, segmentedResult, a_0};
}

template<typename T> 
std::tuple<std::function<T(T)>,std::vector<T>,T> DiffusionMixingModel<T>::getAnalyticalSolutionFunction(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters, std::function<T(T)> fConstant) { 
    // From Channel Start to Channel End for complex input
    std::vector<T> segmentedResult;
    T a_0 = 0.0;
    T a_n = 0.0;

    // calculating segmented results
    for (const auto& parameter : parameters) {
        T a_0_old = parameter.a_0_old;
        T scaleFactor = parameter.scaleFactor;
        for (int n = 1; n < resolution; n++) {
            a_n = a_0_old / (M_PI * n) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth));
            
            for (long unsigned int i = 0; i < parameter.segmentedResult.size(); i++) {
                T translateFactor = parameter.translateFactor;

                int oldN = (i % (resolution - 1)) + 1;

                if (abs(oldN/scaleFactor - n) < 1e-8) { 
                    a_n += 2 * ((0.5 * parameter.endWidth - 0.5 * parameter.startWidth) * std::cos(oldN * M_PI * translateFactor) 
                        + std::sin(oldN * M_PI * translateFactor + 2 * n * M_PI * parameter.endWidth) / (4 * n * M_PI) 
                        - std::sin(oldN * M_PI * translateFactor + 2 * n * M_PI * parameter.startWidth) / (4 * n * M_PI)) 
                        * parameter.segmentedResult[i];
                } else {
                    a_n += (1 / ((oldN * M_PI / scaleFactor) + n * M_PI)) *
                        (std::sin(oldN * M_PI * translateFactor + parameter.endWidth * (oldN * M_PI / scaleFactor + n * M_PI))
                        - std::sin(oldN * M_PI * translateFactor + parameter.startWidth * (oldN * M_PI / scaleFactor + n * M_PI)))
                        * parameter.segmentedResult[i];
                    a_n += (1 / ((oldN * M_PI / scaleFactor) - n * M_PI)) * 
                        (std::sin(oldN * M_PI * translateFactor + parameter.endWidth * (oldN * M_PI / scaleFactor - n * M_PI))
                        - std::sin(oldN * M_PI * translateFactor + parameter.startWidth * (oldN * M_PI / scaleFactor - n * M_PI)))
                        * parameter.segmentedResult[i];
                }
            }
            segmentedResult.push_back(a_n * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * (channelLength/channelWidth)));
        }
    }
    
    // calculating a_0
    for (const auto& parameter : parameters) {
        T a_0_old = parameter.a_0_old;
        a_0 += a_0_old * (parameter.endWidth - parameter.startWidth); 
        for (long unsigned int i = 0; i < parameter.segmentedResult.size(); i++) { 
            T scaleFactor = parameter.scaleFactor;
            T translateFactor = parameter.translateFactor;
            int oldN = (i % (resolution - 1)) + 1;
            a_0 += 2 * parameter.segmentedResult[i] * scaleFactor/(oldN * M_PI) * 
                (std::sin(oldN * M_PI * parameter.endWidth / scaleFactor + oldN * M_PI * translateFactor) 
                - std::sin(oldN * M_PI * parameter.startWidth / scaleFactor + oldN * M_PI * translateFactor));
        }
    }     

    auto f = [a_0, channelLength, channelWidth, resolution, pecletNr, parameters, fConstant](T w) { // This returns C(w, l_1)
        T f_sum = 0.0;
        T a_n = 0.0;

        // Calculating a_n
        for (const auto& parameter : parameters) {
            T a_0_old = parameter.a_0_old;
            T scaleFactor = parameter.scaleFactor;
            for (int n = 1; n < resolution; n++) {
                a_n = a_0_old / (M_PI * n) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth));
                
                for (long unsigned int i = 0; i < parameter.segmentedResult.size(); i++) {
                    T translateFactor = parameter.translateFactor;
                    int oldN = (i % (resolution - 1)) + 1;

                    if (abs(oldN/scaleFactor - n) < 1e-12) { 
                        a_n += 2 * ((0.5 * parameter.endWidth - 0.5 * parameter.startWidth) * std::cos(oldN * M_PI * translateFactor) 
                            + std::sin(oldN * M_PI * translateFactor  + 2 * n * M_PI * parameter.endWidth) / (4 * n * M_PI) 
                            - std::sin(oldN * M_PI * translateFactor  + 2 * n * M_PI * parameter.startWidth) / (4 * n * M_PI)) 
                            * parameter.segmentedResult[i];
                    } else {
                        a_n += (1 / ((oldN * M_PI / scaleFactor) + n * M_PI)) *
                            (std::sin(oldN * M_PI * translateFactor  + parameter.endWidth * (oldN * M_PI / scaleFactor + n * M_PI))
                            - std::sin(oldN * M_PI * translateFactor + parameter.startWidth * (oldN * M_PI / scaleFactor + n * M_PI)))
                            * parameter.segmentedResult[i];
                        a_n += (1 / ((oldN * M_PI / scaleFactor) - n * M_PI)) * 
                            (std::sin(oldN * M_PI * translateFactor  + parameter.endWidth * (oldN * M_PI / scaleFactor - n * M_PI)) 
                            - std::sin(oldN * M_PI * translateFactor  + parameter.startWidth * (oldN * M_PI / scaleFactor - n * M_PI)))
                            * parameter.segmentedResult[i];
                    }
                }
                f_sum += a_n * std::cos(n * M_PI * w) * std::exp(-n*n*M_PI*M_PI* (1 / pecletNr) * (channelLength/channelWidth));          
            }
        }
        return 0.5 * a_0 + f_sum + fConstant(w); 
    };

    return {f, segmentedResult, a_0}; 
}

// From Channel Start to Channel End for complex input
template<typename T>
std::tuple<std::function<T(T)>,std::vector<T>,T> DiffusionMixingModel<T>::getAnalyticalSolutionTotal(
    T channelLength, T currChannelFlowRate, T channelWidth, int resolution, int speciesId, 
    T pecletNr, const std::vector<FlowSection<T>>& flowSections, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& Mixtures) { 
    
    T prevEndWidth = 0.0;

    std::vector<FlowSectionInput<T>> constantFlowSections;
    std::vector<FlowSectionInput<T>> functionFlowSections;

    for (const auto& flowSection : flowSections) {
        T startWidth = prevEndWidth;
        T endWidth = startWidth + ((flowSection.flowRate / currChannelFlowRate));
        prevEndWidth = endWidth;
        T scaleFactor = (endWidth - startWidth) / (flowSection.sectionEnd - flowSection.sectionStart);
        T translateFactor = flowSection.sectionStart - startWidth / scaleFactor;
        #ifdef DEBUG
        assert(flowSection.sectionStart-startWidth >= (-1.0 - 1e-16) && flowSection.sectionStart-startWidth <= (1.0 + 1e-16));
        #endif

        if (!this->filledEdges.count(flowSection.channelId)){
            T concentration = 0.0;
            std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
            std::vector<T> zeroSegmentedResult = {0};
            constantFlowSections.push_back({startWidth, endWidth, 1.0, translateFactor, concentration, zeroFunction, zeroSegmentedResult, T(0.0)});
        } else {
            T mixtureId = this->filledEdges.at(flowSection.channelId); // get the diffusive mixture at a specific channelId
            DiffusiveMixture<T>* diffusiveMixture = dynamic_cast<DiffusiveMixture<T>*>(Mixtures.at(mixtureId).get()); // Assuming diffusiveMixtures is passed
            if (diffusiveMixture->getSpecieConcentrations().find(speciesId) == diffusiveMixture->getSpecieConcentrations().end()) { // the species is not in the mixture
                T concentration = 0.0;
                std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
                std::vector<T> zeroSegmentedResult = {0};
                constantFlowSections.push_back({startWidth, endWidth, 1.0, translateFactor, concentration, zeroFunction, zeroSegmentedResult, T(0.0)});
            } else if (diffusiveMixture->getIsConstant()) { 
                T concentration = diffusiveMixture->getConcentrationOfSpecie(speciesId); 
                std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
                std::vector<T> zeroSegmentedResult = {0};
                constantFlowSections.push_back({startWidth, endWidth, 1.0, translateFactor, concentration, zeroFunction, zeroSegmentedResult, T(0.0)}); 
            } else {
                auto specieDistribution = diffusiveMixture->getSpecieDistributions().at(speciesId);
                // std::function<T(T)> concentrationFunction = specieDistribution.first;
                // std::vector<T> segmentedResults = specieDistribution.second;
                std::function<T(T)> concentrationFunction = std::get<0>(specieDistribution);
                std::vector<T> segmentedResults = std::get<1>(specieDistribution);

                T a_0_old = std::get<2>(specieDistribution);
                // functionFlowSections.push_back({startWidth, endWidth, scaleFactor * (flowSection.width/channelWidth), translateFactor, T(0.0), concentrationFunction, segmentedResults, a_0_old});
                functionFlowSections.push_back({startWidth, endWidth, scaleFactor, translateFactor, T(0.0), concentrationFunction, segmentedResults, a_0_old});
            }
        }
    }

    auto [fConstant, segmentedResultConstant, a_0_Constant] = getAnalyticalSolutionConstant(channelLength, channelWidth, resolution, pecletNr, constantFlowSections);
    auto [fFunction, segmentedResultFunction, a_0_Function] = getAnalyticalSolutionFunction(channelLength, channelWidth, resolution, pecletNr, functionFlowSections, fConstant);

    segmentedResultFunction.insert(segmentedResultFunction.end(), segmentedResultConstant.begin(), segmentedResultConstant.end());

    T a_0 = a_0_Constant + a_0_Function;

    return {fFunction, segmentedResultFunction, a_0};

}



template<typename T>
void DiffusionMixingModel<T>::printTopology() {
    int iteration = 0;
    for (auto& flows : concatenatedFlows) {
        std::cout << "Group " << iteration << std::endl;
        for (auto& flow : flows) {
            std::cout << "Channel " << flow.channelId << " has radial angle = " << flow.radialAngle
                << " and is ";
            if (!flow.inFlow) {
                std::cout << " an outflow.\n";
            } else {
                std::cout << " an inflow.\n";
            }
        }
        std::cout << "\n";
        iteration++;
    }
}

template<typename T>
void DiffusionMixingModel<T>::clean(arch::Network<T>* network) {
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            if (this->mixturesInEdge.count(channel->getId())){
                for (auto& [mixtureId, endPos] : this->mixturesInEdge.at(channel->getId())) {
                    if (endPos == 1.0) {
                        this->mixturesInEdge.at(channel->getId()).pop_front();
                    } else {
                        break;
                    }
                }
            }
        }
    }
}

template<typename T>
void DiffusionMixingModel<T>::printMixturesInNetwork() {
    for (auto& [channelId, deque] : this->mixturesInEdge) {
        for (auto& [mixtureId, endPos] : deque) {
            std::cout << "Mixture " << mixtureId << " in channel " << channelId << 
            " at position " << endPos << std::endl;
        }
    }
}

}   /// namespace sim
