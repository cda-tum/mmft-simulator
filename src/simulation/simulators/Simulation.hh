#include "Simulation.h"

namespace sim {

    template<typename T>
    Simulation<T>::Simulation(Type simType_, Platform platform_, std::shared_ptr<arch::Network<T>> network_) : simType(simType_), platform(platform_), network(network_) {
        if (network_ == nullptr) {
            throw std::logic_error("Network cannot be null.");
        }
        this->simulationResult = std::make_unique<result::SimulationResult<T>>();
    }

    template<typename T>
    void Simulation<T>::set1DResistanceModel() {
        if (fluids.empty()) {
            throw std::logic_error("Cannot set resistance model: No fluids defined.");
        }
        this->resistanceModel = std::make_unique<ResistanceModel1D<T>>(getContinuousPhase()->getViscosity());
    }

    template<typename T>
    void Simulation<T>::setPoiseuilleResistanceModel() {
        if (fluids.empty()) {
            throw std::logic_error("Cannot set resistance model: No fluids defined.");
        }
        this->resistanceModel = std::make_unique<ResistanceModelPoiseuille<T>>(getContinuousPhase()->getViscosity());
    }

    template<typename T>
    std::shared_ptr<Fluid<T>> Simulation<T>::addFluid(T viscosity, T density) {
        auto id = Fluid<T>::getFluidCounter();

        auto result = fluids.insert_or_assign(id, std::shared_ptr<Fluid<T>>(new Fluid<T>(id, density, viscosity)));

        return result.first->second;
    }

    template<typename T>
    std::shared_ptr<Fluid<T>> Simulation<T>::addMixedFluid(int fluid0Id, T volume0, int fluid1Id, T volume1) {
        // check if fluids are identically (no merging needed) and if they exist
        if (fluid0Id == fluid1Id) {
            // try to get the fluid (throws error if the fluid is not present)
            return fluids.at(fluid0Id);
        }

        // get fluids
        auto fluid0 = fluids.at(fluid0Id).get();
        auto fluid1 = fluids.at(fluid1Id).get();

        // compute ratios
        T volume = volume0 + volume1;
        T ratio0 = volume0 / volume;
        T ratio1 = volume1 / volume;

        // compute new fluid values
        T viscosity = ratio0 * fluid0->getViscosity() + ratio1 * fluid1->getViscosity();
        T density = ratio0 * fluid0->getDensity() + ratio1 * fluid1->getDensity();

        // add new fluid
        auto newFluid = this->addFluid(viscosity, density);

        return newFluid;
    }

    template<typename T>
    std::shared_ptr<Fluid<T>> Simulation<T>::addMixedFluid(const std::shared_ptr<Fluid<T>>& fluid0, T volume0, const std::shared_ptr<Fluid<T>>& fluid1, T volume1) {
        return addMixedFluid(fluid0->getId(), volume0, fluid1->getId(), volume1);
    }

    template<typename T>
    std::shared_ptr<Fluid<T>> Simulation<T>::getFluid(int fluidId) const {
        auto it = fluids.find(fluidId);
        if (it == fluids.end()) {
            throw std::logic_error("Could not get fluid with key " + std::to_string(fluidId) + ". Fluid not found.");
        }
        return it->second;
    }

    template<typename T>
    const std::unordered_map<unsigned int, std::shared_ptr<Fluid<T>>>& Simulation<T>::getFluids() const {
        return fluids;
    }

    template<typename T>
    const std::unordered_map<unsigned int, const Fluid<T>*> Simulation<T>::readFluids() const {
        std::unordered_map<unsigned int, const Fluid<T>*> fluidPtrs;
        for (auto& [id, fluid] : this->fluids) {
            fluidPtrs.try_emplace(id, fluid.get());
        }
        return fluidPtrs;
    }

    template<typename T>
    void Simulation<T>::removeFluid(int fluidId) {
        if (fluidId == continuousPhase) {
            throw std::logic_error("Cannot delete continuous phase.");
        }
        if (!fluids.erase(fluidId)) {
            throw std::logic_error("Could not delete fluid with key " + std::to_string(fluidId) + ". Fluid not found.");
        }
    }

    template<typename T>
    void Simulation<T>::removeFluid(const std::shared_ptr<Fluid<T>>& fluid) {
        removeFluid(fluid->getId());
    }

    template<typename T>
    std::shared_ptr<Fluid<T>> Simulation<T>::getContinuousPhase() const {
        return fluids.at(continuousPhase);
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(int fluidId) {
        auto it = fluids.find(fluidId);
        if (it != fluids.end()) {
            this->continuousPhase = fluidId;
            /** TODO: Miscellaneous
             * update resistance model for new continuous phase */
        } else {
            throw std::logic_error("Could not set continuousPhase fluid with key " + std::to_string(fluidId) + ". Fluid not found.");
        }
    }

    template<typename T>
    void Simulation<T>::setContinuousPhase(const std::shared_ptr<Fluid<T>>& fluid) {
        setContinuousPhase(fluid->getId());
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

        nodalAnalysis = std::make_shared<nodal::NodalAnalysis<T>> (network.get());
    }

}   /// namespace sim
