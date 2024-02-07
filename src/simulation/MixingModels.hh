#include "MixingModels.h"

#include <unordered_map>
#include <deque>
#include <iostream>

namespace sim {

template<typename T>
MixingModel<T>::MixingModel() { }

template<typename T>
T MixingModel<T>::getMinimalTimeStep() {
    return this->minimalTimeStep;
}

template<typename T>
InstantaneousMixingModel<T>::InstantaneousMixingModel() { }

template<typename T>
void InstantaneousMixingModel<T>::updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    generateNodeOutflow(sim, mixtures);
    updateChannelInflow(timeStep, network, mixtures);
    clean(network);
    updateMinimalTimeStep(network);
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
                if (mixturesInEdge.count(channel->getId())){
                    for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                        T newEndPos = std::min(endPos + movedDistance, 1.0);
                        endPos = newEndPos;
                        if (newEndPos == 1.0) {
                            // if the mixture front left the channel, it's fully filled
                            if (filledEdges.count(channel->getId())) {
                                filledEdges.at(channel->getId()) = mixtureId;
                            } else {
                                filledEdges.try_emplace(channel->getId(), mixtureId);
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
            if (filledEdges.count(channelId)  && !network->getNode(nodeId)->getSink()) {
                MixtureInFlow<T> mixtureInflow = {filledEdges.at(channelId), inflowVolume};
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
                    injectMixtureInEdge(mixtureOutflowAtNode.at(nodeId), channel->getId());
                }
            }
        }
    }
}

template<typename T>
void InstantaneousMixingModel<T>::clean(arch::Network<T>* network) {
    
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            if (mixturesInEdge.count(channel->getId())){
                for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                    if (endPos == 1.0) {
                        mixturesInEdge.at(channel->getId()).pop_front();
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
void InstantaneousMixingModel<T>::updateMinimalTimeStep(arch::Network<T>* network) {
    this->minimalTimeStep = 0.0;
    for (auto& [channelId, deque] : mixturesInEdge) {
        T channelVolume = network->getChannel(channelId)->getVolume();
        T channelFlowRate = network->getChannel(channelId)->getFlowRate();
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
void InstantaneousMixingModel<T>::injectMixtureInEdge(int mixtureId, int channelId) {
    if (mixturesInEdge.count(channelId)) {
        mixturesInEdge.at(channelId).push_back(std::make_pair(mixtureId, T(0.0)));
    } else {
        std::deque<std::pair<int,T>> newDeque;
        newDeque.push_back(std::make_pair(mixtureId, T(0.0)));
        mixturesInEdge.try_emplace(channelId, newDeque);
    }
}

template<typename T>
void InstantaneousMixingModel<T>::printMixturesInNetwork() {
    for (auto& [channelId, deque] : mixturesInEdge) {
        for (auto& [mixtureId, endPos] : deque) {
            std::cout << "Mixture " << mixtureId << " in channel " << channelId << 
            " at position " << endPos << std::endl;
        }
    }
}

template<typename T>
const std::deque<std::pair<int,T>>& InstantaneousMixingModel<T>::getMixturesInEdge(int channelId) const {
    return mixturesInEdge.at(channelId);
}

template<typename T>
const std::unordered_map<int, std::deque<std::pair<int,T>>>& InstantaneousMixingModel<T>::getMixturesInEdges() const {
    return mixturesInEdge;
}

template<typename T>
const std::unordered_map<int, int>& InstantaneousMixingModel<T>::getFilledEdges() const {
    return filledEdges;
}

}   /// namespace sim