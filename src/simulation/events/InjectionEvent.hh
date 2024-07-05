#include "InjectionEvent.h"

namespace sim {

template<typename T>
DropletInjectionEvent<T>::DropletInjectionEvent(T time, DropletInjection<T>& injection) : 
    Event<T>(time, 1), injection(injection) {}

template<typename T>
void DropletInjectionEvent<T>::performEvent() {
    // injection position of the droplet (center of the droplet)
    auto channelPosition = injection.getInjectionPosition();
    auto channel = channelPosition.getChannel();
    auto droplet = injection.getDroplet();

    // for the injection the two boundaries (basically a head and a tail) of the droplet must lie inside the channel (the volume of the droplet must be small enough)
    // this is already checked at the creation of the injection and, thus, we don't need this check here

    // the droplet length is a relative value between 0 and 1
    T dropletLength = droplet->getVolume() / channel->getVolume();

    // compute position of the two boundaries
    T position0 = (channelPosition.getPosition() - dropletLength / 2);  // is always the position which lies closer to 0
    T position1 = (channelPosition.getPosition() + dropletLength / 2);  // is always the position which lies closer to 1

    // create corresponding boundaries
    // since boundary0 always lies closer to 0, the volume of this boundary points to node1
    // since boundary1 always lies closer to 1, the volume of this boundary points to node0
    droplet->addBoundary(channel, position0, false, BoundaryState::NORMAL);
    droplet->addBoundary(channel, position1, true, BoundaryState::NORMAL);

    // set droplet state
    droplet->setDropletState(DropletState::NETWORK);
}

template<typename T>
void DropletInjectionEvent<T>::print() {
    std::cout << "\n Droplet Injection Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}


template<typename T>
MixtureInjectionEvent<T>::MixtureInjectionEvent(T time, MixtureInjection<T>& injection, MixingModel<T>* mixingModel) : 
    Event<T>(time, 1), injection(injection), mixingModel(mixingModel) { }

template<typename T>
void MixtureInjectionEvent<T>::performEvent() {

    auto channel = injection.getInjectionChannel();
    auto mixture = injection.getMixtureId();

    mixingModel->injectMixtureInEdge(mixture, channel->getId());

    injection.setPerformed(true);
}

template<typename T>
void MixtureInjectionEvent<T>::print() {
    std::cout << "\n Mixture Injection Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
}

}  // namespace sim
