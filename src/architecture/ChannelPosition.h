/**
 * @file ChannelPosition.h
 */

#pragma once

#include "Channel.h"

namespace arch {

/**
 * @brief Class to specify the relative position of a point in a channel.
 */
template<typename T>
class ChannelPosition {
  private:
    RectangularChannel<T>* channel;   ///< Channel in which the point is.
    T position;                       ///< Exact relative position (between 0.0 and 1.0) within the channel.

  public:
    /**
     * @brief Constructor to create the position of the point.
     * @param[in] channel Channel in which this point currently is.
     * @param[in] position Relative position (between 0.0 and 1.0) of the point in this channel.
     */
    ChannelPosition(RectangularChannel<T>* channel, T position);

    /**
     * @brief Change the channel of the channel position (at which the point currently is).
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
     * @param[in] volumeShift Shift of the volume in flow direction in m^3.
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
     * @brief Calculates and returns volume towards node 0.
     * @return Volume towards node 0 in m^3.
     */
    T getVolume0() const;

    /**
     * @brief Calculates and returns volume towards node 1.
     * @return Volume towards node 1 in m^3.
     */
    T getVolume1() const;
};

}  // namespace arch