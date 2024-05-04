#include "Specie.h"

#include <string>

namespace sim {

template<typename T>
Specie<T>::Specie(int id_, T diffusivity_, T satConc_) : id(id_), diffusivity(diffusivity_), satConc(satConc_) { }

template<typename T>
void Specie<T>::setName(std::string name_) {
    this->name = std::move(name_);
}

template<typename T>
int Specie<T>::getId() const {
    return id;
}

template<typename T>
std::string Specie<T>::getName() const {
    return name;
}

template<typename T>
T Specie<T>::getDiffusivity() const {
    return diffusivity;
}

template<typename T>
T Specie<T>::getSatConc() const {
    return satConc;
}

template<typename T>
void Specie<T>::setDiffusivity(T diffusivity_) {
    this->diffusivity = diffusivity_;
}

template<typename T>
void Specie<T>::setSatConc(T satConc_) {
    this->satConc = satConc_;
}


}  // namespace sim