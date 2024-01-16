#include "Mixture.h"

namespace sim {

template<typename T>
MixturePosition<T>::MixturePosition() { }

template<typename T>
Mixture<T>::Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T>&& specieConcentrations, 
                    T viscosity, T density, T largestMolecularSize) : 
                    id(id), species(species), specieConcentrations(specieConcentrations), viscosity(viscosity), 
                    density(density), largestMolecularSize(largestMolecularSize) {
    this->specieConcentrations = std::move(specieConcentrations);
}

template<typename T>
int Mixture<T>::getId() const {
    return id;
}

template<typename T>
T Mixture<T>::getDensity() const {
    return density;
}

template<typename T>
T Mixture<T>::getViscosity() const {
    return viscosity;
}

template<typename T>
T Mixture<T>::getConcentrationOfSpecie(int specieId) const {
    if (specieConcentrations.count(specieId) == 1) {
        return specieConcentrations.at(specieId);
    }
    return 0.0;
}

template<typename T>
T Mixture<T>::getLargestMolecularSize() const {
    return largestMolecularSize;
}

template<typename T>
int Mixture<T>::getSpecieCount() const {
    return species.size();
}

template<typename T>
const std::unordered_map<int, T>& Mixture<T>::getSpecieConcentrations() const {
    return specieConcentrations;
}

}   // namespace sim