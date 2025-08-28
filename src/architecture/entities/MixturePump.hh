#include "MixturePump.h"

namespace arch {

template<typename T>
MixturePump<T>::MixturePump() { }

template<typename T>
void MixturePump<T>::setMixture(int mixtureId) {
    this->mixtureId = mixtureId;
}

template<typename T>
int MixturePump<T>::getMixtureId() {
    return mixtureId;
}

}  // namespace arch