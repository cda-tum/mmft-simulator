#include "Injection.h"

namespace sim {

template<typename T>
DropletInjection<T>::DropletInjection(size_t id, DropletImplementation<T>* droplet, T injectionTime, arch::Channel<T>* channel, T injectionPosition) : 
    id(id), droplet(droplet), injectionTime(injectionTime), injectionPosition(arch::ChannelPosition<T>(channel, injectionPosition)) { ++injectionCounter; }

}  // namespace sim
