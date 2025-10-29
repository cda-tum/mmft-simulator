#include "ConcentrationSemantics.h"

namespace sim {

template<typename T>
ConcentrationSemantics<T>::ConcentrationSemantics(Simulation<T>* simRef, size_t simHash) : simRef(simRef), simHash(simHash) { 
    this->mixingModel = std::make_unique<InstantaneousMixingModel<T>>();
}

template<typename T>
void ConcentrationSemantics<T>::setInstantaneousMixingModel() {
    if (!this->mixingModel->isInstantaneous()) {
        if (!mixtures.empty()) {
            throw std::logic_error("Cannot change to instantaneous mixing model when mixtures are present in the simulation.");
        }
        this->mixingModel = std::make_unique<InstantaneousMixingModel<T>>();
    }
}

template<typename T>
void ConcentrationSemantics<T>::setDiffusiveMixingModel() {
    if (!this->mixingModel->isDiffusive()) {
        if (!mixtures.empty()) {
            throw std::logic_error("Cannot change to diffusive mixing model when mixtures are present in the simulation.");
        }
        this->mixingModel = std::make_unique<DiffusionMixingModel<T>>();
    }
}

template<typename T>
std::shared_ptr<Specie<T>> ConcentrationSemantics<T>::addSpecie(T diffusivity, T satConc) {
    size_t id = Specie<T>::getSpecieCounter();
    
    auto result = species.insert_or_assign(id, std::shared_ptr<Specie<T>>(new Specie<T>(simHash, id, diffusivity, satConc)));

    return result.first->second;
}

template<typename T>
const std::unordered_map<size_t, const Specie<T>*> ConcentrationSemantics<T>::readSpecies() const {
    std::unordered_map<size_t, const Specie<T>*> speciePtrs;
    for (auto& [id, specie] : this->species) {
        speciePtrs.try_emplace(id, specie.get());
    }
    return speciePtrs;
}


template<typename T>
void ConcentrationSemantics<T>::removeSpecie(const std::shared_ptr<Specie<T>>& specie) {
    removeSpecie(specie->getId());
}

template<typename T>
void ConcentrationSemantics<T>::removeSpecie(size_t specieId) {
    auto it = species.find(specieId);
    if (it != species.end()) {
        it->second->resetHash();  // Reset the hash of the specie to avoid dangling references
        // Remove the specie from the species map
        // and remove it from all mixtures that contain it.
        if (species.erase(specieId)) {
            for (auto& [mixtureId, mixture] : mixtures) {
                // If the mixture has no species left, remove it from the simulation.
                if (mixture->getSpecieCount() == 1) {
                    removeMixture(mixtureId);
                } else {
                    mixture->removeSpecie(it->second);
                }
            }
        } 
    } else {
        throw std::out_of_range("Specie with id " + std::to_string(specieId) + " does not exist.");
    }
}

template<typename T>
std::shared_ptr<Mixture<T>> ConcentrationSemantics<T>::addMixture(const std::shared_ptr<Specie<T>>& specie, T concentration) {
    return addMixture(std::vector<std::shared_ptr<Specie<T>>> {specie}, std::vector<T>{concentration});
}

template<typename T>
std::shared_ptr<Mixture<T>> ConcentrationSemantics<T>::addMixture(const std::vector<std::shared_ptr<Specie<T>>>& speciesVec, const std::vector<T>& concentrations) {
    size_t id = Mixture<T>::getMixtureCounter();

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    if (speciesVec.size() != concentrations.size()) {
        throw std::logic_error("Species and concentrations vectors must have the same size.");
    }
    if (speciesVec.empty()) {
        throw std::invalid_argument("At least one species must be present in a mixture.");
    }
    // Transform the vector of species and concentrations into an unordered_map
    std::unordered_map<size_t, std::shared_ptr<Specie<T>>> speciesMap;
    std::unordered_map<size_t, T> specieConcentrationsMap;
    for (size_t i = 0; i < speciesVec.size(); ++i) {
        speciesMap.try_emplace(speciesVec[i]->getId(), speciesVec[i]);
        specieConcentrationsMap.try_emplace(speciesVec[i]->getId(), concentrations[i]);
    }

    auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(simHash, id, speciesMap, specieConcentrationsMap, carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second;
}

template<typename T>
std::shared_ptr<Mixture<T>> ConcentrationSemantics<T>::createMixture(std::unordered_map<size_t, T> specieConcentrations_) {
    size_t id = Mixture<T>::getMixtureCounter();

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    // Transform the vector of species and concentrations into an unordered_map
    std::unordered_map<size_t, std::shared_ptr<Specie<T>>> speciesMap;
    std::unordered_map<size_t, T> specieConcentrationsMap(std::move(specieConcentrations_));
    for (auto& [key, conc] : specieConcentrationsMap) {
        speciesMap.try_emplace(key, species.at(key));
    }

    // Create non-mutable Mixture
    auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(simHash, id, speciesMap, specieConcentrationsMap, carrierFluid)));

    return result.first->second;
}

template<typename T>
const std::unordered_map<size_t, const Mixture<T>*> ConcentrationSemantics<T>::readMixtures() const {
    std::unordered_map<size_t, const Mixture<T>*> mixturePtrs;
    for (auto& [id, mixture] : this->mixtures) {
        mixturePtrs.try_emplace(id, mixture.get());
    }
    return mixturePtrs;
}

template<typename T>
void ConcentrationSemantics<T>::removeMixture(size_t mixtureId) {
    auto it = mixtures.find(mixtureId);
    if (it != mixtures.end()) {
        it->second->resetHash();  // Reset the hash of the mixture to avoid dangling references
        if (mixtures.erase(mixtureId)) {
            // Remove all injections of this mixture
            auto it = injectionMap.find(mixtureId);
            if (it != injectionMap.end()) {
                for (size_t injectionId : it->second) {
                    removeMixtureInjection(injectionId);
                }
                injectionMap.erase(it);
            }
        }
    } else {
        throw std::out_of_range("Mixture with id " + std::to_string(mixtureId) + " does not exist.");
    }
}


template<typename T>
void ConcentrationSemantics<T>::removeMixture(const std::shared_ptr<Mixture<T>>& mixture) {
    removeMixture(mixture->getId());
}

/**
 * TODO: Miscellaneous
 * lot of duplicate code, refactor
 */
template<typename T>
std::shared_ptr<MixtureInjection<T>> ConcentrationSemantics<T>::addMixtureInjection(size_t mixtureId, size_t edgeId, T injectionTime, bool isPermanent) {
    size_t id = MixtureInjection<T>::getMixtureInjectionCounter();
    if (isPermanent) {
        // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
        // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
        if (simRef->getNetwork()->isChannel(edgeId)) {
            // Insert a permanent mixture into a channel
            auto channel = simRef->getNetwork()->getChannel(edgeId);
            auto result = permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            return result.first->second;
        } else if (simRef->getNetwork()->isPressurePump(edgeId)) {
            // If the edge is a pressure pump, the permanent mixture is injected into channels connected to pump outlet
            auto pump = simRef->getNetwork()->getPressurePump(edgeId);
            size_t nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
            for (auto& channel : simRef->getNetwork()->getChannelsAtNode(nodeId)) {
                permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            }
        } else if (simRef->getNetwork()->isFlowRatePump(edgeId)) {
            // If the edge is a flow rate pump, the permanent mixture is injected into channels connected to pump outlet
            auto pump = simRef->getNetwork()->getFlowRatePump(edgeId);
            size_t nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
            for (auto& channel : simRef->getNetwork()->getChannelsAtNode(nodeId)) {
                permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            }
        }
        return nullptr;
    }
    // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
    // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
    if (simRef->getNetwork()->isChannel(edgeId)) {
        // Insert a mixture into a channel_
        auto channel = simRef->getNetwork()->getChannel(edgeId);
        auto result = mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
        return result.first->second;
    } else if (simRef->getNetwork()->isPressurePump(edgeId)) {
        // If the edge is a pressure pump, the mixture is injected into channels connected to pump outlet
        auto pump = simRef->getNetwork()->getPressurePump(edgeId);
        size_t nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
        for (auto& channel : simRef->getNetwork()->getChannelsAtNode(nodeId)) {
            mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
        }
    } else if (simRef->getNetwork()->isFlowRatePump(edgeId)) {
        // If the edge is a flow rate pump, the mixture is injected into channels connected to pump outlet
        auto pump = simRef->getNetwork()->getFlowRatePump(edgeId);
        size_t nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
        for (auto& channel : simRef->getNetwork()->getChannelsAtNode(nodeId)) {
            mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(simHash, id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
        }
    }
    return nullptr;
}

template<typename T>
std::shared_ptr<MixtureInjection<T>> ConcentrationSemantics<T>::addMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime, bool isPermanent) {
    return addMixtureInjection(mixture->getId(), edge->getId(), injectionTime, isPermanent);
}

template<typename T>
std::shared_ptr<MixtureInjection<T>> ConcentrationSemantics<T>::addPermanentMixtureInjection(size_t mixtureId, size_t edgeId, T injectionTime) {
    return addMixtureInjection(mixtureId, edgeId, injectionTime, true);
}

template<typename T>
std::shared_ptr<MixtureInjection<T>> ConcentrationSemantics<T>::addPermanentMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime) {
    return addMixtureInjection(mixture->getId(), edge->getId(), injectionTime, true);
}

template<typename T>
std::shared_ptr<MixtureInjection<T>> ConcentrationSemantics<T>::getMixtureInjection(size_t mixtureInjectionId) const {
    // Check if the mixture exists in mixtureInjections
    auto it_1 = mixtureInjections.find(mixtureInjectionId);   
    if (it_1 != mixtureInjections.end()) {
        return it_1->second;
    } else {
        // Check if the mixture exists in permanentMixtureInjections
        auto it_2 = permanentMixtureInjections.find(mixtureInjectionId);   
        if (it_2 != permanentMixtureInjections.end()) {
            return it_2->second;
        } else {
            // If the mixture injection does not exist, throw an exception
            throw std::out_of_range("Mixture injection with id " + std::to_string(mixtureInjectionId) + " does not exist.");
        }
    }
}

template<typename T>
void ConcentrationSemantics<T>::removeMixtureInjection(size_t mixtureInjectionId) {
    // This function removes the injectionId from the injectionMap for the given mixtureId
    // If the injectionMap for the mixtureId becomes empty, it removes the mixtureId from the injectionMap
    auto updateMap = [this, mixtureInjectionId](size_t mixtureId) {
        injectionMap.at(mixtureId).erase(mixtureInjectionId);
        if (injectionMap.at(mixtureId).empty()) { injectionMap.erase(mixtureId); }
    };
    // Check if the mixture exists in mixtureInjections
    auto it_1 = mixtureInjections.find(mixtureInjectionId);   
    if (it_1 != mixtureInjections.end()) {
        size_t mixtureId = it_1->second->getMixtureId();
        mixtureInjections.erase(it_1);
        updateMap(mixtureId);
    } else {
        // Check if the mixture exists in permanentMixtureInjections
        auto it_2 = permanentMixtureInjections.find(mixtureInjectionId);   
        if (it_2 != permanentMixtureInjections.end()) {
            size_t mixtureId = it_2->second->getMixtureId();
            permanentMixtureInjections.erase(it_2);
            updateMap(mixtureId);
        } else {
            // If the mixture injection does not exist, throw an exception
            throw std::out_of_range("Mixture injection with id " + std::to_string(mixtureInjectionId) + " does not exist.");
        }
    }
}

template<typename T>
void ConcentrationSemantics<T>::removeMixtureInjection(const std::shared_ptr<MixtureInjection<T>>& mixtureInjection) {
    removeMixtureInjection(mixtureInjection->getId());
}

//========================================
//========================================
//========================================

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addMixture(std::unordered_map<size_t, T> specieConcentrations) {
    size_t id = Mixture<T>::getMixtureCounter();

    std::unordered_map<size_t, Specie<T>*> species;

    for (auto& [specieId, concentration] : specieConcentrations) {
        species.try_emplace(specieId, getSpecie(specieId).get());
    }

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(simHash, id, species, std::move(specieConcentrations), carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations) {
    size_t id = Mixture<T>::getMixtureCounter();

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(simHash, id, std::move(species), std::move(specieConcentrations), carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addDiffusiveMixture(std::unordered_map<size_t, T> specieConcentrations) {
    size_t id = Mixture<T>::getMixtureCounter();

    std::unordered_map<size_t, Specie<T>*> species;
    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

    for (auto& [specieId, concentration] : specieConcentrations) {
        std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
        std::vector<T> zeroVec = {T(0.0)};
        species.try_emplace(specieId, getSpecie(specieId));
        specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
    }

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(simHash, id, species, std::move(specieConcentrations), specieDistributions, carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations) {
    size_t id = Mixture<T>::getMixtureCounter();

    std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

    for (auto& [specieId, concentration] : specieConcentrations) {
        std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
        std::vector<T> zeroVec = {T(0.0)};
        specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
    }

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(simHash, id, std::move(species), std::move(specieConcentrations), specieDistributions, carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addDiffusiveMixture(std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions) {
    size_t id = Mixture<T>::getMixtureCounter();

    std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species;
    std::unordered_map<size_t, T> specieConcentrations;

    for (auto& [specieId, distribution] : specieDistributions) {
        species.try_emplace(specieId, getSpecie(specieId));
        specieConcentrations.try_emplace(specieId, T(0));
    }

    Fluid<T>* carrierFluid = simRef->getContinuousPhase().get();

    auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(simHash, id, species, specieConcentrations, std::move(specieDistributions), carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
Mixture<T>* ConcentrationSemantics<T>::addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions) {
    size_t id = Mixture<T>::getMixtureCounter();

    std::unordered_map<size_t, T> specieConcentrations;

    for (auto& [specieId, distribution] : specieDistributions) {
        specieConcentrations.try_emplace(specieId, T(0));
    }

    Fluid<T>* carrierFluid = simRef->getContinuousPhase();

    auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(simHash, id, std::move(species), specieConcentrations, std::move(specieDistributions), carrierFluid)));
    result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

    return result.first->second.get();
}

template<typename T>
void ConcentrationSemantics<T>::assertInitialized() const {
    // Assert that mixing model is set.
    if (!this->mixingModel) {
        throw std::logic_error("Simulation not initialized: Mixing model is not set for Mixing platform.");
    }
}

}   // namespace sim