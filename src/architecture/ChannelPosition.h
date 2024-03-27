/**
 * @file ChannelPosition.h
 */

#pragma once

namespace arch {

// Forward declared dependencies
template<typename T>
class RectangularChannel;

/**
 * @brief Class to specify the boundary position of one end of a droplet.
 */
template<typename T>
class ChannelPosition {
  private:
    RectangularChannel<T>* channel;  ///< Channel in which one end of droplet currently is.
    T position;   ///< Exact relative position (between 0.0 and 1.0) within the channel.

  public:
    /**
     * @brief Constructor to create the position of one end of a droplet.
     * @param[in] channel Channel in which this end of the droplet currently is.
     * @param[in] position Relative position (between 0.0 and 1.0) of the droplet end in this channel.
     */
    ChannelPosition(RectangularChannel<T>* channel, T position);

    /**
     * @brief Change the channel of the channel position (at which one end of the droplet currently is).
     * @param[in] channel New channel to which the position should be set.
     */
    void setChannel(RectangularChannel<T>* const channel);

    /**
     * @brief Reset relative position.
     * @param[in] position Relative position (between 0.0 and 1.0) within a channel.
     */
    void setPosition(T position);

    /**
     * @brief Add the volume shift to the current position.
     * @param[in] volumeShift Shift of the volume in flow direction in L.
     */
    void addToPosition(T volumeShift);

    /**
     * @brief Returns pointer to channel in which this end of the droplet currently is.
     * @return Pointer to channel at which this end of the droplet currently is.
     */
    RectangularChannel<T>* getChannel() const;

    /**
     * @brief Returns relative position within the channel.
     * @return Relative position (between 0.0 and 1.0) at which this end of the droplet currently is.
     */
    T getPosition() const;

    /**
     * @brief Returns absolute position within channel in m.
     * @return Absolute position in m.
     */
    T getAbsolutePosition() const;

    /**
     * @brief Calculates and returns volume towards node A.
     * @return Volume towards node A in L.
     */
    T getVolumeA() const;

    /**
     * @brief Calculates and returns volume towards node B.
     * @return Volume towards node B in L.
     */
    T getVolumeB() const;
};

}  // namespace arch