
#include "CfdEvent.h"

namespace sim {

template<typename T>
CfdChannelInflowEvent<T>::CfdChannelInflowEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, int nodeId, CFDSimulator<T>& simulator) : 
    Event<T>("CFD Channel Inflow Event", time, 1), droplet(droplet), boundary(boundary), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdChannelInflowEvent<T>::performEvent() {

    const auto channelPtr = simulator.getVirtualChannel(nodeId);

    boundary.getChannelPosition().setChannel(channelPtr);
    boundary.getChannelPosition().setPosition(0.0);

    simulator.addPendingDroplet(&droplet, nodeId);
    
    // The head boundary of the droplet is entering the virtual channel
    if (droplet.getDropletState() != DropletState::VIRTUAL) {
        droplet.setDropletState(DropletState::VIRTUAL);
    }

    if (droplet.getVolume() > channelPtr->getVolume()) {
        throw std::runtime_error("A droplet that entered a virtual channel could not be fully contained (droplet volume > channel volume).");
    }
}

template<typename T>
CfdChannelOutflowEvent<T>::CfdChannelOutflowEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, int nodeId, CFDSimulator<T>& simulator) : 
    Event<T>("CFD Channel Outflow Event", time, 1), droplet(droplet), boundary(boundary), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdChannelOutflowEvent<T>::performEvent() {

    const auto channelPtr = simulator.getRealChannel(nodeId);

    // Update the position of the passed boundary
    boundary.getChannelPosition().setChannel(channelPtr);
    if (channelPtr->getNodeA() == nodeId) {
        boundary.getChannelPosition().setPosition(0.0);
    } else {
        boundary.getChannelPosition().setPosition(1.0);
    }

    // The head boundary of the droplet is leaving the virtual channel
    if (droplet.getDropletState() != DropletState::NETWORK) {
        droplet.setDropletState(DropletState::NETWORK);
    }

    // If one of the boundaries is still in the virtual channel, return here
    for (auto& boundary : droplet.getBoundaries()) {
        if (boundary->getChannelPosition().getChannel() == simulator.getVirtualChannel(nodeId)) {
            return;
        }
    }

    // The droplet is erased from the createdDroplet vector, if no boundaries are in the virtual channel
    simulator.eraseCreatedDroplet(droplet.getId(), nodeId);
}

template<typename T>
CfdInjectionEvent<T>::CfdInjectionEvent(T time, Droplet<T>& droplet, int nodeId, CFDSimulator<T>& simulator) : 
    Event<T>("CFD Injection Event", time, 1), droplet(droplet), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdInjectionEvent<T>::performEvent() {

    droplet.setDropletState(DropletState::SINK);
    simulator.generateDroplet(&droplet, nodeId);
    simulator.erasePendingDroplet(droplet.getId(), nodeId);
}

}  // namespace sim
