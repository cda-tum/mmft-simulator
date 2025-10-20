/**
 * @file Edge.h
 */

#pragma once

#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Node;

/**
 * @brief Abstract class to specify an edge, which is a connection between two nodes. Multiple components of a chip are an edge.
 */
template<typename T>
class Edge {
private:
  const size_t id;          ///< Id of the edge.
  size_t nodeA;             ///< Node at one end of the edge.
  size_t nodeB;             ///< Node at other end of the edge.

protected:
  /**
   * @brief Constructor of the edge.
   * @param[in] id Id of the edge.
   * @param[in] nodeA Node at one end of the edge.
   * @param[in] nodeB Node at other end of the edge.
   */
  Edge(size_t id, size_t nodeA, size_t nodeB);

public:

  virtual ~Edge() = default;

  /**
   * @brief Get id of the edge.
   * @return Id of the edge.
   */
  [[nodiscard]] inline size_t getId() const { return id; }

  /**
   * @brief Get the id of node A(node at one end of the edge).
   * @return Id of node A (node at one end of the edge).
   */
  [[nodiscard]] inline size_t getNodeAId() const { return nodeA; }

  /**
   * @brief Get the id of node B (node at other end of the edge).
   * @return Id of node B (node at other end of the edge).
   */
  [[nodiscard]] inline size_t getNodeBId() const { return nodeB; }

  /**
   * @brief Get the nodes at the ends of the channel.
   * @returns Nodes at the ends of the channel.
  */
  [[nodiscard]] inline std::vector<size_t> getNodeIds() const { return std::vector<size_t> {nodeA, nodeB}; }

  /**
   * @brief Get pressure over the edge.
   * @return Pressure over the edge in Pa.
   */
  virtual T getPressure() const = 0;

  /**
   * @brief Get flow rate in the edge.
   * @return Flow rate of the edge in m^3/s.
   */
  virtual T getFlowRate() const = 0;

  /**
   * @brief Get overall resistance over an edge.
   * @return Resistance over an edge in Pas/m^3.
   */
  virtual T getResistance() const = 0;

};

}  // namespace arch