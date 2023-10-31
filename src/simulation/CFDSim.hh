#include "CFDSim.h"

#include <tuple>

namespace sim {

    template<typename T>
    std::tuple<bool, T> conductCFDSimulation(const arch::Network<T>* network, int iteration) {

        bool allConverge = true;

        // loop through modules and perform the collide and stream operations
        for (const auto& module : network->getModules()) {
            
            module.second->solve();

            module.second->getResults();

            // TODO: Convergence of continuous flow should have a more elaborate role in transient sim
            if (module.second->getModuleType() == arch::ModuleType::CONTINUOUS) {
                std::shared_ptr<arch::ContinuousModule<T>> continuousPtr = std::dynamic_pointer_cast<arch::ContinuousModule<T>> (module.second);
                if (!continuousPtr->hasConverged()) {
                    allConverge = false;
                }
            }
        }
        T timestep = 0.0;

        return std::make_tuple(allConverge, timestep);
    }

}   // namespace sim