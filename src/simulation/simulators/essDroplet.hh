#include "essContinuous.h"
#include <mpi.h>

#include <iostream>
#include <type_traits>

namespace sim{

    template<typename T>
    essLbmDropletSimulator<T>::essLbmDropletSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_,  std::unordered_map<int, arch::Opening<T>> openings_,
                                ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) :
            essLbmSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_)
    {
        this->cfdModule->setModuleTypeEssLbm();
    }

    template<typename T>
    void essLbmSimulator<T>::lbmInit(T dynViscosity, T density)
    {

        std::string work_dir = "/home/michel/Git/mmft-hybrid-simulator/build/";
        const auto& allNodes = this->moduleNetwork->getNodes();
        std::unordered_map<int, ess::BoundaryNode> nodes(allNodes.size());
        std::unordered_map<int, ess::Opening> openings;

        for(auto& op : this->moduleOpenings)
        {
            ess::Opening opening;
            opening.width = op.second.width;
            opening.height = op.second.height;
            opening.normal = {op.second.normal[0],op.second.normal[1],op.second.normal[2]};
            openings.try_emplace(op.first, opening);

            ess::BoundaryNode node(allNodes.at(op.first)->getPosition()[0],
                                   allNodes.at(op.first)->getPosition()[1],
                                   allNodes.at(op.first)->getPosition()[2],
                                   allNodes.at(op.first)->getGround());

            nodes.try_emplace(op.first, node);
        }

        solver_ = std::make_shared<ess::lbmSolver>(work_dir, this->stlFile, openings, nodes, charPhysLength, charPhysVelocity, 1.0f / resolution, epsilon, relaxationTime, density, dynViscosity);
        solver_->prepareLattice();

        #ifdef DEBUG
        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->moduleOpenings.size() > 1);
        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());
        #endif

        // Initialize pressure, flowRate and resistance value-containers
        for (auto& [key, node] : this->moduleOpenings)
        {
            pressures.try_emplace(key, 0.0f);
            flowRates.try_emplace(key, 0.0f);
        }

        setFlowRates(flowRates);
        setPressures(pressures);

        #ifdef VERBOSE
            std::cout << "[essLbmModule] lbmInit " << this->name << "... OK" << std::endl;
        #endif
    }

    template<typename T>
    int essLbmDropletSimulator<T>::generateDroplet(sim::Droplet<T>* dropletPtr, Node<T>* entrypoint) {

        int dropletId = 1;
        std::vector<T> nodePosition = (0.0, 0.0, 0.0);
        T density = 1.0;
        T viscosity = 1.0;
        T volume = 1.0;
        this->solver_->generateDroplet(dropletId, nodePosition, density, viscosity, volume);
        droplet->setDropletState(sim::DropletState::IDLE);
        lbmDroplets.try_emplace(dropletId, nullptr);
    }

    template<typename T>
    void essLbmDropletSimulator<T>::eraseDroplet(int dropletId, int entrypointId) {        
        for (auto& droplet : pendingDroplets.at(nodeId)) {
            if (droplet->getId() == dropletId) {
                pendingDroplets.erase(droplet);
                return;
            }
        }
        throw std::range_error("Droplet wasn't found in pendingDroplets.");
    }

    template<typename T>
    int essLbmDropletSimulator<T>::purgeDroplet(int dropletId) {
        dropletId = 1;
        assert(lbmDroplets.count(dropletId));
        lbmDroplets.at(dropletId)->setDropletState(sim::DropletState::NETWORK);
        lbmDroplets.erase(dropletId);
    }

    template<typename T>
    int essLbmDropletSimulator<T>::addDroplet(int parentDropletId, T volumeParentDroplet, T volumeChildDroplet) {
        int dropletId = 2;
        lbmDroplets.try_emplace(dropletId, nullptr);
        // TODO: Add child droplet to droplet map in simulation object
        return dropletId;
    }

}   // namespace arch
