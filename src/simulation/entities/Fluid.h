/**
 * @file Fluid.h
 */

#pragma once

#include <string>
#include <vector>

// Forward declared dependencies
class BigDropletTest;

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Simulation;

/**
 * @brief Class to define the fluid parameters of a droplet or the continuous phase.
 */
template<typename T>
class Fluid {
  private:
    inline static int fluidCounter = 0;                     ///< Global counter for amount of created fluid objects.
    unsigned int const id;                                  ///< Unique identifier of the fluid.
    std::string name = "";                                  ///< Name of the fluid.
    T density = 1.0e3;                                      ///< Density of the continuous phase in [kg/m^3].
    T viscosity = 1.0e-3;                                   ///< Dynamic viscosity of the continuous phase in [Pa s].
    T molecularSize { std::numeric_limits<T>::epsilon() };  ///< Molecular size in [m^3].

    /**
     * @brief A static member function that resets the fluid object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetFluidCounter() { fluidCounter = 0; }

    /**
     * @brief A static member function that returns the amount of fluid objects that have been created.
     * Is used in (friend) Simulation<T>::addFluid() to create a fluid object and add it to the simulation.
     * @returns The number of created fluid objects: fluidCounter.
     */
    static unsigned int getFluidCounter() { return fluidCounter; }

    /**
     * @brief Constructs a fluid.
     * @param[in] id Unique identifier of the fluid.
     * @param[in] density Density of the fluid in [kg/m^3].
     * @param[in] viscosity Viscosity of the fluid in [Pa s].
     */
    Fluid(unsigned int id, T density, T viscosity);

    /**
     * @brief Constructs a fluid.
     * @param[in] id Unique identifier of the fluid.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] viscosity Viscosity of the fluid in [Pa s].
     * @param[in] name Name of the fluid.
     */
    Fluid(unsigned int id, T density, T viscosity, std::string name);

  public:

    /**
     * @brief Retrieve the unique identifier of the fluid.
     * @return Unique identifier of the fluid.
     */
    [[nodiscard]] inline unsigned int getId() const { return id; }

    /**
     * @brief Set name of fluid.
     * @param[in] name Name of the fluid.
     */
    void setName(std::string name);

    /**
     * @brief Retrieve name of the fluid.
     * @return The name of the fluid.
     */
    [[nodiscard]] inline std::string getName() const { return name; }

    /**
     * @brief Set the fluid viscosity.
     * @param[in] viscosity The new viscosity value of this fluid in [Pa s].
     */
    inline void setViscosity(T viscosity) { this->viscosity = viscosity; }

    /**
     * @brief Retrieve viscosity of the fluid.
     * @return The viscosity of the fluid in [Pa s].
     */
    [[nodiscard]] inline T getViscosity() const { return viscosity; }

    /**
     * @brief Set the fluid density.
     * @param[in] density The new density value of this fluid in [kg/m^3].
     */
    inline void setDensity(T density) { this->density = density; }

    /**
     * @brief Retrieve density of the fluid.
     * @return Density of the fluid in [kg/m^3].
     */
    [[nodiscard]] inline T getDensity() const { return density; }

    // Friend class definition, because the Fluid constructors are private
    friend class Simulation<T>;
    friend class test::definitions::GlobalTest<T>;
};

}  // namespace sim