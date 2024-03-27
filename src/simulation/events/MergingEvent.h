#pragma once

namespace sim {

// Forward declared dependencies
template<typename T>
class Droplet;

template<typename T>
class DropletBoundary;

template<typename T>
class Event;

template<typename T>
class Simulation;

/**
 * @brief Class that specifies a merge event that takes place at a bifurcation.
 */
template<typename T>
class MergeBifurcationEvent : public Event<T> {
  private:
    Droplet<T>& droplet0;             ///< First droplet that will be merged during that event (is the droplet that flows into the droplet at the bifurcation).
    Droplet<T>& droplet1;             ///< Second droplet that will be merged (is the droplet at the bifurcation).
    DropletBoundary<T>& boundary0;    ///< The boundary of droplet0 (will be "removed" from the merged droplet).
    Simulation<T>& simulation;        ///< Simulation class

  public:
    /**
     * @brief Constructs a new Merge Bifurcation Event object
     *
     * @param time At which the event takes place (elapsed since the start of the simulation) in s.
     * @param droplet0 First droplet that will be merged during that event (is the droplet that flows into the droplet at the bifurcation).
     * @param boundary0 The boundary of droplet0 (will be "removed" from the merged droplet).
     * @param droplet1 Second droplet that will be merged (is the droplet at the bifurcation).
     * @param simulation Simulation class.
     */
    MergeBifurcationEvent(T time, Droplet<T>& droplet0, Droplet<T>& droplet1, DropletBoundary<T>& boundary0, Simulation<T>& simulation);

    /**
     * @brief Conducts the merge at bifurcation event.
     */
    void performEvent() override;

    /**
     * @brief Print the merge at bifurcation event.
     */
    void print() override;
};

/**
 * @brief Class that specifies a merge event that takes place within a channel.
 */
template<typename T>
class MergeChannelEvent : public Event<T> {
  private:
    Droplet<T>& droplet0;           ///< First droplet that will be merged during that event.
    Droplet<T>& droplet1;           ///< Second droplet that will be merged during that event.
    DropletBoundary<T>& boundary0;  ///< The boundary of droplet0 (will be "removed" from the merged droplet).
    DropletBoundary<T>& boundary1;  ///< The boundary of droplet1 (will be "removed" from the merged droplet).
    Simulation<T>& simulation;      ///< Simulation class

  public:
    /**
     * @brief Construct a new Merge Channel Event object
     * @param time Time at which the event takes place in s elapsed since the start of the simulation
     * @param droplet0 First droplet that will be merged during that event.
     * @param boundary0 The boundary of droplet0 (will be "removed" from the merged droplet).
     * @param droplet1 Second droplet that will be merged during that event.
     * @param boundary1 The boundary of droplet1 (will be "removed" from the merged droplet).
     * @param simulation Simulation class.
     */
    MergeChannelEvent(T time, Droplet<T>& droplet0, Droplet<T>& droplet1, DropletBoundary<T>& boundary0, DropletBoundary<T>& boundary1, Simulation<T>& simulation);

    /**
     * @brief  Perform the merge that happens within a channel sequence.
     */
    void performEvent() override;

    /**
     * @brief  Print the merge that happens within a channel sequence.
     */
    void print() override;
};

}  // namespace sim
