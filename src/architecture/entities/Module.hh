#include "Module.h"

namespace arch {

template<typename T>
Module<T>::Module (size_t id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes_) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_) { }

template<typename T>
Module<T>::Module (size_t id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<size_t, std::shared_ptr<Node<T>>> boundaryNodes_, ModuleType type) : 
id(id_), pos(pos_), size(size_), boundaryNodes(boundaryNodes_), moduleType(type) { }

template<typename T>
Module<T>::Module (size_t id_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<size_t, Opening<T>> openings, ModuleType type) : 
id(id_), pos(pos_), size(size_) { 
    for (auto& [key, opening] : openings) {
        boundaryNodes.try_emplace(key, opening.node);
    }
    this->moduleType = type;
}

template<typename T>
CfdModule<T>::CfdModule(size_t id_, std::vector<T> pos_, std::vector<T> size_, std::string stlFile_, std::unordered_map<size_t, Opening<T>> openings_) : 
Module<T>(id_, pos_, size_, openings_, ModuleType::LBM), stlFile(stlFile_), moduleOpenings(openings_) { 
    // Create this module's network, required for initial condition
    moduleNetwork = Network<T>::createNetwork(this->getNodes());
}

template<typename T>
void CfdModule<T>::initialize(const sim::ResistanceModel<T>* resistanceModel_) { 
    // Complete this module's network by defining the channels lengths and resistances for a fully connected graph
    for (auto& [key, channel] : moduleNetwork->getChannels()) {
        auto& nodeA = moduleNetwork->getNodes().at(channel->getNodeAId());
        auto& nodeB = moduleNetwork->getNodes().at(channel->getNodeBId());
        T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
        T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
        channel->setLength(sqrt(dx*dx + dy*dy));
    }
    for (auto& [key, channel] : moduleNetwork->getChannels()) {
        // Check the shape of the channel and compute the resistance accordingly
        if (channel->isRectangular()) {
            T resistance = resistanceModel_->getChannelResistance(dynamic_cast<RectangularChannel<T>*>(channel.get()));
            channel->setResistance(resistance);
        } else if (channel->isCylindrical()) {
            throw std::invalid_argument("CfdModule::initialize: Cylindrical channels are not supported in CFD modules.");
        } else {
            throw std::invalid_argument("CfdModule::initialize: Channel " + std::to_string(channel->getId()) + " is not rectangular.");
        }
    }

    this->isInitialized = true;
}

template<typename T>
void CfdModule<T>::removeNode(size_t nodeId) {
    Module<T>::removeNode(nodeId);
    moduleOpenings.erase(nodeId);
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