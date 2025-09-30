/**
 * @file Specie.h
 */

#pragma once

#include <string>
#include <vector>

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class ConcentrationSemantics;

/**
 * @brief Class to define a specie.
 */
template<typename T>
class Specie {
  private:
    inline static size_t specieCounter = 0; ///< Global counter for amount of created specie objects.
    size_t simHash;                         ///< Hash of the simulation that created this specie object.
    const size_t id;                        ///< Unique identifier of the specie.
    std::string name = "";                  ///< Name of the specie.
    T diffusivity = 0.0;                    ///< Diffusivity coefficient of the specie in the continuous phase in m^2/s.
    T satConc = 0.0;                        ///< Saturation concentration of the specie in the continuous phase in g/m^3.
    
    /**
     * @brief A static member function that resets the specie object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetSpecieCounter() noexcept { specieCounter = 0; }

    /**
     * @brief A static member function that returns the amount of specie objects that have been created.
     * Is used in (friend) AbstractMixing<T>::addSpecie() to create a specie object and add it to the simulation.
     * @returns The number of created specie objects: specieCounter.
     */
    static size_t getSpecieCounter() noexcept { return specieCounter; }

    /**
     * @brief Constructs a specie.
     * @param[in] simHash Hash of the simulation that created this specie object.
     * @param[in] id Unique identifier of the specie.
     * @param[in] diffusivity Diffusivity of the specie in the continuous phase in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in de continuous phase in g/m^3.
     */
    Specie(size_t simHash, size_t id, T diffusivity, T satConc);

    /**
     * @brief Set the simulation hash of this specie.
     * @param[in] simHash Hash of the simulation that created this specie object.
     */
    void resetHash() noexcept { this->simHash = 0; }

  public:

    /**
     * @brief Retrieve the unique identifier of the specie.
     * @return Unique identifier of the specie.
     */
    [[nodiscard]] inline size_t getId() const noexcept { return id; }

    /**
     * @brief Set name of specie.
     * @param[in] name Name of the specie.
     */
    inline void setName(std::string name) noexcept { this->name = std::move(name); }

    /**
     * @brief Retrieve name of the specie.
     * @return The name of the specie.
     */
    [[nodiscard]] inline std::string getName() const noexcept { return name; }

      /**
     * @brief Set the diffusivity of the specie in the continuous phase.
     * @param[in] diffusivity The diffusivity of the specie in m^2/s.
     */
    inline void setDiffusivity(T diffusivity) noexcept { this-> diffusivity = diffusivity; }

    /**
     * @brief Retrieve diffusivity of the specie in the continuous phase.
     * @return The diffusivity of the specie in m^2/s.
     */
    [[nodiscard]] inline T getDiffusivity() const noexcept { return diffusivity; }

    /**
     * @brief Set the saturation concentration of the specie in the continuous phase.
     * @param[in] satConc The saturation concentration of the specie in g/m^3.
     */
    inline void setSatConc(T satConc) noexcept { this->satConc = satConc; }

    /**
     * @brief Retrieve saturation concentration of the specie in the continuous phase.
     * @return The saturation concentration of the specie in g/m^3.
     */
    [[nodiscard]] inline T getSatConc() const noexcept { return satConc; }

    // Friend class definitions
    friend class ConcentrationSemantics<T>;
    friend class test::definitions::GlobalTest<T>;

};

}  // namespace sim