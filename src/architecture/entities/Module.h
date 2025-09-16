/**
 * @file Module.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace sim {

// Forward declared dependencies
template<typename T>
class CfdSimulator;

template<typename T>
class ResistanceModel;

}

namespace arch {

// Forward declared dependencies
template<typename T>
class Node;

template<typename T>
struct Opening;

/**
 * @brief An enum to specify the type of module.
*/
enum class ModuleType {
    NORMAL,     ///< A placeholder module with no special functionality.
    MIXER,      ///< A mixer module that mixes the incoming fluids to a certain degree.
    HEATER,     ///< A heater module that heats the passing fluid to a certain degree.
    ORGAN,      ///< An organ module that models the functionality of an organ in the network.
    LBM,        ///< A CFD module that maps the LBM simulation result from an OLB solver on the 1D level.
    ESS_LBM     ///< A CFD module that maps the LBM simulation result from an ESS solver on the 1D level.

};

/**
 * @brief Class to specify a module, which is a functional component in a network.
*/
template<typename T>
class Module {
private:
    const size_t id;                ///< Id of the module.
    std::vector<T> pos;             ///< Position (x, y) of the lower left corner of the module.
    std::vector<T> size;            ///< Size (x, y) of the rectangular module.
    std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes;    ///< List of nodes that are placed on the boundary of the module.
    ModuleType moduleType = ModuleType::NORMAL;     ///< Type of module.

protected:

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] boundaryNodes Map of nodes that are on the boundary of the module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes);

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] boundaryNodes Map of nodes that are on the boundary of the module.
     * @param[in] type The module type of this module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes, ModuleType type);

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] type The module type of this module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<size_t, Opening<T>> openings, ModuleType type);

    /**
     * @brief Set the nodes on the boundary of the module.
     * @param[in] boundaryNodes 
    */
    inline void setNodes(std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes) { this->boundaryNodes = boundaryNodes; }

    /**
     * @brief Removes a node from the module.
     * @param[in] nodeId Id of the node that should be removed.
     */
    virtual void removeNode(size_t nodeId) { boundaryNodes.erase(nodeId); }

public:

    /**
     * @brief Get id of the module.
     * @returns id.
    */
    [[nodiscard]] inline int getId() const { return id; }

    /**
     * @brief Get position of the module.
     * @returns Absolute position of the left bottom corner of the module, with respect to the left bottom corner of the device.
    */
    [[nodiscard]] inline const std::vector<T>& getPosition() const { return pos; }

    /**
     * @brief Get size of the module.
     * @returns Size of the module in m x m.
    */
    [[nodiscard]] inline const std::vector<T>& getSize() const { return size; }

    /**
     * @brief Get the nodes on the boundary of the module.
     * @returns Nodes on the boundary of the module.
    */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Node<T>>>& getNodes() const { return boundaryNodes; }

    /** TODO: Shouldn't this be hidden, and set by inherited object? (today)
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    inline void setModuleTypeLbm() { moduleType = ModuleType::LBM; }

    /**
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    inline void setModuleTypeEssLbm() { moduleType = ModuleType::ESS_LBM; }

    /**
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    [[nodiscard]] inline ModuleType getModuleType() const { return moduleType; }
};

/**
 * @brief A class that specifies a CFD module. On this module, an LBM simulation using a OpenLB simulator can be conducted. 
*/
template<typename T>
class CfdModule final : virtual public Module<T> {
private:
    std::string stlFile;                                    ///< The STL file of the CFD domain.
    std::unordered_map<size_t, Opening<T>> moduleOpenings;     ///< Map of openings. <nodeId, arch::Opening>
    std::shared_ptr<Network<T>> moduleNetwork = nullptr;    ///< Fully connected graph as network for the initial approximation.

    bool isInitialized = false;

    /**
     * @brief Constructs a CFD module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] stlFile Location of the stl file that gives the geometry of the domain.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @note The module type is defaulted to ModuleType::LBM
    */
    CfdModule(size_t id, std::vector<T> pos, std::vector<T> size, std::string stlFile, std::unordered_map<size_t, Opening<T>> openings);

    /**
     * @brief Complete the definition of the local network by setting the channel resistances of the virtual channels
     * in the fully connected graph. This graph is used to obtain an initial condition of the boundary values.
     * @param[in] resistanceModel The resistance model that is used in the simulator to obtain channel resistances.
     */
    void initialize(const sim::ResistanceModel<T>* resistanceModel);

    /**
     * @brief Removes a node and its opening from the CFD module.
     * @param[in] nodeId Id of the node that should be removed.
     */
    void removeNode(size_t nodeId) override;

public:

    /**
     * @brief Checks whether this CFD module has valid STL and Openings definitions.
     * @throws invalid_argument if a definition is missing for STL and Openings.
     */
    void assertInitialized();

    /**
     * @brief Get the fully connected graph of this module, that is used for the initial approximation.
     * @return Network of the fully connected graph.
    */
    [[nodiscard]] inline std::shared_ptr<Network<T>> getNetwork() const { return moduleNetwork; }

    /**
     * @brief Get the openings of the module.
     * @returns Module openings.
     */
    [[nodiscard]] inline const std::unordered_map<size_t, Opening<T>>& getOpenings() const { return moduleOpenings; }

    /**
     * @brief Get the stl file of the module.
     * @returns The location of the stl file.
     */
    [[nodiscard]] inline std::string getStlFile() const { return stlFile; }

    // Friend definitions
    friend class Network<T>;
    friend class sim::CFDSimulator<T>;
};

}   // namespace arch
