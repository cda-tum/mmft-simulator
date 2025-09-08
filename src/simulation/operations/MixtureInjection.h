/**
 * @file MixtureInjection.h
 */

#pragma once

#include <memory>
#include <string>
#include <utility>

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
class AbstractMixing;

template<typename T>
class Mixture;

template<typename T>
class MixtureInjectionEvent;

template<typename T>
class PermanentMixtureInjectionEvent;

/**
 * @brief Class that contains all paramaters necessary to conduct an injection of a mixture.
 */
template<typename T>
class MixtureInjection final {
  private:
    inline static size_t injectionCounter = 0;                ///< Global counter for amount of created mixtureInjection objects.
    const size_t simHash;                                     ///< Hash of the simulation that created this mixture injection object.
    const size_t id;                                          ///< Unique identifier of an injection.
    Mixture<T>* const mixture;                                ///< Pointer to mixture to be injected.
    arch::Channel<T>* injectionChannel = nullptr;             ///< Edge at which the fluid is injected.
    T injectionTime = 0.0;                                    ///< Time at which the injection should start in s elapsed since the start of the simulation.
    std::string name = "";                                    ///< Name of the injection.
    bool performed = false;                                   ///< Information if the change of the input mixture was already performed or not.

    /**
     * @brief A static member function that resets the MixtureInjection object counter to zero.
     * Used as a helper function to reset the static variable between tests.
     * Is called in (friend) test::definitions::GlobalTest<T>::SetUp(), which overrides ::testing::Test.
     */
    static void resetMixtureInjectionCounter() { injectionCounter = 0; }

    /**
     * @brief A static member function that returns the amount of MixtureInjection objects that have been created.
     * Is used in (friend) AbstractMixing<T>::addMixtureInjection() to create a mixture object and add it to the simulation.
     * @returns The number of created MixtureInjection objects: injectionCounter.
     */
    static size_t getMixtureInjectionCounter() { return injectionCounter; }

    /**
     * @brief Create a mixture injection.
     * @param[in] simulationHash Hash to track which simulation object created this.
     * @param[in] id Unique identifier of an injection.
     * @param[in] mixture Pointer to the mixture to be injected.
     * @param[in] channel Channel into which the fluid is injected
     * @param[in] injectionTime Time at which the fluid injection should start, in s elapsed since the start of the simulation in s.
     */
    MixtureInjection(size_t simulationHash, size_t id, Mixture<T>* mixtureId, arch::Channel<T>* channel, T injectionTime);

    /**
     * @brief Update the state if the input mixture change was already performed or not.
     * 
     * @param performed If the input mixture change was performed or not.
     */
    inline void setPerformed(bool performed) { this->performed = performed; }

    /**
     * @brief Get information if the change of the input mixture was already performed or not.
     * @return true: Change was already performed.
     * @return false: Change was not yet performed. 
     */
    [[nodiscard]] inline const bool wasPerformed() { return performed; }

  public:

    /**
     * @brief Retrieve unique identifier of injection.
     * @return Unique identifier of injection.
     */
    [[nodiscard]] inline size_t getId() const { return id; }

    /**
     * @brief Retireve name of injection.
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
     * @return Time in s elapsed since the start of the simulation at which the injection of the fluid should take place in s.
     */
    [[nodiscard]] inline T getInjectionTime() const { return injectionTime; }

    /**
     * @brief Set the time of the injection
     * @param[in] injectionTime Time of the injection
     */
    inline void setInjectionTime(T injectionTime) { this->injectionTime = injectionTime; }

    /**
     * @brief Retrieve channel or pump at which the injection should take place.
     * @return Edge from which the injection should take place. 
     */
    [[nodiscard]] const arch::Channel<T>* getInjectionChannel() const { return injectionChannel; }

    /**
     * @brief Set the channel of the injection
     * @param[in] channel Channel of the injection
     */
    inline void setInjectionChannel(arch::Channel<T>* channel) { this->injectionChannel = channel; }

    /**
     * @brief Retrieve identification of the mixture that should be injected.
     * @return MixtureId of the mixture to be injected.
     */
    [[nodiscard]] inline size_t getMixtureId() { return mixture->getId(); }

    // Friend classes that need access to private member functions
    friend class AbstractMixing<T>;
    friend class MixtureInjectionEvent<T>;
    friend class PermanentMixtureInjectionEvent<T>;
    friend class test::definitions::GlobalTest<T>;
};

}  // namespace sim