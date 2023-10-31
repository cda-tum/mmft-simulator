#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Fluid.h"
#include "Mixture.h"

#include "../architecture/Network.h"

namespace sim { 

template<typename T>
struct MixtureInFlow {

    int mixtureId;
    T inflowVolume;

};

template<typename T>
class InstantaneousMixingModel {

private:

    std::vector<Mixture<T>*> mixtures;
    std::unordered_map<int, std::deque<std::pair<int,T>>> mixturesInEdge;       ///< Which mixture currently flows in which edge <EdgeID, <MixtureID, currPos>>>
    std::unordered_map<int, std::vector<MixtureInFlow<T>>> mixtureInflowAtNode;    // <nodeId <mixtureId, inflowVolume>>
    std::unordered_map<int, int> mixtureOutflowAtNode;
    std::unordered_map<int, T> totalInflowVolumeAtNode;

public:

    InstantaneousMixingModel();

    int addMixture(std::unordered_map<int, T>& fluidConcentrations, int id);

    Mixture<T>* getMixture();

    std::vector<Mixture<T>*> getMixtures();

    std::unordered_map<int, T> getDiffusivities();

    void updateMixtures(T timeStep, arch::Network<T>* network);
};

}