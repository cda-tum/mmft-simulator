#include "cfdSimulator.h"

namespace sim{ 

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, ResistanceModel<T>* resistanceModel_) :
    id(id_), name(name_), stlFile(stlFile_), cfdModule(cfdModule_), moduleOpenings(openings_)
    { 
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

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, std::shared_ptr<mmft::Scheme<T>> updateScheme_, ResistanceModel<T>* resistanceModel_) :
    CFDSimulator<T> (id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_)
{ 
    updateScheme = updateScheme_;
}

template<typename T>
int CFDSimulator<T>::getId() const {
    return id;
}

template <typename T>
std::shared_ptr<arch::Module<T>> CFDSimulator<T>::getModule() const {
    return cfdModule;
};

template <typename T>
std::shared_ptr<arch::Network<T>> CFDSimulator<T>::getNetwork() const {
    return moduleNetwork;
}


template <typename T>
bool CFDSimulator<T>::getInitialized() const {
    return initialized;
}

template <typename T>
void CFDSimulator<T>::setInitialized(bool initialization_) {
    this->initialized = initialization_;
}

template <typename T>
void CFDSimulator<T>::setUpdateScheme(const std::shared_ptr<mmft::Scheme<T>>& updateScheme_) {
    this->updateScheme = updateScheme_;
}

template<typename T>
void CFDSimulator<T>::setVtkFolder(std::string vtkFolder_) {
    this->vtkFolder = vtkFolder_;
}

template<typename T>
std::string CFDSimulator<T>::getVtkFile() {
    return this->vtkFile;
}

template <typename T>
T CFDSimulator<T>::getAlpha(int nodeId_) {
    return updateScheme->getAlpha(nodeId_);
}

template <typename T>
T CFDSimulator<T>::getBeta(int nodeId_) {
    return updateScheme->getBeta(nodeId_);
}

template<typename T>
void CFDSimulator<T>::setGroundNodes(std::unordered_map<int, bool> groundNodes_){
    this->groundNodes = groundNodes_;
}

template<typename T>
std::unordered_map<int, arch::Opening<T>> CFDSimulator<T>::getOpenings() const {
    return moduleOpenings;
}

template<typename T>
std::unordered_map<int, bool> CFDSimulator<T>::getGroundNodes(){
    return groundNodes;
}

} // namespace sim