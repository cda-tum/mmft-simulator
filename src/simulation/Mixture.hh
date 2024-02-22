#include "Mixture.h"

namespace sim {

template<typename T>
Mixture<T>::Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
                    T viscosity, T density, T largestMolecularSize) : 
                    id(id), species(species), specieConcentrations(specieConcentrations), viscosity(viscosity), 
                    density(density), largestMolecularSize(largestMolecularSize) { }

template<typename T>
Mixture<T>::Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
                    T viscosity, T density) : 
                    id(id), species(species), specieConcentrations(specieConcentrations), viscosity(viscosity), 
                    density(density), largestMolecularSize(0.0) { }

template<typename T>
Mixture<T>::Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
                    Fluid<T>* carrierFluid) : 
                    id(id), species(species), specieConcentrations(specieConcentrations), 
                    viscosity(carrierFluid->getViscosity()), density(carrierFluid->getDensity()), 
                    largestMolecularSize(0.0) { }

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

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density, T largestMolecularSize) { }

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density) { }

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, Fluid<T>* carrierFluid) { }

template<typename T>
std::function<T(T)> DiffusiveMixture<T>::getConcentrationOfSpecie(int specieId) const {
    auto it = this->speciesConcentrations.find(specieId);
    if (it != this->speciesConcentrations.end()) {
        // Return the function part of the pair directly
        return it->second.first;
    }
    // Return a default function if the specie is not found
    // For example, a function that always returns 0
    return [](T) -> T { return 0.0; };
}

template<typename T>
void DiffusiveMixture<T>::changeFluidConcentration(int fluidId, T concentrationChange) {
    // This might involve adjusting the function or the vector in a way that reflects the concentration change
    // Example: Adding a constant change might not make sense for a function, so this is a placeholder
    // This operation needs to be defined based on how you intend to model diffusion and its effects on concentration
}

template<typename T>
const std::unordered_map<int, std::pair<std::function<T(T)>, std::vector<T>>>& DiffusiveMixture<T>::getSpecieConcentrations() const {
    return this->speciesConcentrations;
}

template<typename T>
bool DiffusiveMixture<T>::getIsConstant() {
    return this->isConstant;
}

template<typename T>
void DiffusiveMixture<T>::setNonConstant() {
    this->isConstant = false;
}

}   // namespace sim