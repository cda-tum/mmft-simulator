#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Fluid.h"
#include "Mixture.h"

namespace sim { 

template<typename T>
struct MixtureInFlow {

    int mixtureId;
    T inflowVolume;

};

template<typename T>
class InstantaneousMixingModel {

private:

    std::vector<Mixture> mixtures;
    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>
    std::unordered_map<int, std::vector<MixtureInFlow>> mixtureInflowAtNode;    // <nodeId <mixtureId, inflowVolume>>
    std::unordered_map<int, int> mixtureOutflowAtNode;
    std::unordered_map<int, T> totalInflowVolumeAtNode;

public:

    InstantaneousMixingModel();

    void addMixture();

    Mixture* getMixture();

    void updateMixtures(T timeStep);
};

}