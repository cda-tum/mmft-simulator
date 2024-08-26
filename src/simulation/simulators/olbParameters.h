/**
 * @file olbParameters.h
 */

#pragma once

namespace sim {

/**
 * @brief Struct that defines an opening, which is an in-/outlet of the CFD domain.
*/
template<typename T>
struct lbmParameters {
    T charPhysLength;
    T charPhysVelocity;
    T resolution;
    T epsilon;
    T tau;

    /**
     * @brief Constructor of struct containing lbmParameters.
     * @param[in] charPhysLength Characteristic physical length of this simulator.
     * @param[in] charPhysVelocity Characteristic physical velocity of this simulator.
     * @param[in] resolution Resolution of this simulator.
     * @param[in] epsilon Error tolerance for convergence criterion of this simulator.
     * @param[in] tau Relaxation time of this simulator (0.5 < tau < 2.0).
    */
    lbmParameters(T charPhysLength_, T charPhysVelocity_, T resolution_, T epsilon_, T tau) :
        charPhysLength(charPhysLength_), 
        charPhysVelocity(charPhysVelocity_), 
        resolution(resolution_), 
        epsilon(epsilon_)
        tau(tau_) { }
};

}   // namespace sim
