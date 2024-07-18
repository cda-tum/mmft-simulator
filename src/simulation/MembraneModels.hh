#include "MembraneModels.h"

#include <unordered_map>

namespace sim {

template<typename T>
MembraneModel<T>::MembraneModel() { }

//=========================================================================================
//=============================  permeability Membrane  ===================================
//=========================================================================================

template<typename T>
PermeabilityMembraneModel<T>::PermeabilityMembraneModel() : MembraneModel<T>() { }


//=========================================================================================
//============================  poreResistance Membrane  ==================================
//=========================================================================================

template<typename T>
PoreResistanceMembraneModel<T>::PoreResistanceMembraneModel() : MembraneModel<T>() { }


}   /// namespace sim
