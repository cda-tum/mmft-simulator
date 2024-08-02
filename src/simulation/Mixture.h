/**
 * @file Mixture.h
 */

#pragma once

#include <functional>
#include <unordered_map>

namespace arch { 

// Forward declared dependencies
template<typename T>
class Channel;

template<typename T>
class ChannelPosition;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Fluid;

template<typename T>
class Specie;

/**
 * @brief Struct that contains the start and end positions of a mixture in a channel.
*/
template<typename T>
struct MixturePosition {
    int mixtureId;
    int channel;
    T position1;
    T position2;

    /**
     * @brief Constructs a mixture position
     * @param[in] mixtureId Id of the mixture.
     * @param[in] channelId Id of the channel containing the mixture.
     * @param[in] channelPos1 Position of the start of this mixture, relative to channel length (0.0 - 1.0).
     * @param[in] channelPos2 Position of the end of this mixture, relative to channel length (0.0 - 1.0).
    */
    MixturePosition(int mixtureId, int channelId, T channelPos1, T channelPos2) : 
        mixtureId(mixtureId), channel(channelId), position1(channelPos1), position2(channelPos2) { }
};

/**
 * @brief Class that describes a mixture.
*/
template<typename T>
class Mixture {
private:

    int const id;
    std::unordered_map<int, Specie<T>*> species;
    std::unordered_map<int, T> specieConcentrations;
    T viscosity;
    T density;
    T largestMolecularSize;

public:
    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     * @param largestMolecularSize Largest molecular size in that mixture in m^3.
     */
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density, T largestMolecularSize);

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     */
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, T viscosity, T density);

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param fluid Carrier fluid of the mixture.
     */
    Mixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, Fluid<T>* carrierFluid);

    /**
     * @brief Overload == operator to check if mixtures are equal or not
     */
    bool operator== (const Mixture<T> &t);

    /**
     * @brief Get the id of this mixture
     * @return Unique identifier of the mixture.
     */
    int getId() const;

    /**
     * @brief Get the density of this mixture.
     * @return Density of the mixture in kg/m^3.
     */
    T getDensity() const;

    /**
     * @brief Get the viscosity of the mixture.
     * @return Viscosity of the mixture in Pas.
     */
    T getViscosity() const;

    /**
     * @brief Get the concentration of a specific specie in that mixture.
     * @param specieId Id of the specie.
     * @return Volume concentration of that specie within the mixture.
     */
    T getConcentrationOfSpecie(int specieId) const;

    /**
     * @brief Get the largest molecular size in this mixture.
     * @return Largest molecular size in the mixture in \\TODO unit.
     */
    T getLargestMolecularSize() const;

    /**
     * @brief Get the number of fluids this mixture consists of.
     * @return Number of fluids this mixture consists of. 
     */
    int getSpecieCount() const;

    /**
     * @brief Get a map of all flud id ids and their volume concentrations of the mixture.
     * @return Map of fluid id and volume concentration pairs.
     */
    const std::unordered_map<int, T>& getSpecieConcentrations() const;

    /**     
     * @brief Get the species that are contained in this mixture.
     * @return Map of specie id and pointer to the specie in this mixture.
    */
    const std::unordered_map<int, Specie<T>*>& getSpecies() const;

    virtual const std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>>& getSpecieDistributions() const {
        throw std::invalid_argument("Tried to access species distribution for non-diffusive mixture.");
    };

};

template<typename T>
class DiffusiveMixture : public Mixture<T> {
private:

    std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;
    int resolution;
    bool isConstant = true;

public:

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
        std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions,T viscosity, T density, T largestMolecularSize, int resolution=10);

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
        std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, T viscosity, T density, int resolution=10);

    DiffusiveMixture(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations, 
        std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, Fluid<T>* carrierFluid, int resolution=10);

    std::function<T(T)> getDistributionOfSpecie(int specieId) const;

    const std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>>& getSpecieDistributions() const override;

    bool getIsConstant();

    void setNonConstant();

    void setResolution(int resolution);

    int getResolution();
};

}   /// namespace sim