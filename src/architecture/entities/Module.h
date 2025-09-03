/**
 * @file Module.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
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
    std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes;    ///< List of nodes that are placed on the boundary of the module.
    ModuleType moduleType = ModuleType::NORMAL;     ///< Type of module.

protected:

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] boundaryNodes Map of nodes that are on the boundary of the module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes);

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] boundaryNodes Map of nodes that are on the boundary of the module.
     * @param[in] type The module type of this module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes, ModuleType type);

    /**
     * @brief Constructor of the module.
     * @param[in] id Id of the module.
     * @param[in] pos Absolute position of the module in _m_, from the bottom left corner of the microfluidic device.
     * @param[in] size Size of the module in _m_.
     * @param[in] openings Map of openings corresponding to the nodes.
     * @param[in] type The module type of this module.
    */
    Module(size_t id, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, Opening<T>> openings, ModuleType type);

public:

    /**
     * @brief Get id of the module.
     * @returns id.
    */
    int getId() const;

    /**
     * @brief Get position of the module.
     * @returns Absolute position of the left bottom corner of the module, with respect to the left bottom corner of the device.
    */
    std::vector<T> getPosition() const;

    /**
     * @brief Get size of the module.
     * @returns Size of the module in m x m.
    */
    std::vector<T> getSize() const;

    /**
     * @brief Set the nodes on the boundary of the module.
     * @param[in] boundaryNodes 
    */
    void setNodes(std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes);

    /**
     * @brief Get the nodes on the boundary of the module.
     * @returns Nodes on the boundary of the module.
    */
    std::unordered_map<int, std::shared_ptr<Node<T>>> getNodes() const;

    /**
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    void setModuleTypeLbm();

    /**
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    void setModuleTypeEssLbm();

    /**
     * @brief Returns the type of the module.
     * @returns What type the channel has.
     */
    virtual ModuleType getModuleType() const;
};

/**
 * @brief A class that specifies a CFD module. On this module, an LBM simulation using a OpenLB simulator can be conducted. 
*/
template<typename T>
class CfdModule final : public Module<T> {
private:
    std::string stlFile;                                    ///< The STL file of the CFD domain.
    std::unordered_map<int, Opening<T>> moduleOpenings;     ///< Map of openings. <nodeId, arch::Opening>
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
    CfdModule(size_t id, std::vector<T> pos, std::vector<T> size, std::string stlFile, std::unordered_map<int, Opening<T>> openings);

public:

    /**
     * @brief Complete the definition of the local network by setting the channel resistances of the virtual channels
     * in the fully connected graph. This graph is used to obtain an initial condition of the boundary values.
     * @param[in] resistanceModel The resistance model that is used in the simulator to obtain channel resistances.
     */
    void initialize(const sim::ResistanceModel<T>* resistanceModel);

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
    [[nodiscard]] inline const std::unordered_map<int, Opening<T>>& getOpenings() const { return moduleOpenings; }

    /**
     * @brief Get the stl file of the module.
     * @returns The location of the stl file.
     */
    [[nodiscard]] inline std::string getStlFile() const { return stlFile; }


};

}   // namespace arch
