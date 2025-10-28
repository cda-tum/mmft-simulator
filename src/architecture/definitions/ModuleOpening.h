/**
 * @file ModuleOpening.h
 */

#pragma once

#include <iostream>

namespace arch {

/**
 * @brief Struct that defines an opening, which is an in-/outlet of the CFD domain.
*/
template<typename T>
struct Opening {
    std::shared_ptr<Node<T>> node;
    T width;
    T height;
    std::vector<T> normal;
    std::vector<T> tangent;
    T radial;

    /**
     * @brief Constructor of an opening.
     * @param[in] node The module node in the network that corresponds to this opening.
     * @param[in] normal The normal direction of the opening, pointing into the CFD domain.
     * @param[in] width The width of the opening.
     * @param[in] height The height of the opening.
    */
    Opening(std::shared_ptr<Node<T>> node_, std::vector<T> normal_, T width_, T height_=1e-4) :
        node(node_), width(width_), height(height_) {
            // Normalize normal vector
            T norm_mag = std::sqrt(normal_[0]*normal_[0] + normal_[1]*normal_[1]);
            if(norm_mag != 0) {
                normal = {normal_[0] / norm_mag, normal_[1] / norm_mag};
            } else {
                // Handle zero vector edge case
                normal = {T(0), T(0)};
                std::cout<<"Warning: Opening normal is zero [0.0, 0.0]"<<std::endl;
            }

            // Rotate the normal vector 90 degrees counterclockwise to get the tangent
            T theta = 0.5*M_PI;
            tangent = { std::cos(theta)*normal[0] - std::sin(theta)*normal[1],
                        std::sin(theta)*normal[0] + std::cos(theta)*normal[1]};


            radial = (-1) * std::atan2(normal[1], normal[0]);
        }
};

}   // namespace arch

