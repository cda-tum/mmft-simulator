#include "Results.h"

#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace result {

template<typename T>
State<T>::State(int id, T time) : id(id), time(time) { }

template<typename T>
T State<T>::getPressure(int nodeId) const {
    return pressures.at(nodeId);
}

template<typename T>
T State<T>::getPressureDrop(int node0Id, int node1Id) const {
    return getPressure(node0Id) - getPressure(node1Id);
}

template<typename T>
T State<T>::getFlowRate(int channelId) const {
    return flowRates.at(channelId);
}


std::unordered_map<int, double> SimulationResult::getPressures() const {
    return states[0].pressures;
}

std::unordered_map<int, double> SimulationResult::getFlowRates() const {
    return states[0].flowRates;
}

}  // namespace droplet
