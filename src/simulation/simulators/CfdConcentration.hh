#include "CfdConcentration.h"

namespace sim {

template<typename T>
CfdConcentration<T>::CfdConcentration(std::shared_ptr<arch::Network<T>> network, int radialResolution) 
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
CfdConcentration<T>::CfdConcentration(std::vector<T> position,
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
void CfdConcentration<T>::assertInitialized() const {
    CfdContinuous<T>::assertInitialized();
    ConcentrationSemantics<T>::assertInitialized();
}

template<typename T>
std::shared_ptr<Specie<T>> CfdConcentration<T>::addSpecie(T diffusivity, T satConc, T initialConcentration) {
    auto addedSpecie = ConcentrationSemantics<T>::addSpecie(diffusivity, satConc);
    // Additionally specie must be added to simulator
    if (!mixingSimulator->addSpecie(addedSpecie->getId(), addedSpecie.get(), initialConcentration)) {
        throw std::logic_error("Specie with id " + std::to_string(addedSpecie->getId()) + " could not be added to simulator.");
    }
    return addedSpecie;
}

template<typename T>
void CfdConcentration<T>::removeSpecie(const std::shared_ptr<Specie<T>>& specie) {
    // Specie must additionally be removed from simulator
    if(!mixingSimulator->removeSpecie(specie->getId())) {
        throw std::logic_error("Specie with id " + std::to_string(specie->getId()) + " could not be removed from simulator.");
    } else {
        setInitialConcentrations.erase(specie->getId());
    }
    ConcentrationSemantics<T>::removeSpecie(specie->getId());
}

template<typename T>
void CfdConcentration<T>::addConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie, T concentration) {
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
void CfdConcentration<T>::setConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie, T concentration) {
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
void CfdConcentration<T>::removeConcentrationBC(std::shared_ptr<arch::Node<T>> node, const std::shared_ptr<Specie<T>>& specie) {
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
void CfdConcentration<T>::setBoundaryConditions() {
    // Set the CFD continuous BCs (pressure and flowrate)
    CfdContinuous<T>::setBoundaryConditions();
    // Set the concentration BCs
    mixingSimulator->storeConcentrations(concentrationBCs);
}

template<typename T>
std::pair<T,T> CfdConcentration<T>::getGlobalConcentrationBounds(const std::shared_ptr<Specie<T>>& specie) {
    std::tuple<T,T> bounds = this->getCFDSimulator()->getConcentrationBounds(specie->getId());
    return std::pair<T,T> {std::get<0>(bounds), std::get<1>(bounds)};
}

template<typename T>
void CfdConcentration<T>::writeConcentrationPpm(const std::shared_ptr<Specie<T>>& specie, std::pair<T,T> bounds, int resolution) {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    mixingSimulator->writeConcentrationPpm(specie->getId(), 0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdConcentration<T>::simulate() {
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
