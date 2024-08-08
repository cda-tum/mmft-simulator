
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
class essLbmDropletSimulator;

template<typename T>
class Event;

/**
 * @brief Class for a Cfd event that takes place when the boundary head reaches a Cfd module.
 */
template<typename T>
class CfdChannelEvent : public Event<T> {
  private:
    Droplet<T>& droplet;                ///< Droplet for which the event should take place.
    int nodeId;
    const essLbmDropletSimulator<T>& simulator;    ///< Microfluidic biochip that is simulated.

  public:
    /**
     * @brief Construct a new Cfd event.
     * @param time Time at which the event should happen, in s elapsed since the start of the simulation.
     * @param droplet Droplet for which the event should happen.
     * @param nodeId Id of the node at which the droplet enters the Cfd domain.
     * @param simulator Simulator which the droplet enters.
     */
    CfdChannelEvent(T time, Droplet<T>& droplet, DropletBoundary<T>& boundary, int nodeId, const essLbmDropletSimulator<T>& simulator);

    /**
     * @brief Perform the Cfd event.
     */
    void performEvent() override;
};

/**
 * @brief Class for a Cfd event that takes place when the boundary head reaches a Cfd module.
 */
template<typename T>
class CfdInjectionEvent : public Event<T> {
  private:
    Droplet<T>& droplet;                ///< Droplet for which the event should take place.
    int nodeId;
    const essLbmDropletSimulator<T>& simulator;    ///< Microfluidic biochip that is simulated.

  public:
    /**
     * @brief Construct a new Cfd event.
     * @param time Time at which the event should happen, in s elapsed since the start of the simulation.
     * @param droplet Droplet for which the event should happen.
     * @param nodeId Id of the node at which the droplet enters the Cfd domain.
     * @param simulator Simulator which the droplet enters.
     */
    CfdInjectionEvent(T time, Droplet<T>& droplet, int nodeId, const essLbmDropletSimulator<T>& simulator);

    /**
     * @brief Perform the Cfd event.
     */
    void performEvent() override;
};

}  // namespace sim
