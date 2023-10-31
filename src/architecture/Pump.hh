#include "Pump.h"

namespace arch {

template<typename T>
Pump<T>::Pump() { }

template<typename T>
void Pump<T>::setMixture(int mixtureId) {
    this->mixtureId = mixtureId;
}

template<typename T>
int Pump<T>::getMixtureId() {
    return mixtureId;
}

}  // namespace arch