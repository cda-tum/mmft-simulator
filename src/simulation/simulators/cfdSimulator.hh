#include "cfdSimulator.h"

namespace sim{ 

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::unordered_map<int, arch::Opening<T>> openings_, T alpha_) :
    id(id_), name(name_), stlFile(stlFile_), moduleOpenings(openings_), alpha(alpha_) { }

template <typename T>
void CFDSimulator<T>::setModuleTypeLBM() {
    // TODO
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

template<typename T>
void CFDSimulator<T>::setVtkFolder(std::string vtkFolder_) {
    this->vtkFolder = vtkFolder_;
}

template <typename T>
T CFDSimulator<T>::getAlpha() {
    return alpha;
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