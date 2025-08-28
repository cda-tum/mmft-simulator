/**
 * @file MixturePump.h
 */

#pragma once

#include "Edge.h"
#include "Fluid.h"

namespace arch {

/**
 * @brief Abstract class to specify a pump, which is a component of a chip.
 * 
 */
template<typename T>
class MixturePump : public virtual Edge<T> {
  private:
    int mixtureId;  // Id of the mixture that is pumped through the chip by the pump.

  public:
    /**
     * @brief Construct a new Pump object
     */
    MixturePump();

    /**
     * @brief Set the mixture that should be pumped through the chip by this pump
     * 
     * @param mixtureId Id of the mixture.
     */
    void setMixture(int mixtureId);

    /**
     * @brief Get the mixture that is pumped through the chip by this pump
     * 
     * @return Id of the mixture.
     */
    int getMixtureId();
};

}  // namespace arch