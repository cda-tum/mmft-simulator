#pragma once

#include <string>

#include <Node.h>

namespace arch {

/**
 * @brief Abstract class to specify an edge, which is a connection between two nodes. Multiple components of a chip are an edge.
 */
template<typename T>
class Edge {
  protected:
    int const id;           ///< Id of the edge.
    int nodeA;              ///< Node at one end of the edge.
    int nodeB;              ///< Node at other end of the edge.

  public:
    /**
     * @brief Constructor of the edge.
     * @param[in] id Id of the edge.
     * @param[in] nodeA Node at one end of the edge.
     * @param[in] nodeB Node at other end of the edge.
     */
    Edge(int id, int nodeA, int nodeB);

    /**
     * @brief Get id of the edge.
     * @return Id of the edge.
     */
    int getId() const;

    /**
     * @brief Get pressure over the edge.
     * @return Pressure over the edge in Pa.
     */
    virtual double getPressure() const = 0;

    /**
     * @brief Get flow rate in the edge.
     * @return Flow rate of the edge in m^3/s.
     */
    virtual double getFlowRate() const = 0;

    /**
     * @brief Get overall resistance over an edge.
     * @return Resistance over an edge in Pas/m^3.
     */
    virtual double getResistance() const = 0;

    /**
     * @brief Get pointer to node 0 (node at one end of the edge).
     * @return Pointer to node 0 (node at one end of the edge).
     */
    int getNodeA() const;

    /**
     * @brief Get pointer to node 1 (node at other end of the edge).
     * @return Pointer to node 1 (node at other end of the edge).
     */
    int getNodeB() const;

    /**
     * @brief Get the nodes at the ends of the channel.
     * @returns Nodes at the ends of the channel.
    */
    std::vector<int> getNodes() const;
};

}  // namespace arch