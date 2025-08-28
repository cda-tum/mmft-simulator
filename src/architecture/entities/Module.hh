#include "Module.h"

namespace arch {

template<typename T>
Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_) { }

template<typename T>
Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> boundaryNodes_, ModuleType type) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_), moduleType(type) { }

template<typename T>
Module<T>::Module (int id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, Opening<T>> openings, ModuleType type) : 
id(id_), pos(pos_), size(size_) { 
    for (auto& [key, opening] : openings) {
        boundaryNodes.try_emplace(key, opening.node);
    }
    this->moduleType = type;
}

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
CfdModule<T>::CfdModule(int id_, std::vector<T> pos_, std::vector<T> size_, std::string stlFile_, std::unordered_map<int, Opening<T>> openings_) : 
Module<T>(id_, pos_, size_, openings_, ModuleType::LBM), stlFile(stlFile_), moduleOpenings(openings_) { 
    // Create this module's network, required for initial condition
    moduleNetwork = Network<T>::createNetwork(this->getNodes());
}

template<typename T>
void CfdModule<T>::initialize(const sim::ResistanceModel<T>* resistanceModel_) { 
    // Complete this module's network by defining the channels lengths and resistances for a fully connected graph
    for (auto& [key, channel] : moduleNetwork->getChannels()) {
        auto& nodeA = moduleNetwork->getNodes().at(channel->getNodeA());
        auto& nodeB = moduleNetwork->getNodes().at(channel->getNodeB());
        T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
        T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
        channel->setLength(sqrt(dx*dx + dy*dy));
    }
    for (auto& [key, channel] : moduleNetwork->getChannels()) {
        T resistance = resistanceModel_->getChannelResistance(channel.get());
        channel->setResistance(resistance);
    }

    this->isInitialized = true;
}

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