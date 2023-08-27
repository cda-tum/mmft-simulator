#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Fluid.h"

namespace sim {

/**
 * @brief Class to define a droplet.
 */
template<typename T>
class Droplet {
  private:
    int const id;                       ///< Unique identifier of the droplet.
    T volume;                           ///< Volume of the droplet in m^3.
    std::shared_ptr<Fluid<T>> fluid;    ///< Pointer to the fluid that describes the droplet.
    int channelId;                      ///< Id of the channel that contains the droplet, equals -1 if not in a channel.
    int moduleId;                       ///< Id of the module that contains the droplet, equals -1 if not in a module.
    uint8_t stage;                      ///< Stage of the droplet. 0=idle; 1=inChannel; 2=inModule.
    T pos[2];                           ///< Relative position in a channel or module.

  public:
    /**
     * @brief Constructs a fluid.
     * @param[in] id Unique identifier of the fluid.
     * @param[in] density Density of the fluid in kg/m^3.
     * @param[in] viscosity Viscosity of the fluid in Pas.
     */
    Droplet(int id, T volume, std::shared_ptr<Fluid<T>> fluid, int stage=0);

    /**
     * @brief Retrieve the unique identifier of the fluid.
     * @return Unique identifier of the fluid.
     */
    int getId() const;

    /**
     * @brief Retrieve volume of the droplet.
     * @return The volume of the droplet in m^3.
     */
    T getVolume() const;

    /**
     * @brief 
     * @return 
     */
    std::shared_ptr<Fluid<T>> getFluid() const;

    /**
     * @brief
     * @return
     */
    T getLength() const;

    /**
     * @brief 
     * @return
     */
    uint8_t getStage() const;

    /**
     * @brief 
     * @return 
     */
    void setStage(int newStage);

    /**
     * @brief 
     * @return 
     */
    void setPosition(T pos[2]);
};

}  // namespace sim