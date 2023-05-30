#pragma once

#include <Edge.h>
#include <Node.h>

namespace arch {

/**
 * @brief Class to specify a pressure pump, which is a component of a chip.
 */
template<typename T>
class PressurePump : public virtual Edge<T> {
  private:
    T pressure;      ///< Pressure of pump in Pa.
    T flowRate = 0;  ///< Flow rate of pump in m^3/s.

  public:
    /**
     * @brief Constructor to create pressure pump.
     * @param[in] id Id of the pressure pump.
     * @param[in] node0 Pointer to node at one end of the pressure pump.
     * @param[in] node1 Pointer to node at other end of the pressure pump.
     * @param[in] pressure Pressure of the pump in Pa.
     */
    PressurePump(int id, int nodeA, int nodeB, T pressure);

    /**
     * @brief Set pressure of the pump.
     * @param[in] pressure Pressure of the pump in Pa.
     */
    void setPressure(T pressure);

    /**
     * @brief Set flow rate of the pump.
     * @param[in] flowRate Flow rate of pump in m^3/s.
     */
    void setFlowRate(T flowRate);

    /**
     * @brief Get pressure of pump.
     * @return Pressure of pump in Pa.
     */
    T getPressure() const override;

    /**
     * @brief Get flow rate of pump.
     * @return Flow rate of pump in m^3/s.
     */
    T getFlowRate() const override;

    /**
     * @brief Get resistance of pump.
     * @return Resistance of pump in Pas/L.
     */
    T getResistance() const override;
};

}  // namespace arch
