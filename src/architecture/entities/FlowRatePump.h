/**
 * @file FlowRatePump.h
 */

#pragma once

namespace nodal {

// Forward declared dependencies
template<typename T>
class NodalAnalysis;

}

namespace arch {

// Forward declared dependencies
template<typename T>
class Edge;

template<typename T>
class Network;

/**
 * @brief Class to specify a flow rate pump, which is a component of a chip.
 */
template<typename T>
class FlowRatePump final : public Edge<T> {
private:
    T flowRate;     ///< Volumetric flow rate of the pump in m^3/s.
    T pressure;     ///< Pressure of a channel in Pa.

    /**
     * @brief Constructor to create a flow rate pump.
     * @param[in] id Id of the flow rate pump.
     * @param[in] nodeA Pointer to node at one end of the flow rate pump.
     * @param[in] nodeB Pointer to node at other end of the flow rate pump.
     * @param[in] flowRate Volumetric flow rate of the pump in m^3/s.
     */
    FlowRatePump(size_t id, size_t nodeA, size_t nodeB, T flowRate);

    /**
     * @brief Set pressure of the pump.
     * @param[in] pressure Pressure of the pump in Pa.
     */
    inline void setPressure(T pressure) { this->pressure = pressure; }

public:

    /**
     * @brief Set volumetric flow rate of the pump.
     * @param[in] flowRate New volumetric flow rate to set in m^3/s.
     */
    inline void setFlowRate(T flowRate) { this->flowRate = flowRate; }

    /**
     * @brief Get pressure of the pump.
     * @return Pressure of flow rate pump in Pa.
     */
    [[nodiscard]] inline T getPressure() const override { return pressure; }

    /**
     * @brief Get volumetric flow rate of pump.
     * @return Volumetric flow rate of pump in m^3/s.
     */
    [[nodiscard]] inline T getFlowRate() const override { return flowRate; }

    /**
     * @brief Get resistance over flow rate pump.
     * @return Resistance over flow rate pump in Pas/L.
     */
    [[nodiscard]] inline T getResistance() const override { return getPressure() / getFlowRate(); }

    // friend definitions
    friend class Network<T>;
    friend class nodal::NodalAnalysis<T>;
};

}  // namespace arch