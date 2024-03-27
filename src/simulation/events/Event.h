/**
 * @file Event.h
 */
#pragma once

namespace sim {

/**
 * @brief
 * Interface for all events.
 */
template<typename T>
class Event {
  protected:
    T time;   ///< Time at which the event should take place, in s elapsed since the start of the simulation.
    int priority;  ///< Priority of the event.

    /**
     * @brief Specifies an event to take place.
     * @param[in] time The time at which the event should take place, in s elapsed since the start of the simulation.
     * @param[in] priority Priority of an event, which is important when two events occur at the same time (the lower the value the higher the priority).
     */
    Event(T time, int priority) : time(time), priority(priority) {}

  public:
    /**
     * @brief Virtual constructor of an event to take place.
     */
    virtual ~Event() {}

    /**
     * @brief Function to get the time at which an event should take place.
     * @return Time in s (elapsed since the start of the simulation).
     */
    T getTime() const { return time; }

    /**
     * @brief Get the priority of the event
     * @return Priority value
     */
    T getPriority() const { return priority; }

    /**
     * @brief Function that is called at the time of the event to perform the event.
     */
    virtual void performEvent() = 0;

    /**
     * @brief Function that prints the contents of this Event.
    */
    virtual void print() = 0;
};

/**
 * @brief Class to trigger the calculation of the simulation parameters after a minimal time step.
 */
template<typename T>
class TimeStepEvent : public Event<T> {
  public:
    /**
     * @brief Construct class to schedule a minimal tim estep event.
     * @param[in] time Time after minimal time step passed in s elapsed since the start of the simulation.
     */
    TimeStepEvent(T time) : Event<T>(time, 2) { }

    /**
     * @brief Do nothing except for logging the event. As the event exists, the simulation will be forwarded to this time point in the simulation algorithm and therefore it is ensured that the simulation parameters at this point in time are calculated.
     */
    void performEvent() override { return; };

    /**
     * @brief Print the time step event
     */
    void print() override { 
      std::cout << "\n Time Step Event at t=" << this->time << " with priority " << this->priority << "\n" << std::endl;
    };
};

}  // namespace sim