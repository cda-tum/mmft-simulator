/**
 * @file Mixture.h
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>

namespace arch { 

// Forward declared dependencies
template<typename T>
class Channel;

template<typename T>
class ChannelPosition;

}

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class AbstractMixing;

template<typename T>
class DiffusionMixingModel;

template<typename T>
class InstantaneousMixingModel;

template<typename T>
class Fluid;

template<typename T>
class Specie;

/**
 * @brief Struct that contains the start and end positions of a mixture in a channel.
*/
template<typename T>
struct MixturePosition {
    int mixtureId;              ///< Id of the mixture.
    int channel;                ///< Id of the channel containing the mixture.
    T position1;                ///< Position of the start of this mixture, relative to channel length (0.0 - 1.0).
    T position2;                ///< Position of the end of this mixture, relative to channel length (0.0 - 1.0).

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
    inline static size_t mixtureCounter = 0;                        ///< Global counter for amount of created mixture objects.
    size_t simHash = 0;                                             ///< Hash of the simulation that created this mixture object.
    const size_t id;                                                ///< Unique identifier of the mixture.   
    std::string name = "";                                          ///< Name of the mixture.   
    std::unordered_map<size_t, Specie<T>*> species;                 ///< Map of specie id and pointer to the specie in this mixture.
    std::unordered_map<size_t, T> specieConcentrations;             ///< Map of specie id and concentration.
    T viscosity;                                                    ///< Viscosity of this mixture, defaults to bulk (continuous phase) viscosity.
    T density;                                                      ///< Density of this mixture, defaults to bulk (continuous phase) density.
    T largestMolecularSize;                                         ///< Largest molecular size in the mixture in [m^3].
    bool isMutable = false;                                         ///< Is this Mixture object mutable through the plublic API?

    /**
     * @brief A static member function that resets the mixture object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetMixtureCounter() noexcept { mixtureCounter = 0; }

    /**
     * @brief A static member function that returns the amount of mixture objects that have been created.
     * Is used in (friend) AbstractMixing<T>::addMixture() to create a mixture object and add it to the simulation.
     * @returns The number of created mixture objects: mixtureCounter.
     */
    static size_t getMixtureCounter() noexcept { return mixtureCounter; }

    /**
     * @brief Dummy constructor for temporary objects, does not increase mixtureCounter. Used in MixingModels
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     * @param largestMolecularSize Largest molecular size in that mixture in m^3.
     */
    Mixture(size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, Fluid<T>* carrierFluid);

    /**
     * @brief Set a Mixture object to be mutable.
     * @note Friend classes (AbstractMixing) can set the mutability property of an object.
     * @note By default, a Mixture object is non-mutable. Once set mutable it cannot be reversed.
     */
    inline void setMutable() { this->isMutable = true; }

    /**
     * @brief Set the simulation hash of this mixture.
     * @param[in] simHash Hash of the simulation that created this mixture object.
     */
    void resetHash() noexcept { this->simHash = 0; }

protected:

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     * @param largestMolecularSize Largest molecular size in that mixture in m^3.
     */
    Mixture(size_t simHash, size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, T viscosity, T density, T largestMolecularSize=0.0);

    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * @param simHash Hash of the simulation that created this mixture object.  
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param fluid Carrier fluid of the mixture.
     */
    Mixture(size_t simHash, size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, Fluid<T>* carrierFluid);

    /**
     * @brief Checks whether this object is mutable or not.
     * @throws logic_error if the object is non-mutable.
     */
    bool checkMutability() const;

    /**
     * @brief Checks whether the provided specie is created by the same Simulation object.
     * @throws invalid_argument if Specie's hash does not match the Mixture hash.
     */
    bool checkHashes(const Specie<T>* speciePtr) const;

    /**
     * @brief Get the concentration of a specific specie in that mixture.
     * @param[in] specieId Id of the specie.
     * @return Volume concentration of that specie within the mixture.
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    [[nodiscard]] T getConcentrationOfSpecie(size_t specieId) const;

    /**
     * @brief Get a map of all flud id ids and their volume concentrations of the mixture.
     * @return Map of fluid id and volume concentration pairs.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, T>& getSpecieConcentrations() const { return specieConcentrations; }

    /**     
     * @brief Get the species that are contained in this mixture.
     * @return Map of specie id and pointer to the specie in this mixture.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, Specie<T>*>& getSpecies() const { return species; }

    /**
     * @brief Update a specie concentration for a given specie for a given difference.
     * @param[in] specieId Identifier of the specie for which the concentration should be changed.
     * @param[in] concentrationChange The concentration difference. The new value is old + difference.
     */
    void changeSpecieConcentration(int specieId, T concentrationChange);

public:
    /**
     * @brief Overload == operator to check if mixtures are equal or not
     */
    bool operator== (const Mixture<T>& t) const;

    /**
     * @brief Get the id of this mixture
     * @return Unique identifier of the mixture.
     */
    [[nodiscard]] inline size_t getId() const { return id; }

    /**
     * @brief Retrieve the name of the mixture.
     * @return The name of the mixture.
     */
    [[nodiscard]] inline std::string getName() const { return name; }

    /**
     * @brief Set name of mixture.
     * @param[in] name New name of mixture.
     */
    inline void setName(std::string name) { this->name = std::move(name); }

    /**
     * @brief Adds a specie to a mixture.
     * @param[in] speciePtr A pointer to the Specie object that is to be added.
     * @param[in] concentration The concentration value of the specie.
     * @returns True if the specie was added successfully, false if it was already listed.
     * @throws invalid_argument if the passed Specie is already listed in the Mixture object.
     * @note A specie can only be added to mutable Mixture objects.
     * @note A specie can only be added if they were created by the same Simulation object.
     */
    [[maybe_unused]] bool addSpecie(Specie<T>* speciePtr, T concentration);

    /**
     * @brief Adds a specie to a mixture.
     * @param[in] speciePtr A pointer to the Specie object that is to be added.
     * @param[in] concentration The concentration value of the specie.
     * @returns True if the specie was added successfully, false if it was already listed.
     * @throws invalid_argument if the passed Specie is already listed in the Mixture object.
     * @note A specie can only be added to mutable Mixture objects.
     * @note A specie can only be added if they were created by the same Simulation object.
     */
    [[maybe_unused]] bool addSpecie(std::shared_ptr<Specie<T>>& speciePtr, T concentration);

    /** 
     * @brief Returns a read-only pointer to a set of <specie, concentration>.
     * @param[in] specieId Identifier of the specie to be read.
     * @returns std::tuple<speciePtr, concentration>
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    std::tuple<const Specie<T>*, T> readSpecie(size_t specieId) const;

    /**
     * @brief Returns a read-only map the listed species
     * @returns Copy of the species unordered_map<id, const specie<T>*>
     */
    std::unordered_map<size_t, const Specie<T>*> readSpecies() const;

    /**
     * @brief Returns a read-only map of the listed specieConcentrations
     * @returns Reference of the concentrations unordered_map<id, conc>
     */
    const std::unordered_map<size_t, T>& readSpecieConcentrations() const { return specieConcentrations; }

    /**
     * @brief Get the concentration of a specific specie in that mixture.
     * @param[in] speciePtr Pointer to the specie.
     * @return Volume concentration of that specie within the mixture.
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    [[nodiscard]] T getConcentrationOfSpecie(const Specie<T>* speciePtr) const;

    /**
     * @brief Get the concentration of a specific specie in that mixture.
     * @param[in] speciePtr Pointer to the specie.
     * @return Volume concentration of that specie within the mixture.
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    [[nodiscard]] T getConcentrationOfSpecie(const std::shared_ptr<Specie<T>>& speciePtr) const;


    /**
     * @throws logic_error if called on this type.
     * @note This function can only be called on a DiffusiveMixture type.
     */
    virtual const std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>>& getSpecieDistributions() const {
        throw std::invalid_argument("Tried to access species distribution for non-diffusive mixture.");
    };

    /**
     * @brief Set a new concentration value for a Specie listed in this mixture.
     * @param[in] speciePtr Pointer to the Specie which is listed.
     * @param[in] concentration New concentration value.
     * @returns True if the concentration was set successfully.
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    [[maybe_unused]] bool setSpecieConcentration(const Specie<T>* speciePtr, T concentration);

    /**
     * @brief Set a new concentration value for a Specie listed in this mixture.
     * @param[in] speciePtr Pointer to the Specie which is listed.
     * @param[in] concentration New concentration value.
     * @returns True if the concentration was set successfully.
     * @throws invalid_argument if the specie is not listed in the mixture.
     */
    [[maybe_unused]] bool setSpecieConcentration(const std::shared_ptr<Specie<T>>&, T concentration);

    /**
     * @brief Get the number of species listed in this mixture.
     * @return Number of species in this mixture.
     */
    [[nodiscard]] inline size_t getSpecieCount() const { return species.size(); }

    /**
     * @brief Removes a specie from a mixture, if it is mutable.
     * @param[in] speciePtr pointer to the specie that should be removed from the mixture.
     * @return True if the specie was removed successfully, false if it was not listed.
     * @throws invalid_argument if the specie is not listed in the Mixture.
     */
    [[maybe_unused]] bool removeSpecie(Specie<T>* speciePtr);

    /**
     * @brief Removes a specie from a mixture, if it is mutable.
     * @param[in] speciePtr pointer to the specie that should be removed from the mixture.
     * @return True if the specie was removed successfully, false if it was not listed.
     * @throws invalid_argument if the specie is not listed in the Mixture.
     */
    [[maybe_unused]] bool removeSpecie(std::shared_ptr<Specie<T>>& speciePtr);

    /**
     * @brief Get the density of this mixture.
     * @return Density of the mixture in kg/m^3.
     */
    [[nodiscard]] inline T getDensity() const { return density; }

    /**
     * @brief Get the viscosity of the mixture.
     * @return Viscosity of the mixture in Pa s.
     */
    [[nodiscard]] inline T getViscosity() const {return viscosity; }

    /** TODO: Miscellaneous */
    /** This value should be updated once a while
     * @brief Get the largest molecular size in this mixture.
     * @return Largest molecular size in the mixture in \\TODO unit.
     */
    [[nodiscard]] inline T getLargestMolecularSize() const {return largestMolecularSize;}

    virtual ~Mixture() = default;

    // Friend class definition
    friend class AbstractMixing<T>;
    friend class InstantaneousMixingModel<T>;
    friend class test::definitions::GlobalTest<T>;
};

template<typename T>
class DiffusiveMixture : public Mixture<T> {
private:

    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;     ///< Map of specie id and tuple of distribution function, parameters and maximum value.
    int resolution;                                     ///< Spectral resolution of the distribution function.
    bool isConstant = true;                             ///< If the distribution is constant or not.

    /**
     * @brief Construct a new DiffusiveMixture object.
     * @param id Id of the mixture.
     * @param species Map of specie id and pointer to the specie in this mixture.
     * @param specieConcentrations Map of specie id and concentration.
     * @param specieDistributions Map of specie id and tuple of distribution function, parameters and maximum value.
     * @param viscosity Viscosity of the mixture in [Pa s].
     * @param density Density of the mixture in [kg/m^3].
     * @param largestMolecularSize Largest molecular size in the mixture in [m^3].
     * @param resolution Spectral resolution of the distribution function.
     */
    DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, 
        std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions,T viscosity, T density, T largestMolecularSize, int resolution=10);
    
    /**
     * @brief Construct a new DiffusiveMixture object.
     * @param id Id of the mixture.
     * @param species Map of specie id and pointer to the specie in this mixture.
     * @param specieConcentrations Map of specie id and concentration.
     * @param specieDistributions Map of specie id and tuple of distribution function, parameters and maximum value.
     * @param viscosity Viscosity of the mixture in [Pa s].
     * @param density Density of the mixture in [kg/m^3].
     * @param largestMolecularSize Largest molecular size in the mixture in [m^3].
     * @param resolution Spectral resolution of the distribution function.
     */
    DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, 
        std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, T viscosity, T density, int resolution=10);

    /**
     * @brief Construct a new DiffusiveMixture object.
     * @param id Id of the mixture.
     * @param species Map of specie id and pointer to the specie in this mixture.
     * @param specieConcentrations Map of specie id and concentration.
     * @param specieDistributions Map of specie id and tuple of distribution function, parameters and maximum value.
     * @param viscosity Viscosity of the mixture in [Pa s].
     * @param density Density of the mixture in [kg/m^3].
     * @param largestMolecularSize Largest molecular size in the mixture in [m^3].
     * @param resolution Spectral resolution of the distribution function.
     */
    DiffusiveMixture(size_t simHash, size_t id, std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations, 
        std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions, Fluid<T>* carrierFluid, int resolution=10);

    /**
     * @brief Get the distribution function of a specific specie in this mixture.
     * @param[in] specieId Id of the specie.
     * @return Distribution function of the specie.
     */
    [[nodiscard]] std::function<T(T)> getDistributionOfSpecie(size_t specieId) const;

    /**
     * @brief Set whether the distribution is constant (uniform distribution) or not.
     * @returns True if the distribution is constant, false otherwise.
     */
    [[nodiscard]] bool inline getIsConstant() const { return this->isConstant; }

    /**
     * @brief Set the distribution to be non-constant.
     * @note This function can only be called once, after which the distribution is set to non-constant.
     */
    inline void setNonConstant() { this->isConstant = false; }

public:

    /**
     * @brief Get the distribution function of a specific specie in this mixture.
     * @param[in] speciePtr Pointer to the specie.
     * @return Distribution function of the specie.`
     */
    [[nodiscard]] std::function<T(T)> getDistributionOfSpecie(const Specie<T>* speciePtr) const;

    /**
     * @brief Get the distribution function of a specific specie in this mixture.
     * @param[in] speciePtr Pointer to the specie.
     * @return Distribution function of the specie.`
     */
    [[nodiscard]] std::function<T(T)> getDistributionOfSpecie(const std::shared_ptr<Specie<T>>& speciePtr) const;

    /**
     * @brief Get the specie distributions of this mixture.
     * @return Map of specie id and tuple of distribution function, and parameters.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>>& getSpecieDistributions() const override { return specieDistributions; }

    /**
     * @brief Set the spectral resolution of the distribution function.
     * @param[in] resolution The new spectral resolution of the distribution function.
     */
    inline void setResolution(int resolution) { this->resolution = resolution; }

    /**
     * @brief Get the spectral resolution of the distribution function.
     * @return The spectral resolution of the distribution function.
     */
    [[nodiscard]] inline int getResolution() { return this->resolution; }

    // Friend class definitions
    friend class AbstractMixing<T>;
    friend class DiffusionMixingModel<T>;
};

}   /// namespace sim