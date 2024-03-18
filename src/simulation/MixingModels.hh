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
    std::cout << "The minimal timestep is now " << this->minimalTimeStep << std::endl;
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
InstantaneousMixingModel<T>::InstantaneousMixingModel() { }

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
void InstantaneousMixingModel<T>::injectMixtureInEdge(int mixtureId, int channelId) {
    if (this->mixturesInEdge.count(channelId)) {
        this->mixturesInEdge.at(channelId).push_back(std::make_pair(mixtureId, T(0.0)));
    } else {
        std::deque<std::pair<int,T>> newDeque;
        newDeque.push_back(std::make_pair(mixtureId, T(0.0)));
        this->mixturesInEdge.try_emplace(channelId, newDeque);
    }
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
void DiffusionMixingModel<T>::updateDiffusiveMixtures(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures) {
    updateNodeInflow(timeStep, network, sim, mixtures);
    clean(network);
}

template<typename T>
void DiffusionMixingModel<T>::updateNodeInflow(T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures) {
    std::cout<< "Getting here 1" << std::endl;
    for (auto& [nodeId, node] : network->getNodes()) {
        bool generateInflow = false;
        for (auto& [channelId, channel] : network->getChannels()) {
            std::cout<< "Getting here 2" << std::endl;
            // If the channel flows into this node
            if ((channel->getFlowRate() > 0.0 && channel->getNodeB() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeA() == nodeId)) {
                std::cout<< "Getting here 3" << std::endl;
                T inflowVolume = std::abs(channel->getFlowRate()) * timeStep;
                T movedDistance = inflowVolume / channel->getVolume();
                std::cout << "The current channelId is " << channelId << std::endl;
                if (this->mixturesInEdge.count(channel->getId())) {
                    for (auto& [mixtureId, endPos] : this->mixturesInEdge.at(channel->getId())) {
                        std::cout<< "Getting here 4" << std::endl;
                        // Propage the mixture positions in the channel with movedDistance
                        T newEndPos = std::min(endPos + movedDistance, 1.0);
                        endPos = newEndPos;
                        if (newEndPos == 1.0) {
                            std::cout<< "Getting here 5" << std::endl;
                            // if the mixture front left the channel, it's fully filled with the current mixture
                            if (filledEdges.count(channel->getId())) {
                                filledEdges.at(channel->getId()) = mixtureId;
                            } else {
                                filledEdges.try_emplace(channel->getId(), mixtureId);
                            }
                            // We must generate the outflow of this node, i.e., the inflow of the channels that flow out of the node
                            generateInflow = true;
                        }   
                    }
                }
            }
        }
        std::cout<< "Getting here 6" << std::endl;
        // Due to the nature of the diffusive mixing model, per definition a new mixture is created.
        // It is unlikely that this exact mixture, with same species and functions already exists
        if (generateInflow) {
            std::cout<< "Getting here 7" << std::endl;
            generateInflows(nodeId, timeStep, network, sim, mixtures);
        }
    }
}

template<typename T>
void DiffusionMixingModel<T>::generateInflows(int nodeId, T timeStep, arch::Network<T>* network, Simulation<T>* sim, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& mixtures) {
    // Perform topoology analysis at node, to know how the inflow sections and their relative order.
    std::cout<< "Start topology analysis for node " << nodeId << std::endl;
    topologyAnalysis(network, nodeId);
    std::cout<< "Finished topology analysis for node " << nodeId << std::endl;
    for (auto& [channelId, channel] : network->getChannels()) {
        // If the channel flows out of this node
        if ((channel->getFlowRate() > 0.0 && channel->getNodeA() == nodeId) || (channel->getFlowRate() < 0.0 && channel->getNodeB() == nodeId)) {
            std::cout<< "Getting here 8" << std::endl;
            // Loop over all species in the incoming mixtures and add to set of present species
            std::set<int> presentSpecies;
            for (auto& [channelId, fowSections] : outflowDistributions) {
                std::cout << "We have flowsections listed for channel " << channelId << std::endl;  
            }
            for (auto& section : outflowDistributions.at(channel->getId())) {
                std::cout<< "Getting here 8.1" << std::endl;
                std::cout<< "section.channelId is " << section.channelId << std::endl;
                for (auto& [channelId, mixtureId] : filledEdges) {
                    std::cout << "In filled edges we have channel " << channelId << " with mixture " << mixtureId << std::endl;  
                }
                std::cout << "The size of mixtures is " << mixtures.size() << std::endl;
                for (auto& [mixtureId, mixture_ptr] : mixtures) {
                    std::cout << "In mixtures we have mixture " << mixtureId << std::endl;
                }
                if (filledEdges.count(section.channelId)) {
                    for (auto& [specieId, pair] : mixtures.at(filledEdges.at(section.channelId))->getSpecieDistributions()) {
                        std::cout<< "Getting here 8.2" << std::endl;
                        std::cout << "The specie concentrations are for specie " << specieId << std::endl;
                        if (presentSpecies.find(specieId) == presentSpecies.end()) {
                            presentSpecies.emplace(specieId);
                        }
                    }
                }
            }
            std::cout<< "Getting here 9" << std::endl;
            // We should evaluate the final function of the mixture reaching the end of this channel
            std::unordered_map<int, std::tuple<std::function<T(T)>,std::vector<T>,T>> newDistributions;
            std::cout<< "Getting here 9.1" << std::endl;
            for (auto& specieId : presentSpecies) {
                std::cout<< "Getting here 9.2" << std::endl;
                T pecletNr = (channel->getFlowRate() / channel->getHeight()) / (sim->getSpecie(specieId))->getDiffusivity();
                std::cout<< "Getting here 9.3" << std::endl;
                std::tuple<std::function<T(T)>, std::vector<T>, T> analyticalResult = getAnalyticalSolutionTotal(
                    channel->getLength(), channel->getFlowRate(), channel->getWidth(), this->resolution, specieId, 
                    pecletNr, outflowDistributions.at(channel->getId()), mixtures);
                std::cout<< "Getting here 9.4" << std::endl;
                newDistributions.try_emplace(specieId, analyticalResult); // TODO
                std::cout<< "Getting here 9.5" << std::endl;
                int numValues = 100;
                T step = 1.0 / (numValues);
                for (int i = 0; i < numValues; ++i) {
                    T x = i * step;
                    T y;
                    // y = analyticalResult.first(x);
                    y = std::get<0>(analyticalResult)(x);
                    std::cout << std::setprecision(4) << x << "," << y << "\n"; 
                }
            }
            std::cout<< "Getting here 10" << std::endl;
            //Create new DiffusiveMixture
            DiffusiveMixture<T>* newMixture = sim->addDiffusiveMixture(newDistributions);
            newMixture->setNonConstant();
            std::cout<< "Getting here 11" << std::endl;
            this->injectMixtureInEdge(newMixture->getId(), channelId);
            std::cout<< "Getting here 12" << std::endl;
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
    if (!node->getGround()){

        // 1. List all connecting channels with angle and inflow
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
            T flowRateIn;
            T flowRateOut;
            for (auto& channel : concatenatedFlows[in]) {
                flowRateIn += network->getChannel(channel.channelId)->getFlowRate();
            }
            for (auto& channel : concatenatedFlows[out]) {
                flowRateOut += network->getChannel(channel.channelId)->getFlowRate();
            }

            std::vector<T> inflowCuts = {0.0};
            std::vector<int> channelInIDs;
            // Calculate inflow separations
            for (auto channelIn = concatenatedFlows[in].rbegin(); channelIn != concatenatedFlows[in].rend(); ++channelIn) {
                T newCut = inflowCuts.back() + network->getChannel(channelIn->channelId)->getFlowRate()/flowRateIn;
                inflowCuts.push_back(std::min(newCut, 1.0));
                channelInIDs.push_back(channelIn->channelId);
            }

            std::vector<T> outflowCuts = {0.0};
            std::vector<int> channelOutIDs;
            for (auto& channelOut : concatenatedFlows[out]) {
                T newCut = outflowCuts.back() + network->getChannel(channelOut.channelId)->getFlowRate()/flowRateOut;
                outflowCuts.push_back(std::min(newCut, 1.0));
                channelOutIDs.push_back(channelOut.channelId);
            }

            int n_in = 0;
            int n_out = 0;
            T start = 0.0;
            T end = 0.0;
            for (auto& channelOutId : channelOutIDs) {
                for (auto& channelInId : channelInIDs) {
                    FlowSection<T> inFlowSection;
                    inFlowSection.channelId = channelInId;
                    if (inflowCuts[n_in + 1] < outflowCuts[n_out + 1]) {
                        T flowRate = (1.0 - start) * network->getChannel(channelInId)->getFlowRate();
                        inFlowSection = FlowSection<T> {channelInId, start, 1.0, flowRate};
                        if (outflowDistributions.count(channelOutId)) {
                            outflowDistributions.at(channelOutId).push_back(inFlowSection);
                        } else {
                            std::vector<FlowSection<T>> newFlowSectionVector = {inFlowSection};
                            outflowDistributions.try_emplace(channelOutId, newFlowSectionVector);
                        }
                        n_in++;
                        start = 0.0;
                    } else {
                        T flowRate = (outflowCuts[n_out + 1] - std::max(inflowCuts[n_in], outflowCuts[n_out]))*flowRateIn;
                        end = start + flowRate / network->getChannel(channelInId)->getFlowRate();
                        inFlowSection = FlowSection<T> {channelInId, start, end, flowRate};
                        if (outflowDistributions.count(channelOutId)) {
                            outflowDistributions.at(channelOutId).push_back(inFlowSection);
                        } else {
                            std::vector<FlowSection<T>> newFlowSectionVector = {inFlowSection};
                            outflowDistributions.try_emplace(channelOutId, newFlowSectionVector);
                        }
                        n_out++;
                        start = end;
                        break;
                    }
                }                    
            }

        } else if (concatenatedFlows.size() == 4){
            // Assume case with 2 pairs of opposing channels
            int lowest = 0;
            int n = 0;
            for (auto flowIt : concatenatedFlows) {
                if (network->getChannel(flowIt[0].channelId)->getFlowRate() < network->getChannel(concatenatedFlows.at(lowest)[0].channelId)->getFlowRate()) {
                    lowest = n;
                }
                n++;
            }
            auto flow = concatenatedFlows.at(lowest);
            auto clockWise = concatenatedFlows.at((lowest + 1) % 4);
            auto counterClockWise = concatenatedFlows.at((lowest - 1) % 4);
            auto opposed = concatenatedFlows.at((lowest + 2) % 4);

            if (flow.at(0).inFlow) {

                T ratio = (network->getChannel(counterClockWise.at(0).channelId)->getFlowRate() - 0.5*network->getChannel(flow.at(0).channelId)->getFlowRate()) 
                            / network->getChannel(opposed.at(0).channelId)->getFlowRate();

                FlowSection<T> flow1 {flow.at(0).channelId, 0.0, 0.5, 0.5*network->getChannel(flow.at(0).channelId)->getFlowRate()};
                std::vector<FlowSection<T>> newFlow1Vector = {flow1};
                outflowDistributions.try_emplace(clockWise.at(0).channelId, newFlow1Vector);
                FlowSection<T> flow2 {opposed.at(0).channelId, 0.0, ratio, ratio*network->getChannel(opposed.at(0).channelId)->getFlowRate()};
                std::vector<FlowSection<T>> newFlow2Vector = {flow2};
                outflowDistributions.try_emplace(counterClockWise.at(0).channelId, newFlow2Vector);

                FlowSection<T> flow3 {flow.at(0).channelId, 0.5, 1.0, 0.5*network->getChannel(flow.at(0).channelId)->getFlowRate()};
                outflowDistributions.at(counterClockWise.at(0).channelId).push_back(flow3);
                FlowSection<T> flow4 {opposed.at(0).channelId, ratio, 1.0, (1.0 - ratio)*network->getChannel(opposed.at(0).channelId)->getFlowRate()};
                outflowDistributions.at(clockWise.at(0).channelId).push_back(flow4);

            } else {
                T ratio = network->getChannel(clockWise.at(0).channelId)->getFlowRate() / 
                            (network->getChannel(clockWise.at(0).channelId)->getFlowRate() + network->getChannel(counterClockWise.at(0).channelId)->getFlowRate());
                T ratio1 = (ratio*network->getChannel(flow.at(0).channelId)->getFlowRate())/(network->getChannel(clockWise.at(0).channelId)->getFlowRate());
                T ratio2 = ((1.0-ratio)*network->getChannel(flow.at(0).channelId)->getFlowRate())/(network->getChannel(counterClockWise.at(0).channelId)->getFlowRate());

                FlowSection<T> flow1 {clockWise.at(0).channelId, 0.0, (1.0-ratio1), (1.0-ratio1)*network->getChannel(clockWise.at(0).channelId)->getFlowRate()};
                std::vector<FlowSection<T>> newFlow1Vector = {flow1};
                outflowDistributions.try_emplace(opposed.at(0).channelId, newFlow1Vector);

                FlowSection<T> flow2 {counterClockWise.at(0).channelId, 0.0, ratio2, ratio2*network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()};
                std::vector<FlowSection<T>> newFlow2Vector = {flow2};
                outflowDistributions.try_emplace(flow.at(0).channelId, newFlow2Vector);

                FlowSection<T> flow3 {clockWise.at(0).channelId, (1.0-ratio1), 1.0, ratio1*network->getChannel(clockWise.at(0).channelId)->getFlowRate()};
                outflowDistributions.at(flow.at(0).channelId).push_back(flow3);
                FlowSection<T> flow4 {counterClockWise.at(0).channelId, ratio2, 1.0, (1.0 - ratio2)*network->getChannel(counterClockWise.at(0).channelId)->getFlowRate()};
                outflowDistributions.at(opposed.at(0).channelId).push_back(flow4);
            }
        } else {
            throw std::invalid_argument("Too many channels at node " + std::to_string(nodeId) + " for DiffusionMixingModel.");
        }
    }
}

template<typename T>
std::tuple<std::function<T(T)>, std::vector<T>, T>DiffusionMixingModel<T>::getAnalyticalSolutionConstant(T channelLength, T channelWidth, int resolution, T pecletNr, const std::vector<FlowSectionInput<T>>& parameters) { 
    T a_0 = 0.0;
    std::vector<T> segmentedResult;

    for (const auto& parameter : parameters) {
        for (int n = 1; n < resolution; n++) {
            T a_n = (2/(n * M_PI))  * (parameter.concentrationAtChannelEnd) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth)); 
                segmentedResult.push_back(a_n * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * channelLength));
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
                f_sum += a_n * std::cos(n * M_PI * (w)) * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * channelLength);
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
    for (const auto& parameter : parameters) {
        T a_0_old = parameter.a_0_old;
        T stretchFactor = parameter.stretchFactor;
        for (int n = 1; n < resolution; n++) {
            a_n = a_0_old / (M_PI * n) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth));
            
            for (int i = 0; i < parameter.segmentedResult.size(); i++) {
                T startWidthIfFunctionWasSplit = parameter.startWidthIfFunctionWasSplit;
                int oldN = (i % (resolution - 1)) + 1;

                if (abs(oldN/stretchFactor - n) < 1e-8) { 
                    a_n += 2 * (0.5 * parameter.endWidth - 0.5 * parameter.startWidth + std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + 2 * n * M_PI * parameter.endWidth) / (4 * n * M_PI) - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + 2 * n * M_PI * parameter.startWidth) / (4 * n * M_PI)) 
                        * parameter.segmentedResult[i];
                } else {
                    a_n += (1 / ((oldN * M_PI / stretchFactor) + n * M_PI)) *
                        (std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.endWidth * (oldN * M_PI / stretchFactor) + parameter.endWidth * (n * M_PI))
                        - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.startWidth * (oldN * M_PI / stretchFactor) + parameter.startWidth * (n * M_PI)))
                        * parameter.segmentedResult[i];
                    a_n += (1 / ((oldN * M_PI / stretchFactor) - n * M_PI)) * 
                        (std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.endWidth * (oldN * M_PI / stretchFactor) - parameter.endWidth * (n * M_PI)) 
                        - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.startWidth * (oldN * M_PI / stretchFactor) - parameter.startWidth * (n * M_PI)))
                        * parameter.segmentedResult[i];
                }
            }
            segmentedResult.push_back(a_n * std::exp(-pow(n, 2) * pow(M_PI, 2) * (1 / pecletNr) * channelLength));
        }
    }

    auto f = [channelLength, channelWidth, resolution, pecletNr, parameters, fConstant](T w) { // This returns C(w, l_1)
        T f_sum = 0.0;
        T a_0 = 0.0;
        T a_n = 0.0;

        // calculating a_0
        for (const auto& parameter : parameters) {
            T a_0_old = parameter.a_0_old;
            a_0 += a_0_old * (parameter.endWidth - parameter.startWidth); 
            for (int i = 0; i < parameter.segmentedResult.size(); i++) { 
                T stretchFactor = parameter.stretchFactor;
                T startWidthIfFunctionWasSplit = parameter.startWidthIfFunctionWasSplit;
                int oldN = (i % (resolution - 1)) + 1;
                a_0 += 2 * parameter.segmentedResult[i] * stretchFactor/(oldN * M_PI) * 
                    (std::sin(oldN * M_PI * parameter.endWidth/stretchFactor + oldN * M_PI * startWidthIfFunctionWasSplit) 
                    - std::sin(oldN * M_PI * parameter.startWidth/stretchFactor + oldN * M_PI * startWidthIfFunctionWasSplit));
            }
        }     

        // Calculating a_n
        for (const auto& parameter : parameters) {
            T a_0_old = parameter.a_0_old;
            T stretchFactor = parameter.stretchFactor;
            for (int n = 1; n < resolution; n++) {
                a_n = a_0_old / (M_PI * n) * (std::sin(n * M_PI * parameter.endWidth) - std::sin(n * M_PI * parameter.startWidth));
                
                for (int i = 0; i < parameter.segmentedResult.size(); i++) {
                    T startWidthIfFunctionWasSplit = parameter.startWidthIfFunctionWasSplit;
                    int oldN = (i % (resolution - 1)) + 1;

                    if (abs(oldN/stretchFactor - n) < 1e-8) { 
                        a_n += 2 * (0.5 * parameter.endWidth - 0.5 * parameter.startWidth + std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + 2 * n * M_PI * parameter.endWidth) / (4 * n * M_PI) - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + 2 * n * M_PI * parameter.startWidth) / (4 * n * M_PI)) 
                            * parameter.segmentedResult[i];
                    } else {
                        a_n += (1 / ((oldN * M_PI / stretchFactor) + n * M_PI)) *
                            (std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.endWidth * (oldN * M_PI / stretchFactor) + parameter.endWidth * (n * M_PI))
                            - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.startWidth * (oldN * M_PI / stretchFactor) + parameter.startWidth * (n * M_PI)))
                            * parameter.segmentedResult[i];
                        a_n += (1 / ((oldN * M_PI / stretchFactor) - n * M_PI)) * 
                            (std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.endWidth * (oldN * M_PI / stretchFactor) - parameter.endWidth * (n * M_PI)) 
                            - std::sin(oldN * M_PI * startWidthIfFunctionWasSplit + parameter.startWidth * (oldN * M_PI / stretchFactor) - parameter.startWidth * (n * M_PI)))
                            * parameter.segmentedResult[i];
                    }
                }
                f_sum += a_n * std::cos(n * M_PI * w) * std::exp(-n*n*M_PI*M_PI* (1 / pecletNr) * channelLength);          
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
    T pecletNr, const std::vector<FlowSection<T>>& flowSections, std::unordered_map<int, std::unique_ptr<DiffusiveMixture<T>>>& diffusiveMixtures) { 
    std::cout << "Getting into getAnalyticalSolutionTotal" << std::endl;
    T prevEndWidth = 0.0;

    std::vector<FlowSectionInput<T>> constantFlowSections;
    std::vector<FlowSectionInput<T>> functionFlowSections;

    for (const auto& flowSection : flowSections) {
        T startWidth = prevEndWidth;
        T endWidth = startWidth + (1/(flowSection.flowRate / currChannelFlowRate));
        prevEndWidth = endWidth;
        T stretchFactor = flowSection.flowRate / currChannelFlowRate;
        T startWidthIfFunctionWasSplit = flowSection.sectionStart;

        if (!filledEdges.count(flowSection.channelId)){
            T concentration = 0.0;
            std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
            std::vector<T> zeroSegmentedResult = {0};
            constantFlowSections.push_back({startWidth, endWidth, stretchFactor, startWidthIfFunctionWasSplit, concentration, zeroFunction, zeroSegmentedResult, T(0.0)});
        } else {
            T mixtureId = filledEdges.at(flowSection.channelId); // get the diffusive mixture at a specific channelId
            DiffusiveMixture<T>* diffusiveMixture = diffusiveMixtures.at(mixtureId).get(); // Assuming diffusiveMixtures is passed
            if (diffusiveMixture->getSpecieConcentrations().find(speciesId) == diffusiveMixture->getSpecieConcentrations().end()) { // the species is not in the mixture
                T concentration = 0.0;
                std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
                std::vector<T> zeroSegmentedResult = {0};
                constantFlowSections.push_back({startWidth, endWidth, stretchFactor, startWidthIfFunctionWasSplit, concentration, zeroFunction, zeroSegmentedResult, T(0.0)});
            } else if (diffusiveMixture->getIsConstant()) { 
                T concentration = diffusiveMixture->getConcentrationOfSpecie(speciesId); 
                std::function<T(T)> zeroFunction = [](T) -> T { return 0.0; };
                std::vector<T> zeroSegmentedResult = {0};
                constantFlowSections.push_back({startWidth, endWidth, stretchFactor, startWidthIfFunctionWasSplit, concentration, zeroFunction, zeroSegmentedResult, T(0.0)}); 
            } else {
                auto specieDistribution = diffusiveMixture->getSpecieDistributions().at(speciesId);
                // std::function<T(T)> concentrationFunction = specieDistribution.first;
                // std::vector<T> segmentedResults = specieDistribution.second;
                std::function<T(T)> concentrationFunction = std::get<0>(specieDistribution);
                std::vector<T> segmentedResults = std::get<1>(specieDistribution);

                T a_0_old = std::get<2>(specieDistribution);
                functionFlowSections.push_back({startWidth, endWidth, stretchFactor, startWidthIfFunctionWasSplit, T(0.0), concentrationFunction, segmentedResults, a_0_old});
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
void DiffusionMixingModel<T>::injectMixtureInEdge(int mixtureId, int channelId) {
    if (this->mixturesInEdge.count(channelId)) {
        this->mixturesInEdge.at(channelId).push_back(std::make_pair(mixtureId, T(0.0)));
    } else {
        std::deque<std::pair<int,T>> newDeque;
        newDeque.push_back(std::make_pair(mixtureId, T(0.0)));
        this->mixturesInEdge.try_emplace(channelId, newDeque);
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
