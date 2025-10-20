#include "Specie.h"

#include <string>

namespace sim {

template<typename T>
Specie<T>::Specie(size_t simHash, size_t id, T diffusivity, T satConc) : 
    simHash(simHash), id(id), diffusivity(diffusivity), satConc(satConc) { ++specieCounter; }

}  // namespace sim