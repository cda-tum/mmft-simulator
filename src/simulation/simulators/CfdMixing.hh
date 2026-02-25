#include "CfdMixing.h"

namespace sim {

template<typename T>
CfdMixing<T>::CfdMixing(std::shared_ptr<arch::Network<T>> network, int radialResolution) 
    : CfdContinuous<T>(Platform::Concentration, network, radialResolution), 
      ConcentrationSemantics<T>(dynamic_cast<Simulation<T>*>(this), this->getHash())
{
    // Define the simulator (lbmMixingSimulator)
    std::string name = "lbmMixing";
    mixingSimulator = std::shared_ptr<lbmMixingSimulator<T>>(new lbmMixingSimulator<T>(
        0, name, this->getCfdModule(), this->readSpecies(), this->getResolution(), this->getCharPhysLength(), this->getCharPhysVelocity(), this->getEpsilon(), this->getTau(), adRelaxationTime));
    this->setSimulator(mixingSimulator);
}

template<typename T>
CfdMixing<T>::CfdMixing(std::vector<T> position,
                        std::vector<T> size,
                        std::string stlFile,
                        std::unordered_map<size_t, arch::Opening<T>> openings) 
                        : CfdContinuous<T>(Platform::Concentration, position, size, stlFile, openings),
                          ConcentrationSemantics<T>(dynamic_cast<Simulation<T>*>(this), this->getHash())
{
    // Define the simulator (lbmMixingSimulator)
    std::string name = "lbmMixing";
    mixingSimulator = std::shared_ptr<lbmMixingSimulator<T>>(new lbmMixingSimulator<T>(
        0, name, this->getCfdModule(), this->readSpecies(), this->getResolution(), this->getCharPhysLength(), this->getCharPhysVelocity(), this->getEpsilon(), this->getTau(), adRelaxationTime));
    this->setSimulator(mixingSimulator);
}

template<typename T>
void CfdMixing<T>::assertInitialized() const {
    CfdContinuous<T>::assertInitialized();
    ConcentrationSemantics<T>::assertInitialized();
}

template<typename T>
void CfdMixing<T>::addConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie, T concentration) {
    // Check that the node is a dangling node with no BC yet
    if (this->getDanglingNodes().find(node) == this->getDanglingNodes().end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because it is not a boundary node.");
    }
    auto it = concentrationBCs.find(node->getId());
    if (it != concentrationBCs.end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because a BC is already set.");
    }
    // Add the BC
    if (concentrationBCs.find(node->getId()) == concentrationBCs.end()) {
        auto it = concentrationBCs.try_emplace(node->getId(), std::unordered_map<size_t, T>{});
        it.first->second.emplace(specie->getId(), concentration);
    } else {
        auto specieIt = concentrationBCs.at(node->getId()).find(specie->getId());
        if (specieIt != concentrationBCs.at(node->getId()).end()) {
            throw std::logic_error("Cannot add a boundary condition for specie " + std::to_string(specie->getId()) + " at node " + std::to_string(node->getId()) + " because a BC is already set.");
        } else {
            concentrationBCs.at(node->getId()).try_emplace(specie->getId(), concentration);
        }
    }
}

template<typename T>
void CfdMixing<T>::setConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie, T concentration) {
    // Check that the node is a pressure BC
    auto it = concentrationBCs.find(node->getId());
    if (it == concentrationBCs.end()) {
        throw std::logic_error("Cannot set pressure value at node " + std::to_string(node->getId()) + " because it is not a concentration BC.");
    }
    if (it->second.find(specie->getId()) == it->second.end()) {
        throw std::logic_error("Cannot set pressure value for specie " + std::to_string(specie->getId()) + " at node " + std::to_string(node->getId()) + " because it is not a concentration BC.");
    } else {
        // Set the BC
        it->second[specie->getId()] = concentration;
    }
}

template<typename T>
void CfdMixing<T>::removeConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie) {
    // Check that the node is a pressure BC
    auto it = concentrationBCs.find(node->getId());
    if (it == concentrationBCs.end()) {
        throw std::logic_error("Cannot remove pressure BC at node " + std::to_string(node->getId()) + " because it is not a concentration BC.");
    }
    if (it->second.find(specie->getId()) == it->second.end()) {
        throw std::logic_error("Cannot remove pressure BC for specie " + std::to_string(specie->getId()) + " at node " + std::to_string(node->getId()) + " because it is not a concentration BC.");
    } else {
        // Remove the BC
        it->second.erase(specie->getId());
    }
    // If no specie BCs are left at this node, remove the node entry as well
    if (it->second.empty()) {   
        concentrationBCs.erase(it);
    }
}

template<typename T>
void CfdMixing<T>::setBoundaryConditions() {
    // Set the CFD continuous BCs (pressure and flowrate)
    CfdContinuous<T>::setBoundaryConditions();
    // Set the concentration BCs
    mixingSimulator->storeConcentrations(concentrationBCs);
}

template<typename T>
std::pair<T,T> CfdMixing<T>::getGlobalConcentrationBounds(const std::shared_ptr<Specie<T>>& specie) {
    std::tuple<T,T> bounds = this->getCFDSimulator()->getConcentrationBounds(specie->getId());
    return std::pair<T,T> {std::get<0>(bounds), std::get<1>(bounds)};
}

template<typename T>
void CfdMixing<T>::writeConcentrationPpm(const std::shared_ptr<Specie<T>>& specie, std::pair<T,T> bounds, int resolution) {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    mixingSimulator->writeConcentrationPpm(specie->getId(), 0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdMixing<T>::simulate() {
    assertInitialized();
    this->initialize();

    // Solve the CFD domain
    mixingSimulator->solveCFD(this->getMaxIter());;

    if (this->isWritePpm()) {
        this->writePressurePpm(this->getGlobalPressureBounds());
        this->writeVelocityPpm(this->getGlobalVelocityBounds());
        for (const auto& [specieId, speciePtr] : this->getSpecies()) {
            auto bounds = getGlobalConcentrationBounds(speciePtr);
            this->writeConcentrationPpm(speciePtr, bounds);
        }
    }

    this->saveState();
}

}   /// namespace sim
