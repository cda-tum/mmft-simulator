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

    printMixturesInNetwork();
    std::cout << "[Debug] Calculate mixture outflow at each node from all inflows." << std::endl;
    generateNodeOutflow(sim, mixtures);
    printMixturesInNetwork();
    std::cout << "[Debug] Add the node outflow as inflow to the channels." << std::endl;
    updateChannelInflow(timeStep, network, mixtures);
    printMixturesInNetwork();
    std::cout << "[Debug] Remove mixtures that have 'outflowed' their channel." << std::endl;
    clean(network);
    printMixturesInNetwork();
    std::cout << "[Debug] Update minimal timestep." << std::endl;
    updateMinimalTimeStep(network);
    printMixturesInNetwork();
    std::cout << "[Debug] Finish updateMixtures." << std::endl;
}

template<typename T>
void InstantaneousMixingModel<T>::updateNodeInflow(T timeStep, arch::Network<T>* network) {

    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& [channelId, channel] : network->getChannels()) {
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                if (mixturesInEdge.count(channel->getId())){
                    for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                        T movedDistance = (std::abs(channel->getFlowRate()) * timeStep) / channel->getVolume();
                        T newEndPos = std::min(endPos + movedDistance, 1.0);
                        endPos = newEndPos;
                        if (newEndPos == 1.0 && !node->getSink()) {
                            std::cout<< "We do recognize mixture " << mixtureId << " as flowing into node " << nodeId << std::endl;
                            // specie flows into node, add to mixture inflow
                            T inflowVolume = movedDistance * channel->getArea();
                            MixtureInFlow<T> mixtureInflow = {mixtureId, inflowVolume};
                            mixtureInflowAtNode[nodeId].push_back(mixtureInflow);
                            auto [iterator, inserted] = totalInflowVolumeAtNode.try_emplace(nodeId, inflowVolume);
                            if (!inserted) {
                                iterator->second = iterator->second + inflowVolume;
                            }
                        }   
                    }
                } 
            }
        }
    }
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

        Mixture<T>* newMixture = sim->addMixture(newConcentrations);
        mixtureOutflowAtNode.try_emplace(nodeId, newMixture->getId());
    }
    for (auto& [nodeId, mixtureId] : mixtureOutflowAtNode) {
        std::cout << "We have mixture " << mixtureId << " flowing out at node " << nodeId << std::endl;
    }
}

template<typename T>
void InstantaneousMixingModel<T>::updateChannelInflow(T timeStep, arch::Network<T>* network, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            // check if edge is an outflow edge to this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
                //T newPos = std::abs(channel->getFlowRate()) * timeStep / channel->getVolume();
                //assert(newPos <= 1.0 && newPos >= 0.0);
                //bool oldEqualsNewConcentration = true;
                //if (mixturesInEdge.count(channel->getId())){
                //auto& oldConcentrations = mixtures.at(mixturesInEdge.at(channel->getId()).back().first)->getSpecieConcentrations();
                if (mixtureOutflowAtNode.count(nodeId)) {
                    injectMixtureInEdge(mixtureOutflowAtNode.at(nodeId), channel->getId());
                    //mixturesInEdge.at(channel->getId()).push_back(std::make_pair(mixtureOutflowAtNode.at(nodeId), 0.0));
                }
                //}
            }
        }
    }
}

template<typename T>
void InstantaneousMixingModel<T>::clean(arch::Network<T>* network) {
    
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            if (mixturesInEdge.count(channel->getId())){
                //auto count = 0;  // to not remove the 1.0 fluid if there is only one
                for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                    //remove mixtures that completely flow out of channel (only 1 fluid with pos 1.0 left)
                    if (endPos == 1.0) {
                        //if (count != 0) {
                        mixturesInEdge.at(channel->getId()).pop_front();
                        // if the mixture front left the channel, it's fully filled
                        if (filledEdges.count(channel->getId())) {
                            filledEdges.at(channel->getId()) = mixtureId;
                        } else {
                            filledEdges.try_emplace(channel->getId(), mixtureId);
                        }
                        //}
                    } else {
                        break;
                    }
                    //count++;
                }
            }
        }
    }
    mixtureInflowAtNode.clear();
    mixtureOutflowAtNode.clear();
}

template<typename T>
void InstantaneousMixingModel<T>::updateMinimalTimeStep(arch::Network<T>* network) {
    this->minimalTimeStep = 0.0;
    std::cout << "[Debug] Set timestep to 0.0" << std::endl;
    for (auto& [channelId, deque] : mixturesInEdge) {
        T channelVolume = network->getChannel(channelId)->getVolume();
        T channelFlowRate = network->getChannel(channelId)->getFlowRate();
        for (auto& [mixtureId, endPos] : deque) {
            std::cout << "This is mixture " << mixtureId << " in channel " << channelId << 
            " at position " << endPos << std::endl;
            T flowTime = (1.0 - endPos)*channelVolume/channelFlowRate;
            if (this->minimalTimeStep < 1e-12) {
                this->minimalTimeStep = flowTime;
                std::cout << "[Debug] Update timestep to " << (1.0 - endPos)*channelVolume << "/" << channelFlowRate << " = " << flowTime << std::endl;
            } else if (flowTime < this->minimalTimeStep) {
                this->minimalTimeStep = flowTime;
                std::cout << "[Debug] Update timestep to " << (1.0 - endPos)*channelVolume << "/" << channelFlowRate << " = " << flowTime << std::endl;
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
            std::cout << "This is mixture " << mixtureId << " in channel " << channelId << 
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