#include "MixingModels.h"

#include <unordered_map>
#include <deque>
#include <iostream>

#define M_PI 3.14159265358979323846

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


//=========================================================================================
//================================  diffusive Mixing  =====================================
//=========================================================================================

template<typename T>
DiffusionMixingModel<T>::DiffusionMixingModel() : MixingModel<T>() { }

template<typename T>
void DiffusionMixingModel<T>::updateMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<Mixture<T>>>& mixtures) {

}

template<typename T>
void DiffusionMixingModel<T>::updateMixtureDistributions(T timeStep, arch::Network<T>* network, Simulation<T>* sim) {
    topologyAnalysis();
}

template<typename T>
void DiffusionMixingModel<T>::topologyAnalysis( arch::Network<T>* network ) {
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
    for (auto& [nodeId, node] : network->getNodes()) {
        if (!node->getGround()){

            // 1. List all connecting angles with angle and inflow
            std::vector<RadialPosition<T>> channelOrder;
            for (auto& [channelId, channel] : network->getChannels()) {
                bool inflow;
                arch::Node<T>* nodeA = network->getNode(channel->getNodeA()).get();
                arch::Node<T>* nodeB = network->getNode(channel->getNodeB()).get();
                T dx = ( nodeId == channel->getNodeA() ) ? nodeB->getPosition()[0]-nodeA->getPosition()[0] : nodeA->getPosition()[0]-nodeB->getPosition()[0];
                T dy = ( nodeId == channel->getNodeA() ) ? nodeB->getPosition()[1]-nodeA->getPosition()[1] : nodeA->getPosition()[1]-nodeB->getPosition()[1];
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
                int n = 0;(relative, 0.0-1.0)
                for (auto& flow : channelOrder) {
                    if (flow.inFlow == concatenatedFlow.back().inFlow) {
                        concatenatedFlow.emplace_back( std::move(channelOrder.front()) );
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
                T flowRateIn;
                T flowRateOut;
                for (auto& channel : concatenatedFlows[in]) {
                    flowRateIn += network->getChannel(channel.channelId)->getFlowRate();
                }
                for (auto& channel : concatenatedFlows[out]) {
                    flowRateOut += network->getChannel(channel.channelId)->getFlowRate();
                }

                std::vector<T> inflow = {0.0};
                std::vector<int> channelInIDs;
                // Calculate inflow separations
                for (auto& channelIn = concatenatedFlows[in].rbegin(); channelIn != concatenatedFlows[in].rend(); ++channelIn) {
                    T newCut = inflow.back() + network->getChannel(channelIn.channelId)->getFlowRate()/flowRateIn;
                    inflow.push_back(std::min(newCut, 1.0));
                    channelInIDs.push_back(channelIn.channelId);
                }

                for (auto& channelOut : concatenatedFlows[out]) {
                    bool filled = false;
                }


                FlowSection<T> inFlowSection = {angle, channel->getId(), inflow};
                outflowDistributions.push_back()
            } else {
                // loop over groups and look at their neighbouring groups
            }

                /*
                for (auto& channelOut : concatenatedFlows[out]) {
                    bool filled = false;
                    T percentage = 0.0;
                    std::vector<FlowSection<T>> outflowDistribution;
                    for (auto& channelIn = begin(concatenatedFlows[in] + n); channelIn != end(concatenatedFlows[in]); channelIn++ ) {
                        T start;
                        T end;
                        if (!outflowDistribution.empty() && !filled) {
                            end = outflowDistribution.back().start;
                        } else {
                            end = 1.0;
                        }
                        if (flowRateIn < restFlowRateOut) {
                            start = 1.0 - (flowRateIn/flowRateOut);
                            n++;
                        } else {
                            start = 0.0;
                            filled = true;
                        }
                        FlowSection<T> inFlowSection = {channelIn->channelId, start, end};
                        outflowDistribution.push_back(inFlowSection);
                        if (filled) {
                            break;
                        }
                    }
                    std::reverse(outflowDistribution.begin(), outflowDistribution.end());
                    outflowDistributions.try_emplace(channelOut.channelId, outflowDistribution);
                }
                */

            for (auto& group : concatenatedFlows) {
                // If it is an outflow group
                if ( !group.front().inflow ) {

                }
            }


            if (concatenatedFlows.size() > 2) {

            }

            /**
             * What we want to have in the end
             * 
             * The channels need to know what stuff is flowing in.
             * So the nodes need to list PER OUTFLOWING CHANNEL, what is going to flow into them.
             * 
             * More precisely, per outflowing channel:
             * - list of concatenated flows
             * - these are sections of incoming flows
             * - and are mapped to a new relative width.
            */

            // 5. Inflow group is always (?) split into the 1 or 2 neighbouring outflow groups

            // 6. Outflow groups always (?) merge the incoming 1 or 2 neighbouring inflow groups

            // 7. Outflow groups always (?) split the total income into channels.

        }
    }

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
void DiffusionMixingModel<T>::updateNodeInflow() {
    // TODO
}

template<typename T>
void DiffusionMixingModel<T>::generateInflows() {
    // TODO
}

template<typename T>
void DiffusionMixingModel<T>::generateNodeOutflow() {
    // TODO
}

template<typename T>
void DiffusionMixingModel<T>::updateChannelInflow() {
    // TODO
}

}   /// namespace sim