#include "CFDSim.h"

namespace sim {

    template<typename T>
    bool conductCFDSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T, DIMENSION>>>& cfdSimulators, int iteration) {

        bool allConverge = true;

        // loop through modules and perform the collide and stream operations
        for (const auto& cfdSimulator : cfdSimulators) {
            
            // Assertion that the current module is of lbm type, and can conduct CFD simulations.
            #ifndef USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::LBM);
            #elif USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::ESS_LBM);
            #endif
            cfdSimulator.second->solve();

            if (!cfdSimulator.second->hasConverged()) {
                allConverge = false;
            }
            
        }

        return allConverge;
    }

}   // namespace sim
