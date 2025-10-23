#include "Mixture.h"

namespace sim {

template<typename T>
Mixture<T>::Mixture(size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    Fluid<T>* carrierFluid) : 
    id(id), species(species), specieConcentrations(specieConcentrations), viscosity(carrierFluid->getViscosity()), 
    density(carrierFluid->getDensity()), largestMolecularSize(0.0) {  }

template<typename T>
Mixture<T>::Mixture(size_t simHash, size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    T viscosity, T density, T largestMolecularSize) : 
    simHash(simHash), id(id), species(species), specieConcentrations(specieConcentrations), viscosity(viscosity), 
    density(density), largestMolecularSize(largestMolecularSize) { ++mixtureCounter; }

template<typename T>
Mixture<T>::Mixture(size_t simHash, size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    Fluid<T>* carrierFluid) : Mixture<T>(simHash, id, species, specieConcentrations, carrierFluid->getViscosity(), carrierFluid->getDensity()) { 
        carrierFluid->checkHashes(simHash); // Ensure the fluid belongs to the same simulation
    }

template<typename T>
bool Mixture<T>::checkMutability() const {
    if (!isMutable) {
        throw std::logic_error("Tried to modify non-mutable Mixture object.");
    }
    return isMutable;
}

template<typename T>
bool Mixture<T>::checkHashes(const Specie<T>* speciePtr) const {
    if (speciePtr->getHash() == 0) {
        throw std::invalid_argument("Specie was removed from Simulation.");
    }
    else if (speciePtr->getHash() != simHash) {
        throw std::invalid_argument("Hash mismatch. Specie was removed from Simulation or created by other Simulation.");
    }
    return true;
}

template<typename T>
void Mixture<T>::changeSpecieConcentration(int specieId, T concentrationChange) {
    auto specieIter = specieConcentrations.find(specieId);
    if (specieIter != specieConcentrations.end()) {
        auto newConcentration = specieIter->second + concentrationChange;
        assert(newConcentration >= -0.1 && newConcentration <= 1.1);
        specieIter->second = std::clamp(newConcentration, 0.0, 1.0);
    } else {
        assert(concentrationChange >= -0.1 && concentrationChange <= 1.1);
        if (concentrationChange > 0.0) {
            specieConcentrations.insert({specieId, std::clamp(concentrationChange, 0.0, 1.0)});
        }
    }
}

template<typename T>
bool Mixture<T>::operator== (const Mixture<T>& t) const {
    if (species == t.species && 
        specieConcentrations == t.specieConcentrations && 
        viscosity == t.viscosity &&
        density == t.density) { 
        return true; 
    }
    return false;
}

template<typename T>
bool Mixture<T>::addSpecie(std::shared_ptr<Specie<T>> speciePtr, T concentration) {
    size_t key = speciePtr->getId();
    // Check if the Mixture and Specie combination is valid.
    checkMutability();
    if (species.find(key) != species.end()) {
        throw std::invalid_argument("Cannot add specie with id " + std::to_string(key) + ". Specie already listed.");
    }
    checkHashes(speciePtr.get());

    // Checks passed, we can add the new specie
    auto result1 = species.try_emplace(key, speciePtr);
    auto result2 = specieConcentrations.try_emplace(key, concentration);

    return result1.second && result2.second; // Return true if both insertions were successful
}

template<typename T>
const std::tuple<std::shared_ptr<Specie<T>>, T> Mixture<T>::getSpecie(size_t specieId) const {
    auto it = species.find(specieId);
    if(it == species.end()) {
        throw std::invalid_argument("Cannot read specie with id: " + std::to_string(specieId) + ". Specie not found.");
    } else {
        // We should also check the hash of the specie to ensure it belongs to this simulation
        checkHashes(it->second.get());
    }
    return {species.at(specieId), specieConcentrations.at(specieId)};
}

template<typename T>
T Mixture<T>::getConcentrationOfSpecie(size_t specieId) const {
    auto it = specieConcentrations.find(specieId);
    if (it != specieConcentrations.end()) {
        return it->second;
    }
    return 0.0;
}

template<typename T>
T Mixture<T>::getConcentrationOfSpecie(const std::shared_ptr<Specie<T>>& speciePtr) const {
    return getConcentrationOfSpecie(speciePtr->getId());
}

template<typename T>
bool Mixture<T>::setSpecieConcentration(const std::shared_ptr<Specie<T>>& speciePtr, T concentration) {
    // Check if the Mixture and Specie combination is valid
    checkMutability();
    checkHashes(speciePtr.get());
    if (species.find(speciePtr->getId()) == species.end()) {
        throw std::invalid_argument("Cannot set concentration for specie with id: " + std::to_string(speciePtr->getId()) + ". Specie not found.");
    }
    specieConcentrations.at(speciePtr->getId()) = concentration;
    return true; // Indicate that the concentration was set successfully
}

template<typename T>
bool Mixture<T>::removeSpecie(std::shared_ptr<Specie<T>>& speciePtr) {
    // Check if the Mixture and Specie combination is valid
    checkMutability();
    checkHashes(speciePtr.get());

    return forceRemoveSpecie(speciePtr);
}

template<typename T>
bool Mixture<T>::forceRemoveSpecie(std::shared_ptr<Specie<T>>& speciePtr) {
    size_t key = speciePtr->getId();

    if (species.find(key) == species.end()) {
        throw std::invalid_argument("Cannot remove specie with id: " + std::to_string(speciePtr->getId()) + ". Specie not found.");
    }
    auto result1 = specieConcentrations.erase(key); 
    auto result2 = species.erase(key);

    return (result1 > 0 && result2 > 0); // Return true if both removals were successful
}

// ================================================================================================================
// ==================================Diffusive==Mixing=============================================================
// ================================================================================================================

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, T viscosity, T density, T largestMolecularSize, int resolution) : 
    Mixture<T>(simHash, id, species, specieConcentrations, viscosity, density, largestMolecularSize), specieDistributions(specieDistributions), resolution(resolution) { }

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, T viscosity, T density, int resolution) :
    Mixture<T>(simHash, id, species, specieConcentrations, viscosity, density), specieDistributions(specieDistributions), resolution(resolution) { }

template<typename T>
DiffusiveMixture<T>::DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species, std::unordered_map<size_t, T> specieConcentrations, 
    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, Fluid<T>* carrierFluid, int resolution) : 
    Mixture<T>(simHash, id, species, specieConcentrations, carrierFluid), specieDistributions(specieDistributions), resolution(resolution) {
        carrierFluid->checkHashes(simHash); // Ensure the fluid belongs to the same simulation
    }

template<typename T>
std::function<T(T)> DiffusiveMixture<T>::getDistributionOfSpecie(size_t specieId) const {
    auto it = this->specieDistributions.find(specieId);
    if (it != this->specieDistributions.end()) {
        // Return the function part of the pair directly
        return std::get<0>(it->second);
    }
    // Return a default function if the specie is not found
    // For example, a function that always returns 0
    return [](T) -> T { return 0.0; };
}

template<typename T>
std::function<T(T)> DiffusiveMixture<T>::getDistributionOfSpecie(const std::shared_ptr<Specie<T>>& speciePtr) const {
    return getDistributionOfSpecie(speciePtr->getId());
}

}   // namespace sim