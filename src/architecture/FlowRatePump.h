#pragma once

#include <Edge.h>
#include <Node.h>

namespace arch {

    /**
     * @brief Class to specify a flow rate pump, which is a component of a chip.
     */
    template<typename T>
    class FlowRatePump : public virtual Edge<T> {
    private:
        T flowRate;     ///< Volumetric flow rate of the pump in m^3/s.
        T pressure;     ///< Pressure of a channel in Pa.

    public:
        /**
         * @brief Constructor to create a flow rate pump.
         * @param[in] id Id of the flow rate pump.
         * @param[in] nodeA Pointer to node at one end of the flow rate pump.
         * @param[in] nodeB Pointer to node at other end of the flow rate pump.
         * @param[in] flowRate Volumetric flow rate of the pump in m^3/s.
         */
        FlowRatePump(int id, int nodeA, int nodeB, T flowRate);

        /**
         * @brief Set pressure of the pump.
         * @param[in] pressure Pressure of the pump in Pa.
         */
        void setPressure(T pressure);

        /**
         * @brief Set volumetric flow rate of the pump.
         * @param[in] flowRate New volumetric flow rate to set in m^3/s.
         */
        void setFlowRate(T flowRate);

        /**
         * @brief Get pressure of the pump.
         * @return Pressure of flow rate pump in Pa.
         */
        T getPressure() const override;

        /**
         * @brief Get volumetric flow rate of pump.
         * @return Volumetric flow rate of pump in m^3/s.
         */
        T getFlowRate() const override;

        /**
         * @brief Get resistance over flow rate pump.
         * @return Resistance over flow rate pump in Pas/L.
         */
        T getResistance() const override;
    };

}  // namespace arch