#include "Mixture.h"

namespace sim {

template<typename T>
Mixture<T>::Mixture(int id, std::unordered_map<int, T>&& fluidConcentrations, T viscosity, T density, T largestMolecularSize) : id(id), viscosity(viscosity), density(density), largestMolecularSize(largestMolecularSize) {
    this->fluidConcentrations = std::move(fluidConcentrations);
    this->fluidCount = fluidConcentrations.size();
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
T Mixture<T>::getDiffusivity() const {
    return diffusivity;
}

template<typename T>
T Mixture<T>::getConcentrationOfFluid(int fluidId) const {
    if (fluidConcentrations.count(fluidId) == 1) {
        return fluidConcentrations.at(fluidId);
    }
    return 0.0;
}

template<typename T>
T Mixture<T>::getLargestMolecularSize() const {
    return largestMolecularSize;
}

template<typename T>
int Mixture<T>::getFluidCount() const {
    return fluidCount;
}

template<typename T>
const std::unordered_map<int, T>& Mixture<T>::getFluidConcentrations() const {
    return fluidConcentrations;
}

template<typename T>
void Mixture<T>::changeFluidConcentration(int fluidId, T concentrationChange) {
    auto fluidIter = fluidConcentrations.find(fluidId);
    if (fluidIter != fluidConcentrations.end()) {
        auto newConcentration = fluidIter->second + concentrationChange;
        assert(newConcentration >= -0.1 && newConcentration <= 1.1);
        if (newConcentration < 0.0) {
            newConcentration = 0.0;
        } else if (newConcentration > 1.0) {
            newConcentration = 1.0;
        }
        fluidIter->second = newConcentration;
    } else {
        assert(concentrationChange >= -0.1 && concentrationChange <= 1.1);
        if (concentrationChange < 0.0) {
            concentrationChange = 0.0;
        } else if (concentrationChange > 1.0) {
            concentrationChange = 1.0;
        }
        if (concentrationChange > 0.0) {
            fluidConcentrations.insert({fluidId, concentrationChange});
            fluidCount++;
        }
    }
}

}   // namespace sim