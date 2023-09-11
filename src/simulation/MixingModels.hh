#include "MixingModels.h"

namespace sim {

template<typename T>
InstantaneousMixingModel<T>::InstantaneousMixingModel() { }

template<typename T>
void InstantaneousMixingModel<T>::updateMixtures(T timeStep) {

    /**
     * Calculate and store the mixtures flowing into all nodes.
    */
    for (auto& [nodeId, node] : network->getNodes()) {
        // only consider channels where there is an inflow at this node (= where the flow direction is into the node)
        if ((channel->getFlowRate() > 0.0 && channel->getNodeB()->getId() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA()->getId() == nodeId)) {
            for (auto& [mixtureId, endPos] : mixturesInEdge.at(channel->getId())) {
                T movedDistance = (std::abs(channel->getFlowRate()) * timeStep) / channel->getVolume();
                T newEndPos = std::min(endPos + movedDistance, 1.0);
                endPos = newEndPos;
                if (newEndPos == 1.0) {
                    // fluid flows into node, add to mixture inflow
                    T inflowVolume = movedDistance * channel->getArea();
                    MixtureInflow<T> mixtureInflow = {mixtureId, inflowVolume};
                    mixtureInflowAtNode[nodeId].push_back(mixtureInflow);
                    auto [iterator, inserted] = totalInflowVolumeAtNode.try_emplace(nodeId, inflowVolume);
                    if (!inserted) {
                        iterator->second = iterator->second + inflowVolume;
                    }
                }
            }
        }
    }

    /**
     * Calculate mixture outflow at each node from all inflows
    */
    for (auto& [nodeId, mixtureInflowList] : mixtureInflowAtNode) {
        std::unordered_map<int, T> newFluidConcentrations;
        for (auto& mixtureInflow : mixtureInflowList) {
            for (auto& [fluidId, oldConcentration] : mixtures.at(mixtureInflow.mixtureId).getFluidConcentrations()) {
                T newConcentration = oldConcentration * mixtureInflow.inflowVolume / totalInflowVolumeAtNode.at(nodeId);
                auto [iterator, inserted] = newFluidConcentrations.try_emplace(fluidId, newConcentration);
                if (!inserted) {
                    iterator->second = iterator->second + newConcentration;
                }
            }
        }

        int newMixtureId = addMixture(std::move(newFluidConcentrations));
        mixtureOutflowAtNode.try_emplace(nodeId, newMixtureId);
    }

    /**
     * Add the node outflow as inflow to the channels
    */
    for (auto& [nodeId, node] : chip->getNodes()) {
        for (auto& channel : chip->getChannelsAtNode(nodeId)) {
            // check if edge is an outflow edge to this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeA()->getId() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB()->getId() == nodeId)) {
                T newPos = std::abs(channel->getFlowRate()) * timeStep / channel->getVolume();
                assert(newPos <= 1.0 && newPos >= 0.0);
                bool oldEqualsNewConcentration = true;
                auto& oldFluidConcentrations = mixtures.at(mixturesInEdge.at(channel->getId()).back().first).getFluidConcentrations();
                if (mixtureOutflowAtNode.count(nodeId)) {
                    mixturesInEdge.at(channel->getId()).push_back(std::make_pair(mixtureOutflowAtNode.at(nodeId), newPos));
                }
            }
        }
    }

    /**
     * Remove fluids that have 'outflowed' their channel
    */
    for (auto& [nodeId, node] : chip->getNodes()) {
        for (auto& channel : chip->getChannelsAtNode(nodeId)) {
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