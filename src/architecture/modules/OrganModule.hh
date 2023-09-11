#include "OrganModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "CFDModule.h"

namespace arch {

template<typename T>
OrganModule<T>::OrganModule (
    int id_, std::string name_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, 
    std::unordered_map<int, Opening<T>> openings_, std::string stlFile_, T charPhysLength_, T charPhysVelocity_, T resolution_, T relaxationTime_) : 
        CFDModule<T,olb::descriptors::D2Q9<>>(id_, name_, pos_, size_, nodes_, openings_, stlFile_, charPhysLength_, charPhysVelocity_, resolution_, relaxationTime_)
    { 
        this->moduleType = ModuleType::ORGAN;
    } 

template<typename T>
void OrganModule<T>::lbmInit() {
    this->conversionFactorLength = converterNS->getConversionFactorLength();

}

template<typename T>
void OrganModule<T>::initIntegrals() {

    for (auto& [key, Opening] : this->moduleOpenings) {

    }
}

template<typename T>
void OrganModule<T>::prepareLattice() {

}

template<typename T>
void OrganModule<T>::prepareCoupling() {

}

template<typename T>
void OrganModule<T>::solve() {
    
}

template<typename T>
void OrganModule<T>::getResults(int iT_) {
    
}

template<typename T>
void OrganModule<T>::writeVTK(int iT_) {
    
}

template<typename T>
void OrganModule<T>::evaluateShearStress() {
    
}

} // namespace arch

