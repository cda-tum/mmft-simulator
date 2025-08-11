#include "Injection.h"

namespace sim {

template<typename T>
DropletInjection<T>::DropletInjection(unsigned int id, DropletImplementation<T>* droplet, T injectionTime, arch::RectangularChannel<T>* channel, T injectionPosition) : 
    id(id), droplet(droplet), injectionTime(injectionTime), injectionPosition(arch::ChannelPosition<T>(channel, injectionPosition)) { }

}  // namespace sim
