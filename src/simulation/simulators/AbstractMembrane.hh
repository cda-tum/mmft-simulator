#include "AbstractMembrane.h"

namespace sim {

    template<typename T>
    AbstractMembrane<T>::AbstractMembrane(std::shared_ptr<arch::Network<T>> network) : AbstractMixing<T>(Type::Abstract, Platform::Membrane, network) {
        membraneModel = std::unique_ptr<MembraneModel9<T>>(new MembraneModel9<T>());
    }

    template<typename T>
    void AbstractMembrane<T>::assertInitialized() const {
        AbstractMixing<T>::assertInitialized();     // Assert initialization for base class.
        if (this->membraneModel == nullptr) {
            throw std::logic_error("Simulation not initialized: Mixing model is not set for Mixing platform.");
        }
    }

    template<typename T>
    void AbstractMembrane<T>::simulate() {
        this->assertInitialized();              // perform initialization checks
        this->initialize();                     // initialize the simulation
        
        T simulationResultTimeCounter = 0.0;

        #ifdef VERBOSE
        std::cout << "Running Abstract Membrane simulation..." << std::endl;
        #endif
        auto* instantMixingModel = dynamic_cast<InstantaneousMixingModel<T>*>(this->getMixingModel());
        if (!instantMixingModel) {
            throw std::logic_error { "Unable to run membrane simulation with non-instantaneous mixing" };
        }
        while (true) {
            #ifdef VERBOSE
                // only print on change of highest digit in iteration; so ..., 80, 90, 100, 200, 300, ...
                if (this->getIterations() < 10 || this->getIterations() % static_cast<int>(std::pow(10, static_cast<int>(std::log10(this->getIterations())))) == 0) {
                    std::cout << "Iteration " << this->getIterations() << " (" << time << "s)" << std::endl;
                }
            #endif

            // compute nodal analysis
            this->conductNodalAnalysis();

            // store simulation results of current state if result time is reached
            if (simulationResultTimeCounter <= 0) {
                this->saveState();
                if ((this->getPermanentMixtureInjections().empty() && this->getMixtureInjections().empty())) {
                    simulationResultTimeCounter = 0;
                } else {  // for continuous fluid simulation without droplets only store simulation time steps
                    simulationResultTimeCounter = this->getWriteInterval();
                }
            }

            // compute internal minimal timestep and make sure that it is not too big to skip next save timepoint
            auto timeToNextResult = this->getWriteInterval() - std::fmod(this->getTime(), this->getWriteInterval());
            this->calculateNewMixtures(this->getDt());

            // update minimal timestep and limit it so that the next time the state should be written is not overstepped
            instantMixingModel->fixedMinimalTimeStep(this->getNetwork().get());
            instantMixingModel->limitMinimalTimeStep(0.0, timeToNextResult);

            // compute events
            auto events = this->computeMixingEvents();

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

            // get next event or break loop, if no events remain
            Event<T>* nextEvent = nullptr;
            if (events.size() != 0) {
                nextEvent = events[0].get();
            } else {
                break;
            }

            auto nextEventTime = nextEvent->getTime();
            this->getTime() += nextEventTime;
            simulationResultTimeCounter -= nextEventTime;
            this->getDt() = nextEventTime;

            if (nextEventTime > 0.0) {
                // move droplets until event is reached
                // moveDroplets(nextEventTime);
                instantMixingModel->moveMixtures(this->getDt(), this->getNetwork().get());
                instantMixingModel->calculateMembraneExchange(this->getDt(), this, this->getNetwork().get(), this->getMixtures());
                instantMixingModel->updateNodeInflow(this->getDt(), this->getNetwork().get());
            }

            // perform event (inject droplet, move droplet to next channel, block channel, merge channels)
            // only one event at a time is performed in order to ensure a correct state
            // hence, it might happen that the next time for an event is 0 (e.g., when  multiple events happen at the same time)
            assert(nextEventTime >= 0.0);
            nextEvent->performEvent();

            if (this->getTime() >= this->getTMax()) {
                break;
            }

            ++this->getIterations();
        }

        //store simulation state once more at the end of the simulation (even if this is not part of the simulation result timestep)
        this->saveState();
        this->saveMixtures();
    }

}   /// namespace sim
