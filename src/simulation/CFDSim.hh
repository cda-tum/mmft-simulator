#include "CFDSim.h"

namespace sim {

    template<typename T>
    bool conductCFDSimulation(const arch::Network<T>* network, int iteration) {
        bool allConverge = true;
        // loop through modules and perform the collide and stream operations
        for (const auto& module : network->getModules()) {
            // Assertion that the current module is of lbm type, and can conduct CFD simulations.
            assert(module.second->getModuleType() == arch::ModuleType::LBM);
            
            module.second->solve(iteration);
            module.second->getResults();
            
        }
        
        allConverge = false;

        return allConverge;
    }

}   // namespace sim