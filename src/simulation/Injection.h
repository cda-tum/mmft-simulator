/**
 * @file Injection.h
 */

#pragma once

#include <memory>
#include <string>

namespace arch {

// Forward declared dependencies
template<typename T>
class ChannelPosition;

template<typename T>
class RectangularChannel;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

/**
 * @brief Class that contains all paramaters necessary to conduct an injection.
 */
template<typename T>
class DropletInjection {
  private:
    const int id;                             ///< Unique identifier of an injection.
    std::string name = "";                    ///< Name of the injection.
    Droplet<T>* droplet;                          ///< Pointer to droplet to be injected.
    T injectionTime;                          ///< Time at which the injection should take place in s elapsed since the start of the simulation.
    arch::ChannelPosition<T> injectionPosition;   ///< Position at which the droplet should be injected.

  public:
    /**
     * @brief Create an injection.
     * @param[in] id Unique identifier of an injection.
     * @param[in] droplet Pointer to the droplet to be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s elapsed since the start of the simulation in s.
     * @param[in] channel Channel in which the droplet should be injected. The channel must be able to fully contain the droplet.
     * @param[in] injectionPosition Relative position (between 0.0 and 1.0) of the middle of the droplet in channel. Head and tail position must be in same channel.
     */
    DropletInjection(int id, Droplet<T>* droplet, T injectionTime, arch::RectangularChannel<T>* channel, T injectionPosition);

    /**
     * @brief Set name of injection.
     * @param[in] name Name of injection.
     */
    void setName(std::string name);

    /**
     * @brief Retrieve unique identifier of injection.
     * @return Unique identifier of injection.
     */
    int getId() const;

    /**
     * @brief Retireve name of injection.
     * @return Name of injection.
     */
    std::string getName() const;

    /**
     * @brief Retrieve time at which the droplet should be injected.
     * @return Time in s elapsed since the start of the simulation at which the injection of the droplet should take place in s.
     */
    T getInjectionTime() const;

    /**
     * @brief Retrieve position at which the injection should take place.
     * @return Position at which the injection should take place. An injection can only take place in channels that are able to fully contain droplet.
     */
    const arch::ChannelPosition<T>& getInjectionPosition() const;

    /**
     * @brief Retrieve pointer to the droplet that should be injected.
     * @return Droplet to be injected.
     */
    Droplet<T>* getDroplet() const;
};

}  // namespace sim
