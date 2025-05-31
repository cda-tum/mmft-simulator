#pragma once

#include <unordered_map>

#include "../src/baseSimulator.h"

namespace test::helpers {
/**
 * Calculate average concentration for all mixtures in an edge based
 * on a saved state in a simulation result.
 * This is the percentage of an edge filled by a mixture multiplied by
 * its concentration.
 * The percentage that is filled by the continuous phase is stored in
 * the returned map at index -1.
 */
inline std::unordered_map<int, double> getAverageFluidConcentrationsInEdge(const result::SimulationResult<double>& result, int stateId, int edgeId) {
    if (stateId >= static_cast<int>(result.getStates().size())) {
        // state does not exist, thus there are no fluid concentrations
        return {};
    }
    const auto& state = *result.getStates().at(stateId);
    if (!state.mixturePositions.count(edgeId)) {
        // if there is no mixture position, this means the entire edge is filled
        // with continuous phase which is usually not stored as a mixture
        return { { -1, 1.0 } };
    }
    const auto& mixturePositions = state.mixturePositions.at(edgeId);
    const auto& mixtures = result.getMixtures();

    std::unordered_map<int, double> averageConcentrations;

    // concentration of continuous phase (does not contain any specie)
    double continuousPhaseConcentration = 1.0;
    for (const auto& mixturePosition : mixturePositions) {
        auto mixtureId = mixturePosition.mixtureId;
        auto position1 = mixturePosition.position1;
        auto position2 = mixturePosition.position2;
        auto mixtureLength = position2 - position1;

        const auto& species = mixtures.at(mixtureId)->getSpecieConcentrations();
        for (const auto& [specieId, concentration]: species) {
            // percentage of channel filled with this mixture
            double newConcentration = concentration * mixtureLength;
            auto [iterator, inserted] = averageConcentrations.try_emplace(specieId, newConcentration);
            if (!inserted) {
                // add up all concentrations weighted by the percentage of the channel
                // filled with the mixture
                iterator->second = iterator->second + newConcentration;
            }

            // remaining/empty space in edge must be filled with continuous phase;
            // if there is a species in the mixture, it is not a "continuous phase mixture"
            continuousPhaseConcentration -= newConcentration;
        }
    }
    averageConcentrations[-1] = continuousPhaseConcentration;
    return averageConcentrations;
}
} // namespace test::helpers
