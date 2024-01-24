#include "Droplet.h"

namespace sim {

template<typename T>
DropletPosition<T>::DropletPosition() { }

///-----------------------------Droplet------------------------------------///

template<typename T>
Droplet<T>::Droplet(int id, T volume, Fluid<T>* fluid) : 
    id(id), volume(volume), fluid(fluid) { }

template<typename T>
void Droplet<T>::setVolume(T volume) {
    this->volume = volume;
}

template<typename T>
void Droplet<T>::setName(std::string name) {
    this->name = std::move(name);
}

template<typename T>
void Droplet<T>::setDropletState(DropletState dropletState) {
    this->dropletState = dropletState;
}

template<typename T>
int Droplet<T>::getId() const {
    return id;
}

template<typename T>
std::string Droplet<T>::getName() const {
    return name;
}

template<typename T>
T Droplet<T>::getVolume() const {
    return volume;
}

template<typename T>
DropletState Droplet<T>::getDropletState() const {
    return dropletState;
}

template<typename T>
const Fluid<T>* Droplet<T>::getFluid() const {
    return fluid;
}

template<typename T>
void Droplet<T>::addDropletResistance(const ResistanceModel<T>& model) {
    // check if droplet is in a single channel
    if (isInsideSingleChannel()) {
        auto channel = boundaries[0]->getChannelPosition().getChannel();
        // volumeInsideChannel = volumeChannel - (volumeBoundary0 - volumeChannel) - (volumeBoundary1 - volumeChannel) = volumeBoundary0 + volumeBoundary1 - volumeChannel
        T volumeInsideChannel = boundaries[0]->getVolume() + boundaries[1]->getVolume() - channel->getVolume();
        channel->addDropletResistance(model.getDropletResistance(channel, this, volumeInsideChannel));
    } else {
        // loop through boundaries
        for (auto& boundary : boundaries) {
            auto channel = boundary->getChannelPosition().getChannel();
            channel->addDropletResistance(model.getDropletResistance(channel, this, boundary->getVolume()));
        }

        // loop through fully occupied channels (if present)
        for (auto& channel : channels) {
            channel->addDropletResistance(model.getDropletResistance(channel, this, channel->getVolume()));
        }
    }
}

template<typename T>
const std::vector<std::unique_ptr<DropletBoundary<T>>>& Droplet<T>::getBoundaries() {
    return boundaries;
}

template<typename T>
std::vector<arch::RectangularChannel<T>*>& Droplet<T>::getFullyOccupiedChannels() {
    return channels;
}

template<typename T>
bool Droplet<T>::isAtBifurcation() {
    // TODO: maybe refine the definition, so it is only true if no channel branches of within the droplet range (then needs a chip reference as input)
    // right now it is true if the droplet is not in a single channel
    return !isInsideSingleChannel();
}

template<typename T>
bool Droplet<T>::isInsideSingleChannel() {
    // is inside a single channel when only two boundaries are present and they are in the same channel
    return channels.size() == 0 && boundaries.size() == 2 && boundaries[0]->getChannelPosition().getChannel() == boundaries[1]->getChannelPosition().getChannel();
}

template<typename T>
void Droplet<T>::addBoundary(arch::RectangularChannel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state) { 
    boundaries.push_back(std::make_unique<DropletBoundary<T>>(channel, position, volumeTowardsNodeA, state));
}

template<typename T>
void Droplet<T>::addFullyOccupiedChannel(arch::RectangularChannel<T>* channel) {
    channels.push_back(channel);
}

template<typename T>
void Droplet<T>::removeBoundary(DropletBoundary<T>& boundaryReference) {
    // TODO: remove more than one boundary at once (remove_if)

    for (unsigned int i = 0; i < boundaries.size(); i++) {
        if (boundaries[i].get() == &boundaryReference) {
            boundaries.erase(boundaries.begin() + i);
            break;
        }
    }
}

template<typename T>
void Droplet<T>::removeFullyOccupiedChannel(int channelId) {
    for (unsigned int i = 0; i < channels.size(); i++) {
        if (channels[i]->getId() == channelId) {
            channels.erase(channels.begin() + i);
            break;
        }
    }
}

template<typename T>
std::vector<DropletBoundary<T>*> Droplet<T>::getConnectedBoundaries(int nodeId, DropletBoundary<T>* doNotConsider) {
    std::vector<DropletBoundary<T>*> connectedBoundaries;
    for (auto& boundary : boundaries) {
        // do not consider boundary
        if (boundary.get() == doNotConsider) {
            continue;
        }

        if (boundary->getReferenceNode() == nodeId) {
            connectedBoundaries.push_back(boundary.get());
        }
    }

    return connectedBoundaries;
}

template<typename T>
std::vector<arch::RectangularChannel<T>*> Droplet<T>::getConnectedFullyOccupiedChannels(int nodeId) {
    std::vector<arch::RectangularChannel<T>*> connectedChannels;
    for (auto& channel : channels) {
        if (nodeId == channel->getNodeA() || nodeId == channel->getNodeB()) {
            connectedChannels.push_back(channel);
        }
    }

    return connectedChannels;
}

template<typename T>
void Droplet<T>::updateBoundaries(const arch::Network<T>& network) {
    // compute the average flow rates of all boundaries, since the inflow does not necessarily have to match the outflow (qInput != qOutput)
    // in order to avoid an unwanted increase/decrease of the droplet volume an average flow rate is computed
    // the actual flow rate of a boundary is then determined accordingly to the ratios of the different flowRates inside the channels

    // determine the state of the droplet
    T qInflow = 0;
    T qOutflow = 0;
    std::vector<DropletBoundary<T>*> outflowBoundaries;
    std::vector<DropletBoundary<T>*> inflowBoundaries;

    // loop through boundaries
    for (auto& boundary : boundaries) {
        // a boundary can stop at a bifurcation, e.g., when all flow rates of the connected channels have an inflow (point to the center of the boundary), or when only a bypass channel is present
        // when this happens it usually goes into a WaitOutflow state where the flow rate of the boundary becomes zero
        // here we have to check again, if this state still holds, or if the boundary can flow normally again
        boundary->updateWaitState(network);

        // if the boundary is still in a Wait state, then do not add it to the computation of the inflow/outflow
        if (boundary->isInWaitState()) {
            continue;
        }

        // this flow rate is already oriented in such a way, that a negative value indicates an inflow and a positive value an outflow
        // inflow => boundary moves towards the droplet center
        // outflow => boundary moves away from the droplet center
        T flowRate = boundary->getChannelFlowRate();

        if (flowRate < 0) {
            // inflow occurs
            inflowBoundaries.push_back(boundary.get());
            qInflow += -flowRate;  // only the absolute value of qInflow is important, hence, the minus sign
        } else if (flowRate > 0) {
            // outflow occurs
            outflowBoundaries.push_back(boundary.get());
            qOutflow += flowRate;
        } else {
            // flow rate is zero and, thus, does not contribute to the in/outflow
        }
    }

    if (inflowBoundaries.size() == 0 && outflowBoundaries.size() != 0) {
        // only outflow and no inflow occurs
        // this scenario is not supported yet and just stops the movement of all boundaries of this droplet
        // possible solution would be to split the droplet, or that slower boundaries get dragged along faster ones

        // just print a warning
        std::cout << "WARNING: All boundaries of droplet (id=" << id << ") move away from the center of the droplet. Droplet volume conservation cannot be guaranteed, hence the droplet movement is stopped." << std::endl;

        for (auto boundary : outflowBoundaries) {
            boundary->setFlowRate(0);
        }
    } else if (inflowBoundaries.size() != 0 && outflowBoundaries.size() == 0) {
        // only inflow and no outflow occurs
        // this scenario is not supported yet and just stops the movement of all boundaries of this droplet

        // just print a warning
        std::cout << "WARNING: All boundaries of droplet (id=" << id << ") move towards the center of the droplet. Droplet volume conservation cannot be guaranteed, hence the droplet movement is stopped." << std::endl;

        for (auto boundary : inflowBoundaries) {
            boundary->setFlowRate(0);
        }
    } else if (inflowBoundaries.size() != 0 && outflowBoundaries.size() != 0) {
        // outflow and inflow occurs

        // determine the average flow rate (important, when inflow and outflow are not the same for example due to a bypass channel)
        auto qAverage = (qInflow + qOutflow) / 2;

        // outflow boundaries
        for (auto boundary : outflowBoundaries) {
            //consider the slipFactor (droplet is faster than the continuous phase)
            boundary->setFlowRate(slipFactor * qAverage * boundary->getChannelFlowRate() / qOutflow);
        }

        // inflow boundaries
        for (auto boundary : inflowBoundaries) {
            //consider the slipFactor (droplet is faster than the continuous phase)
            boundary->setFlowRate(slipFactor * qAverage * boundary->getChannelFlowRate() / qInflow);
        }
    } else {
        // should not happen
        std::invalid_argument("This error should not occurr.");
    }
}

template<typename T>
void Droplet<T>::addMergedDroplet(Droplet<T>* droplet) {
    mergedDroplets.push_back(droplet);
}

template<typename T>
const std::vector<Droplet<T>*>& Droplet<T>::getMergedDroplets() const {
    return mergedDroplets;
}

///--------------------------DropletBoundary------------------------------------///

template<typename T>
DropletBoundary<T>::DropletBoundary(arch::RectangularChannel<T>* channel, T position, bool volumeTowardsNodeA, BoundaryState state) : 
    channelPosition(channel, position), volumeTowardsNodeA(volumeTowardsNodeA), state(state) { }

template<typename T>
arch::ChannelPosition<T>& DropletBoundary<T>::getChannelPosition() {
    return channelPosition;
}

template<typename T>
T DropletBoundary<T>::getFlowRate() const {
    return flowRate;
}

template<typename T>
bool DropletBoundary<T>::isVolumeTowardsNodeA() const {
    return volumeTowardsNodeA;
}

template<typename T>
BoundaryState DropletBoundary<T>::getState() const {
    return state;
}

template<typename T>
void DropletBoundary<T>::setFlowRate(T flowRate) {
    this->flowRate = flowRate;
}

template<typename T>
void DropletBoundary<T>::setVolumeTowardsNodeA(bool volumeTowardsNodeA) {
    this->volumeTowardsNodeA = volumeTowardsNodeA;
}

template<typename T>
void DropletBoundary<T>::setState(BoundaryState state) {
    this->state = state;
}

template<typename T>
arch::Node<T>* DropletBoundary<T>::getReferenceNode(arch::Network<T>* network) {
    if (volumeTowardsNodeA) {
        return network->getNode(channelPosition.getChannel()->getNodeA()).get();
    } else {
        return network->getNode(channelPosition.getChannel()->getNodeB()).get();
    }
}

template<typename T>
int DropletBoundary<T>::getReferenceNode() {
    if (volumeTowardsNodeA) {
        return channelPosition.getChannel()->getNodeA();
    } else {
        return channelPosition.getChannel()->getNodeB();
    }
}

template<typename T>
arch::Node<T>* DropletBoundary<T>::getOppositeReferenceNode(arch::Network<T>* network) {
    if (volumeTowardsNodeA) {
        return network->getNode(channelPosition.getChannel()->getNodeB()).get();
    } else {
        return network->getNode(channelPosition.getChannel()->getNodeA()).get();
    }
}

template<typename T>
int DropletBoundary<T>::getOppositeReferenceNode() {
    if (volumeTowardsNodeA) {
        return channelPosition.getChannel()->getNodeB();
    } else {
        return channelPosition.getChannel()->getNodeA();
    }
}

template<typename T>
T DropletBoundary<T>::getRemainingVolume() {
    T volume = 0;

    if (flowRate < 0) {
        // boundary moves towards the droplet center
        if (volumeTowardsNodeA) {
            volume = channelPosition.getVolumeA();
        } else {
            volume = channelPosition.getVolumeB();
        }
    } else if (flowRate > 0) {
        // boundary moves away from the droplet center
        if (volumeTowardsNodeA) {
            volume = channelPosition.getVolumeB();
        } else {
            volume = channelPosition.getVolumeA();
        }
    }

    return volume;
};

template<typename T>
T DropletBoundary<T>::getVolume() {
    if (volumeTowardsNodeA) {
        return channelPosition.getVolumeA();
    } else {
        return channelPosition.getVolumeB();
    }
}

template<typename T>
T DropletBoundary<T>::getChannelFlowRate() {
    T flowRate = channelPosition.getChannel()->getFlowRate();
    if (volumeTowardsNodeA) {
        return flowRate;
    } else {
        return -flowRate;
    }
}

template<typename T>
T DropletBoundary<T>::getTime() {
    T time = 0;

    if (flowRate < 0) {
        time = getRemainingVolume() / -flowRate;
    } else if (flowRate > 0) {
        time = getRemainingVolume() / flowRate;
    }

    return time;
}

template<typename T>
void DropletBoundary<T>::moveBoundary(T timeStep) {
    // check in which direction the volume goes
    if (volumeTowardsNodeA) {
        // positive flow rate indicates an outflow (movement towards node1) and, thus, the position inside the channel must increase
        // negative flow rate indicates an inflow (movement towards node0) and, thus, the position inside the channel must decrease
        channelPosition.addToPosition(flowRate * timeStep);
    } else {
        // positive flow rate indicates an outflow (movement towards node0) and, thus, the position inside the channel must decrease
        // negative flow rate indicates an inflow (movement towards node1) and, thus, the position inside the channel must increase
        channelPosition.addToPosition(-flowRate * timeStep);
    }
}

template<typename T>
void DropletBoundary<T>::updateWaitState(arch::Network<T> const& network) {
    if (state == BoundaryState::WAIT_INFLOW) {
        // in this scenario the boundary is in a Wait state while an inflow occurred (movement to the droplet center)
        // in this implementation this does not happen, because the boundary gets immediately deleted when it would reach this state (was not the case in a previous concept)
        // however, it is here for consistency and possible future implementations

        // get oriented channel flow rate
        T channelFlowRate = getChannelFlowRate();

        // check if the flow rate is not an inflow anymore (>0)
        // this indicates that the flow rate has changed since the last state and that the state should be Normal again
        if (channelFlowRate > 0) {
            state = BoundaryState::NORMAL;
        }
    } else if (state == BoundaryState::WAIT_OUTFLOW) {
        // in this scenario the boundary is in a Wait state while an outflow occurred (movement away from the droplet center)
        // this scenario happens at the "end" of a channel when a boundary wants to switch to another channel
        // it indicates that the boundary could not move any further, because no channel was available that matches the criteria when a boundary switches channels (e.g., only a bypass channel was available or no flow rate of the other channels had an outflow)
        // hence, it must be checked if the actual flow rate inside the channel has changed, or if there are now channels available that matches the criteria

        // get oriented channel flow rate
        T channelFlowRate = getChannelFlowRate();

        // check if the flow rate is not an outflow anymore (<0)
        // this indicates that the flow rate has changed since the last state and that the state should be Normal again (the boundary would move in the other direction again)
        if (channelFlowRate < 0) {
            state = BoundaryState::NORMAL;
            return;
        }

        // if the flow rate did not change, then check for valid channels
        auto boundaryChannel = channelPosition.getChannel();
        int nodeId = isVolumeTowardsNodeA() ? boundaryChannel->getNodeB() : boundaryChannel->getNodeA();
        for (auto& channel : network.getChannelsAtNode(nodeId)) {
            // do not consider boundary channel or channel that is not a Normal one
            if (channel == boundaryChannel || channel->getChannelType() != arch::ChannelType::NORMAL) {
                continue;
            }

            // check if a channel exists with an outflow (flow rate goes away from the node)
            T flowRate = nodeId == channel->getNodeA() ? channel->getFlowRate() : -channel->getFlowRate();
            if (flowRate > 0) {
                state = BoundaryState::NORMAL;
                return;
            }
        }
    }
}

template<typename T>
bool DropletBoundary<T>::isInWaitState() {
    return state == BoundaryState::WAIT_INFLOW || state == BoundaryState::WAIT_OUTFLOW;
}

}  // namespace sim
