#include "Fluid.h"

namespace sim {

template<typename T>
Fluid<T>::Fluid(int id_, T density_, T viscosity_, T concentration_) : id(id_), density(density_), 
    viscosity(viscosity_), concentration(concentration_) { }

template<typename T>
Fluid<T>::Fluid(int id_, T density_, T viscosity_, T concentration_, std::string name_) : id(id_), 
    density(density_), viscosity(viscosity_), name(name_) { }

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

template<typename T>
T Fluid<T>::getConcentration() const {
    return concentration;
}

template<typename T>
void Fluid<T>::addMixedFluid(Fluid<T>* fluid) {
    mixedFluids.push_back(fluid);
}

template<typename T>
const std::vector<Fluid<T>*>& Fluid<T>::getMixedFluids() const {
    return mixedFluids;
}

}  // namespace sim