/**
 * @file Mixture.h
 */

#pragma once

#include <unordered_map>

#include "../architecture/Channel.h"
#include "../architecture/ChannelPosition.h"

#include "Fluid.h"
#include "Specie.h"

namespace sim {

// Forward declared dependencies
template<typename T>
class Mixture;

template<typename T>
struct MixturePosition {
    int mixtureId;
    int channel;
    T position1;
    T position2;

    /**
     * @brief Constructs a mixture position
    */
    MixturePosition(int mixtureId, int channelId, T channelPos1, T channelPos2) : 
        mixtureId(mixtureId), channel(channelId), position1(channelPos1), position2(channelPos2) { }
};

template<typename T>
class Mixture {
private:

    int const id;
    T viscosity;
    T density;
    T largestMolecularSize;
    std::unordered_map<int, Specie<T>*> species;
    std::unordered_map<int, T> specieConcentrations;

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
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density, T largestMolecularSize);

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * 
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     */
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density);

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * 
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param fluid Carrier fluid of the mixture.
     */
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, Fluid<T>* carrierFluid);

    /**
     * @brief Get the id of this mixture
     * 
     * @return Unique identifier of the mixture.
     */
    int getId() const;

    /**
     * @brief Get the concentration of a specific specie in that mixture.
     * 
     * @param specieId Id of the specie.
     * @return Volume concentration of that specie within the mixture.
     */
    T getConcentrationOfSpecie(int specieId) const;

    /**
     * @brief Get the viscosity of the mixture.
     * 
     * @return Viscosity of the mixture in Pas.
     */
    T getViscosity() const;

    /**
     * @brief Get the density of this mixture.
     * 
     * @return Density of the mixture in kg/m^3.
     */
    T getDensity() const;

    /**
     * @brief Get the largest molecular size in this mixture.
     * 
     * @return Largest molecular size in the mixture in \\TODO unit.
     */
    T getLargestMolecularSize() const;

    /**
     * @brief Get the number of fluids this mixture consists of.
     * 
     * @return Number of fluids this mixture consists of. 
     */
    int getSpecieCount() const;

    /**
     * @brief Get a map of all flud id ids and their volume concentrations of the mixture.
     * 
     * @return Map of fluid id and volume concentration pairs.
     */
    const std::unordered_map<int, T>& getSpecieConcentrations() const;

    /**
     * @brief Change the concentration of a fluid within a mixture. As mixtures are not stored for every state, be aware that if you use this function, only the end concentration of this mixture will be returned to the user, which means intermediate steps are not visible. 
     * 
     * @param fluidId Id of the fluid for which the concentration should be changed.
     * @param concentrationChange The change of concentration that will be added to the current concentration of the fluid.
     */
    void changeFluidConcentration(int fluidId, T concentrationChange);
};

template<typename T>
class DiffusiveMixture : public Mixture<T> {
private:

    bool isConstant = true;

    int resolution = 10;

    std::unordered_map<int, std::pair<std::function<T(T)>, std::vector<T>>> specieDistributions;

public:

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density, T largestMolecularSize);

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density);

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, Fluid<T>* carrierFluid);

    std::function<T(T)> getConcentrationOfSpecie(int specieId) const override;

    void changeFluidConcentration(int fluidId, T concentrationChange) override;

    const std::unordered_map<int, std::pair<std::function<T(T)>, std::vector<T>>>& getSpecieConcentrations() const;

    bool getIsConstant();

    void setNonConstant();
};

}   /// namespace sim