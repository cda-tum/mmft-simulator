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

namespace test::definitions {
// Forward declared dependencies
template<typename T>
class GlobalTest;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class AbstractDroplet;

template<typename T>
class Droplet;

/**
 * @brief Class that contains all paramaters necessary to conduct an injection.
 */
template<typename T>
class DropletInjection final {
  private:
    inline static size_t injectionCounter = 0;        ///< Global counter for amount of created dropletInjection objects.
    const size_t id;                                  ///< Unique identifier of an injection.
    DropletImplementation<T>* const droplet;          ///< Pointer to droplet to be injected.
    std::string name = "";                            ///< Name of the injection.
    T injectionTime = 0.0;                            ///< Time at which the injection should take place in s elapsed since the start of the simulation.
    arch::ChannelPosition<T> injectionPosition;       ///< Position at which the droplet should be injected.
    const sim::AbstractDroplet<T>* simRef = nullptr;  ///< Pointer to the simulation in which this injection takes place.

    /**
     * @brief A static member function that resets the dropletInjection object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetDropletInjectionCounter() { injectionCounter = 0; }

    /**
     * @brief A static member function that returns the amount of dropletInjection objects that have been created.
     * Is used in (friend) AbstractDroplet<T>::addDropletInjection() to create a droplet object and add it to the simulation.
     * @returns The number of created dropletInjection objects: injectionCounter.
     */
    static size_t getDropletInjectionCounter() { return injectionCounter; }

    /**
     * @brief Create an injection.
     * @param[in] id Unique identifier of an injection.
     * @param[in] droplet Pointer to the droplet to be injected.
     * @param[in] injectionTime Time at which the droplet should be injected in s elapsed since the start of the simulation in s.
     * @param[in] channel Channel in which the droplet should be injected. The channel must be able to fully contain the droplet.
     * @param[in] injectionPosition Relative position (between 0.0 and 1.0) of the middle of the droplet in channel. Head and tail position must be in same channel.
     */
    DropletInjection(size_t id, DropletImplementation<T>* droplet, T injectionTime, arch::Channel<T>* channel, T injectionPosition, AbstractDroplet<T>* simRef);

    /**
     * @brief Retrieve pointer to the droplet that should be injected.
     * @return Droplet to be injected.
     */
    [[nodiscard]] inline DropletImplementation<T>* getDroplet() const { return droplet; } // Declared private for improved encapsulation

  public:

    /**
     * @brief Retrieve unique identifier of injection.
     * @return Unique identifier of injection.
     */
    [[nodiscard]] inline size_t getId() const { return id; }

    /**
     * @brief Retrieve name of injection.
     * @return Name of injection.
     */
    [[nodiscard]] inline const std::string& getName() const { return name; }

    /**
     * @brief Set name of injection.
     * @param[in] name Name of injection.
     */
    inline void setName(std::string name) { this->name = std::move(name); }

    /**
     * @brief Retrieve time at which the droplet should be injected.
     * @return Time in s elapsed since the start of the simulation at which the injection of the droplet should take place in s.
     */
    [[nodiscard]] inline T getInjectionTime() const { return injectionTime; }

    /**
     * @brief Set the time of the injection
     * @param[in] injectionTime Time of the injection
     */
    inline void setInjectionTime(T injectionTime) { this->injectionTime = injectionTime; }

    /**
     * @brief Retrieve position at which the injection should take place.
     * @return Position at which the injection should take place. An injection can only take place in channels that are able to fully contain droplet.
     */
    [[nodiscard]] const T getInjectionPosition() const { return injectionPosition.getPosition(); }

    /**
     * @brief Set the position of the injection
     * @param[in] position Position of the injection
     * @note position should be between 0.0 and 1.0
     */
    void setInjectionPosition(T position);

    /**
     * @brief Retrieve position at which the injection should take place.
     * @return Position at which the injection should take place. An injection can only take place in channels that are able to fully contain droplet.
     */
    [[nodiscard]] const std::shared_ptr<arch::Channel<T>> getInjectionChannel() const;

    /**
     * @brief Set the position of the injection
     * @param[in] position Position of the injection
     */
    inline void setInjectionChannel(const std::shared_ptr<arch::Channel<T>>& channel);

    // Friend classes that need access to private member functions
    friend class AbstractDroplet<T>; 
    friend class DropletInjectionEvent<T>;
    friend class test::definitions::GlobalTest<T>;
};

}  // namespace sim
