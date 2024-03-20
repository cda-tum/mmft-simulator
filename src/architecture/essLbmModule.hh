#include "essLbmModule.h"

#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>

#include "Module.h"

namespace arch{

    essLbmModule::essLbmModule(int id_, std::string name_, std::vector<float> pos_, std::vector<float> size_, std::unordered_map<int, std::shared_ptr<Node<float>>> nodes_, std::unordered_map<int, Opening<float>> openings_)
            : Module<float>(id_, pos_, size_, nodes_),  name(name_), moduleOpenings(openings_)
    {
        this->moduleType = ModuleType::ESS_LBM;
    }


    void essLbmModule::setBoundaryValues()
    {
        solver_->setFlowRates(flowRates);
        solver_->setPressures(pressures);
    }

    void essLbmModule::getResults()
    {
        flowRates = solver_->getFlowRates();
        pressures = solver_->getPressures();
    }

    void essLbmModule::initialize(std::string filename)
    {
        solver_ = std::make_shared<ess::lbmSolver>(filename);

        solver_->prepareLattice();

        this->moduleNetwork = std::make_shared<Network<float>> (this->boundaryNodes);

        // There must be more than 1 node to have meaningful flow in the module domain
        assert(this->boundaryNodes.size() > 1);
        // We must have exactly one opening assigned to each boundaryNode
        assert(this->moduleOpenings.size() == this->boundaryNodes.size());
        

        // Initialize pressure, flowRate and resistance value-containers
        for (auto& [key, node] : this->boundaryNodes)
        {
            pressures.try_emplace(key, 0.0f);
            flowRates.try_emplace(key, 0.0f);
        }

        solver_->setFlowRates(flowRates);
        solver_->setPressures(pressures);

        #ifdef VERBOSE
            std::cout << "[essLbmModule] lbmInit " << name << "... OK" << std::endl;
        #endif
    }

    void essLbmModule::solve()
    {
        solver_->solve(10, 10, 10);
        getResults();
    }

    void essLbmModule::setPressures(std::unordered_map<int, float> pressure_)
    {
        pressures = pressure_;
        solver_->setPressures(pressure_);
    }

    void essLbmModule::setFlowRates(std::unordered_map<int, float> flowRate_)
    {
        flowRates = flowRate_;
        solver_->setFlowRates(flowRate_);
    }

    void essLbmModule::setGroundNodes(std::unordered_map<int, bool> groundNodes_)
    {
        groundNodes = groundNodes_;
    }

    void essLbmModule::setInitialized(bool initialization_)
    {
        initialized = initialization_;
    }

}   // namespace arch
