#include "CfdContinuous.h"

namespace sim {

template<typename T>
CfdContinuous<T>::CfdContinuous(std::shared_ptr<arch::Network<T>> network, int radialResolution) : CfdContinuous<T>(Platform::Continuous, network, radialResolution) { }

template<typename T>
CfdContinuous<T>::CfdContinuous(Platform platform, std::shared_ptr<arch::Network<T>> network, int radialResolution) : Simulation<T>(Type::CFD, platform, network), radialResolution(radialResolution)
{
    // A fitting stl network definition must be creates from the given network.
    network_stl = std::make_shared<stl::Network>();

    // Initial definition for network_stl and stlNetwork
    updateNetworkSTL();
    generateSTL();

    // Create a local tmp folder if it doesn't exist yet
    std::filesystem::path tmp = "./tmp";
    if (!std::filesystem::exists(tmp)) {
        std::filesystem::create_directories(tmp);
    }

    // Create a temporary file for the STL
    fName = "./tmp/networkSTL-" + std::to_string(this->getHash());
    updateSTL();

    // Define the CfdModule
    /**
     * TODO:
     */

    // Define the CFDSimulator (lbmSimulator)
    /**
     * TODO:
     */
}

template<typename T>
CfdContinuous<T>::CfdContinuous(std::vector<T> position,
                                std::vector<T> size,
                                std::string stlFile,
                                std::unordered_map<size_t, arch::Opening<T>> openings) 
                                : CfdContinuous<T>(Platform::Continuous, position, size, stlFile, openings) { }

template<typename T>
CfdContinuous<T>::CfdContinuous(Platform platform, 
                                std::vector<T> position,
                                std::vector<T> size,
                                std::string stlFile,
                                std::unordered_map<size_t, arch::Opening<T>> openings) 
                                : Simulation<T>(Type::CFD, platform, nullptr) 
{
    /** TODO: */
}

template<typename T>
void setNetwork(std::shared_ptr<arch::Network<T>> network) {
    // Store the network in the simulation object
    Simulation<T>::setNetwork(network);

    // Update the STL definitions for the new network
    updateNetworkSTL();
    generateSTL();
    updateSTL();
}

template<typename T>
void addPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure) {
    /** TODO: */
}

template<typename T>
void setPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure) {
    /** TODO: */
}

template<typename T>
void addVelocityBC(std::shared_ptr<arch::Node<T>> node, T velocity) {
    /** TODO: */
}

template<typename T>
void setVelocityBC(std::shared_ptr<arch::Node<T>> node, T velocity) {
    /** TODO: */
}

template<typename T>
void removePressureBC(std::shared_ptr<arch::Node<T>> node) {
    /** TODO: */
}

template<typename T>
void removeVelocityBC(std::shared_ptr<arch::Node<T>> node) {
    /** TODO: */
}

template<typename T>
void CfdContinuous<T>::generateSTL() {
    /** TODO: */
}

template<typename T>
void CfdContinuous<T>::set1DResistanceModel() {
    throw std::logic_error("Cannot set the resistance model for a CFD simulation.");
}

template<typename T>
void CfdContinuous<T>::setPoiseuilleResistanceModel() {
    throw std::logic_error("Cannot set the resistance model for a CFD simulation.");
}

template<typename T>
std::pair<T,T> CfdContinuous<T>::getGlobalPressureBounds() const {
    return cfdSimulator->getPressureBounds();
}

template<typename T>
std::pair<T,T> CfdContinuous<T>::getGlobalVelocityBounds() const {
    return cfdSimulator->getVelocityBounds();
}


template<typename T>
void CfdContinuous<T>::writePressurePpm(std::pair<T,T> bounds, int resolution) const {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    cfdSimulator->writePressurePpm(0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdContinuous<T>::writeVelocityPpm(std::pair<T,T> bounds, int resolution) const {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    cfdSimulator->writeVelocityPpm(0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdContinuous<T>::assertInitialized() const {
    /** TODO: */
}

template<typename T>
void CfdContinuous<T>::initialize() {
    /** TODO: */
}

template<typename T>
void CfdContinuous<T>::simulate() {
    /** TODO: */
}

template<typename T>
void CfdContinuous<T>::saveState() {
    std::unordered_map<int, T> savePressures;
    std::unordered_map<int, T> saveFlowRates;
    std::unordered_map<int, std::string> vtkFiles;

    // vtk Files
    for (auto& [id, simulator] : this->cfdSimulators) {
        vtkFiles.try_emplace(simulator->getId(), simulator->getVtkFile());
    }

    // state
    this->getSimulationResults()->addState(this->getTime(), vtkFiles);
}

template<typename T>
void CfdContinuous<T>::generateSTL() {
    // Generate the STL definition from the stl network definition object (network_stl).
    stlNetwork = std::make_unique<stl::NetworkSTL>(network_stl, radialResolution);
}

template<typename T>
void CfdContinuous<T>::updateNetworkSTL() {
    // Update the set of dangling nodes
    danglingNodes = this->getNetwork->getDanglingNodes();

    // Add definitions for the nodes
    for (auto& [k, n] : this->getNetwork->getNodes()) {
        if(danglingNodes.find(n) = danglingNodes.end()) {
            network_stl->addNode(double(n->getPosition().at(0)), double(n->getPosition().at(1)), 0.0, false);
        } else {
            network_stl->addNode(double(n->getPosition().at(0)), double(n->getPosition().at(1)), 0.0, true);
        }
    }

    // Add definitions for the channels
    for (auto& [k, c] : this->getNetwork->getChannels()) {
        if (c->isRectangular()) {
            arch::RectangularChannel<T>* tmpChannel = dynamic_cast<arch::RectangularChannel<T>*>(c.get());
            network_stl->addChannel(c->getNodeAId(), c->getNodeBId(), double(tmpChannel->getWidth()), double(tmpChannel->getHeight()));
        } else {
            throw std::logic_error("STL generation is not supported for cylindrical channels.");
        }
    }
}

template<typename T>
void CfdContinuous<T>::updateSTL() {
    stlNetwork->writeSTL(fName);
}
    

}   /// namespace sim
