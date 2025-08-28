#include "cfdSimulator.h"

namespace sim{ 

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::CfdModule<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_) :
    id(id_), name(name_), stlFile(stlFile_), cfdModule(cfdModule_), moduleOpenings(openings_) { ++simulatorCounter; }

template <typename T>
CFDSimulator<T>::CFDSimulator (int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::CfdModule<T>> cfdModule_, std::unordered_map<int, arch::Opening<T>> openings_, std::shared_ptr<mmft::Scheme<T>> updateScheme_) :
    CFDSimulator<T> (id_, name_, stlFile_, cfdModule_, openings_)
{ 
    this->updateScheme = updateScheme_;
}

template<typename T>
void CFDSimulator<T>::initialize(const ResistanceModel<T>* resistanceModel_) {
    // Create this module's network, required for initial condition
    moduleNetwork = arch::Network<T>::createNetwork(this->cfdModule->getNodes());

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

} // namespace sim