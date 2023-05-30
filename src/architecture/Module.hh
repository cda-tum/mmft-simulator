#include "Module.h"

#include <Node.h>

namespace arch {

    template<typename T>
    Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_) : 
    id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_) { }

    template<typename T>
    int Module<T>::getId() const {
        return id;
    }

    template<typename T>
    std::vector<T> Module<T>::getPosition() const {
        return pos;
    }

    template<typename T>
    std::vector<T> Module<T>::getSize() const {
        return size; 
    }

    template<typename T>
    void Module<T>::setNodes(std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_) {
        this->boundaryNodes = boundaryNodes_;
    }

    template<typename T>
    std::unordered_map<int, std::shared_ptr<Node<T>>> Module<T>::getNodes() const {
        return boundaryNodes;
    }

    template<typename T>
    ModuleType Module<T>::getModuleType() const {
        return moduleType;
    }

}   // namespace arch