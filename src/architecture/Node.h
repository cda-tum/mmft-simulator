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

        public:
        /**
         * @brief Constructor of the node.
        */
        Node(int id, T x, T y);

        /**
         * @brief Get position of the node.
        */
        void setPosition(std::vector<T> pos);

        /**
         * @brief Set pressure level at the node.
         * @param[in] pressure Pressure level at the node in Pa.
         */
        void setPressure(T pressure);

        /**
         * @brief Get id of the node.
        */
        int getId() const;

        /**
         * @brief Get position of the node.
        */
        std::vector<T> getPosition() const;

        /**
         * @brief Get pressure level at node.
         * @return Pressure level at node.
         */
        T getPressure() const;
    };

}   // namespace arch