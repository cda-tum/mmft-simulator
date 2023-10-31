#include "BolusInjection.h"

#include <memory>
#include <utility>

#include "ChannelPosition.h"

namespace sim {

template<typename T>
BolusInjection<T>::BolusInjection(int id, int mixtureId, arch::Pump<T>* injectionPump, T injectionTime) : id(id), mixtureId(mixtureId), injectionPump(injectionPump), injectionTime(injectionTime) {}

template<typename T>
void BolusInjection<T>::setName(std::string name) {
    name = std::move(name);
}

template<typename T>
int BolusInjection<T>::getId() const {
    return id;
}

template<typename T>
std::string BolusInjection<T>::getName() const {
    return name;
}

template<typename T>
T BolusInjection<T>::getInjectionTime() const {
    return injectionTime;
}

template<typename T>
arch::Pump<T>* BolusInjection<T>::getInjectionPump() {
    return injectionPump;
}

template<typename T>
int BolusInjection<T>::getMixtureId() {
    return mixtureId;
}

template<typename T>
void BolusInjection<T>::setPerformed(bool performed) {
    this->performed = performed;
}

template<typename T>
bool BolusInjection<T>::wasPerformed() {
    return performed;
}

}  // namespace sim