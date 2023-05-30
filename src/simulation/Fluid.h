#pragma once

#include <string>
#include <vector>

namespace sim {

/**
 * @brief Class to define the fluid parameters of a droplet or the continuous phase.
 */
template<typename T>
class Fluid {
  private:
    int const id;                       ///< Unique identifier of the fluid.
    std::string name = "";              ///< Name of the fluid.
    //std::vector<Specie*> species;       ///< List of species dissolved in the fluid.
    T density;                          ///< Density of the continuous phase in kg/m^3.
    T viscosity;                        ///< Dynamic viscosity of the continuous phase in Pas.

  public:
    /**
     * @brief Constructs a fluid.
     * @param[in] id Unique identifier of the fluid.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     */
    Fluid(int id, T density, T viscosity);

    /**
     * @brief Set name of fluid.
     * @param[in] name Name of the fluid.
     */
    void setName(std::string name);

    /**
     * @brief Retrieve the unique identifier of the fluid.
     * @return Unique identifier of the fluid.
     */
    int getId() const;

    /**
     * @brief Retrieve name of the fluid.
     * @return The name of the fluid.
     */
    std::string getName() const;

    /**
     * @brief Retrieve viscosity of the fluid.
     * @return The viscosity of the fluid in Pas.
     */
    T getViscosity() const;

    /**
     * @brief Retrieve density of the fluid.
     * @return Density of the fluid in kg/m^3.
     */
    T getDensity() const;

};

}  // namespace sim