#include "MixtureInjection.h"

namespace sim {

template<typename T>
MixtureInjection<T>::MixtureInjection(size_t simHash, size_t id, Mixture<T>* mixture, arch::Channel<T>* injectionChannel, T injectionTime) : 
    simHash(simHash), id(id), mixture(mixture), injectionChannel(injectionChannel), injectionTime(injectionTime) { ++injectionCounter; }

}  // namespace sim