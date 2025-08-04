/**
 * @file Specie.h
 */

#pragma once

#include <string>
#include <vector>

namespace sim {

/**
 * @brief Class to define a specie.
 */
template<typename T>
class Specie {
  private:
    int const id;                       ///< Unique identifier of the specie.
    std::string name = "";              ///< Name of the specie.
    T diffusivity;                      ///< Diffusivity coefficient of the specie in the continuous phase in m^2/s.
    T satConc;                          ///< Saturation concentration of the specie in the continuous phase in g/m^3.

  public:
    /**
     * @brief Constructs a specie.
     * @param[in] id Unique identifier of the specie.
     * @param[in] diffusivity Diffusivity of the specie in the continuous phase in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in de continuous phase in g/m^3.
     */
    Specie(int id, T diffusivity, T satConc);

    /**
     * @brief Set name of specie.
     * @param[in] name Name of the specie.
     */
    void setName(std::string name);

    /**
     * @brief Retrieve the unique identifier of the specie.
     * @return Unique identifier of the specie.
     */
    int getId() const;

    /**
     * @brief Retrieve name of the specie.
     * @return The name of the specie.
     */
    std::string getName() const;

    /**
     * @brief Retrieve diffusivity of the specie in the continuous phase.
     * @return The diffusivity of the specie in m^2/s.
     */
    T getDiffusivity() const;

    /**
     * @brief Retrieve saturation concentration of the specie in the continuous phase.
     * @return The saturation concentration of the specie in g/m^3.
     */
    T getSatConc() const;

      /**
     * @brief Set the diffusivity of the specie in the continuous phase.
     * @param[in] diffusivity The diffusivity of the specie in m^2/s.
     */
    void setDiffusivity(T diffusivity);

    /**
     * @brief Set the saturation concentration of the specie in the continuous phase.
     * @param[in] satConc The saturation concentration of the specie in g/m^3.
     */
    void setSatConc(T satConc);

};

}  // namespace sim