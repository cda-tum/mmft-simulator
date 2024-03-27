#include "MergingEvent.h"

namespace sim {

template<typename T>
MergeBifurcationEvent<T>::MergeBifurcationEvent(T time, Droplet<T>& droplet0, Droplet<T>& droplet1, DropletBoundary<T>& boundary0, Simulation<T>& simulation) : 
    Event<T>(time, 0), droplet0(droplet0), droplet1(droplet1), boundary0(boundary0), simulation(simulation) { }

template<typename T>
void MergeBifurcationEvent<T>::performEvent() {
    auto newDroplet = simulation.mergeDroplets(droplet0.getId(), droplet1.getId());

    // add boundaries from droplet0
    for (auto& boundary : droplet0.getBoundaries()) {
        // do not add boundary to new droplet
        if (boundary.get() == &boundary0) {
            continue;
        }

        // add boundary
        newDroplet->addBoundary(boundary->getChannelPosition().getChannel(), boundary->getChannelPosition().getPosition(), boundary->isVolumeTowardsNodeA(), boundary->getState());
    }

    // add fully occupied channels from droplet0
    for (auto& channel : droplet0.getFullyOccupiedChannels()) {
        newDroplet->addFullyOccupiedChannel(channel);
    }

    // add boundaries from droplet1
    for (auto& boundary : droplet1.getBoundaries()) {
        newDroplet->addBoundary(boundary->getChannelPosition().getChannel(), boundary->getChannelPosition().getPosition(), boundary->isVolumeTowardsNodeA(), boundary->getState());
    }

    // add fully occupied channels from droplet1
    for (auto& channel : droplet1.getFullyOccupiedChannels()) {
        newDroplet->addFullyOccupiedChannel(channel);
    }

    // check if droplet0 is inside a single channel, because if not then also a fully occupied channel has to be added
    if (!droplet0.isInsideSingleChannel()) {
        newDroplet->addFullyOccupiedChannel(boundary0.getChannelPosition().getChannel());
    }

    // add/remove droplets form network
    newDroplet->setDropletState(DropletState::NETWORK);
    droplet0.setDropletState(DropletState::SINK);
    droplet1.setDropletState(DropletState::SINK);
}

template<typename T>
void MergeBifurcationEvent<T>::print() {
    std::cout << "\n Merge Bifurcation Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}

template<typename T>
MergeChannelEvent<T>::MergeChannelEvent(T time, Droplet<T>& droplet0, Droplet<T>& droplet1, DropletBoundary<T>& boundary0, DropletBoundary<T>& boundary1, Simulation<T>& simulation) : 
    Event<T>(time, 0), droplet0(droplet0), droplet1(droplet1), boundary0(boundary0), boundary1(boundary1), simulation(simulation) {}

template<typename T>
void MergeChannelEvent<T>::performEvent() {
    auto newDroplet = simulation.mergeDroplets(droplet0.getId(), droplet1.getId());

    // add boundaries from droplet0
    for (auto& boundary : droplet0.getBoundaries()) {
        // do not add boundary0 to new droplet
        if (boundary.get() == &boundary0) {
            continue;
        }

        // add boundary
        newDroplet->addBoundary(boundary->getChannelPosition().getChannel(), boundary->getChannelPosition().getPosition(), boundary->isVolumeTowardsNodeA(), boundary->getState());
    }

    // add fully occupied channels from droplet0
    for (auto& channel : droplet0.getFullyOccupiedChannels()) {
        newDroplet->addFullyOccupiedChannel(channel);
    }

    // add boundaries from droplet1
    for (auto& boundary : droplet1.getBoundaries()) {
        // do not add boundary1 to new droplet
        if (boundary.get() == &boundary1) {
            continue;
        }

        // add boundary
        newDroplet->addBoundary(boundary->getChannelPosition().getChannel(), boundary->getChannelPosition().getPosition(), boundary->isVolumeTowardsNodeA(), boundary->getState());
    }

    // add fully occupied channels from droplet1
    for (auto& channel : droplet1.getFullyOccupiedChannels()) {
        newDroplet->addFullyOccupiedChannel(channel);
    }

    // check if droplet0 and droplet1 are not inside a single channel, because then a fully occupied channel has to be added
    if (!droplet0.isInsideSingleChannel() && !droplet1.isInsideSingleChannel()) {
        // boundary0 and boundary1 must have the same channel
        newDroplet->addFullyOccupiedChannel(boundary0.getChannelPosition().getChannel());
    }

    // add/remove droplets from network
    newDroplet->setDropletState(DropletState::NETWORK);
    droplet0.setDropletState(DropletState::SINK);
    droplet1.setDropletState(DropletState::SINK);
}

template<typename T>
void MergeChannelEvent<T>::print() {
    std::cout << "\n Merge Channel Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}

}  // namespace sim
