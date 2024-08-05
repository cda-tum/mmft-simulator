#include "essMixing.h"
#include <mpi.h>

#include <iostream>
#include <type_traits>

namespace sim{

    template<typename T>
    essLbmMixingSimulator<T>::essLbmMixingSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_,  std::unordered_map<int, arch::Opening<T>> openings_,
                                ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) :
            essLbmSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, resistanceModel_, charPhysLength_, charPhysVelocity_, alpha_, resolution_, epsilon_, relaxationTime_)
    {
        this->cfdModule->setModuleTypeEssLbm();
    }

    template<typename T>
    void essLbmMixingSimulator<T>::setBoundaryValues(int iT)
    {
        this->storeFlowRates(this->flowRates);
        this->storePressures(this->pressures);
        storeConcentrations(concentrations);
    }

    template<typename T>
    void essLbmMixingSimulator<T>::storeCfdResults()
    {
        for(auto& [key,value] : this->solver_->getPressures())
            this->pressures[key] = value;
        for(auto& [key,value] : this->solver_->getFlowRates())
            this->flowRates[key] = value;
        for(auto& [key,value] : std::dynamic_pointer_cast<ess::lbmMixingSolver>(this->solver_)->getConcentrations())
            concentrations.at(0)[key] = value;
    }

    template<typename T>
    void essLbmMixingSimulator<T>::lbmInit(T dynViscosity, T density)
    {

        std::string work_dir = "/home/michel/Git/mmft-hybrid-simulator/build/";
        const auto& allNodes = this->moduleNetwork->getNodes();
        std::unordered_map<int, ess::BoundaryNode> nodes(allNodes.size());
        std::unordered_map<int, ess::Opening> openings;

        T concentration = 1.0;

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

        this->solver_ = std::make_shared<ess::lbmMixingSolver>(work_dir, this->stlFile, openings, nodes, this->charPhysLength, 
                                                                this->charPhysVelocity, 1.0f / this->resolution, this->epsilon, 
                                                                this->relaxationTime, density, dynViscosity, concentration);
        this->solver_->prepareLattice();

        #ifdef DEBUG
        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->moduleOpenings.size() > 1);
        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->cfdModule->getNodes().size());
        #endif

        // Initialize pressure, flowRate and resistance value-containers
        concentrations.try_emplace(0, std::unordered_map<int,T>());
        for (auto& [key, node] : this->moduleOpenings)
        {
            this->pressures.try_emplace(key, 0.0f);
            this->flowRates.try_emplace(key, 0.0f);
            concentrations.at(0).try_emplace(key, 0.0f);
        }

        this->storeFlowRates(this->flowRates);
        this->storePressures(this->pressures);
        storeConcentrations(concentrations);

        #ifdef VERBOSE
            std::cout << "[essLbmModule] lbmInit " << this->name << "... OK" << std::endl;
        #endif
    }

    template<typename T>
    void essLbmMixingSimulator<T>::solve()
    {
        this->solver_->solve(10, 10, 10);
        essLbmMixingSimulator<T>::storeCfdResults();
    }

    template<typename T>
    void essLbmMixingSimulator<T>::storeConcentrations(std::unordered_map<int, std::unordered_map<int, T>> concentrations_)
    {
        std::unordered_map<int, float> interface;
        for(auto& [key, value] : concentrations_.at(0))
            interface.try_emplace(key, value);
        concentrations = concentrations_;
        std::dynamic_pointer_cast<ess::lbmMixingSolver>(this->solver_)->setConcentrations(interface);
    }

    template<typename T>
    std::unordered_map<int, std::unordered_map<int, T>> essLbmMixingSimulator<T>::getConcentrations() const {
        return concentrations;
    }



}   // namespace arch
