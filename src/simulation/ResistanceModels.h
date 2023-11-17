/**
 * @file ResistanceModels.h
 */

#pragma once

#include "Fluid.h"

#include "architecture/Channel.h"
#include "architecture/lbmModule.h"
#include "Droplet.h"

namespace sim {

  // Forward declared dependencies
  template<typename T>
  class Droplet;

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

    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::Channel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const;

};

/**
 * @brief Class that defines the functionality of the 1D resistance model assuming a 2D poiseuille flow.
 */
template<typename T>
class ResistanceModelPoiseuille{
  private:
    T continuousPhaseViscosity;  ///< The viscosity of the continuous phase in Pas.

  public:
    /**
     * @brief Instantiate the resistance model.
     * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
     */
    ResistanceModelPoiseuille(T continuousPhaseViscosity);

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