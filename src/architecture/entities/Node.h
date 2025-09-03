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
    const size_t id;
    std::vector<T> pos;
    T pressure = 0;
    bool ground = false;
    bool sink = false;

    /**
     * @brief Constructor of the node.
     * @param[in] id Id of the node.
     * @param[in] x Absolute x position of the node.
     * @param[in] y Absolute y position of the node.
     * @param[in] ground Boolean value whether this node is a ground node.
     * @note the default value for the optional argument ground is false.
    */
    Node(size_t id, T x, T y, bool ground=false);

public:
    /**
     * @brief Get id of the node.
     * @returns id.
    */
    [[nodiscard]] inline size_t getId() const { return id; }

    /**
     * @brief Get position of the node.
     * @param[in] pos Vector of the absolute position of the node.
    */
    inline void setPosition(std::vector<T> pos) { this->pos =  std::move(pos); }

    /**
     * @brief Get position of the node.
     * @returns Absolute position of the node
    */
    [[nodiscard]] inline std::vector<T> getPosition() const { return pos; }

    /**
     * @brief Set pressure level at the node.
     * @param[in] pressure Pressure level at the node in Pa.
     */
    inline void setPressure(T pressure) { this->pressure = pressure; }

    /**
     * @brief Get pressure level at node.
     * @return Pressure level at node.
     */
    [[nodiscard]] inline T getPressure() const { return pressure; }

    /**
     * @brief Set the sink role to the node.
     * @param[in] sink Boolean value for sink role.
    */
    inline void setSink(bool sink) { this->sink = sink; }

    /**
     * @brief Get the sink node role of the node.
     * @returns Boolean value for sink role.
    */
    [[nodiscard]] inline bool getSink() { return this->sink; }

    /**
     * @brief Set the ground node role to the node.
     * @param[in] ground Boolean value for ground node role.
    */
    inline void setGround(bool ground) { this->ground = ground; }

    /**
     * @brief Get the ground node role of the node.
     * @returns Boolean value for ground node role.
    */
    [[nodiscard]] inline bool getGround() { return this->ground; }

};

}   // namespace arch