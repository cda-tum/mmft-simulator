#include "HybridMixing.h"

namespace sim {

template<typename T>
HybridMixing<T>::HybridMixing(std::shared_ptr<arch::Network<T>> network) : HybridContinuous<T>(Platform::Concentration, network), ConcentrationSemantics<T>(dynamic_cast<Simulation<T>*>(this), this->getHash()) { }

template<typename T>
void HybridMixing<T>::assertInitialized() const 
{
    HybridContinuous<T>::assertInitialized();
    ConcentrationSemantics<T>::assertInitialized();
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, std::string name)
{
    size_t resolutionDefault = 20;      // Some reasonable value

    return addLbmSimulator(module, resolutionDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, size_t resolution, std::string name)
{
    T epsilonDefault = 1e-1;                                        // Some reasonable value
    T tauDefault = 0.932;                                           // For quadratic accuracy
    T adTauDefault = 0.932;                                         // For quadratic accuracy
    T charPhysLengthDefault = this->getCharacteristicLength();      // Smallest opening width / height
    T charPhysVelocityDefault = this->getCharacteristicVelocity();  // Largest expected average velocity in the system

    return addLbmSimulator(module, resolution, epsilonDefault, tauDefault, adTauDefault, charPhysLengthDefault, charPhysVelocityDefault, std::move(name));
}

template<typename T>
std::shared_ptr<lbmSimulator<T>> HybridMixing<T>::addLbmSimulator(std::shared_ptr<arch::CfdModule<T>> const module, 
    size_t resolution, T epsilon, T tau, T adTau, T charPhysLength, T charPhysVelocity, std::string name)
{
    if (this->hasValidResistanceModel()) {
        // create Simulator
        auto id = CFDSimulator<T>::getSimulatorCounter();
        auto addCfdSimulator = std::shared_ptr<lbmMixingSimulator<T>>(new lbmMixingSimulator<T>(id, std::move(name), module, this->readSpecies(), resolution, charPhysLength, charPhysVelocity, epsilon, tau, adTau));

        for (auto& [key, specie] : this->readSpecies()) {
            if (!addCfdSimulator->setBulkConcentration(key, setInitialConcentrations[key])) {
                throw std::logic_error("Specie with id " + std::to_string(key) + " could not set bul concentration.");
            }
        }

        // add Simulator
        const auto& [it, inserted] = this->getCFDSimulators().try_emplace(id, addCfdSimulator);
        if (inserted) {
            addCfdSimulator->initialize(this->getResistanceModel(), this->getMixingModel());
        } else {
            throw std::logic_error("Could not emplace new lbmSimulator.");
        }

        return addCfdSimulator;
    } else {
        throw std::invalid_argument("Attempt to add CFD Simulator without valid resistanceModel.");
    }
}

template<typename T>
std::shared_ptr<Specie<T>> HybridMixing<T>::addSpecie(T diffusivity, T satConc, T initialConcentration) {
    auto addedSpecie = ConcentrationSemantics<T>::addSpecie(diffusivity, satConc);
    // Additionally specie must be added to all simulators
    for (auto& [key, simulator] : this->getCFDSimulators()) {
         if (!simulator->addSpecie(addedSpecie->getId(), addedSpecie.get(), initialConcentration)) {
            throw std::logic_error("Specie with id " + std::to_string(addedSpecie->getId()) + " could not be added to simulator with id " + std::to_string(key) + ".");
        }
    }
    setInitialConcentrations.try_emplace(addedSpecie->getId(), initialConcentration);

    return addedSpecie;
}

template<typename T>
void HybridMixing<T>::removeSpecie(const std::shared_ptr<Specie<T>>& specie) {
    // Specie must additionally be removed from all simulators
    for (auto& [key, simulator] : this->getCFDSimulators()) {
         if (!simulator->removeSpecie(specie->getId())) {
            throw std::logic_error("Specie with id " + std::to_string(specie->getId()) + " is not present in simulator with id " + std::to_string(key) + ".");
        } else {
            setInitialConcentrations.erase(specie->getId());
        }
    }
    ConcentrationSemantics<T>::removeSpecie(specie->getId());
}


template<typename T>
void HybridMixing<T>::setInstantaneousMixingModel() {
    if (this->getCFDSimulators().size() > 0) {
        throw std::logic_error("Cannot change to instantaneous mixing model when CFD simulators are already added to the simulation.");
    }
    ConcentrationSemantics<T>::setInstantaneousMixingModel();
}

template<typename T>
void HybridMixing<T>::setDiffusiveMixingModel() {
    if (this->getCFDSimulators().size() > 0) {
        throw std::logic_error("Cannot change to diffusive mixing model when CFD simulators are already added to the simulation.");
    }
    ConcentrationSemantics<T>::setDiffusiveMixingModel();
}

template<typename T>
std::pair<T,T> HybridMixing<T>::getGlobalConcentrationBounds(size_t adKey) const {
    T minP = std::numeric_limits<T>::max();
    T maxP = 0.0;
    for (auto& [key, simulator] : this->readCFDSimulators()) {
        auto localBounds = simulator->getConcentrationBounds(adKey);
        if (std::get<0>(localBounds) < minP) {
            minP = std::get<0>(localBounds);
        }
        if (std::get<1>(localBounds) > maxP) {
            maxP = std::get<1>(localBounds);
        }
    }
    return std::pair<T,T> {minP, maxP};
}

template<typename T>
void HybridMixing<T>::writeConcentrationPpm(size_t adKey, std::pair<T,T> bounds, int resolution) const {
    for (auto& [key, simulator] : this->readCFDSimulators()) {
        // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
        simulator->writeConcentrationPpm(adKey, 0.98*bounds.first, 1.02*bounds.second, resolution);
    }
}

template<typename T>
void HybridMixing<T>::simulate() {
    Simulation<T>::simulate();
    this->assertInitialized();              // perform initialization checks
    this->initialize();                     // initialize the simulation

    // Catch runtime error, not enough CFD simulators.
    if (this->getNetwork()->getCfdModules().size() <= 0 ) {
        throw std::runtime_error("There are no CFD simulators defined for the Hybrid simulation.");
    }

    bool allConverged = false;
    bool pressureConverged = false;

    // Initialization of NS CFD domains
    while (! allConverged) {
        allConverged = conductCFDSimulation(this->getCFDSimulators());
    }

    // Obtain overal steady-state flow result
    while (! allConverged || !pressureConverged) {
        // conduct CFD simulations
        allConverged = conductCFDSimulation(this->getCFDSimulators());
        // compute nodal analysis again
        pressureConverged = HybridContinuous<T>::conductNodalAnalysis().value();
    }

    #ifdef VERBOSE     
        this->printResults();
        std::cout << "[Simulation] All pressures have converged." << std::endl; 
    #endif

    if (this->getWritePpm()) {
        this->writePressurePpm(this->getGlobalPressureBounds());
        this->writeVelocityPpm(this->getGlobalVelocityBounds());
        for (auto& [key, specie] : this->readSpecies()) {
            this->writeConcentrationPpm(key, getGlobalConcentrationBounds(key));
        }
    }

    this->saveState();

    // Couple the resulting CFD flow field to the AD fields
    coupleNsAdLattices(this->getCFDSimulators());

    // Obtain overal steady-state concentration results
    bool concentrationConverged = false;
    while (!concentrationConverged) {
        this->getMixingModel()->propagateSpecies(this->getNetwork().get(), this);
        concentrationConverged = conductADSimulation(this->getCFDSimulators());
    }

    this->saveState();
    this->saveMixtures();
}

template<typename T>
void HybridMixing<T>::saveState() {
    std::unordered_map<int, T> savePressures;
    std::unordered_map<int, T> saveFlowRates;
    std::unordered_map<int, std::deque<MixturePosition<T>>> saveMixturePositions;
    std::unordered_map<int, std::string> vtkFiles;

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

    // vtk Files
    for (auto& [id, simulator] : this->readCFDSimulators()) {
        vtkFiles.try_emplace(simulator->getId(), simulator->getVtkFile());
    }

    // state
    this->getSimulationResults()->addState(this->getTime(), savePressures, saveFlowRates, saveMixturePositions, vtkFiles);
}

template<typename T>
void HybridMixing<T>::saveMixtures() {
    this->getSimulationResults()->setMixtures(this->getMixtures());   
}

}   // namespace sim
