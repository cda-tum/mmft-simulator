#include "Fluid.h"

namespace sim {

template<typename T>
Fluid<T>::Fluid(size_t id_, size_t simHash, T density_, T viscosity_) : id(id_), simHash(simHash), density(density_), viscosity(viscosity_) { ++fluidCounter; }

template<typename T>
Fluid<T>::Fluid(size_t id_, size_t simHash, T density_, T viscosity_, std::string name_) : Fluid(id_, simHash, density_, viscosity_) {
    this->name = std::move(name_);
}

template<typename T>
bool Fluid<T>::checkHashes(size_t simHash) const {
    if (this->simHash == 0) {
        throw std::invalid_argument("Fluid was removed from Simulation.");
    }
    else if (this->simHash != simHash) {
        throw std::invalid_argument("Hash mismatch. Fluid was created by other Simulation.");
    }
    return true;
}

template<typename T>
void Fluid<T>::setName(std::string name_) {
    this->name = std::move(name_);
}

}  // namespace sim