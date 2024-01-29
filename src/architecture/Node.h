/**
 * @file Node.h
 */

#pragma once

#include <vector>

namespace arch {

/**
 * @brief Class to specify a node, which defines connections between components of a Network.
*/
template<typename T>
class Node {
private:
    int const id;
    std::vector<T> pos;
    T pressure = 0;
    bool ground = false;
    bool sink = false;

public:
    /**
     * @brief Constructor of the node.
     * @param[in] id Id of the node.
     * @param[in] x Absolute x position of the node.
     * @param[in] y Absolute y position of the node.
    */
    Node(int id, T x, T y, bool ground=false);

    /**
     * @brief Get position of the node.
     * @param[in] pos Vector of the absolute position of the node.
    */
    void setPosition(std::vector<T> pos);

    /**
     * @brief Set pressure level at the node.
     * @param[in] pressure Pressure level at the node in Pa.
     */
    void setPressure(T pressure);

    /**
     * @brief Set the sink role to the node.
     * @param[in] sink Boolean value for sink role.
    */
    void setSink(bool sink);

    /**
     * @brief Set the ground node role to the node.
     * @param[in] ground Boolean value for ground node role.
    */
    void setGround(bool ground);

    /**
     * @brief Get the sink node role of the node.
     * @returns Boolean value for sink role.
    */
    bool getSink();

    /**
     * @brief Get the ground node role of the node.
     * @returns Boolean value for ground node role.
    */
    bool getGround();

    /**
     * @brief Get id of the node.
     * @returns id.
    */
    int getId() const;

    /**
     * @brief Get position of the node.
     * @returns Absolute position of the node
    */
    std::vector<T> getPosition() const;

    /**
     * @brief Get pressure level at node.
     * @return Pressure level at node.
     */
    T getPressure() const;
};

}   // namespace arch