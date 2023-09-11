#pragma once

#include <unordered_map>

#include "Fluid.h"

namespace sim {

template<typename T>
class Mixture {

private:

    int const id;
    double viscosity;
    double density;
    double largestMolecularSize;
    int fluidCount;
    std::unordered_map<int, double> fluidConcentrations;

public:

    Mixture()

    int getId() const;

    T getConcentrationOfFluid(int fluidId) const;

    T getViscosity() const;

    T getDensity() const;

    T getLargestMolecularSie() const;

    int getFluidCount() const;

    const std::unordered_map<int, T>& getFluidConcentrations() const;

    void changeFluidConcentration(int fluidId, double concentrationChange);
};

}   /// namespace sim