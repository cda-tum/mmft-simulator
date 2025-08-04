#include "AbstractMixing.h"

namespace sim {

    template<typename T>
    AbstractMixing<T>::AbstractMixing(arch::Network<T>* network) : Simulation<T>(Type::Abstract, Platform::Mixing, network) { }

    template<typename T>
    AbstractMixing<T>::AbstractMixing(Type type_, Platform platform_, arch::Network<T>* network) : Simulation<T>(type_, platform_, network) { }

    template<typename T>
    Specie<T>* AbstractMixing<T>::addSpecie(T diffusivity, T satConc) {
        auto id = species.size();
        
        auto result = species.insert_or_assign(id, std::make_unique<Specie<T>>(id, diffusivity, satConc));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addMixture(std::unordered_map<int, T> specieConcentrations) {
        auto id = mixtures.size();

        std::unordered_map<int, Specie<T>*> species;

        for (auto& [specieId, concentration] : specieConcentrations) {
            species.try_emplace(specieId, getSpecie(specieId));
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<Mixture<T>>(id, species, std::move(specieConcentrations), carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations) {
        auto id = mixtures.size();

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<Mixture<T>>(id, species, specieConcentrations, carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<int, T> specieConcentrations) {
        auto id = mixtures.size();

        std::unordered_map<int, Specie<T>*> species;
        std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

        for (auto& [specieId, concentration] : specieConcentrations) {
            std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
            std::vector<T> zeroVec = {T(0.0)};
            species.try_emplace(specieId, getSpecie(specieId));
            specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<DiffusiveMixture<T>>(id, species, specieConcentrations, specieDistributions, carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, T> specieConcentrations) {
        auto id = mixtures.size();

        std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions;

        for (auto& [specieId, concentration] : specieConcentrations) {
            std::function<T(T)> zeroFunc = [](T) -> T { return 0.0; };
            std::vector<T> zeroVec = {T(0.0)};
            specieDistributions.try_emplace(specieId, std::tuple<std::function<T(T)>, std::vector<T>, T>{zeroFunc, zeroVec, T(0.0)});
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<DiffusiveMixture<T>>(id, species, specieConcentrations, specieDistributions, carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>,T>> specieDistributions) {
        auto id = mixtures.size();

        std::unordered_map<int, Specie<T>*> species;
        std::unordered_map<int, T> specieConcentrations;

        for (auto& [specieId, distribution] : specieDistributions) {
            species.try_emplace(specieId, getSpecie(specieId));
            specieConcentrations.try_emplace(specieId, T(0));
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<DiffusiveMixture<T>>(id, species, specieConcentrations, specieDistributions, carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::addDiffusiveMixture(std::unordered_map<int, Specie<T>*> species, std::unordered_map<int, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions) {
        auto id = mixtures.size();

        std::unordered_map<int, T> specieConcentrations;

        for (auto& [specieId, distribution] : specieDistributions) {
            specieConcentrations.try_emplace(specieId, T(0));
        }

        Fluid<T>* carrierFluid = this->getContinuousPhase();

        auto result = mixtures.try_emplace(id, std::make_unique<DiffusiveMixture<T>>(id, species, specieConcentrations, specieDistributions, carrierFluid));

        return result.first->second.get();
    }

    template<typename T>
    MixtureInjection<T>* AbstractMixing<T>::addMixtureInjection(int mixtureId, int edgeId, T injectionTime) {
        auto id = mixtureInjections.size();
        // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
        // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
        if (this->getNetwork()->isChannel(edgeId)) {
            // Insert a mixture into a channel_
            auto channel = this->getNetwork()->getChannel(edgeId);
            auto result = mixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            return result.first->second.get();
        } else if (this->getNetwork()->isPressurePump(edgeId)) {
            // If the edge is a pressure pump, the mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getPressurePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeB() : pump->getNodeA());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                mixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            }
        } else if (this->getNetwork()->isFlowRatePump(edgeId)) {
            // If the edge is a flow rate pump, the mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getFlowRatePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeB() : pump->getNodeA());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                mixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            }
        }
        return nullptr;
    }

    template<typename T>
    MixtureInjection<T>* AbstractMixing<T>::addPermanentMixtureInjection(int mixtureId, int edgeId, T injectionTime) {
        auto id = permanentMixtureInjections.size();
        // Mixtures can only be injected into edges that are channels. Otherwise a nullptr is returned
        // If the edges are pumps, the mixture is injected into adjacent channels at the pump outflow.
        if (this->getNetwork()->isChannel(edgeId)) {
            // Insert a permanent mixture into a channel
            auto channel = this->getNetwork()->getChannel(edgeId);
            auto result = permanentMixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            return result.first->second.get();
        } else if (this->getNetwork()->isPressurePump(edgeId)) {
            // If the edge is a pressure pump, the permanent mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getPressurePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeB() : pump->getNodeA());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                permanentMixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            }
        } else if (this->getNetwork()->isFlowRatePump(edgeId)) {
            // If the edge is a flow rate pump, the permanent mixture is injected into channels connected to pump outlet
            auto pump = this->getNetwork()->getFlowRatePump(edgeId);
            int nodeId = (pump->getFlowRate() >= 0.0 ? pump->getNodeB() : pump->getNodeA());
            for (auto& channel : this->getNetwork()->getChannelsAtNode(nodeId)) {
                permanentMixtureInjections.insert_or_assign(id, std::make_unique<MixtureInjection<T>>(id, mixtureId, channel, injectionTime));
            }
        }
        return nullptr;
    }

    template<typename T>
    void AbstractMixing<T>::setMixingModel(MixingModel<T>* model_) {
        this->mixingModel = model_;
    }

    template<typename T>
    void AbstractMixing<T>::calculateNewMixtures(double timestep_) {
        this->mixingModel->updateMixtures(timestep_, this->getNetwork(), this, this->mixtures);
    }

    template<typename T>
    MixtureInjection<T>* AbstractMixing<T>::getMixtureInjection(int injectionId) const {
        return mixtureInjections.at(injectionId).get();
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>>& AbstractMixing<T>::getMixtureInjections() const {
        return mixtureInjections;
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<MixtureInjection<T>>>& AbstractMixing<T>::getPermanentMixtureInjections() const {
        return permanentMixtureInjections;
    }

    template<typename T>
    MixingModel<T>* AbstractMixing<T>::getMixingModel() const {
        return mixingModel;
    }

    template<typename T>
    Mixture<T>* AbstractMixing<T>::getMixture(int mixtureId) const {
        return mixtures.at(mixtureId).get();
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<Mixture<T>>>& AbstractMixing<T>::getMixtures() const {
        return mixtures;
    }

    template<typename T>
    Specie<T>* AbstractMixing<T>::getSpecie(int specieId) const {
        return species.at(specieId).get();
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<Specie<T>>>& AbstractMixing<T>::getSpecies() const {
        return species;
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
                events.push_back(std::make_unique<MixtureInjectionEvent<T>>(injectionTime - this->getTime(), *injection, mixingModel));
            }
        }
        for (auto& [key, injection] : permanentMixtureInjections) {
            double injectionTime = injection->getInjectionTime();
            if (!injection->wasPerformed()) {
                events.push_back(std::make_unique<PermanentMixtureInjectionEvent<T>>(injectionTime - this->getTime(), *injection, mixingModel));
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
        if (this->mixingModel == nullptr) {
            throw std::logic_error("Simulation not initialized: Mixing model is not set for Mixing platform.");
        }
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
                this->mixingModel->updateMinimalTimeStep(this->getNetwork());
            } else if (this->mixingModel->isDiffusive()) {
                this->mixingModel->updateMinimalTimeStep(this->getNetwork());
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
                auto* instantMixingModel = dynamic_cast<InstantaneousMixingModel<T>*>(this->mixingModel);
                assert(instantMixingModel);
                instantMixingModel->moveMixtures(timestep, this->getNetwork());
                instantMixingModel->updateNodeInflow(timestep, this->getNetwork());
            } else if (this->mixingModel->isDiffusive()) {
                // Diffusive mixing model
                this->mixingModel->updateMixtures(timestep, this->getNetwork(), this, mixtures);
            }
            
            nextEvent->performEvent();
            ++this->getIterations();
        }

        // Store the mixtures that were in the simulation
        saveMixtures();
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
