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
    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * 
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     * @param largestMolecularSize Largest molecular size in that mixture in molecular size in m^3.
     */
    Mixture(int id, std::unordered_map<int, double>&& fluidConcentrations, double viscosity, double density, double largestMolecularSize);

    /**
     * @brief Get the id of this mixture
     * 
     * @return Unique identifier of the mixture.
     */
    int getId() const;

    /**
     * @brief Get the concentration of a specific fluid in that mixture.
     * 
     * @param fluidId Id of the fluid
     * @return Volume concentration of that fluid within the mixture.
     */
    double getConcentrationOfFluid(int fluidId) const;

    /**
     * @brief Get the viscosity of the mixture.
     * 
     * @return Viscosity of the mixture in Pas.
     */
    double getViscosity() const;

    /**
     * @brief Get the density of this mixture.
     * 
     * @return Density of the mixture in kg/m^3.
     */
    double getDensity() const;

    /**
     * @brief Get the largest molecular size in this mixture.
     * 
     * @return Largest molecular size in the mixture in \\TODO unit.
     */
    double getLargestMolecularSize() const;

    /**
     * @brief Get the number of fluids this mixture consists of.
     * 
     * @return Number of fluids this mixture consists of. 
     */
    int getFluidCount() const;

    /**
     * @brief Get a map of all flud id ids and their volume concentrations of the mixture.
     * 
     * @return Map of fluid id and volume concentration pairs.
     */
    const std::unordered_map<int, double>& getFluidConcentrations() const;

    /**
     * @brief Change the concentration of a fluid within a mixture. As mixtures are not stored for every state, be aware that if you use this function, only the end concentration of this mixture will be returned to the user, which means intermediate steps are not visible. 
     * 
     * @param fluidId Id of the fluid for which the concentration should be changed.
     * @param concentrationChange The change of concentration that will be added to the current concentration of the fluid.
     */
    void changeFluidConcentration(int fluidId, double concentrationChange);
};

}   /// namespace sim