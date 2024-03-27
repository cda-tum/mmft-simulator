
#include "BoundaryEvent.h"

namespace sim {

template<typename T>
BoundaryHeadEvent<T>::BoundaryHeadEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, const arch::Network<T>& network) : 
    Event<T>(time, 1), droplet(droplet), boundary(boundary), network(network) { }

template<typename T>
void BoundaryHeadEvent<T>::performEvent() {
    // get boundary channel
    auto boundaryChannel = boundary.getChannelPosition().getChannel();

    // get correct node (is the opposite node of the boundary reference node inside the channel)
    int node = boundary.isVolumeTowardsNodeA() ? boundaryChannel->getNodeB() : boundaryChannel->getNodeA();

    // if this node is a sink then remove the whole droplet from the network
    if ((network.isSink(node))) {
        droplet.setDropletState(DropletState::SINK);
        return;
    }

    // get next channels
    std::vector<arch::RectangularChannel<T>*> nextChannels = network.getChannelsAtNode(node);

    // choose branch with the highest instantaneous flow rate
    T maxFlowRate;
    arch::RectangularChannel<T>* nextChannel = nullptr;
    for (auto* channel : nextChannels) {
        // do not consider the boundary channel and only consider Normal channels
        if (channel == boundaryChannel || channel->getChannelType() != arch::ChannelType::NORMAL) {
            continue;
        }

        // get the correct direction of the flow rate, where a positive flow rate means an outflow (away from the droplet center)
        T flowRate = channel->getNodeA() == node ? channel->getFlowRate() : -(channel->getFlowRate());

        // only consider channels with a positive flow rate
        if (flowRate <= 0) {
            continue;
        }

        // find maximal flow rate
        if (nextChannel == nullptr || flowRate > maxFlowRate) {
            maxFlowRate = flowRate;
            nextChannel = channel;
        }
    }

    if (nextChannel == nullptr) {
        // no new channel was found => the boundary goes into a Wait state
        boundary.setState(BoundaryState::WAIT_OUTFLOW);
        return;
    }

    // check if the droplet was in a single channel, if not then the boundary channel has to be added as fully occupied channel
    if (!droplet.isInsideSingleChannel()) {
        droplet.addFullyOccupiedChannel(boundaryChannel);
    }

    // get new position (is either 0.0 or 1.0, depending on if node0 or node1 of the nextChannel is the referenceNode)
    T channelPosition = nextChannel->getNodeA() == node ? 0.0 : 1.0;
    bool volumeTowardsNodeA = nextChannel->getNodeA() == node;

    // set new channel, position, direction of volume, and state of the boundary
    boundary.getChannelPosition().setChannel(nextChannel);
    boundary.getChannelPosition().setPosition(channelPosition);
    boundary.setVolumeTowardsNodeA(volumeTowardsNodeA);
    boundary.setState(BoundaryState::NORMAL);
}

template<typename T>
void BoundaryHeadEvent<T>::print() {
    std::cout << "\n Boundary Head Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}

template<typename T>
BoundaryTailEvent<T>::BoundaryTailEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, const arch::Network<T>& network) : 
    Event<T>(time, 1), droplet(droplet), boundary(boundary), network(network) { }

template<typename T>
void BoundaryTailEvent<T>::performEvent() {
    // get reference node of boundary
    auto referenceNode = boundary.getReferenceNode();

    // get the other boundaries and fully occupied channels inside this droplet that have the same reference node
    auto boundaries = droplet.getConnectedBoundaries(referenceNode, &boundary);  // do not consider the actual boundary
    auto fullyOccupiedChannels = droplet.getConnectedFullyOccupiedChannels(referenceNode);

    // if more than a single entity (boundary or fully occupied channel) is present, then remove the boundary, otherwise switch the channel
    if (boundaries.size() + fullyOccupiedChannels.size() == 1) {
        // only one entity is present => switch channel

        // get next channel
        auto nextChannel = boundaries.size() == 1 ? boundaries[0]->getChannelPosition().getChannel() : fullyOccupiedChannels[0];

        // get new position (is either 0.0 or 1.0, depending on if node0 or node1 of the nextChannel is the referenceNode)
        T channelPosition = nextChannel->getNodeA() == referenceNode ? 0.0 : 1.0;
        bool volumeTowardsNodeA = nextChannel->getNodeA() != referenceNode;

        // set new channel, position, direction of volume, and state of the boundary
        boundary.getChannelPosition().setChannel(nextChannel);
        boundary.getChannelPosition().setPosition(channelPosition);
        boundary.setVolumeTowardsNodeA(volumeTowardsNodeA);
        boundary.setState(BoundaryState::NORMAL);

        // remove fully occupied channel if present
        if (fullyOccupiedChannels.size() == 1) {
            droplet.removeFullyOccupiedChannel(nextChannel->getId());
        }
    } else {
        // more than one entity is present => remove boundary
        droplet.removeBoundary(boundary);
    }
}

template<typename T>
void BoundaryTailEvent<T>::print() {
    std::cout << "\n Boundary Tail Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}

}  // namespace sim
