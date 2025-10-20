#include "cfdSimulator.h"

namespace sim{ 

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::shared_ptr<arch::CfdModule<T>> cfdModule_) :
    id(id_), name(name_), cfdModule(cfdModule_) { ++simulatorCounter; }

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::shared_ptr<arch::CfdModule<T>> cfdModule_, std::shared_ptr<mmft::Scheme<T>> updateScheme_) :
    CFDSimulator<T> (id_, name_, cfdModule_)
{ 
    this->updateScheme = updateScheme_;
}

template<typename T>
void CFDSimulator<T>::initialize(const ResistanceModel<T>* resistanceModel_) {
    this->cfdModule->initialize(resistanceModel_);
}

} // namespace sim