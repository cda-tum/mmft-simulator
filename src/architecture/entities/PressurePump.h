/**
 * @file PressurePump.h
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
 * @brief Class to specify a pressure pump, which is a component of a chip.
 */
template<typename T>
class PressurePump final : public Edge<T> {
private:
  T pressure;      ///< Pressure of pump in Pa.
  T flowRate = 0;  ///< Flow rate of pump in m^3/s.

  /**
   * @brief Constructor to create pressure pump.
   * @param[in] id Id of the pressure pump.
   * @param[in] node0 Pointer to node at one end of the pressure pump.
   * @param[in] node1 Pointer to node at other end of the pressure pump.
   * @param[in] pressure Pressure of the pump in Pa.
   */
  PressurePump(size_t id, size_t nodeA, size_t nodeB, T pressure);

  /**
   * @brief Set flow rate of the pump.
   * @param[in] flowRate Flow rate of pump in m^3/s.
   */
  inline void setFlowRate(T flowRate) { this->flowRate = flowRate; }

public:
  /**
   * @brief Set pressure of the pump.
   * @param[in] pressure Pressure of the pump in Pa.
   */
  inline void setPressure(T pressure) { this->pressure = pressure; }

  /**
   * @brief Get pressure of pump.
   * @return Pressure of pump in Pa.
   */
  [[nodiscard]] inline T getPressure() const override { return pressure; }

  /**
   * @brief Get flow rate of pump.
   * @return Flow rate of pump in m^3/s.
   */
  [[nodiscard]] inline T getFlowRate() const override { return flowRate; }

  /**
   * @brief Get resistance of pump.
   * @return Resistance of pump in Pas/L.
   */
  [[nodiscard]] inline T getResistance() const override { return getPressure() / getFlowRate(); }

    // friend definitions
    friend class Network<T>;
    friend class nodal::NodalAnalysis<T>;
};

}  // namespace arch
