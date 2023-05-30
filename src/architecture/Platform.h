#pragma once

namespace arch{

    /**
     * @brief An enum to specify the platform of the network
    */
    enum class Platform{
        BASE,       ///< The base platform for closed channel-based microfluidics
        DROPLET     ///< The platform for closed channel-based microfluidics with droplets
    };

}   // namespace arch