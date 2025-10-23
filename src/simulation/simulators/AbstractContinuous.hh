#include "AbstractContinuous.h"

namespace sim {

    template<typename T>
    AbstractContinuous<T>::AbstractContinuous(std::shared_ptr<arch::Network<T>> network) : Simulation<T>(Type::Abstract, Platform::Continuous, network) { }

    template<typename T>
    void AbstractContinuous<T>::simulate() {
        Simulation<T>::simulate();
        this->assertInitialized();      // perform initialization checks
        this->initialize();             // initialize the simulation
        this->conductNodalAnalysis();   // compute nodal analysis
        saveState();                    // store simulation results of current state
    }

    template<typename T>
    void AbstractContinuous<T>::saveState() {
        std::unordered_map<int, T> savePressures;
        std::unordered_map<int, T> saveFlowRates;

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

        // state
        this->getSimulationResults()->addState(this->getTime(), savePressures, saveFlowRates);
    }
    
}   /// namespace sim
