
#include "CfdEvent.h"

namespace sim {

template<typename T>
CfdChannelEvent<T>::CfdChannelEvent(T time, Droplet<T>& droplet, int nodeId, const essLbmDropletSimulator<T>& simulator) : 
    Event<T>("CFD Channel Event", time, 1), droplet(droplet), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdChannelEvent<T>::performEvent() {

    /** TODO:
     * The droplet is transferred to a virtual channel of the cfd Simulator
     * 
     * Set the droplet status to Virtual
     * Add the droplet to pending droplets in the virtual channel of the node Id
     * 
     * If the droplet status is already virtual, it means that the tail boundary
     * is now entering the virtual channel
     */
}

template<typename T>
CfdInjectionEvent<T>::CfdInjectionEvent(T time, Droplet<T>& droplet, int nodeId, const essLbmDropletSimulator<T>& simulator) : 
    Event<T>("CFD Injection Event", time, 1), droplet(droplet), nodeId(nodeId), simulator(simulator) { }

template<typename T>
void CfdInjectionEvent<T>::performEvent() {

    /** TODO:
     * This event is triggered when the droplet reaches 0.5 of virtual channel
     * 
     * Remove the droplet from pending droplets in the virtual channel
     * Generate the droplet in the CFD domain.
     */

}

}  // namespace sim
