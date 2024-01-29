
/**
 * @file BoundaryHeadEvent.h
 */
#pragma once

#include <memory>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

template<typename T>
class DropletBoundary;

template<typename T>
class Event;

/**
 * @brief Class for a boundary head event that takes place when the boundary head reaches the end of a channel.
 */
template<typename T>
class BoundaryHeadEvent : public Event<T> {
  private:
    Droplet<T>& droplet;                ///< Droplet for which the event should take place.
    DropletBoundary<T>& boundary;       ///< Boundary of the droplet that is effected by the event.
    const arch::Network<T>& network;    ///< Microfluidic biochip that is simulated.

  public:
    /**
     * @brief Construct a new boundary head event.
     * @param time Time at which the event should take place, in s elapsed since the start of the simulation.
     * @param droplet Droplet for which the event should happen.
     * @param boundary Boundary that is effected by the event.
     * @param chip Microfluidic biochip.
     */
    BoundaryHeadEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, const arch::Network<T>& network);

    /**
     * @brief Perform the boundary head event.
     */
    void performEvent() override;

    /**
     * @brief Print the boundary head event.
     */
    void print() override;
};

/**
 * @brief Class for a boundary tail event that takes place when the tail boundary reaches the end of a channel.
 */
template<typename T>
class BoundaryTailEvent : public Event<T> {
  private:
    Droplet<T>& droplet;              ///< Droplet for which the event should take place.
    DropletBoundary<T>& boundary;     ///< Boundary of the droplet that is effected by the event.
    const arch::Network<T>& network;  ///< Microfluidic biochip that is simulated.

  public:
    /**
     * @brief Construct a new boundary tail event.
     * @param time Time at which the event should take place, in s elapsed since the start of the simulation.
     * @param droplet Droplet for which the event should happen.
     * @param boundary Boundary that is effected by the event.
     * @param chip Microfluidic biochip.
     */
    BoundaryTailEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, const arch::Network<T>& network);

    /**
     * @brief Perform the boundary tail event.
     */
    void performEvent() override;

    /**
     * @brief print the boundary tail event.
     */
    void print() override;
};

}  // namespace sim
