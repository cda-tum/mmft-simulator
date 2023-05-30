#include "Edge.h"

#include <Node.h>

namespace arch {

    template<typename T>
    Edge<T>::Edge(int id_, int nodeA_, int nodeB_) : id(id_), nodeA(nodeA_), nodeB(nodeB_){};

    template<typename T>
    int Edge<T>::getId() const {
        return id;
    }

    template<typename T>
    int Edge<T>::getNodeA() const {
        return nodeA;
    }

    template<typename T>
    int Edge<T>::getNodeB() const {
        return nodeB;
    }

    template<typename T>
    std::vector<int> Edge<T>::getNodes() const {
        return std::vector<int> (nodeA, nodeB);
    }

}  // namespace arch