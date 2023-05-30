#pragma once

#include <vector>

#include <Node.h>

namespace arch {

    /**
     * @brief An enum to specify the type of module.
    */
    enum class ModuleType {
        NORMAL,     ///< A placeholder module with no special functionality.
        MIXER,      ///< A mixer module that mixes the incoming fluids to a certain degree.
        HEATER,     ///< A heater module that heats the passing fluid to a certain degree.
        ORGAN,      ///< An organ module that models the functionality of an organ in the network.
        LBM         ///< A CFD module that maps the LBM simulation result on the 1D level.
    };

    /**
     * @brief Class to specify a module, which is a functional component in a network.
    */
    template<typename T>
    class Module {
        protected:
            int const id;                   ///< Id of the module.
            std::vector<T> pos;             ///< Position (x, y) of the lower left corner of the module.
            std::vector<T> size;            ///< Size (x, y) of the rectangular module.
            std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes; ///< List of nodes that are placed on the boundary of the module.
            ModuleType moduleType = ModuleType::NORMAL;

        public:
        /**
         * @brief Constructor of the module.
        */
        Module(int id, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes);

        /**
         * @brief Get id of the module.
        */
        int getId() const;

        /**
         * @brief Get position of the module.
        */
        std::vector<T> getPosition() const;

        /**
         * @brief Get size of the module.
        */
        std::vector<T> getSize() const;

        /**
         * @brief Set the nodes on the boundary of the module.
        */
        void setNodes(std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes);

        /**
         * @brief Get the nodes on the boundary of the module.
        */
        std::unordered_map<int, std::shared_ptr<Node<T>>> getNodes() const;

        /**
         * @brief Returns the type of the module.
         * @returns What type the channel has.
         */
        ModuleType getModuleType() const;
    };
}   // namespace arch