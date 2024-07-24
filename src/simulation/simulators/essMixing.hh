#include "essContinuous.h"
#include <mpi.h>

#include <iostream>
#include <type_traits>

namespace sim{

    template<typename T>
    essLbmSimulator<T>::essLbmSimulator(int id_, std::string name_, std::string stlFile_, std::shared_ptr<arch::Module<T>> cfdModule_,  std::unordered_map<int, arch::Opening<T>> openings_,
                                ResistanceModel<T>* resistanceModel_, T charPhysLength_, T charPhysVelocity_, T alpha_, T resolution_, T epsilon_, T relaxationTime_) :
            CFDSimulator<T>(id_, name_, stlFile_, cfdModule_, openings_, alpha_, resistanceModel_), 
            charPhysLength(charPhysLength_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), 
            epsilon(epsilon_), relaxationTime(relaxationTime_)
    {
        this->cfdModule->setModuleTypeEssLbm();
        allNodes = cfdModule_->getNodes();
    }

    template<typename T>
    void essLbmSimulator<T>::setBoundaryValues(int iT)
    {
        setFlowRates(flowRates);
        setPressures(pressures);
    }

    template<typename T>
    void essLbmSimulator<T>::storeCfdResults()
    {
        for(auto& [key,value] : solver_->getPressures())
            pressures[key] = value;
        for(auto& [key,value] : solver_->getFlowRates())
            flowRates[key] = value;
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
    void essLbmSimulator<T>::solve()
    {
        solver_->solve(10, 10, 10);
        storeCfdResults();
    }

    template<typename T>
    void essLbmSimulator<T>::setPressures(std::unordered_map<int, T> pressure_)
    {
        std::unordered_map<int, float> interface;
        for(auto& [key, value] : pressure_)
            interface.try_emplace(key, value);
        pressures = pressure_;
        solver_->setPressures(interface);
    }

    template<typename T>
    void essLbmSimulator<T>::setFlowRates(std::unordered_map<int, T> flowRate_)
    {
        std::unordered_map<int, float> interface;
        for(auto& [key, value] : flowRate_)
            interface.try_emplace(key, value);
        flowRates = flowRate_;
        solver_->setFlowRates(interface);
    }

    template<typename T>
    std::unordered_map<int, T> essLbmSimulator<T>::getPressures() const {
        return pressures;
    }


    template<typename T>
    std::unordered_map<int, T> essLbmSimulator<T>::getFlowRates() const {
        return flowRates;
    }

    template<typename T>
    bool essLbmSimulator<T>::hasConverged() const
    {
        return solver_->hasConverged();
    }

    template<typename T>
    void essLbmSimulator<T>::setVtkFolder(std::string vtkFolder_) {
        this->vtkFolder = vtkFolder_;
    }

}   // namespace arch
