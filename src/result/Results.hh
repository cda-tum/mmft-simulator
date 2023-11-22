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
T State<T>::getPressures() const {
    return pressures;
}

template<typename T>
T State<T>::getFlowRates() const {
    return flowRates.at();
}

template<typename T>
std::unordered_map<int, T> SimulationResult::getPressures() const {
    return pressures;
}

template<typename T>
std::unordered_map<int, T> SimulationResult::getFlowRates() const {
    return flowRates;
}

template<typename T>
T SimulationResult::getTime() const {
    return time;
}

}  // namespace droplet
