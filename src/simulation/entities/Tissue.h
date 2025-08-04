/**
 * @file Tissue.h
 */

#pragma once

#include <string>
#include <unordered_map>

namespace sim {

// Forward declared dependencies
template<typename T>
class Specie;

/**
 * @brief Class that describes a tissue.
*/
template<typename T>
class Tissue {
private:

    int id;
    std::string name;

    std::unordered_map<int, Specie<T>*> species;
    std::unordered_map<int, T> vMax;                    // vMax values for the enzymatic kinetics per species for this tissue, according to Michaelis-Menten kinetics
    std::unordered_map<int, T> kM;                      // kM values for the enzymatic kinetics per species for this tissue, according to Michaelis-Menten kinetics

public:
    /**
     * @brief Construct a new tissue.
     * @param[in] id The id of this tissue.
     */
    Tissue(int id);

    /**
     * @brief Construct a new tissue.
     * @param[in] id The id of this tissue.
     * @param[in] specie A pointer to a species that interacts in enzymatic transformation.
     * @param[in] vMax The maximum elimination rate of a species in enzymatic transformation.
     * @param[in] kM The concentration associated with 0.5vMax for a species in enzymatic transformation.
     */
    Tissue(int id, Specie<T>* specie, T vMax, T kM);

    /**
     * @brief Construct a new tissue.
     * @param[in] id The id of this tissue.
     * @param[in] species An unordered map of pointers to species that interact in enzymatic transformation.
     * @param[in] vMax An unordered map of the maximum eleimination rate of species in enzymatic transformation.
     * @param[in] kM An unordered map of the concentration associated with 0.5vMax for species in enzymatic transformation.
     */
    Tissue(int id, std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> vMax, std::unordered_map<int, T> kM);

    /**
     * @brief Get the name of this tissue.
     */
    std::string getName();

    /**
     * @brief Set the name of this tissue.
     * @param[in] name The new name of this tissue.
     */
    void setName(std::string name);

    /**
     * @brief Add a new specie interaction to this tissue.
     * @param[in] specie A pointer to a species that interacts in enzymatic transformation.
     * @param[in] vMax The maximum eleimination rate of a species in enzymatic transformation.
     * @param[in] kM The concentration associated with 0.5vMax for a species in enzymatic transformation.
     */
    void addSpecie(Specie<T>* species, T vMax, T kM);

    /**
     * @brief Add a set of species interactions to this tissue.
     * @param[in] species An unordered map of pointers to species that interact in enzymatic transformation.
     * @param[in] vMax An unordered map of the maximum eleimination rate of species in enzymatic transformation.
     * @param[in] kM An unordered map of the concentration associated with 0.5vMax for species in enzymatic transformation.
     */
    void addSpecies(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> vMax, std::unordered_map<int, T> kM);

    std::unordered_map<int, Specie<T>*>& getSpecies();

    T* getVmax(int speciesId);

    T* getkM(int speciesId);

};

}   /// namespace sim