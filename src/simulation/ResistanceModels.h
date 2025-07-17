/**
 * @file ResistanceModels.h
 */

#pragma once

namespace arch {

// Forward declared dependencies
template<typename T>
class RectangularChannel;

template<typename T>
class CylindricalChannel;

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
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const override;  //write a parent function to get Channel resistance for all resistance models then add override here

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const override;   //write a parent function to compute Factor A for all resistance models then add override here

    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const override;   //write a parent function to get droplet resistance for all resistance models then add override here

};

/**
 * @brief Class that defines the functionality of the Planar poiseuille resistance model
 */
template<typename T>
class ResistanceModelPlanarPoiseuille : public ResistanceModel<T>{

  public:
    /**
     * @brief Instantiate the resistance model.
     * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
     */
    ResistanceModelPlanarPoiseuille(T continuousPhaseViscosity);

    /**
     * @brief Calculate and returns the resistance of the channel itself.
     * @param[in] channel A pointer to the channel for which the resistance should be calculated.
     * @return The resistance of the channel itself in Pas/L.
     */
    T getChannelResistance(arch::RectangularChannel<T> const* const channel) const override;  //write a parent function to get Channel resistance for all resistance models then add override here

    /**
     * @brief Compute the a factor.
     * @param[in] width Width of the channel in m.
     * @param[in] height Height of the channel in m.
     * @return The a factor.
     */
    T computeFactorA(T width, T height) const override;   //write a parent function to compute Factor A for all resistance models then add override here

    /**
    * @brief Compute uniform film thickness H_inf for rectangular channel
    * @param[in] lambda dynamic viscosity i.e. the ration of droplet fluid viscosity to channel fluid viscosity
    * @param[in] Ca the capillary number defined for the channel
    * @param[in] radius Radius of the channel, here radius = width/2 
    * @return the uniform film thickness
    */
    T computeH_inf_rect(T lambda, T Ca, T radius) const; //write a parent function to get H_inf for all resistance models then add override here


    /**
    * @brief Compute the resistance that the channel fluid now replaced by the droplet would have posed in a rectangular channel. this is to be subtracted from total channel resistance
    * @param[in] viscosity viscosity of continuous phase(fluid in channel)
    * @param[in] droplet_length length of the droplet i.e. the section where the channel fluid was replaced
    * @param[in] channel_width width of the rectangular channel 
    * @param[in] channel_height height of the rectangular channel
    */
    T computeContinuousPhaseResistance_rect(T viscosity, T droplet_length, T channel_width, T channel_height) const; //write a parent function to get continuous phase resistance for all resistance models then add override here

    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const override;

    /**
    * @brief Compute the resistance posed by the droplet in a rectangular channel
    * @param[in] lambda dynamic viscosity i.e. the ration of droplet fluid viscosity to channel fluid viscosity
    * @param[in] dropletviscosity viscosity of the droplet fluid
    * @param[in] droplet_length lenght of the droplet in the channel
    * @param[in] channelradius Radius of the channel, here radius = width/2
    * @param[in] uniform_H the uniform film thickness
    * @return the droplet resistance in rectangular channel
    */
    T computeDropletResistance_rect(T lambda, T dropletviscosity, T droplet_length, T channelradius, T uniform_H) const; //same override logiic

    /**
    * @brief Compute the relative velocity of a droplet and uniform flow in a rectangular channel.
    * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
    * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
    */
    T getRelativeDropletVelocity(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const;  //same ovverride logic
};

/**
 * @brief Class that defines the functionality of the Cylindrical poiseuille resistance model
 */
template<typename T>
class ResistanceModelCylindricalPoiseuille : public ResistanceModel<T>{

  public:
    /**
     * @brief Instantiate the resistance model.
     * @param[in] continuousPhaseViscosity The viscosity of the continuous phase in Pas.
     */
    ResistanceModelCylindricalPoiseuille(T continuousPhaseViscosity);

    /**
     * @brief Calculate and returns the resistance of the channel itself.
     * @param[in] channel A pointer to the channel for which the resistance should be calculated.
     * @return The resistance of the channel itself in Pas/L.
     */
    T getChannelResistance(arch::CylindricalChannel<T> const* const channel) const override; //same override logiic


    /**
     * @brief Retrieve the resistance caused by the specified droplet in the specified channel.
     * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
     * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
     * @param[in] volumeInsideChannel The volume inside the channel in m^3.
     * @return Resistance caused by the droplet in the channel in Pas/L.
     */
    T getDropletResistance(arch::CylindricalChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const override;  //same override logiic


    /**
    * @brief Compute uniform film thickness H_inf for cylindrical channel
    * @param[in] lambda dynamic viscosity i.e. the ration of droplet fluid viscosity to channel fluid viscosity
    * @param[in] Ca the capillary number defined for the channel
    * @param[in] radius Radius of the channel
    * @return the uniform film thickness
    */
    T computeH_inf_cyl(T lambda, T Ca, T radius) const; //same override logiic

    /**
    * @brief Compute the resistance that the channel fluid now replaced by the droplet would have posed in a rectangular channel. this is to be subtracted from total channel resistance
    * @param[in] viscosity viscosity of continuous phase(fluid in channel)
    * @param[in] droplet_length length of the droplet i.e. the section where the channel fluid was replaced
    * @param[in] channelRadius Radius of the channel
    */
    T computeContinuousPhaseResistance_cyl(T viscosity, T droplet_length, T channelRadius) const; //same override logiic

    /**
    * @brief Compute the resistance posed by the droplet in a cylindrical channel
    * @param[in] lambda dynamic viscosity i.e. the ration of droplet fluid viscosity to channel fluid viscosity
    * @param[in] dropletviscosity viscosity of the droplet fluid
    * @param[in] droplet_length lenght of the droplet in the channel
    * @param[in] channelradius Radius of the channel
    * @param[in] uniform_H the uniform film thickness
    * @return the droplet resistance in cylindrical channel
    */
    T computeDropletResistance_cyl(T lambda, T dropletviscosity, T droplet_length, T channelradius, T uniform_H) const;  //same override logiic


    /**
    * @brief Compute the relative velocity of a droplet and uniform flow in a rectangular channel.
    * @param[in] channel Pointer to channel for which the droplet resistance should be calculated.
    * @param[in] droplet Pointer to droplet that causes the droplet resistance in the channel.
    */
    T getRelativeDropletVelocity_cyl(arch::CylindricalChannel<T> const* const channel, Droplet<T>* droplet) const;  //same override logiic


};

}   // namespace sim