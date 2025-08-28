#include "Module.h"

namespace arch {

template<typename T>
Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_) { }

template<typename T>
Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_, ModuleType type) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_), moduleType(type) { }

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
void Module<T>::setModuleTypeLbm() {
    moduleType = ModuleType::LBM;
}

template<typename T>
void Module<T>::setModuleTypeEssLbm() {
    moduleType = ModuleType::ESS_LBM;
}

template<typename T>
ModuleType Module<T>::getModuleType() const {
    return moduleType;
}

template<typename T>
CfdModule<T>::CfdModule(int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_) : 
Module<T>(id_, pos_, size_, boundaryNodes_, ModuleType::LBM) { }

template<typename T>
void CfdModule<T>::assertInitialized() {
    /**
     * TODO: Miscellaneous
     * Check whether a valid STL file is set
     * Check whether the Opening definitions are set and valid.
     */
    /** TODO: Miscellaneous
     * Check if the module is correct (i.e., bijective nodes/opening mapping, defined STL shape, with adequate bounds)
     */
}

}   // namespace arch