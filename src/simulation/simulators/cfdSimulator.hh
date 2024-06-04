#include "cfdSimulator.h"

namespace sim{ 

template <typename T, int DIM>
CFDSimulator<T,DIM>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, T alpha_, ResistanceModel<T>* resistanceModel_) :
    id(id_), name(name_), stlFile(stlFile_), cfdModule(cfdModule_), moduleOpenings(openings_), alpha(alpha_)
    { 
        if (!(DIM == 2 || DIM == 3)) {
            throw std::invalid_argument("Valid dimensions for CFD regions are 2 or 3.");
        }
        assert(DIM == 2 || DIM == 3);

        // Create this module's network, required for initial condition
        moduleNetwork = std::make_shared<arch::Network<T>> (cfdModule_->getNodes());

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
    }

template <typename T, int DIM>
int CFDSimulator<T,DIM>::getId() const {
    return id;
}

template <typename T, int DIM>
void CFDSimulator<T,DIM>::setModuleTypeLBM() {
    // TODO
}

template <typename T, int DIM>
std::shared_ptr<arch::Module<T>> CFDSimulator<T,DIM>::getModule() const {
    return cfdModule;
};

template <typename T, int DIM>
std::shared_ptr<arch::Network<T>> CFDSimulator<T,DIM>::getNetwork() const {
    return moduleNetwork;
}


template <typename T, int DIM>
bool CFDSimulator<T,DIM>::getInitialized() const {
    return initialized;
}

template <typename T, int DIM>
void CFDSimulator<T,DIM>::setInitialized(bool initialization_) {
    this->initialized = initialization_;
}

template <typename T, int DIM>
void CFDSimulator<T,DIM>::setVtkFolder(std::string vtkFolder_) {
    this->vtkFolder = vtkFolder_;
}

template <typename T, int DIM>
T CFDSimulator<T,DIM>::getAlpha() {
    return alpha;
}

template <typename T, int DIM>
void CFDSimulator<T,DIM>::setGroundNodes(std::unordered_map<int, bool> groundNodes_){
    this->groundNodes = groundNodes_;
}

template <typename T, int DIM>
std::unordered_map<int, arch::Opening<T>> CFDSimulator<T,DIM>::getOpenings() const {
    return moduleOpenings;
}

template <typename T, int DIM>
std::unordered_map<int, bool> CFDSimulator<T,DIM>::getGroundNodes(){
    return groundNodes;
}

} // namespace sim