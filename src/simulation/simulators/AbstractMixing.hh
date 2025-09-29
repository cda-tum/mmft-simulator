#include "AbstractMixing.h"

namespace sim {

    template<typename T>
    AbstractMixing<T>::AbstractMixing(std::shared_ptr<arch::Network<T>> network) : AbstractMixing<T>(Type::Abstract, Platform::Mixing, network) { }

    template<typename T>
    AbstractMixing<T>::AbstractMixing(Type type_, Platform platform_, std::shared_ptr<arch::Network<T>> network) : Simulation<T>(type_, platform_, network) {
        this->mixingModel = std::make_unique<InstantaneousMixingModel<T>>();
    }

    template<typename T>
    void AbstractMixing<T>::setInstantaneousMixingModel() {
        if (!this->mixingModel->isInstantaneous()) {
            if (!mixtures.empty()) {
                throw std::logic_error("Cannot change to instantaneous mixing model when mixtures are present in the simulation.");
            }
            this->mixingModel = std::make_unique<InstantaneousMixingModel<T>>();
        }
    }

    template<typename T>
    void AbstractMixing<T>::setDiffusiveMixingModel() {
        if (!this->mixingModel->isDiffusive()) {
            if (!mixtures.empty()) {
                throw std::logic_error("Cannot change to diffusive mixing model when mixtures are present in the simulation.");
            }
            this->mixingModel = std::make_unique<DiffusionMixingModel<T>>();
        }
    }

    template<typename T>
    std::shared_ptr<Specie<T>> AbstractMixing<T>::addSpecie(T diffusivity, T satConc) {
        size_t id = Specie<T>::getSpecieCounter();
        
        auto result = species.insert_or_assign(id, std::shared_ptr<Specie<T>>(new Specie<T>(this->getHash(), id, diffusivity, satConc)));

        return result.first->second;
    }

    template<typename T>
    void AbstractMixing<T>::removeSpecie(const std::shared_ptr<Specie<T>>& specie) {
        removeSpecie(specie->getId());
    }

    template<typename T>
    void AbstractMixing<T>::removeSpecie(size_t specieId) {
        auto it = species.find(specieId);
        if (it != species.end()) {
            auto speciePtr = it->second;
            speciePtr->resetHash();  // Reset the hash of the specie to avoid dangling references
            // Remove the specie from the species map
            // and remove it from all mixtures that contain it.
            if (species.erase(specieId)) {
                for (auto& [mixtureId, mixture] : mixtures) {
                    // If the mixture has no species left, remove it from the simulation.
                    if (mixture->getSpecieCount() == 1) {
                        removeMixture(mixtureId);
                    } else {
                        mixture->removeSpecie(speciePtr);
                    }
                }
            } 
        } else {
            throw std::out_of_range("Specie with id " + std::to_string(specieId) + " does not exist.");
        }
    }

    template<typename T>
    std::shared_ptr<Mixture<T>> AbstractMixing<T>::addMixture(const std::shared_ptr<Specie<T>>& specie, T concentration) {
        return addMixture(std::vector<std::shared_ptr<Specie<T>>> {specie}, std::vector<T>{concentration});
    }

    template<typename T>
    std::shared_ptr<Mixture<T>> AbstractMixing<T>::addMixture(const std::vector<std::shared_ptr<Specie<T>>>& speciesVec, const std::vector<T>& concentrations) {
        size_t id = Mixture<T>::getMixtureCounter();

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        if (speciesVec.size() != concentrations.size()) {
            throw std::logic_error("Species and concentrations vectors must have the same size.");
        }
        if (speciesVec.empty()) {
            throw std::invalid_argument("At least one species must be present in a mixture.");
        }
        // Transform the vector of species and concentrations into an unordered_map
        std::unordered_map<size_t, Specie<T>*> speciesMap;
        std::unordered_map<size_t, T> specieConcentrationsMap;
        for (size_t i = 0; i < speciesVec.size(); ++i) {
            speciesMap.try_emplace(speciesVec[i]->getId(), speciesVec[i].get());
            specieConcentrationsMap.try_emplace(speciesVec[i]->getId(), concentrations[i]);
        }

        auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(this->getHash(), id, speciesMap, specieConcentrationsMap, carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second;
    }

    template<typename T>
    std::shared_ptr<Mixture<T>> AbstractMixing<T>::createMixture(std::unordered_map<size_t, T> specieConcentrations_) {
        size_t id = Mixture<T>::getMixtureCounter();

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        // Transform the vector of species and concentrations into an unordered_map
        std::unordered_map<size_t, Specie<T>*> speciesMap;
        std::unordered_map<size_t, T> specieConcentrationsMap(std::move(specieConcentrations_));
        for (auto& [key, conc] : specieConcentrationsMap) {
            speciesMap.try_emplace(key, species.at(key).get());
        }

        // Create non-mutable Mixture
        auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(this->getHash(), id, speciesMap, specieConcentrationsMap, carrierFluid)));

        return result.first->second;
    }

    template<typename T>
    const std::unordered_map<size_t, const Mixture<T>*> AbstractMixing<T>::readMixtures() const {
        std::unordered_map<size_t, const Mixture<T>*> mixturePtrs;
        for (auto& [id, mixture] : this->mixtures) {
            mixturePtrs.try_emplace(id, mixture.get());
        }
        return mixturePtrs;
    }

    template<typename T>
    void AbstractMixing<T>::removeMixture(size_t mixtureId) {
        auto it = mixtures.find(mixtureId);
        if (it != mixtures.end()) {
            it->second->resetHash();  // Reset the hash of the mixture to avoid dangling references
            if (mixtures.erase(mixtureId)) {
                // Remove all injections of this mixture
                auto it = injectionMap.find(mixtureId);
                if (it != injectionMap.end()) {
                    for (int injectionId : it->second) {
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
    void AbstractMixing<T>::removeMixture(const std::shared_ptr<Mixture<T>>& mixture) {
        removeMixture(mixture->getId());
    }

    /**
     * TODO: Miscellaneous
     * lot of duplicate code, refactor
     */
    template<typename T>
    std::shared_ptr<MixtureInjection<T>> AbstractMixing<T>::addMixtureInjection(int mixtureId, int edgeId, T injectionTime, bool isPermanent) {
        size_t id = MixtureInjection<T>::getMixtureInjectionCounter();
        if (isPermanent) {
            // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
            // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
            if (this->getNetwork()->isChannel(edgeId)) {
                // Insert a permanent mixture into a channel
                auto channel = this->getNetwork()->getChannel(edgeId);
                auto result = permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
                return result.first->second;
            } else if (this->getNetwork()->isPressurePump(edgeId)) {
                // If the edge is a pressure pump, the permanent mixture is injected into channels connected to pump outlet
                auto pump = this->getNetwork()->getPressurePump(edgeId);
                int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
                for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                    permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
                }
            } else if (this->getNetwork()->isFlowRatePump(edgeId)) {
                // If the edge is a flow rate pump, the permanent mixture is injected into channels connected to pump outlet
                auto pump = this->getNetwork()->getFlowRatePump(edgeId);
                int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
                for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                    permanentMixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
                }
            }
            return nullptr;
        }
        // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
        // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
        if (this->getNetwork()->isChannel(edgeId)) {
            // Insert a mixture into a channel_
            auto channel = this->getNetwork()->getChannel(edgeId);
            auto result = mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            return result.first->second;
        } else if (this->getNetwork()->isPressurePump(edgeId)) {
            // If the edge is a pressure pump, the mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getPressurePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            }
        } else if (this->getNetwork()->isFlowRatePump(edgeId)) {
            // If the edge is a flow rate pump, the mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getFlowRatePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeBId() : pump->getNodeAId());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                mixtureInjections.insert_or_assign(id, std::shared_ptr<MixtureInjection<T>>(new MixtureInjection<T>(this->getHash(), id, mixtures.at(mixtureId).get(), channel.get(), injectionTime)));
            }
        }
        return nullptr;
    }

    template<typename T>
    std::shared_ptr<MixtureInjection<T>> AbstractMixing<T>::addMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime, bool isPermanent) {
        return addMixtureInjection(mixture->getId(), edge->getId(), injectionTime, isPermanent);
    }

    template<typename T>
    std::shared_ptr<MixtureInjection<T>> AbstractMixing<T>::addPermanentMixtureInjection(int mixtureId, int edgeId, T injectionTime) {
        return addMixtureInjection(mixtureId, edgeId, injectionTime, true);
    }

    template<typename T>
    std::shared_ptr<MixtureInjection<T>> AbstractMixing<T>::addPermanentMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime) {
        return addMixtureInjection(mixture->getId(), edge->getId(), injectionTime, true);
    }

    template<typename T>
    std::shared_ptr<MixtureInjection<T>> AbstractMixing<T>::getMixtureInjection(int mixtureInjectionId) const {
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
    void AbstractMixing<T>::removeMixtureInjection(size_t mixtureInjectionId) {
        // This function removes the injectionId from the injectionMap for the given mixtureId
        // If the injectionMap for the mixtureId becomes empty, it removes the mixtureId from the injectionMap
        auto updateMap = [this, mixtureInjectionId](size_t mixtureId) {
            injectionMap.at(mixtureId).erase(mixtureInjectionId);
            if (injectionMap.at(mixtureId).empty()) { injectionMap.erase(mixtureId); }
        };
        // Check if the mixture exists in mixtureInjections
        auto it_1 = mixtureInjections.find(mixtureInjectionId);   
        if (it_1 != mixtureInjections.end()) {
            int mixtureId = it_1->second->getMixtureId();
            mixtureInjections.erase(it_1);
            updateMap(mixtureId);
        } else {
            // Check if the mixture exists in permanentMixtureInjections
            auto it_2 = permanentMixtureInjections.find(mixtureInjectionId);   
            if (it_2 != permanentMixtureInjections.end()) {
                int mixtureId = it_2->second->getMixtureId();
                permanentMixtureInjections.erase(it_2);
                updateMap(mixtureId);
            } else {
                // If the mixture injection does not exist, throw an exception
                throw std::out_of_range("Mixture injection with id " + std::to_string(mixtureInjectionId) + " does not exist.");
            }
        }
    }

    template<typename T>
    void AbstractMixing<T>::removeMixtureInjection(const std::shared_ptr<MixtureInjection<T>>& mixtureInjection) {
        removeMixtureInjection(mixtureInjection->getId());
    }

    template<typename T>
    void AbstractMixing<T>::simulate() {
        this->assertInitialized();      // perform initialization checks
        this->initialize();             // initialize the simulation
        this->conductNodalAnalysis();   // compute nodal analysis

        T timestep = 0.0;
        while(true) {
            if (this->getIterations() >= this->getMaxIterations()) {
                throw std::runtime_error("Max iterations exceeded.");
            }

            // Update and propagate the mixtures 
            if (this->mixingModel->isInstantaneous()){
                calculateNewMixtures(timestep);
                this->mixingModel->updateMinimalTimeStep(this->getNetwork().get());
            } else if (this->mixingModel->isDiffusive()) {
                this->mixingModel->updateMinimalTimeStep(this->getNetwork().get());
            }
            
            // store simulation results of current state
            saveState();
            
            // compute events
            auto events = computeMixingEvents();
            
            // sort events
            // closest events in time with the highest priority come first
            std::sort(events.begin(), events.end(), [](auto& a, auto& b) {
                if (a->getTime() == b->getTime()) {
                    return a->getPriority() < b->getPriority();  // ascending order (the lower the priority value, the higher the priority)
                }
                return a->getTime() < b->getTime();  // ascending order
            });

            #ifdef DEBUG  
            for (auto& event : events) {
                event->print();
            }
            #endif
            
            Event<T>* nextEvent = nullptr;
            if (events.size() != 0) {
                nextEvent = events[0].get();
            } else {
                break;
            }

            timestep = nextEvent->getTime();
            this->getTime() += nextEvent->getTime();
            
            // Depending on the mixing model, the process looks different
            if (this->mixingModel->isInstantaneous()){
                // Instantaneous mixing model
                auto* instantMixingModel = dynamic_cast<InstantaneousMixingModel<T>*>(this->mixingModel.get());
                assert(instantMixingModel);
                instantMixingModel->moveMixtures(timestep, this->getNetwork().get());
                instantMixingModel->updateNodeInflow(timestep, this->getNetwork().get());
            } else if (this->mixingModel->isDiffusive()) {
                // Diffusive mixing model
                this->mixingModel->updateMixtures(timestep, this->getNetwork().get(), this, mixtures);
            }
            
            nextEvent->performEvent();
            ++this->getIterations();
        }

        // Store the mixtures that were in the simulation
        saveMixtures();
    }

//========================================
//========================================
//========================================

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addMixture(std::unordered_map<size_t, T> specieConcentrations) {
        size_t id = Mixture<T>::getMixtureCounter();

        std::unordered_map<size_t, Specie<T>*> species;

        for (auto& [specieId, concentration] : specieConcentrations) {
            species.try_emplace(specieId, getSpecie(specieId).get());
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(this->getHash(), id, species, std::move(specieConcentrations), carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations) {
        size_t id = Mixture<T>::getMixtureCounter();

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        auto result = mixtures.try_emplace(id, std::shared_ptr<Mixture<T>>(new Mixture<T>(this->getHash(), id, std::move(species), std::move(specieConcentrations), carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<size_t, T> specieConcentrations) {
        size_t id = Mixture<T>::getMixtureCounter();

        std::unordered_map<size_t, Specie<T>*> species;
        std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

        for (auto& [specieId, concentration] : specieConcentrations) {
            std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
            std::vector<T> zeroVec = {T(0.0)};
            species.try_emplace(specieId, getSpecie(specieId));
            specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(this->getHash(), id, species, std::move(specieConcentrations), specieDistributions, carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations) {
        size_t id = Mixture<T>::getMixtureCounter();

        std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

        for (auto& [specieId, concentration] : specieConcentrations) {
            std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
            std::vector<T> zeroVec = {T(0.0)};
            specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(this->getHash(), id, std::move(species), std::move(specieConcentrations), specieDistributions, carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions) {
        size_t id = Mixture<T>::getMixtureCounter();

        std::unordered_map<size_t, Specie<T>*> species;
        std::unordered_map<size_t, T> specieConcentrations;

        for (auto& [specieId, distribution] : specieDistributions) {
            species.try_emplace(specieId, getSpecie(specieId).get());
            specieConcentrations.try_emplace(specieId, T(0));
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase().get();

        auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(this->getHash(), id, species, specieConcentrations, std::move(specieDistributions), carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions) {
        size_t id = Mixture<T>::getMixtureCounter();

        std::unordered_map<size_t, T> specieConcentrations;

        for (auto& [specieId, distribution] : specieDistributions) {
            specieConcentrations.try_emplace(specieId, T(0));
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::shared_ptr<DiffusiveMixture<T>>(new DiffusiveMixture<T>(this->getHash(), id, std::move(species), specieConcentrations, std::move(specieDistributions), carrierFluid)));
        result.first->second->setMutable();     // This mixture is added through the public API -> object is mutable

        return result.first->second.get();
    }

    template<typename T>
    void AbstractMixing<T>::calculateNewMixtures(double timestep_) {
        this->mixingModel->updateMixtures(timestep_, this->getNetwork().get(), this, this->mixtures);
    }

    template<typename T>
    std::vector<std::unique_ptr<Event<T>>> AbstractMixing<T>::computeMixingEvents() {
        // events
        std::vector<std::unique_ptr<Event<T>>> events;

        T minimalTimeStep = 0.0;
        
        // injection events
        for (auto& [key, injection] : mixtureInjections) {
            double injectionTime = injection->getInjectionTime();
            if (!injection->wasPerformed()) {
                events.push_back(std::make_unique<MixtureInjectionEvent<T>>(injectionTime - this->getTime(), *(injection.get()), mixingModel.get()));
            }
        }
        for (auto& [key, injection] : permanentMixtureInjections) {
            double injectionTime = injection->getInjectionTime();
            if (!injection->wasPerformed()) {
                events.push_back(std::make_unique<PermanentMixtureInjectionEvent<T>>(injectionTime - this->getTime(), *(injection.get()), mixingModel.get()));
            }
        }
        minimalTimeStep = mixingModel->getMinimalTimeStep();

        // time step event
        if (minimalTimeStep > 0.0) {
            events.push_back(std::make_unique<TimeStepEvent<T>>(minimalTimeStep));
        }

        return events;
    }

    template<typename T>
    void AbstractMixing<T>::assertInitialized() const {
        Simulation<T>::assertInitialized();     // Assert initialization for base class.
        if (!this->mixingModel) {               // Assert that mixing model is set.
            throw std::logic_error("Simulation not initialized: Mixing model is not set for Mixing platform.");
        }
    }


    template<typename T>
    void AbstractMixing<T>::saveState() {
        std::unordered_map<int, T> savePressures;
        std::unordered_map<int, T> saveFlowRates;
        std::unordered_map<int, std::deque<MixturePosition<T>>> saveMixturePositions;

        // pressures
        for (auto& [id, node] : this->getNetwork()->getNodes()) {
            savePressures.try_emplace(node->getId(), node->getPressure());
        }

        // flow rates
        for (auto& [id, channel] : this->getNetwork()->getChannels()) {
            saveFlowRates.try_emplace(channel->getId(), channel->getFlowRate());
        }
        for (auto& [id, pump] : this->getNetwork()->getFlowRatePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }
        for (auto& [id, pump] : this->getNetwork()->getPressurePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }

        // Add a mixture position for all filled edges
        for (auto& [channelId, mixingId] : mixingModel->getFilledEdges()) {
            std::deque<MixturePosition<T>> newDeque;
            MixturePosition<T> newMixturePosition(mixingId, channelId, 0.0, 1.0);
            newDeque.push_front(newMixturePosition);
            saveMixturePositions.try_emplace(channelId, newDeque);
        }
        // Add all mixture positions
        for (auto& [channelId, deque] : mixingModel->getMixturesInEdges()) {
            for (auto& pair : deque) {
                if (!saveMixturePositions.count(channelId)) {
                    std::deque<MixturePosition<T>> newDeque;
                    MixturePosition<T> newMixturePosition(pair.first, channelId, 0.0, deque.front().second);
                    newDeque.push_front(newMixturePosition);
                    saveMixturePositions.try_emplace(channelId, newDeque);
                } else {
                    MixturePosition<T> newMixturePosition(pair.first, channelId, 0.0, pair.second);
                    saveMixturePositions.at(channelId).front().position1 = pair.second;
                    saveMixturePositions.at(channelId).push_front(newMixturePosition);
                }
            }
        }

        // state
        this->getSimulationResults()->addState(this->getTime(), savePressures, saveFlowRates, saveMixturePositions);
    }

    template<typename T>
    void AbstractMixing<T>::saveMixtures() {
        std::unordered_map<int, Mixture<T>*> mixtures_ptr;
        for (auto& [mixtureId, mixture] : this->mixtures) {
            mixtures_ptr.try_emplace(mixtureId, mixture.get());
        }
        this->getSimulationResults()->setMixtures(mixtures_ptr);   
    }

}   /// namespace sim
