/**
 * @file ResistanceModels.h
 */

#pragma once

namespace arch {

// Forward declared dependencies
template<typename T>
class RectangularChannel;

}   // namespace arch

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

/**
 * @brief Virtual class that describes the necessary functionality of resistance models.
*/
template<typename T>
class ResistanceModel {
  protected:
    T continuousPhaseViscosity;   ///< The viscosity of the continuous phase in Pas.

  public:
  /**
   * @brief Instantiate the resistance model.
   * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
   */
  ResistanceModel(T continuousPhaseViscosity);

  /**
   * @brief Calculate and returns the resistance of the channel itself.
   * @param[in] channel A pointer to the channel for which the resistance should be calculated.
   * @return The resistance of the channel itself in Pas/L.
   */
  virtual T getChannelResistance(arch::RectangularChannel<T> const* const channel) const = 0;

  /**
   * @brief Compute the a factor.
   * @param[in] width Width of the channel in m.
   * @param[in] height Height of the channel in m.
   * @return The a factor.
   */
  virtual T computeFactorA(T width, T height) const = 0;

  /**
   * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
   * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
   * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
   * @param[in] volumeInsideChannel The volume inside the channel in m^3.
   * @return Resistance caused by the droplet in the channel in Pas/L.
   */
  virtual T getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const = 0;
};

/**
 * @brief Class that defines the functionality of the 1D resistance model.
 */
template<typename T>
class ResistanceModel1D : public ResistanceModel<T> {

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
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const override;

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const override;

    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const override;

};

/**
 * @brief Class that defines the functionality of the 1D resistance model assuming a 2D poiseuille flow.
 */
template<typename T>
class ResistanceModelPoiseuille : public ResistanceModel<T>{

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
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const override;

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const override;

    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const override;

};

}   // namespace sim