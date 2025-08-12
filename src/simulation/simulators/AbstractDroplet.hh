#include "AbstractDroplet.h"

namespace sim {

    template<typename T>
    AbstractDroplet<T>::AbstractDroplet(std::shared_ptr<arch::Network<T>> network) : Simulation<T>(Type::Abstract, Platform::BigDroplet, network) { }

    template<typename T>
    std::shared_ptr<Droplet<T>> AbstractDroplet<T>::addDroplet(int fluidId, T volume) {
        auto id = DropletImplementation<T>::getDropletCounter();
        auto fluid = this->getFluids().at(fluidId).get();

        auto result = droplets.insert_or_assign(id, std::shared_ptr<DropletImplementation<T>>(new DropletImplementation<T>(id, volume, fluid)));

        return result.first->second;
    }

    template<typename T>
    std::shared_ptr<Droplet<T>> AbstractDroplet<T>::addDroplet(const std::shared_ptr<Fluid<T>>& fluid, T volume) {
        return addDroplet(fluid->getId(), volume);
    }

    template<typename T>
    std::shared_ptr<DropletImplementation<T>> AbstractDroplet<T>::addMergedDropletImplementation(const DropletImplementation<T>& droplet0, const DropletImplementation<T>& droplet1) {
        // compute volumes
        T volume0 = droplet0.getVolume();
        T volume1 = droplet1.getVolume();
        T volume = volume0 + volume1;

        // merge fluids
        auto fluid = this->addMixedFluid(droplet0.getFluid()->getId(), volume0, droplet1.getFluid()->getId(), volume1);

        // add new droplet
        auto newDroplet = addDroplet(fluid->getId(), volume);

        return droplets.at(newDroplet->getId()); 
    }

    template<typename T>
    std::shared_ptr<Droplet<T>> AbstractDroplet<T>::addMergedDroplet(int droplet0Id, int droplet1Id) {
        // check if droplets are identically (no merging needed) and if they exist
        if (droplet0Id == droplet1Id) {
            // try to get the droplet (throws error if the droplet is not present)
            return droplets.at(droplet0Id);
        }

        // get droplets
        auto droplet0 = droplets.at(droplet0Id);
        auto droplet1 = droplets.at(droplet1Id);

        return addMergedDropletImplementation(*droplet0, *droplet1);
    }

    template<typename T>
    std::shared_ptr<Droplet<T>> AbstractDroplet<T>::addMergedDroplet(const std::shared_ptr<Droplet<T>>& droplet0, const std::shared_ptr<Droplet<T>>& droplet1) {
        return addMergedDroplet(droplet0->getId(), droplet1->getId());
    }

    template<typename T>
    std::tuple<bool, std::shared_ptr<Droplet<T>>> AbstractDroplet<T>::getDropletAtNode(int nodeId) const {
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
                return {1, droplet};
            }

            // check if a fully occupied channel is connected with the reference node and if yes then return the droplet immediately
            auto connectedFullyOccupiedChannels = droplet->getConnectedFullyOccupiedChannels(nodeId);
            if (connectedFullyOccupiedChannels.size() != 0) {
                return {1, droplet};
            }
        }

        // if nothing was found than return nullptr
        return {0, nullptr};
    }

    template<typename T>
    std::tuple<bool, std::shared_ptr<Droplet<T>>> AbstractDroplet<T>::getDropletAtNode(const std::shared_ptr<arch::Node<T>>& node) const {
        return getDropletAtNode(node->getId());
    }

    template<typename T>
    void AbstractDroplet<T>::removeDroplet(int dropletId) {
        if (droplets.erase(dropletId)) {
            // Remove all injections of this droplet
            auto it = injectionMap.find(dropletId);
            if (it != injectionMap.end()) {
                for (auto injectionId : it->second) {
                    removeDropletInjection(injectionId);
                }
                injectionMap.erase(it);
            }
        } else {
            throw std::logic_error("Could not delete droplet with key " + std::to_string(dropletId) + ". Droplet not found.");
        }
    }

    template<typename T>
    void AbstractDroplet<T>::removeDroplet(const std::shared_ptr<Droplet<T>>& droplet) {
        removeDroplet(droplet->getId());
    }

    template<typename T>
    std::shared_ptr<DropletInjection<T>> AbstractDroplet<T>::addDropletInjection(int dropletId, T injectionTime, int channelId, T injectionPosition) {
        auto id = DropletInjection<T>::getDropletInjectionCounter();
        auto droplet = droplets.at(dropletId).get();
        auto channel = this->getNetwork()->getChannel(channelId);

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

        auto result = dropletInjections.try_emplace(id, std::shared_ptr<DropletInjection<T>>(new DropletInjection<T>(id, droplet, injectionTime, channel, injectionPosition)));
        if (result.second) { injectionMapInsertion(dropletId, id); }
        return result.first->second;
    }

    template<typename T>
    std::shared_ptr<DropletInjection<T>> AbstractDroplet<T>::addDropletInjection(const std::shared_ptr<Droplet<T>>& droplet, T injectionTime, const std::shared_ptr<arch::RectangularChannel<T>>& channel, T injectionPosition) {
        addDropletInjection(droplet->getId(), injectionTime, channel->getId(), injectionPosition);
    }

    template<typename T>
    void AbstractDroplet<T>::removeDropletInjection(int dropletInjectionId) {
        auto it = dropletInjections.find(dropletInjectionId);
        if (it != dropletInjections.end()) {
            int dropletId = it->second->getDroplet()->getId();
            dropletInjections.erase(it);
            injectionMap.at(dropletId).erase(dropletInjectionId);
            if (injectionMap.at(dropletId).empty()) { injectionMap.erase(dropletId); }
        } else {
            throw std::logic_error("Could not delete droplet injection with key " + std::to_string(dropletInjectionId) + ". Injection not found.");
        }
    }

    template<typename T>
    void AbstractDroplet<T>::removeDropletInjection(const std::shared_ptr<DropletInjection<T>>& dropletInjection) {
        removeDropletInjection(dropletInjection->getId());
    }

    template<typename T>
    void AbstractDroplet<T>::removeFluid(const std::shared_ptr<Fluid<T>>& fluid) {
        for (auto& [id, droplet] : droplets) {
            // Check if droplets are pointing to the fluid that is to be removed
            if (droplet->getFluid()->getId() == fluid->getId()) {
                // Set the droplet's fluid to default, i.e., continuous phase
                droplet->setFluid(this->getContinuousPhase().get());
            } 
        }

        // Remove the fluid as defined in base class
        Simulation<T>::removeFluid(fluid);
    }

    template<typename T>
    void AbstractDroplet<T>::injectionMapInsertion(int dropletId, int injectionId) {
        auto result = injectionMap.find(dropletId);
        if (result != injectionMap.end()) {
            result->second.insert(injectionId);
        } else {
            // Set is automatically ordered and ensures uniqueness of injection ids
            injectionMap.try_emplace(dropletId, std::set<int>{injectionId});
        }
    }

    template<typename T>
    void AbstractDroplet<T>::updateDropletResistances() {
        // set all droplet resistances of all channels to 0.0
        for (auto& [key, channel] : this->getNetwork()->getChannels()) {
            channel->setDropletResistance(0.0);
        }

        // set correct droplet resistances
        for (auto& [key, droplet] : droplets) {
            // only consider droplets that are inside the network (i.e., also trapped droplets)
            if (droplet->getDropletState() == DropletState::INJECTION || droplet->getDropletState() == DropletState::SINK) {
                continue;
            }

            droplet->addDropletResistance(*(this->getResistanceModel()));
        }
    }

    template<typename T>
    void AbstractDroplet<T>::moveDroplets(T timeStep) {
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
    std::vector<std::unique_ptr<Event<T>>> AbstractDroplet<T>::computeEvents() {
        // events
        std::vector<std::unique_ptr<Event<T>>> events;

        // injection events
        for (auto& [key, injection] : dropletInjections) {
            double injectionTime = injection->getInjectionTime();
            if (injection->getDroplet()->getDropletState() == DropletState::INJECTION) {
                events.push_back(std::make_unique<DropletInjectionEvent<T>>(injectionTime - this->getTime(), *injection));
            }
        }

        // define maps that are used for detecting merging inside channels
        std::unordered_map<int, std::vector<DropletBoundary<T>*>> channelBoundariesMap;
        std::unordered_map<DropletBoundary<T>*, DropletImplementation<T>*> boundaryDropletMap;

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
                    events.push_back(std::make_unique<BoundaryTailEvent<T>>(time, *droplet, *boundary, *this->getNetwork()));
                } else if (boundary->getFlowRate() > 0) {
                    // boundary moves away from the droplet center => BoundaryHeadEvent
                    double time = boundary->getTime();

                    // in this scenario also a MergeBifurcationEvent can happen when merging is enabled
                    // this means a boundary comes to a bifurcation where a droplet is already present
                    // hence it is either a MergeBifurcationEvent or a BoundaryHeadEvent that will happen

                    // check if merging is enabled
                    // find droplet to merge (if present)
                    auto referenceNode = boundary->getOppositeReferenceNode(this->getNetwork().get());

                    // mergeDropletRef is here sliced to Droplet<T> object, following getDropletAtNode return type
                    // if not handled with care, this can lead to issues. When used, obtain a new droplet reference 
                    // from droplets using the unique identifier.
                    auto [isMerged, mergeDropletRef] = getDropletAtNode(referenceNode->getId());

                    if (!isMerged) {
                        // no merging will happen => BoundaryHeadEvent
                        if (!boundary->isInWaitState()) {
                            events.push_back(std::make_unique<BoundaryHeadEvent<T>>(time, *droplet, *boundary, *this->getNetwork().get()));
                        }
                    } else {
                        // mergeDropletRef is sliced to Droplet<T>. Hence we need a reference to DropletImplementation<T>
                        auto mergeDropletImplementation = droplets.at(mergeDropletRef->getId());
                        // merging of the actual droplet with the merge droplet will happen => MergeBifurcationEvent
                        events.push_back(std::make_unique<MergeBifurcationEvent<T>>(time, *droplet, *mergeDropletImplementation, *boundary, *this));
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

        /** TODO: Miscellaneous
         * Check: Seems like this never happens
         */
        // time step event
        if (dropletsAtBifurcation && this->getMaximalAdaptiveTimeStep() > 0) {
            events.push_back(std::make_unique<TimeStepEvent<T>>(this->getMaximalAdaptiveTimeStep()));
        }

        return events;
    }

    template<typename T>
    void AbstractDroplet<T>::simulate() {
        this->assertInitialized();              // perform initialization checks
        this->initialize();                     // initialize the simulation
        while (true) {
            if (this->getIterations() >= this->getMaxIterations()) {
                throw std::runtime_error("Max iterations exceeded.");
            }

            #ifdef VERBOSE     
                std::cout << "Iteration " << this->getIterations() << std::endl;
            #endif
            // update droplet resistances (in the first iteration no  droplets are inside the network)
            updateDropletResistances();
            // compute nodal analysis
            this->conductNodalAnalysis();
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
                droplet->updateBoundaries(*this->getNetwork());
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
            // move droplets until event is reached
            this->getTime() += nextEvent->getTime();
            moveDroplets(nextEvent->getTime());

            nextEvent->performEvent();

            ++this->getIterations();
        }
    }

    template<typename T>
    void AbstractDroplet<T>::saveState() {
        std::unordered_map<int, T> savePressures;
        std::unordered_map<int, T> saveFlowRates;
        std::unordered_map<int, DropletPosition<T>> saveDropletPositions;

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

        // droplet positions
        for (auto& [id, droplet] : droplets) {
            // create new droplet position
            DropletPosition<T> newDropletPosition;

            // add boundaries
            for (auto& boundary : droplet->getBoundaries()) {
                // get channel position
                auto channelPosition = boundary->getChannelPosition();
                // add boundary
                newDropletPosition.boundaries.emplace_back(DropletBoundary<T>{channelPosition.getChannel(), channelPosition.getPosition(), boundary->isVolumeTowardsNodeA(), static_cast<BoundaryState>(static_cast<int>(boundary->getState()))});
            }

            // add fully occupied channels
            for (auto& channel : droplet->getFullyOccupiedChannels()) {
                // add channel
                newDropletPosition.channelIds.emplace_back(channel->getId());
            }

            saveDropletPositions.try_emplace(droplet->getId(), newDropletPosition);
        }

        // state
        this->getSimulationResults()->addState(this->getTime(), savePressures, saveFlowRates, saveDropletPositions);
    }

}   /// namespace sim
