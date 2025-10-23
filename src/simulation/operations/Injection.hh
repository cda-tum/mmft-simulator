#include "Injection.h"

namespace sim {

template<typename T>
DropletInjection<T>::DropletInjection(size_t id, DropletImplementation<T>* droplet, T injectionTime, arch::Channel<T>* channel, T injectionPosition, AbstractDroplet<T>* simRef) : 
    id(id), droplet(droplet), injectionTime(injectionTime), injectionPosition(arch::ChannelPosition<T>(channel, injectionPosition)), simRef(simRef) { ++injectionCounter; }

template<typename T>
void DropletInjection<T>::setInjectionPosition(T position) {
    if (position < 0.0 || position > 1.0) {
        throw std::invalid_argument("Injection position should be between 0.0 and 1.0.");
    }
    this->injectionPosition.setPosition(position);  
}

template<typename T>
const std::shared_ptr<arch::Channel<T>> DropletInjection<T>::getInjectionChannel() const { 
    return simRef->getNetwork()->getChannel(injectionPosition.getChannel()->getId()); 
}

template<typename T>
void DropletInjection<T>::setInjectionChannel(const std::shared_ptr<arch::Channel<T>>& channel) { 
    simRef->getNetwork()->getChannel(channel->getId()); // check if channel exists in network
    injectionPosition.setChannel(channel.get()); 
}

}  // namespace sim
