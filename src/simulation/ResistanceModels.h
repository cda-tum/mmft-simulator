#pragma once

#include <Fluid.h>
#include <Channel.h>
#include <lbmModule.h>

namespace sim {

/**
 * @brief Class that defines the functionality of the 1D resistance model.
 */
template<typename T>
class ResistanceModel1D{
  private:
    T continuousPhaseViscosity;  ///< The viscosity of the continuous phase in Pas.

  public:
    /**
     * @brief Instantiate the resistance model.
     * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
     */
    ResistanceModel1D(T continuousPhaseViscosity);

    /**
     * @brief Calculate and returns the resistance of the channel itself.
     * @param[in] channel A pointer to the channel for which the resistance should be calculated.
     * @return The resistance of the channel itself in Pas/L.
     */
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const;

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const;

};

/**
 * @brief Class that defines the functionality of the 1D resistance model.
 */
template<typename T>
class ResistanceModel2DPoiseuille{
  private:
    T continuousPhaseViscosity;  ///< The viscosity of the continuous phase in Pas.

  public:
    /**
     * @brief Instantiate the resistance model.
     * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
     */
    ResistanceModel2DPoiseuille(T continuousPhaseViscosity);

    /**
     * @brief Calculate and returns the resistance of the channel itself.
     * @param[in] channel A pointer to the channel for which the resistance should be calculated.
     * @return The resistance of the channel itself in Pas/L.
     */
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const;

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const;

};

}   // namespace sim