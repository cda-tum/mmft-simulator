#include "Simulation.h"

namespace sim {

    template<typename T>
    Simulation<T>::Simulation(Type simType_, Platform platform_, arch::Network<T>* network_) : simType(simType_), platform(platform_), network(network_) {
        this->simulationResult = std::make_unique<result::SimulationResult<T>>();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::addFluid(T viscosity, T density, T concentration) {
        auto id = fluids.size();

        auto result = fluids.insert_or_assign(id, std::make_unique<Fluid<T>>(id, density, viscosity, concentration));

        return result.first->second.get();
    }

    template<typename T>
    void Simulation<T>::setFixtureId(int fixtureId_) {
        this->fixtureId = fixtureId_;
    }

    template<typename T>
    void Simulation<T>::setNetwork(arch::Network<T>* network_) {
        this->network = network_;
    }

    template<typename T>
    void Simulation<T>::setFluids(std::unordered_map<int, std::unique_ptr<Fluid<T>>> fluids_) {
        this->fluids = std::move(fluids_);
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(int fluidId_) {
        this->continuousPhase = fluidId_;
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(Fluid<T>* fluid) {
        this->continuousPhase = fluid->getId();
    }

    template<typename T>
    void Simulation<T>::setResistanceModel(ResistanceModel<T>* model_) {
        this->resistanceModel = model_;
    }

    template<typename T>
    void Simulation<T>::setMaxEndTime(T maxTime) {
        this->tMax = maxTime;
    }

    template<typename T>
    void Simulation<T>::setWriteInterval(T interval) {
        this->writeInterval = interval;
    }

    template<typename T>
    Platform Simulation<T>::getPlatform() const {
        return this->platform;
    }

    template<typename T>
    Type Simulation<T>::getType() const {
        return this->simType;
    }

    template<typename T>
    int Simulation<T>::getFixtureId() const {
        return this->fixtureId;
    }

    template<typename T>
    arch::Network<T>* Simulation<T>::getNetwork() const {
        return this->network;
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getFluid(int fluidId) const {
        return fluids.at(fluidId).get();
    }

    template<typename T>
    const std::unordered_map<int, std::unique_ptr<Fluid<T>>>& Simulation<T>::getFluids() const {
        return fluids;
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getContinuousPhase() const {
        return fluids.at(continuousPhase).get();
    }

    template<typename T>
    ResistanceModel<T>* Simulation<T>::getResistanceModel() const {
        return resistanceModel;
    }

    template<typename T>
    result::SimulationResult<T>* Simulation<T>::getSimulationResults() const {
        return simulationResult.get();
    }

    template<typename T>
    void Simulation<T>::assertInitialized() const {
        if (network == nullptr) {
            throw std::logic_error("Simulation not initialized: Network is not set.");
        }
        if (resistanceModel == nullptr) {
            throw std::logic_error("Simulation not initialized: Resistance model is not set.");
        }
    }

    template<typename T>
    void Simulation<T>::printResults() const {
        std::cout << "\n";
        // print the pressures in all nodes
        for (auto& [key, node] : network->getNodes()) {
            std::cout << "[Result] Node " << node->getId() << " has a pressure of " << node->getPressure() << " Pa.\n";
        }
        std::cout << "\n";
        // print the flow rates in all channels
        for (auto& [key, channel] : network->getChannels()) {
            std::cout << "[Result] Channel " << channel->getId() << " has a flow rate of " << channel->getFlowRate() << " m^3/s.\n";
        }
        std::cout << std::endl;
    }

    template<typename T>
    void Simulation<T>::initialize() {
        // compute and set channel lengths
        #ifdef VERBOSE
            std::cout << "[Simulation] Compute and set channel lengths..." << std::endl;
        #endif
        for (auto& [key, channel] : network->getChannels()) {
            T calculatedLength = network->calculateNodeDistance(channel->getNodeA(), channel->getNodeB());

            if (channel->getLength() == 0) {
                channel->setLength(calculatedLength);
            } else if (channel->getLength() < calculatedLength) {
                throw std::runtime_error("Invalid channel length: Insufficient to connect nodes");
            }
        }

        // compute channel resistances
        #ifdef VERBOSE
            std::cout << "[Simulation] Compute and set channel resistances..." << std::endl;
        #endif
        for (auto& [key, channel] : network->getChannels()) {
            T resistance = resistanceModel->getChannelResistance(channel.get());
            channel->setResistance(resistance);
            channel->setDropletResistance(0.0);
        }

        nodalAnalysis = std::make_shared<nodal::NodalAnalysis<T>> (network);
    }

}   /// namespace sim
