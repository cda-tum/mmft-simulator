/**
 * @file ModuleOpening.h
 */

#pragma once

namespace arch {

/**
 * @brief Struct that defines an opening, which is an in-/outlet of the CFD domain.
*/
template<typename T>
struct Opening {
    std::shared_ptr<Node<T>> node;
    std::vector<T> normal;
    std::vector<T> tangent;
    T width;
    T height;
    T radial;

    /**
     * @brief Constructor of an opening.
     * @param[in] node The module node in the network that corresponds to this opening.
     * @param[in] normal The normal direction of the opening, pointing into the CFD domain.
     * @param[in] width The width of the opening.
     * @param[in] height The height of the opening.
    */
    Opening(std::shared_ptr<Node<T>> node_, std::vector<T> normal_, T width_, T height_=1e-4) :
        node(node_), normal(normal_), width(width_), height(height_) {

            // Rotate the normal vector 90 degrees counterclockwise to get the tangent
            T theta = 0.5*M_PI;
            tangent = { cos(theta)*normal_[0] - sin(theta)*normal_[1],
                        sin(theta)*normal_[0] + cos(theta)*normal_[1]};
            radial = (-1) * atan2(normal_[1], normal_[0]);
        }
};

}   // namespace arch
