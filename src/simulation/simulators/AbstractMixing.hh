#include "AbstractMixing.h"

namespace sim {

template<typename T>
AbstractMixing<T>::AbstractMixing(std::shared_ptr<arch::Network<T>> network) : AbstractMixing<T>(Type::Abstract, Platform::Mixing, network) { }

template<typename T>
AbstractMixing<T>::AbstractMixing(Type type_, Platform platform_, std::shared_ptr<arch::Network<T>> network) : Simulation<T>(type_, platform_, network), ConcentrationSemantics<T>(dynamic_cast<Simulation<T>*>(this), this->getHash()) { }

template<typename T>
void AbstractMixing<T>::assertInitialized() const {
    // Assert initialization for base classes.
    Simulation<T>::assertInitialized();     
    ConcentrationSemantics<T>::assertInitialized();
}

template<typename T>
std::vector<std::unique_ptr<Event<T>>> AbstractMixing<T>::computeMixingEvents() {
    // events
    std::vector<std::unique_ptr<Event<T>>> events;

    T minimalTimeStep = 0.0;
    
    // injection events
    for (auto& [key, injection] : this->getMixtureInjections()) {
        double injectionTime = injection->getInjectionTime();
        if (!injection->wasPerformed()) {
            events.push_back(std::make_unique<MixtureInjectionEvent<T>>(injectionTime - this->getTime(), *(injection.get()), this->getMixingModel()));
        }
    }
    for (auto& [key, injection] : this->getPermanentMixtureInjections()) {
        double injectionTime = injection->getInjectionTime();
        if (!injection->wasPerformed()) {
            events.push_back(std::make_unique<PermanentMixtureInjectionEvent<T>>(injectionTime - this->getTime(), *(injection.get()), this->getMixingModel()));
        }
    }
    minimalTimeStep = this->getMixingModel()->getMinimalTimeStep();

    // time step event
    if (minimalTimeStep > 0.0) {
        events.push_back(std::make_unique<TimeStepEvent<T>>(minimalTimeStep));
    }

    return events;
}

template<typename T>
void AbstractMixing<T>::calculateNewMixtures(double timestep_) {
    this->getMixingModel()->updateMixtures(timestep_, this->getNetwork().get(), this, this->getMixtures());
}

template<typename T>
void AbstractMixing<T>::simulate() {
    Simulation<T>::simulate();
    this->assertInitialized();      // perform initialization checks
    this->initialize();             // initialize the simulation
    this->conductNodalAnalysis();   // compute nodal analysis

    T timestep = 0.0;
    while(true) {
        if (this->getIterations() >= this->getMaxIterations()) {
            throw std::runtime_error("Max iterations exceeded.");
        }

        // Update and propagate the mixtures 
        if (this->hasInstantaneousMixingModel()) {
            calculateNewMixtures(timestep);
            this->getMixingModel()->updateMinimalTimeStep(this->getNetwork().get());
        } else if (this->hasDiffusiveMixingModel()) {
            this->getMixingModel()->updateMinimalTimeStep(this->getNetwork().get());
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
        if (this->hasInstantaneousMixingModel()) {
            // Instantaneous mixing model
            auto* instantMixingModel = dynamic_cast<InstantaneousMixingModel<T>*>(this->getMixingModel());
            assert(instantMixingModel);
            instantMixingModel->moveMixtures(timestep, this->getNetwork().get());
            instantMixingModel->updateNodeInflow(timestep, this->getNetwork().get());
        } else if (this->hasDiffusiveMixingModel()) {
            // Diffusive mixing model
            this->getMixingModel()->updateMixtures(timestep, this->getNetwork().get(), this, this->getMixtures());
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
    for (auto& [channelId, mixingId] : this->getMixingModel()->getFilledEdges()) {
        std::deque<MixturePosition<T>> newDeque;
        MixturePosition<T> newMixturePosition(mixingId, channelId, 0.0, 1.0);
        newDeque.push_front(newMixturePosition);
        saveMixturePositions.try_emplace(channelId, newDeque);
    }
    // Add all mixture positions
    for (auto& [channelId, deque] : this->getMixingModel()->getMixturesInEdges()) {
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
    this->getSimulationResults()->setMixtures(this->getMixtures());   
}

}   /// namespace sim
