#include "Fluid.h"

namespace sim {

template<typename T>
Fluid<T>::Fluid(unsigned int id_, T density_, T viscosity_) : id(id_), density(density_), viscosity(viscosity_) { ++fluidCounter; }

template<typename T>
Fluid<T>::Fluid(unsigned int id_, T density_, T viscosity_, std::string name_) : Fluid(id_, density_, viscosity_) {
    this->name = std::move(name_);
}

template<typename T>
void Fluid<T>::setName(std::string name_) {
    this->name = std::move(name_);
}

}  // namespace sim