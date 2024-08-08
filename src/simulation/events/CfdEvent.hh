
#include "CfdEvent.h"

namespace sim {

template<typename T>
CfdChannelEvent<T>::CfdChannelEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, int nodeId, const essLbmDropletSimulator<T>& simulator) : 
    Event<T>("CFD Channel Event", time, 1), droplet(droplet), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdChannelEvent<T>::performEvent() {

    const RectangularChannel<T>* channelPtr = simulator.getVirtualChannel(nodeId);

    boundary.getChannelPosition().setChannel(channelPtr);
    boundary.getChannelPosition().setPosition(0.0);
    
    // The head boundary of the droplet is entering the virtual channel
    if (droplet.getDropletState() != DropletState::VIRTUAL) {
        droplet.setDropletState(DropletState::VIRTUAL);
    }

    if (droplet.getVolume() > channelPtr->getVolume()) {
        throw std::runtime_error("A droplet that entered a virtual channel could not be fully contained (droplet volume > channel volume).");
    }
}

template<typename T>
CfdInjectionEvent<T>::CfdInjectionEvent(T time, Droplet<T>& droplet, int nodeId, const essLbmDropletSimulator<T>& simulator) : 
    Event<T>("CFD Injection Event", time, 1), droplet(droplet), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdInjectionEvent<T>::performEvent() {

    droplet.setDropletState(DropletState::SINK);
    simulator->generateDroplet(&droplet, nodeId);
    simulator->eraseDroplet(droplet.getId(), nodeId);
}

}  // namespace sim
