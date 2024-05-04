#include "MixtureInjection.h"

namespace sim {

template<typename T>
MixtureInjection<T>::MixtureInjection(int id, int mixtureId, arch::RectangularChannel<T>* injectionChannel, T injectionTime) : id(id), mixtureId(mixtureId), injectionChannel(injectionChannel), injectionTime(injectionTime) {}

template<typename T>
void MixtureInjection<T>::setName(std::string name) {
    name = std::move(name);
}

template<typename T>
int MixtureInjection<T>::getId() const {
    return id;
}

template<typename T>
std::string MixtureInjection<T>::getName() const {
    return name;
}

template<typename T>
T MixtureInjection<T>::getInjectionTime() const {
    return injectionTime;
}

template<typename T>
arch::RectangularChannel<T>* MixtureInjection<T>::getInjectionChannel() {
    return injectionChannel;
}

template<typename T>
int MixtureInjection<T>::getMixtureId() {
    return mixtureId;
}

template<typename T>
void MixtureInjection<T>::setPerformed(bool performed) {
    this->performed = performed;
}

template<typename T>
bool MixtureInjection<T>::wasPerformed() {
    return performed;
}

}  // namespace sim