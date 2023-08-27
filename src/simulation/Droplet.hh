#include "Fluid.h"

#include <string>

namespace sim {

template<typename T>
Fluid<T>::Fluid(int id_, T density_, T viscosity_) : id(id_), density(density_), viscosity(viscosity_) { }

template<typename T>
void Fluid<T>::setName(std::string name_) {
    this->name = std::move(name_);
}

template<typename T>
int Fluid<T>::getId() const {
    return id;
}

template<typename T>
std::string Fluid<T>::getName() const {
    return name;
}

template<typename T>
T Fluid<T>::getViscosity() const {
    return viscosity;
}

template<typename T>
T Fluid<T>::getDensity() const {
    return density;
}

}  // namespace sim