#include "Tissue.h"

namespace sim {

template<typename T>
Tissue<T>::Tissue(int id_) : id(id_) { }

template<typename T>
Tissue<T>::Tissue(int id_, Specie<T>* specie_, T vMax_, T kM_) : 
    id(id_), species(specie_), vMax(vMax_), kM(kM_) { }

template<typename T>
Tissue<T>::Tissue(int id_, std::unordered_map<int, Specie<T>*> species_, std::unordered_map<int, T> vMax_, std::unordered_map<int, T> kM_) : 
    id(id_), species(species_), vMax(vMax_), kM(kM_) { }

template<typename T>
void Tissue<T>::setName(std::string name_) { 
    this->name = name_;
}

template<typename T>
std::string Tissue<T>::getName() { 
    return this->name;
}

template<typename T>
void Tissue<T>::addSpecie(Specie<T>* specie_, T vMax_, T kM_) { 
    int specieId = specie_->getId();

    auto const result = species->try_emplace(specieId, specie_);
    if (!result.second) {
        std::string errorString = "Cannot add species " + std::to_string(specieId) + " to tissue " + std::to_string(id) + ": This species is already defined for the tissue.";
        throw std::invalid_argument(errorString);
    }
    vMax->try_emplace(specieId, vMax_);
    kM->try_emplace(specieId, kM_);
}

template<typename T>
void Tissue<T>::addSpecies(std::unordered_map<int, Specie<T>*> species_, std::unordered_map<int, T> vMax_, std::unordered_map<int, T> kM_) { 

    for (auto& [key, specie_] : species_) {
        auto const result = species->try_emplace(key, specie_);
        if (!result.second) {
            std::string errorString = "Cannot add species " + std::to_string(key) + " to tissue " + std::to_string(id) + ": This species is already defined for the tissue.";
            throw std::invalid_argument(errorString);
        }
        if (vMax_.count(key)) {
            vMax->try_emplace(key, vMax_.at(key));
        } else {
            std::string errorString = "No vMax value was given for species " + std::to_string(key) + ".";
            throw std::invalid_argument(errorString);
        }
        if (kM_.count(key)) {
            kM->try_emplace(key, kM_.at(key));
        } else {
            std::string errorString = "No kM value was given for species " + std::to_string(key) + ".";
            throw std::invalid_argument(errorString);
        }
    }
}

template<typename T>
std::unordered_map<int, Specie<T>*>& Tissue<T>::getSpecies() {
    return species;
}

template<typename T>
T* Tissue<T>::getVmax(int speciesId) {
    auto vMaxIter = vMax.find(speciesId);
    return &(vMaxIter->second);
}

template<typename T>
T* Tissue<T>::getkM(int speciesId) {
    auto kMIter = kM.find(speciesId);
    return &(kMIter->second);
}

}   // namespace sim