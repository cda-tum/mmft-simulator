#include "Simulation.h"

namespace sim {

    template<typename T>
    Simulation<T>::Simulation() {
        this->simulationResult = std::make_unique<result::SimulationResult<T>>();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::addFluid(T viscosity, T density, T concentration) {
        auto id = fluids.size();

        auto result = fluids.insert_or_assign(id, std::make_unique<Fluid<T>>(id, density, viscosity, concentration));

        return result.first->second.get();
    }

    template<typename T>
    Droplet<T>* Simulation<T>::addDroplet(int fluidId, T volume) {
        auto id = droplets.size();
        auto fluid = fluids.at(fluidId).get();

        auto result = droplets.insert_or_assign(id, std::make_unique<Droplet<T>>(id, volume, fluid));

        return result.first->second.get();
    }

    template<typename T>
    DropletInjection<T>* Simulation<T>::addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition) {
        auto id = dropletInjections.size();
        auto droplet = droplets.at(dropletId).get();
        auto channel = network->getChannel(channelId);

        // --- check if injection is valid ---
        // for the injection the head and tail of the droplet must lie inside the channel (the volume of the droplet must be small enough)
        // the droplet length is a relative value between 0 and 1
        T dropletLength = droplet->getVolume() / channel->getVolume();
        // channel must be able to fully contain the droplet
        if (dropletLength >= 1.0) {
            throw std::invalid_argument("Injection of droplet " + droplet->getName() + " into channel " + std::to_string(channel->getId()) + " is not valid. Channel must be able to fully contain the droplet.");
        }
        
        // compute tail and head position of the droplet
        T tail = (injectionPosition - dropletLength / 2);
        T head = (injectionPosition + dropletLength / 2);
        // tail and head must not be outside the channel (can happen when the droplet is injected at the beginning or end of the channel)
        if (tail < 0 || head > 1.0) {
            throw std::invalid_argument("Injection of droplet " + droplet->getName() + " is not valid. Tail and head of the droplet must lie inside the channel " + std::to_string(channel->getId()) + ". Consider to set the injection position in the middle of the channel.");
        }

        auto result = dropletInjections.insert_or_assign(id, std::make_unique<DropletInjection<T>>(id, droplet, injectionTime, channel, injectionPosition));
        return result.first->second.get();
    }

    template<typename T>
    void Simulation<T>::setPlatform(Platform platform_) {
        this->platform = platform_;
    }

    template<typename T>
    void Simulation<T>::setType(Type type_) {
        this->simType = type_;
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
    void Simulation<T>::setDroplets(std::unordered_map<int, std::unique_ptr<Droplet<T>>> droplets_) {
        this->droplets = std::move(droplets_);
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
    Platform Simulation<T>::getPlatform() {
        return this->platform;
    }

    template<typename T>
    Type Simulation<T>::getType() {
        return this->simType;
    }

    template<typename T>
    int Simulation<T>::getFixtureId() {
        return this->fixtureId;
    }

    template<typename T>
    arch::Network<T>* Simulation<T>::getNetwork() {
        return this->network;
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getFluid(int fluidId) {
        return fluids.at(fluidId).get();
    }

    template<typename T>
    std::unordered_map<int, std::unique_ptr<Fluid<T>>>& Simulation<T>::getFluids() {
        return fluids;
    }

    template<typename T>
    Droplet<T>* Simulation<T>::getDroplet(int dropletId) {
        return droplets.at(dropletId).get();
    }

    template<typename T>
    Droplet<T>* Simulation<T>::getDropletAtNode(int nodeId) {
        // loop through all droplets
        for (auto& [id, droplet] : droplets) {
            // do not consider droplets which are not inside the network
            if (droplet->getDropletState() != DropletState::NETWORK) {
                continue;
            }

            // do not consider droplets inside a single channel (because they cannot span over a node)
            if (droplet->isInsideSingleChannel()) {
                continue;
            }

            // check if a boundary is connected with the reference node and if yes then return the droplet immediately
            auto connectedBoundaries = droplet->getConnectedBoundaries(nodeId);
            if (connectedBoundaries.size() != 0) {
                return droplet.get();
            }

            // check if a fully occupied channel is connected with the reference node and if yes then return the droplet immediately
            auto connectedFullyOccupiedChannels = droplet->getConnectedFullyOccupiedChannels(nodeId);
            if (connectedFullyOccupiedChannels.size() != 0) {
                return droplet.get();
            }
        }

        // if nothing was found than return nullptr
        return nullptr;
    }

    template<typename T>
    DropletInjection<T>* Simulation<T>::getDropletInjection(int injectionId) {
        return dropletInjections.at(injectionId).get();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::getContinuousPhase() {
        return fluids[continuousPhase].get();
    }

    template<typename T>
    result::SimulationResult<T>* Simulation<T>::getSimulationResults() {
        return simulationResult.get();
    }

    template<typename T>
    Fluid<T>* Simulation<T>::mixFluids(int fluid0Id, T volume0, int fluid1Id, T volume1) {
        // check if fluids are identically (no merging needed) and if they exist
        if (fluid0Id == fluid1Id) {
            // try to get the fluid (throws error if the fluid is not present)
            return fluids.at(fluid0Id).get();
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
        T concentration = ratio0 * fluid0->getConcentration() + ratio1 * fluid1->getConcentration();

        // add new fluid
        auto newFluid = addFluid(viscosity, density, concentration);

        //add previous fluids
        newFluid->addMixedFluid(fluid0);
        newFluid->addMixedFluid(fluid1);

        return newFluid;
    }

    template<typename T>
    Droplet<T>* Simulation<T>::mergeDroplets(int droplet0Id, int droplet1Id) {
        // check if droplets are identically (no merging needed) and if they exist
        if (droplet0Id == droplet1Id) {
            // try to get the droplet (throws error if the droplet is not present)
            return droplets.at(droplet0Id).get();
        }

        // get droplets
        auto droplet0 = getDroplet(droplet0Id);
        auto droplet1 = getDroplet(droplet1Id);

        // compute volumes
        T volume0 = droplet0->getVolume();
        T volume1 = droplet1->getVolume();
        T volume = volume0 + volume1;

        // merge fluids
        auto fluid = mixFluids(droplet0->getFluid()->getId(), volume0, droplet1->getFluid()->getId(), volume1);

        // add new droplet
        auto newDroplet = addDroplet(fluid->getId(), volume);

        //add previous droplets
        newDroplet->addMergedDroplet(droplet0);
        newDroplet->addMergedDroplet(droplet1);

        return newDroplet;
    }

    template<typename T>
    void Simulation<T>::simulate() {
        #define VERBOSE
        // initialize the simulation
        initialize();
        //printResults();

        // 1D continuous simulation
        // ##########
        // * conduct nodal analysis
        // * save state
        if (simType == Type::Abstract && platform == Platform::Continuous) {
            // compute nodal analysis
            nodal::conductNodalAnalysis(network);

            // store simulation results of current state
            saveState();
        }

        // Continuous Hybrid simulation
        if (this->simType == Type::Hybrid && this->platform == Platform::Continuous) {
            if (network->getModules().size() > 0 ) {
                bool allConverged = false;
                bool pressureConverged = false;

                // Initialization of CFD domains
                while (! allConverged) {
                    allConverged = conductCFDSimulation(this->network, 1);
                }

                while (! allConverged || !pressureConverged) {
                    //std::cout << "######################## Simulation Iteration no. " << iter << " ####################" << std::endl;

                    // conduct CFD simulations
                    //std::cout << "[Simulation] Conduct CFD simulation " << iter <<"..." << std::endl;
                    allConverged = conductCFDSimulation(this->network, 10);
                
                    // compute nodal analysis again
                    //std::cout << "[Simulation] Conduct nodal analysis " << iter <<"..." << std::endl;
                    pressureConverged = nodal::conductNodalAnalysis(this->network);

                }

                #ifdef VERBOSE     
                    if (pressureConverged && allConverged) {
                        std::cout << "[Simulation] All pressures have converged." << std::endl;
                    } 
                    printResults();
                #endif
            }
            saveState();
        }

        // 1D Droplet simulation
        // ##########
        // Simulation Loop
        // ##########
        // * update droplet resistances
        // * conduct nodal analysis
        // * update droplets (flow rates of boundaries)
        // * compute events
        // * search for next event (break if no event is left)
        // * move droplets
        // * perform event
        if (simType == Type::Abstract && platform == Platform::BigDroplet) {

            while (true) {
                if (iteration >= maxIterations) {
                    throw "Max iterations exceeded.";
                }

                #ifdef VERBOSE     
                    std::cout << "Iteration " << iteration << std::endl;
                #endif
                // update droplet resistances (in the first iteration no  droplets are inside the network)
                updateDropletResistances();
                // compute nodal analysis
                nodal::conductNodalAnalysis(network);
                // update droplets, i.e., their boundary flow rates
                // loop over all droplets
                dropletsAtBifurcation = false;
                for (auto& [key, droplet] : droplets) {
                    // only consider droplets inside the network
                    if (droplet->getDropletState() != DropletState::NETWORK) {
                        continue;
                    }

                    // set to true if droplet is at bifurcation
                    if (droplet->isAtBifurcation()) {
                        dropletsAtBifurcation = true;
                    }

                    // compute the average flow rates of all boundaries, since the inflow does not necessarily have to match the outflow (qInput != qOutput)
                    // in order to avoid an unwanted increase/decrease of the droplet volume an average flow rate is computed
                    // the actual flow rate of a boundary is then determined accordingly to the ratios of the different flowRates inside the channels
                    droplet->updateBoundaries(*network);
                }
                // store simulation results of current state
                saveState();
                // compute events
                auto events = computeEvents();

                // sort events
                // closest events in time with the highest priority come first
                std::sort(events.begin(), events.end(), [](auto& a, auto& b) {
                    if (a->getTime() == b->getTime()) {
                        return a->getPriority() < b->getPriority();  // ascending order (the lower the priority value, the higher the priority)
                    }
                    return a->getTime() < b->getTime();  // ascending order
                });

                #ifdef VERBOSE     
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
                // move droplets until event is reached
                time += nextEvent->getTime();
                moveDroplets(nextEvent->getTime());

                nextEvent->performEvent();

                iteration++;
            }
        }
    }

    template<typename T>
    void Simulation<T>::printResults() {
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
        /*
        std::cout << "\n";
        // print the resistances in all channels
        for (auto& [key, channel] : network->getChannels()) {
            std::cout << "[Result] Channel " << channel->getId() << " has a resistance of " << channel->getResistance() << " Pas/L.\n";
        }*/
        std::cout << std::endl;
    }

    template<typename T>
    void Simulation<T>::initialize() {
        // compute and set channel lengths
        #ifdef VERBOSE
            std::cout << "[Simulation] Compute and set channel lengths..." << std::endl;
        #endif
        for (auto& [key, channel] : network->getChannels()) {
            auto& nodeA = network->getNodes().at(channel->getNodeA());
            auto& nodeB = network->getNodes().at(channel->getNodeB());
            T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
            T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
            channel->setLength(sqrt(dx*dx + dy*dy));
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

        if (this->simType == Type::Hybrid && this->platform == Platform::Continuous) {
            
            for (auto& [key, module] : network->getModules()) {
                module->lbmInit(fluids[continuousPhase]->getViscosity(),
                                fluids[continuousPhase]->getDensity());
            }

            // This is boilerplate code, and can be done way more efficiently in a recursive manner
            for (auto& [modulekey, module] : network->getModules()) {
                for (auto& [key, channel] : module->getNetwork()->getChannels()) {
                    //std::cout << "[Simulation] Channel " << channel->getId();
                    auto& nodeA = network->getNodes().at(channel->getNodeA());
                    auto& nodeB = network->getNodes().at(channel->getNodeB());
                    //std::cout << " has nodes " << nodeA->getId() << " and " << nodeB->getId();
                    T dx = nodeA->getPosition().at(0) - nodeB->getPosition().at(0);
                    T dy = nodeA->getPosition().at(1) - nodeB->getPosition().at(1);
                    channel->setLength(sqrt(dx*dx + dy*dy));
                    //std::cout << " and a length of " << sqrt(dx*dx + dy*dy) <<std::endl;
                }
            }
            // Also boilerplate code that can be done more efficiently
            for (auto& [modulekey, module] : network->getModules()) {
                for (auto& [key, channel] : module->getNetwork()->getChannels()) {
                    T resistance = resistanceModel->getChannelResistance(channel.get());
                    channel->setResistance(resistance);
                }
            }

            // compute nodal analysis
            #ifdef VERBOSE
                std::cout << "[Simulation] Conduct initial nodal analysis..." << std::endl;
            #endif
            nodal::conductNodalAnalysis(this->network);

            // Prepare CFD geometry and lattice
            #ifdef VERBOSE
                std::cout << "[Simulation] Prepare CFD geometry and lattice..." << std::endl;
            #endif

            for (auto& [key, module] : network->getModules()) {
                module->prepareGeometry();
                module->prepareLattice();
            }
        }
    }

    template<typename T>
    void Simulation<T>::updateDropletResistances() {
        // set all droplet resistances of all channels to 0.0
        for (auto& [key, channel] : network->getChannels()) {
            channel->setDropletResistance(0.0);
        }

        // set correct droplet resistances
        for (auto& [key, droplet] : droplets) {
            // only consider droplets that are inside the network (i.e., also trapped droplets)
            if (droplet->getDropletState() == DropletState::INJECTION || droplet->getDropletState() == DropletState::SINK) {
                continue;
            }

            droplet->addDropletResistance(*resistanceModel);
        }
    }

    template<typename T>
    void Simulation<T>::saveState() {

        std::unordered_map<int, T> savePressures;
        std::unordered_map<int, T> saveFlowRates;
        std::unordered_map<int, DropletPosition<T>> saveDropletPositions;

        // pressures
        for (auto& [id, node] : network->getNodes()) {
            savePressures.try_emplace(node->getId(), node->getPressure());
        }

        // flow rates
        for (auto& [id, channel] : network->getChannels()) {
            saveFlowRates.try_emplace(channel->getId(), channel->getFlowRate());
        }
        for (auto& [id, pump] : network->getFlowRatePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }
        for (auto& [id, pump] : network->getPressurePumps()) {
            saveFlowRates.try_emplace(pump->getId(), pump->getFlowRate());
        }

        // droplet positions
        for (auto& [id, droplet] : droplets) {
            // create new droplet position
            DropletPosition<T> newDropletPosition;

            // add boundaries
            for (auto& boundary : droplet->getBoundaries()) {
                // get channel position
                auto channelPosition = boundary->getChannelPosition();
                // add boundary
                newDropletPosition.boundaries.emplace_back(channelPosition.getChannel(), channelPosition.getPosition(), boundary->isVolumeTowardsNodeA(), static_cast<BoundaryState>(static_cast<int>(boundary->getState())));
            }

            // add fully occupied channels
            for (auto& channel : droplet->getFullyOccupiedChannels()) {
                // add channel
                newDropletPosition.channelIds.emplace_back(channel->getId());
            }

            saveDropletPositions.try_emplace(droplet->getId(), newDropletPosition);
        }

        // state
        simulationResult->addState(time, savePressures, saveFlowRates, saveDropletPositions);
    }

    template<typename T>
    void Simulation<T>::moveDroplets(T timeStep) {
        // loop over all droplets
        for (auto& [key, droplet] : droplets) {
            // only consider droplets inside the network (but no trapped droplets)
            if (droplet->getDropletState() != DropletState::NETWORK) {
                continue;
            }

            // loop through boundaries
            for (auto& boundary : droplet->getBoundaries()) {
                // move boundary in correct direction
                boundary->moveBoundary(timeStep);
            }
        }
    }

    template<typename T>
    std::vector<std::unique_ptr<Event<T>>> Simulation<T>::computeEvents() {
        // events
        std::vector<std::unique_ptr<Event<T>>> events;

        // injection events
        for (auto& [key, injection] : dropletInjections) {
            double injectionTime = injection->getInjectionTime();
            if (injection->getDroplet()->getDropletState() == DropletState::INJECTION) {
                events.push_back(std::make_unique<DropletInjectionEvent<T>>(injectionTime - time, *injection));
            }
        }

        // define maps that are used for detecting merging inside channels
        std::unordered_map<int, std::vector<DropletBoundary<T>*>> channelBoundariesMap;
        std::unordered_map<DropletBoundary<T>*, Droplet<T>*> boundaryDropletMap;

        for (auto& [key, droplet] : droplets) {
            // only consider droplets inside the network (but no trapped droplets)
            if (droplet->getDropletState() != DropletState::NETWORK) {
                continue;
            }

            // loop through boundaries
            for (auto& boundary : droplet->getBoundaries()) {
                // the flow rate of the boundary indicates if the boundary moves towards or away from the droplet center and, hence, if a BoundaryTailEvent or BoundaryHeadEvent should occur, respectively
                // if the flow rate of the boundary is 0, then no events will be triggered (the boundary may be in a Wait state)
                if (boundary->getFlowRate() < 0) {
                    // boundary moves towards the droplet center => BoundaryTailEvent
                    double time = boundary->getTime();
                    events.push_back(std::make_unique<BoundaryTailEvent<T>>(time, *droplet, *boundary, *network));
                } else if (boundary->getFlowRate() > 0) {
                    // boundary moves away from the droplet center => BoundaryHeadEvent
                    double time = boundary->getTime();

                    // in this scenario also a MergeBifurcationEvent can happen when merging is enabled
                    // this means a boundary comes to a bifurcation where a droplet is already present
                    // hence it is either a MergeBifurcationEvent or a BoundaryHeadEvent that will happen

                    // check if merging is enabled
                    Droplet<T>* mergeDroplet = nullptr;

                    // find droplet to merge (if present)
                    auto referenceNode = boundary->getOppositeReferenceNode(network);
                    mergeDroplet = getDropletAtNode(referenceNode->getId());

                    if (mergeDroplet == nullptr) {
                        // no merging will happen => BoundaryHeadEvent
                        if (!boundary->isInWaitState()) {
                            events.push_back(std::make_unique<BoundaryHeadEvent<T>>(time, *droplet, *boundary, *network));
                        }
                    } else {
                        // merging of the actual droplet with the merge droplet will happen => MergeBifurcationEvent
                        events.push_back(std::make_unique<MergeBifurcationEvent<T>>(time, *droplet, *mergeDroplet, *boundary, *this));
                    }
                }

                // fill the maps which are later used for merging inside channels (if merging is enabled)
                auto [value, success] = channelBoundariesMap.try_emplace(boundary->getChannelPosition().getChannel()->getId());
                value->second.push_back(boundary.get());

                boundaryDropletMap.emplace(boundary.get(), droplet.get());
            }
        }

        // check for MergeChannelEvents, i.e, for boundaries of other droplets that are in the same channel
        // here the previously defined maps are used => if merging is not enabled these maps are emtpy
        // loop through channelsBoundariesMap
        for (auto& [channelId, boundaries] : channelBoundariesMap) {
            // loop through boundaries that are inside this channel
            for (size_t i = 0; i < boundaries.size(); i++) {
                // get reference boundary and droplet
                auto referenceBoundary = boundaries[i];
                auto referenceDroplet = boundaryDropletMap.at(referenceBoundary);

                // get channel
                auto channel = referenceBoundary->getChannelPosition().getChannel();

                // get velocity and absolute position of the boundary
                // positive values for v0 indicate a movement from node0 towards node1
                auto q0 = referenceBoundary->isVolumeTowardsNodeA() ? referenceBoundary->getFlowRate() : -referenceBoundary->getFlowRate();
                auto v0 = q0 / channel->getArea();
                auto p0 = referenceBoundary->getChannelPosition().getPosition() * channel->getLength();

                // compare reference boundary against all others
                // the two loops are defined in such a way, that only one merge event happens for a pair of boundaries
                for (size_t j = i + 1; j < boundaries.size(); j++) {
                    auto boundary = boundaries[j];
                    auto droplet = boundaryDropletMap.at(boundary);

                    // do not consider if this boundary is form the same droplet
                    if (droplet == referenceDroplet) {
                        continue;
                    }

                    // get velocity and absolute position of the boundary
                    // positive values for v0 indicate a movement from node0 towards node1
                    auto q1 = boundary->isVolumeTowardsNodeA() ? boundary->getFlowRate() : -boundary->getFlowRate();
                    auto v1 = q1 / channel->getArea();
                    auto p1 = boundary->getChannelPosition().getPosition() * channel->getLength();

                    // do not merge when both velocities are equal (would result in infinity time)
                    if (v0 == v1) {
                        continue;
                    }

                    // compute time and merge position
                    auto time = (p1 - p0) / (v0 - v1);
                    auto pMerge = p0 + v0 * time;  // or p1 + v1*time
                    auto pMergeRelative = pMerge / channel->getLength();

                    // do not trigger a merge event when:
                    // * time is negative => indicates that both boundaries go in different directions or that one boundary cannot "outrun" the other because it is too slow
                    // * relative merge position is outside the range of [0, 1] => the merging would happen "outside" the channel and a boundary would already switch a channel before this event could happen
                    if (time < 0 || pMergeRelative < 0 || 1 < pMergeRelative) {
                        continue;
                    }

                    // add MergeChannelEvent
                    events.push_back(std::make_unique<MergeChannelEvent<T>>(time, *referenceDroplet, *droplet, *referenceBoundary, *boundary, *this));
                }
            }
        }

        // time step event
        if (dropletsAtBifurcation && maximalAdaptiveTimeStep > 0) {
            events.push_back(std::make_unique<TimeStepEvent<T>>(maximalAdaptiveTimeStep));
        }

        return events;
    }

}   /// namespace sim
