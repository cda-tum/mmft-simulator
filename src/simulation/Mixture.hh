#include "Mixture.h"

namespace sim {

Mixture::Mixture(int id, std::unordered_map<int, double>&& fluidConcentrations, double viscosity, double density, double largestMolecularSize) : id(id), viscosity(viscosity), density(density), largestMolecularSize(largestMolecularSize) {
    this->fluidConcentrations = std::move(fluidConcentrations);
    this->fluidCount = fluidConcentrations.size();
}

int Mixture::getId() const {
    return id;
}

double Mixture::getDensity() const {
    return density;
}

double Mixture::getViscosity() const {
    return viscosity;
}

double Mixture::getConcentrationOfFluid(int fluidId) const {
    if (fluidConcentrations.count(fluidId) == 1) {
        return fluidConcentrations.at(fluidId);
    }
    return 0.0;
}

double Mixture::getLargestMolecularSize() const {
    return largestMolecularSize;
}

int Mixture::getFluidCount() const {
    return fluidCount;
}

const std::unordered_map<int, double>& Mixture::getFluidConcentrations() const {
    return fluidConcentrations;
}

void Mixture::changeFluidConcentration(int fluidId, double concentrationChange) {
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