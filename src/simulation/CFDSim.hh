#include "CFDSim.h"

namespace sim {

    template<typename T>
    bool conductCFDSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators) {

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

    template<typename T>
    void coupleNsAdLattices(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators) {
        
        // loop through modules and perform the coupling between the NS and AD lattices
        for (const auto& cfdSimulator : cfdSimulators) {
            
            // Assertion that the current module is of lbm type, and can conduct CFD simulations.
            #ifndef USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::LBM);
            #elif USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::ESS_LBM);
            throw std::runtime_error("Coupling between NS and AD fields not defined for ESS LBM.");
            #endif

            cfdSimulator.second->executeCoupling();
            
        }
    }

    template<typename T>
    bool conductADSimulation(const std::unordered_map<int, std::unique_ptr<CFDSimulator<T>>>& cfdSimulators) {

        bool allConverge = true;

        // loop through modules and perform the collide and stream operations
        for (const auto& cfdSimulator : cfdSimulators) {
            
            // Assertion that the current module is of lbm type, and can conduct CFD simulations.
            #ifndef USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::LBM);
            #elif USE_ESSLBM
            assert(cfdSimulator.second->getModule()->getModuleType() == arch::ModuleType::ESS_LBM);
            throw std::runtime_error("Simulation of Advection Diffusion not defined for ESS LBM.");
            #endif
            cfdSimulator.second->adSolve();

            if (!cfdSimulator.second->hasAdConverged()) {
                allConverge = false;
            }
            
        }

        return allConverge;
    }

}   // namespace sim
