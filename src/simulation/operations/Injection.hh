#include "Injection.h"

namespace sim {

template<typename T>
DropletInjection<T>::DropletInjection(int id, Droplet<T>* droplet, T injectionTime, arch::RectangularChannel<T>* channel, T injectionPosition) : 
    id(id), droplet(droplet), injectionTime(injectionTime), injectionPosition(arch::ChannelPosition<T>(channel, injectionPosition)) { }

template<typename T>
void DropletInjection<T>::setName(std::string name) {
    name = std::move(name);
}

template<typename T>
int DropletInjection<T>::getId() const {
    return id;
}

template<typename T>
std::string DropletInjection<T>::getName() const {
    return name;
}

template<typename T>
T DropletInjection<T>::getInjectionTime() const {
    return injectionTime;
}

template<typename T>
const arch::ChannelPosition<T>& DropletInjection<T>::getInjectionPosition() const {
    return injectionPosition;
}

template<typename T>
Droplet<T>* DropletInjection<T>::getDroplet() const {
    return droplet;
}

}  // namespace sim
