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
    void essLbmDropletSimulator<T>::lbmInit(T dynViscosity, T density)
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

        this->solver_ = std::make_shared<ess::lbmDropletSolver>(work_dir, this->stlFile, openings, nodes, this->charPhysLength,
                                                                this->charPhysVelocity, 1.0f / this->resolution, this->epsilon, 
                                                                this->relaxationTime, density, dynViscosity);
        this->solver_->prepareLattice();

        #ifdef DEBUG
        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->moduleOpenings.size() > 1);
        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());
        #endif

        // Initialize pressure, flowRate and resistance value-containers
        for (auto& [key, node] : this->moduleOpenings)
        {
            this->pressures.try_emplace(key, 0.0f);
            this->flowRates.try_emplace(key, 0.0f);
        }

        this->storeFlowRates(this->flowRates);
        this->storePressures(this->pressures);

        #ifdef VERBOSE
            std::cout << "[essLbmModule] lbmInit " << this->name << "... OK" << std::endl;
        #endif
    }

    template<typename T>
    int essLbmDropletSimulator<T>::generateDroplet(sim::Droplet<T>* dropletPtr, int bufferZone) {

        T volume = dropletPtr->getVolume();
        std::dynamic_pointer_cast<ess::lbmDropletSolver>(this->solver_)->generateDroplet(bufferZone, volume);
    }

    template<typename T>
    void essLbmDropletSimulator<T>::eraseDroplet(int dropletId, int entrypointId) {        
        for (auto& droplet : pendingDroplets.at(entrypointId)) {
            if (droplet->getId() == dropletId) {
                pendingDroplets.erase(droplet);
                return;
            }
        }
        throw std::range_error("Droplet wasn't found in pendingDroplets.");
    }

}   // namespace arch
