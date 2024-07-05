/**
 * @file InjectionEvent.h
 */

#pragma once

namespace sim {

// Forward declared dependencies
template<typename T>
class Event;

template<typename T>
class DropletInjection;

/**
 * @brief Class for an injection event that takes place when a droplet is injected into the network.
 */
template<typename T>
class DropletInjectionEvent : public Event<T> {
  private:
    DropletInjection<T>& injection;  ///< Specifies if the injection event.

  public:
    /**
     * @brief Definies an injection event to take place at a certain time.
     * @param[in] time The time at which the event should take place in s elapsed since the start of the simulation.
     * @param[in,out] injection A class containing all details necessary to conduct an injection event.
     */
    DropletInjectionEvent(T time, DropletInjection<T>& injection);

    /**
     * @brief Conducts the injection event.
     */
    void performEvent() override;

    /**
     * @brief Print the injection event.
     */
    void print() override;
};

/**
 * @brief Class for an injection event that takes place when a mixture is injected into the network.
 */
template<typename T>
class MixtureInjectionEvent : public Event<T> {
  private:
    MixtureInjection<T>& injection;           ///< Specifies the mixture injection.
    MixingModel<T>* mixingModel;              ///< Pointer to the active mixing model.

  public:
    /**
     * @brief Definies an injection event to take place at a certain time.
     * @param[in] time The time at which the event should take place in s elapsed since the start of the simulation.
     * @param[in,out] injection A class containing all details necessary to conduct an injection event.
     */
    MixtureInjectionEvent(T time, MixtureInjection<T>& injection, MixingModel<T>* mixingModel);

    /**
     * @brief Conducts the injection event.
     */
    void performEvent() override;

    /**
     * @brief Print the injection event.
     */
    void print() override;
};

}  // namespace sim
