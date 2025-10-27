#include "CfdContinuous.h"

namespace sim {

template<typename T>
CfdContinuous<T>::CfdContinuous(std::shared_ptr<arch::Network<T>> network, int radialResolution) : CfdContinuous<T>(Platform::Continuous, network, radialResolution) { }

template<typename T>
CfdContinuous<T>::CfdContinuous(Platform platform, std::shared_ptr<arch::Network<T>> network, int radialResolution) : Simulation<T>(Type::CFD, platform, network), radialResolution(radialResolution)
{
    // A fitting stl network definition must be creates from the given network.
    network_stl = std::make_shared<stl::Network>();
    // Initial definition for network_stl and stlNetwork, fill dangling nodes
    std::cout<<"Generating STL definition from network..."<<std::endl;
    updateNetworkSTL();
    std::cout<<"Generating STL shape from network..."<<std::endl;
    generateSTL();
    std::cout<<"Storing STL shape to file..."<<std::endl;
    // Create a local tmp folder if it doesn't exist yet
    std::filesystem::path tmp = "./tmp";
    if (!std::filesystem::exists(tmp)) {
        std::filesystem::create_directories(tmp);
    }
    std::cout<<"Storing STL shape to file..."<<std::endl;
    // Create a temporary file for the STL
    fName = "./tmp/networkSTL-" + std::to_string(this->getHash());
    std::string stlLocation = fName + ".stl";
    updateSTL();
    std::cout<<"STL definition generated."<<std::endl;
    // Define the cfdModule
    cfdModule = std::shared_ptr<arch::CfdModule<T>>(new arch::CfdModule<T>(0, getPosition(), getSize(), stlLocation, getOpenings()));
    // Fill idle nodes
    std::cout<<"Updating idle nodes..."<<std::endl;
    updateIdleNodes();
    // Define the simulator (lbmSimulator)
    std::cout<<"Defining LBM simulator..."<<std::endl;
    std::string name = "lbmContinuous";
    simulator = std::shared_ptr<lbmSimulator<T>>(new lbmSimulator<T>(0, name, cfdModule, resolution, charPhysLength, charPhysVelocity, epsilon, relaxationTime));
}

template<typename T>
std::vector<T> CfdContinuous<T>::getPosition() const {
    std::cout<<"Getting position from STL network..."<<std::endl;
    // Calculate position from bounding box
    std::vector<T> position = { std::get<0>(stlNetwork->getBoundingBox())[0],
                                std::get<0>(stlNetwork->getBoundingBox())[1] };
    return position;
}

template<typename T>
std::vector<T> CfdContinuous<T>::getSize() const {
    std::cout<<"Getting size from STL network..."<<std::endl;
    // Calculate size from bounding box
    std::vector<T> size = { std::get<1>(stlNetwork->getBoundingBox())[0] - std::get<0>(stlNetwork->getBoundingBox())[0],
                            std::get<1>(stlNetwork->getBoundingBox())[1] - std::get<0>(stlNetwork->getBoundingBox())[1] };
    return size;
}

template<typename T>
std::unordered_map<size_t, arch::Opening<T>> CfdContinuous<T>::getOpenings() const {
    std::cout<<"Getting openings for Module..."<<std::endl;
    std::unordered_map<size_t, arch::Opening<T>> Openings;
    for (auto& nodePtr : danglingNodes) {
        std::cout<<"Processing opening at node "<<nodePtr->getId()<<"."<<std::endl;
        // Get the normal vector of the node opening from the network
        std::vector<T> normal = stlNetwork->getNodeNormalVector(nodePtr->getId());
        // Get the channel width from the network
        T channelWidth = stlNetwork->getChannelWidthAtNode(nodePtr->getId());
        // Create the opening and add it to the map
        Openings.try_emplace(nodePtr->getId(), arch::Opening<T>(nodePtr, normal, channelWidth));
    }
    return Openings;
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
                                : Simulation<T>(Type::CFD, platform, nullptr), fName(stlFile)
{
    // Define the CFD module
    cfdModule = std::shared_ptr<arch::CfdModule<T>>(new arch::CfdModule<T>(position, size, stlFile, openings));
    // Fill dangling nodes and idle nodes
    updateIdleNodes(openings);
    // Define the simulator (lbmSimulator)
    std::string name = "lbmContinuous";
    simulator = std::shared_ptr<lbmSimulator<T>>(new lbmSimulator<T>(0, name, cfdModule, resolution, charPhysLength, charPhysVelocity, epsilon, relaxationTime));
}

template<typename T>
void CfdContinuous<T>::setNetwork(std::shared_ptr<arch::Network<T>> network) {
    // Store the network in the simulation object
    Simulation<T>::setNetwork(network);

    // Update the STL definitions for the new network
    updateNetworkSTL();
    generateSTL();
    updateSTL();
}

template<typename T>
void CfdContinuous<T>::addPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure) {
    // Check that the node is a dangling node with no BC yet
    if (danglingNodes.find(node) == danglingNodes.end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because it is not a boundary node.");
    }
    auto it = idleNodes.find(node->getId());
    if (it == idleNodes.end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because a BC is already set.");
    }
    // Add the BC
    pressureBCs.try_emplace(node->getId(), pressure);
    // Remove the node from the idle nodes
    idleNodes.erase(it);
}

template<typename T>
void CfdContinuous<T>::addVelocityBC(std::shared_ptr<arch::Node<T>> node, T velocity) {
    // Check that the node is a dangling node with no BC yet
    if (danglingNodes.find(node) == danglingNodes.end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because it is not a boundary node.");
    }
    auto it = idleNodes.find(node->getId());
    if (it == idleNodes.end()) {
        throw std::logic_error("Cannot add a boundary condition at node " + std::to_string(node->getId()) + " because a BC is already set.");
    }
    // Add the BC
    velocityBCs.try_emplace(node->getId(), velocity);
    // Remove the node from the idle nodes
    idleNodes.erase(it);
}

template<typename T>
void CfdContinuous<T>::setPressureBC(std::shared_ptr<arch::Node<T>> node, T pressure) {
    // Check that the node is a pressure BC
    auto it = pressureBCs.find(node->getId());
    if (it == pressureBCs.end()) {
        throw std::logic_error("Cannot set pressure value at node " + std::to_string(node->getId()) + " because it is not a pressure BC.");
    }
    it->second = pressure;
}

template<typename T>
void CfdContinuous<T>::setVelocityBC(std::shared_ptr<arch::Node<T>> node, T velocity) {
    // Check that the node is a velocity BC
    auto it = velocityBCs.find(node->getId());
    if (it == velocityBCs.end()) {
        throw std::logic_error("Cannot set velocity value at node " + std::to_string(node->getId()) + " because it is not a velocity BC.");
    }
    it->second = velocity;
}

template<typename T>
void CfdContinuous<T>::removePressureBC(std::shared_ptr<arch::Node<T>> node) {
    // Check that the node is a pressure BC
    auto it = pressureBCs.find(node->getId());
    if (it == pressureBCs.end()) {
        throw std::logic_error("Cannot remove pressure BC at node " + std::to_string(node->getId()) + " because it is not a pressure BC.");
    }
    pressureBCs.erase(it);
    // Add the node to the idle nodes
    idleNodes.insert(node->getId());
}

template<typename T>
void CfdContinuous<T>::removeVelocityBC(std::shared_ptr<arch::Node<T>> node) {
    // Check that the node is a velocity BC
    auto it = velocityBCs.find(node->getId());
    if (it == velocityBCs.end()) {
        throw std::logic_error("Cannot remove velocity BC at node " + std::to_string(node->getId()) + " because it is not a velocity BC.");
    }
    velocityBCs.erase(it);
    // Add the node to the idle nodes
    idleNodes.insert(node->getId());
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
    std::tuple<T,T> bounds = simulator->getPressureBounds();
    return std::pair<T,T> {std::get<0>(bounds), std::get<1>(bounds)};
}

template<typename T>
std::pair<T,T> CfdContinuous<T>::getGlobalVelocityBounds() const {
    std::tuple<T,T> bounds = simulator->getVelocityBounds();
    return std::pair<T,T> {std::get<0>(bounds), std::get<1>(bounds)};
}


template<typename T>
void CfdContinuous<T>::writePressurePpm(std::pair<T,T> bounds, int resolution) const {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    simulator->writePressurePpm(0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdContinuous<T>::writeVelocityPpm(std::pair<T,T> bounds, int resolution) const {
    // 0.98 and 1.02 factors are there to account for artifical black pixels that might show
    simulator->writeVelocityPpm(0.98*bounds.first, 1.02*bounds.second, resolution);
}

template<typename T>
void CfdContinuous<T>::assertInitialized() const {
    // First, assert that the base simulation is initialized
    if (this->getNetwork() == nullptr) {
        throw std::logic_error("Simulation not initialized: Network is not set.");
    }
    // Then, check that there are no idle nodes
    if (idleNodes.size() > 0) {
        throw std::logic_error("CFD simulation is not initialized: there are " + std::to_string(idleNodes.size()) + " idle nodes without BC defined.");
    }
    // Finally, check that all dangling nodes have a BC defined
    if (danglingNodes.size() != (pressureBCs.size() + velocityBCs.size())) {
        throw std::logic_error("CFD simulation is not initialized: not all dangling nodes have a BC defined.");
    }
    this->getNetwork()->isNetworkValid();
}

template<typename T>
void CfdContinuous<T>::initialize() {
    // Initialize the simulator
    simulator->lbmInit(this->getContinuousPhase()->getViscosity(), this->getContinuousPhase()->getDensity());
    std::cout<<"Preparing geometry..."<<std::endl;
    simulator->prepareGeometry();
    std::cout<<"Preparing lattice..."<<std::endl;
    simulator->prepareLattice();
    std::cout<<"Checking initialization..."<<std::endl;
    simulator->checkInitialized();
}

template<typename T>
void CfdContinuous<T>::simulate() {
    this->assertInitialized();
    this->initialize();

    bool isConverged = false;

    while (!isConverged) {
        // Solve the CFD domain
        simulator->solve();
        if (simulator->hasConverged()) {
            isConverged = true;
        }
    }

    if (writePpm) {
        writePressurePpm(getGlobalPressureBounds());
        writeVelocityPpm(getGlobalVelocityBounds());
    }

    saveState();
}

template<typename T>
void CfdContinuous<T>::saveState() {
    std::unordered_map<int, T> savePressures;
    std::unordered_map<int, T> saveFlowRates;
    std::unordered_map<int, std::string> vtkFiles;

    // vtk File
    vtkFiles.try_emplace(simulator->getId(), simulator->getVtkFile());

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
    danglingNodes = this->getNetwork()->getDanglingNodes();

    // Add definitions for the nodes
    for (int nodeId = 0; nodeId < this->getNetwork()->getNodes().size(); ++nodeId) {
        auto it = std::find_if(danglingNodes.begin(), danglingNodes.end(),
                               [&](const std::shared_ptr<arch::Node<T>>& nodePtr) { return nodePtr->getId() == nodeId; });
        bool isDangling = (it != danglingNodes.end());
        if(!isDangling) {
            network_stl->addNode(double(this->getNetwork()->getNodes().at(nodeId)->getPosition().at(0)), double(this->getNetwork()->getNodes().at(nodeId)->getPosition().at(1)), 0.0, false);
            std::cout << "Added node " << this->getNetwork()->getNodes().at(nodeId)->getId() << " as non-dangling." << std::endl;
        } else {
            network_stl->addNode(double(this->getNetwork()->getNodes().at(nodeId)->getPosition().at(0)), double(this->getNetwork()->getNodes().at(nodeId)->getPosition().at(1)), 0.0, true);
            std::cout << "Added node " << this->getNetwork()->getNodes().at(nodeId)->getId() << " as dangling." << std::endl;
        }
    }
    for (auto& [k, n] : this->getNetwork()->getNodes()) {
        auto it = std::find_if(danglingNodes.begin(), danglingNodes.end(),
                               [&](const std::shared_ptr<arch::Node<T>>& nodePtr) { return nodePtr->getId() == n->getId(); });
        bool isDangling = (it != danglingNodes.end());
        if(!isDangling) {
            network_stl->addNode(int(k), double(n->getPosition().at(0)), double(n->getPosition().at(1)), 0.0, false);
            std::cout << "Added node " << n->getId() << " as non-dangling." << std::endl;
        } else {
            network_stl->addNode(int(k), double(n->getPosition().at(0)), double(n->getPosition().at(1)), 0.0, true);
            std::cout << "Added node " << n->getId() << " as dangling." << std::endl;
        }
    }

    // Add definitions for the channels
    for (auto& [k, c] : this->getNetwork()->getChannels()) {
        if (c->isRectangular()) {
            arch::RectangularChannel<T>* tmpChannel = dynamic_cast<arch::RectangularChannel<T>*>(c.get());
            auto addedChannel = network_stl->addChannel(int(k), c->getNodeAId(), c->getNodeBId(), double(tmpChannel->getWidth()), double(tmpChannel->getHeight()));
            std::cout<< "Added rectangular channel " << addedChannel->getId() << " between nodes " << addedChannel->getNodeA()->getId() << " and " << addedChannel->getNodeB()->getId() << "." << std::endl;
            std::cout<<" Which should have been " << tmpChannel->getId() << " between nodes " << tmpChannel->getNodeAId() << " and " << tmpChannel->getNodeBId() << "."<<std::endl;
        } else {
            throw std::logic_error("STL generation is not supported for cylindrical channels.");
        }
    }
}

template<typename T>
void CfdContinuous<T>::updateSTL() {
    stlNetwork->writeSTL(fName);
}
    
template<typename T>
void CfdContinuous<T>::updateIdleNodes() {
    idleNodes.clear();
    for (auto& nodePtr : danglingNodes) {
        idleNodes.insert(nodePtr->getId());
    }
}

template<typename T>
void CfdContinuous<T>::updateIdleNodes(const std::unordered_map<size_t, arch::Opening<T>>& openings) {
    idleNodes.clear();
    for (auto& [nodeId, opening] : openings) {
        danglingNodes.insert(opening.node);
        idleNodes.insert(nodeId);
    }
}

}   /// namespace sim
