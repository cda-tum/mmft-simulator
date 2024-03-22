#include "essLbmModule.h"
#include <mpi.h>

#include <iostream>

namespace arch{

    template<typename T>
    essLbmModule<T>::essLbmModule(int id_, std::string name_, std::string stlFile_, std::vector<T> pos_, std::vector<T> size_, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes_, std::unordered_map<int, Opening<T>> openings_,
                               T charPhysLenth_, T charPhysVelocity_, T resolution_, T epsilon_, T relaxationTime_)
            : Module<T>(id_, pos_, size_, nodes_),  name(name_), stlFile(stlFile_), moduleOpenings(openings_),
              charPhysLength(charPhysLenth_), charPhysVelocity(charPhysVelocity_), resolution(resolution_), epsilon(epsilon_), relaxationTime(relaxationTime_)
    {
        this->moduleType = ModuleType::ESS_LBM;
    }

    template<typename T>
    void essLbmModule<T>::setBoundaryValues()
    {
        solver_->setFlowRates(flowRates);
        solver_->setPressures(pressures);
    }

    template<typename T>
    void essLbmModule<T>::getResults()
    {
        flowRates = solver_->getFlowRates();
        pressures = solver_->getPressures();
    }

    template<typename T>
    void essLbmModule<T>::lbmInit(T dynViscosity, T density)
    {
        moduleNetwork = std::make_shared<Network<T>> (this->boundaryNodes);

        std::string work_dir = "<path-to-set>";
        const auto& allNodes = moduleNetwork->getNodes();
        std::vector<ess::BoundaryNode> nodes(allNodes.size());
        std::unordered_map<int, ess::Opening> openings;

        for(auto& op : moduleOpenings)
        {
            ess::Opening opening;
            opening.width = op.second.width;
            opening.height = op.second.height;
            opening.normal = op.second.normal;
            openings.try_emplace(op.first, opening);

            ess::BoundaryNode node(allNodes.at(op.first)->getPosition()[0],
                                   allNodes.at(op.first)->getPosition()[1],
                                   allNodes.at(op.first)->getPosition()[2],
                                   allNodes.at(op.first)->getGround());

            std::cout << op.first << ", " << node.x << ", " << node.y << ", " << node.z << std::endl;
            nodes[op.first] = node;
        }

        solver_ = std::make_shared<ess::lbmSolver>(work_dir, stlFile, openings, nodes, charPhysLength, charPhysVelocity, 1.0f / resolution, epsilon, relaxationTime, density, dynViscosity);
        std::cout << "here2" << std::endl;

        solver_->prepareLattice();
        std::cout << "here3" << std::endl;

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

    template<typename T>
    void essLbmModule<T>::solve()
    {
        solver_->solve(10, 10, 10);
        getResults();
    }

    template<typename T>
    void essLbmModule<T>::setPressures(std::unordered_map<int, T> pressure_)
    {
        std::unordered_map<int, float> interface;
        for(auto& [key, value] : pressure_)
            interface.try_emplace(key, value);
        pressures = interface;
        solver_->setPressures(interface);
    }

    template<typename T>
    void essLbmModule<T>::setFlowRates(std::unordered_map<int, T> flowRate_)
    {
        std::unordered_map<int, float> interface;
        for(auto& [key, value] : flowRate_)
            interface.try_emplace(key, value);
        flowRates = interface;
        solver_->setFlowRates(interface);
    }

    template<typename T>
    void essLbmModule<T>::setGroundNodes(std::unordered_map<int, bool> groundNodes_)
    {
        groundNodes = groundNodes_;
    }

    template<typename T>
    void essLbmModule<T>::setInitialized(bool initialization_)
    {
        initialized = initialization_;
    }

}   // namespace arch
