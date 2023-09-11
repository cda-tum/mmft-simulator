#include "CFDSim.h"

namespace sim {

    template<typename T>
    bool conductCFDSimulation(const arch::Network<T>* network, int iteration) {

        bool allConverge = true;

        // loop through modules and perform the collide and stream operations
        for (const auto& module : network->getModules()) {
            std::shared_ptr<arch::CFDModule<T>> cfdPtr = std::dynamic_pointer_cast<arch::CFDModule<T>> (module.second);
            
            cfdPtr->solve();

            // TODO: Convergence of continuous flow should have a more elaborate role in transient sim
            if (module.second->getModuleType() == arch::ModuleType::CONTINUOUS) {
                std::shared_ptr<arch::ContinuousModule<T>> continuousPtr = std::dynamic_pointer_cast<arch::ContinuousModule<T>> (module.second);
                if (!continuousPtr->hasConverged()) {
                    allConverge = false;
                }
            }
        }

        return allConverge;
    }

}   // namespace sim