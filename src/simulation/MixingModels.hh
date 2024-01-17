#include "MixingModels.h"

#include <unordered_map>
#include <iostream>

namespace sim {

template<typename T>
InstantaneousMixingModel<T>::InstantaneousMixingModel() { }

template<typename T>
void InstantaneousMixingModel<T>::updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

    std::cout << "[Debug] Calculate and store the mixtures flowing into all nodes." << std::endl;
    /**
     * Calculate and store the mixtures flowing into all nodes.
    */
    for (auto& [nodeId, node] : network->getNodes()) {
        std::cout << "[Debug] Getting here...1" << std::endl;
        // only consider channels where there is an inflow at this node (= where the flow direction is into the node)
        for (auto& [channelId, channel] : network->getChannels()) {
            std::cout << "[Debug] Getting here...2" << std::endl;
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                std::cout << "[Debug] Getting here...3" << std::endl;
                for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                    std::cout << "[Debug] Getting here...4" << std::endl;
                    T movedDistance = (std::abs(channel->getFlowRate()) * timeStep) / channel->getVolume();
                    T newEndPos = std::min(endPos + movedDistance, 1.0);
                    endPos = newEndPos;
                    if (newEndPos == 1.0) {
                        std::cout << "[Debug] Getting here...5" << std::endl;
                        // specie flows into node, add to mixture inflow
                        T inflowVolume = movedDistance * channel->getArea();
                        MixtureInFlow<T> mixtureInflow = {mixtureId, inflowVolume};
                        mixtureInflowAtNode[nodeId].push_back(mixtureInflow);
                        auto [iterator, inserted] = totalInflowVolumeAtNode.try_emplace(nodeId, inflowVolume);
                        if (!inserted) {
                            std::cout << "[Debug] Getting here...6" << std::endl;
                            iterator->second = iterator->second + inflowVolume;
                        }
                    }   
                }
            }
        }
    }

    std::cout << "[Debug] Calculate mixture outflow at each node from all inflows." << std::endl;

    /**
     * Calculate mixture outflow at each node from all inflows
    */
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

    std::cout << "[Debug] Add the node outflow as inflow to the channels." << std::endl;

    /**
     * Add the node outflow as inflow to the channels
    */
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            // check if edge is an outflow edge to this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
                T newPos = std::abs(channel->getFlowRate()) * timeStep / channel->getVolume();
                assert(newPos <= 1.0 && newPos >= 0.0);
                bool oldEqualsNewConcentration = true;
                auto& oldConcentrations = mixtures.at(mixturesInEdge.at(channel->getId()).back().first)->getSpecieConcentrations();
                if (mixtureOutflowAtNode.count(nodeId)) {
                    mixturesInEdge.at(channel->getId()).push_back(std::make_pair(mixtureOutflowAtNode.at(nodeId), newPos));
                }
            }
        }
    }

    std::cout << "[Debug] Remove mixtures that have 'outflowed' their channel." << std::endl;

    /**
     * Remove mixtures that have 'outflowed' their channel
    */
    for (auto& [nodeId, node] : network->getNodes()) {
        for (auto& channel : network->getChannelsAtNode(nodeId)) {
            auto count = 0;  // to not remove the 1.0 fluid if there is only one
            for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                //remove mixtures that completely flow out of channel (only 1 fluid with pos 1.0 left)
                if (endPos == 1.0) {
                    if (count != 0) {
                        mixturesInEdge.at(channel->getId()).pop_front();
                    }
                } else {
                    break;
                }
                count++;
            }
        }
    }
}

}   /// namespace sim