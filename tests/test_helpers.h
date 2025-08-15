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

        const auto& species = mixtures.at(mixtureId)->readSpecieConcentrations();
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

/**
 * Get the path a droplet has taken through the network in a simulation run.
 * The returned vector contains a vector for each movement of the droplet.
 * This vector contains the channel IDs of all channels that contain the
 * given droplet ID.
 */
inline std::vector<std::vector<int>> getDropletPath(const result::SimulationResult<double>& result, int dropletId) {
    std::vector<std::vector<int>> dropletPath;

    // loop through states
    for (const auto& state : result.getStates()) {
        // get dropletPosition
        auto itDropletPosition = state->dropletPositions.find(dropletId);
        if (itDropletPosition != state->dropletPositions.end()) {
            // all channels that contain the droplet in the current saved state
            std::vector<int> position;

            // add channelIds of boundaries
            for (auto& boundary : itDropletPosition->second.boundaries) {
                position.push_back(boundary.getChannelPosition().getChannel()->getId());
            }

            // add fully occupied channelIds
            for (auto& channelId : itDropletPosition->second.channelIds) {
                position.push_back(channelId);
            }

            // check if the set with the channelIds is the same as in the previous state;
            // if yes then do not consider this new state and pop it from the list (prevents duplicates)
            if (dropletPath.empty() || dropletPath.back() != position) {
                dropletPath.push_back(std::move(position));
            }
        }
    }

    return dropletPath;
}
} // namespace test::helpers
