#include "cfdSimulator.h"

namespace sim{ 

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

template<typename T>
void CFDSimulator<T>::setPressures(std::unordered_map<int, T> pressure_) {
    this->pressures = pressure_;
}

template<typename T>
void CFDSimulator<T>::setFlowRates(std::unordered_map<int, T> flowRate_) {
    this->flowRates = flowRate_;
}

} // namespace sim