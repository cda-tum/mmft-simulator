#include "Results.h"

#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace result {

template<typename T>
State<T>::State(int id_, T time_) : id(id_), time(time_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_), dropletPositions(dropletPositions_) { }

template<typename T>
const std::unordered_map<int, T>& State<T>::getPressures() const {
    return pressures;
}

template<typename T>
const std::unordered_map<int, T>& State<T>::getFlowRates() const {
    return flowRates;
}

template<typename T>
T State<T>::getTime() const {
    return time;
}

template<typename T>
SimulationResult<T>::SimulationResult(  arch::Network<T>* network_, 
                                        std::unordered_map<int, sim::Fluid<T>>* fluids_,
                                        std::unordered_map<int, sim::Droplet<T>>* droplets_) :
                                        network(network_), fluids(fluids_), droplets(droplets_) { }

template<typename T>
void SimulationResult<T>::addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates) {
    int id = states.size();
    std::unique_ptr<State<T>> newState = std::make_unique<State<T>>(id, time, pressures, flowRates);
    states.push_back(std::move(newState));
}

template<typename T>
void SimulationResult<T>::addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions) {
    int id = states.size();
    std::unique_ptr<State<T>> newState = std::make_unique<State<T>>(id, time, pressures, flowRates, dropletPositions);
    states.push_back(std::move(newState));
}

template<typename T>
const std::vector<std::unique_ptr<State<T>>>& SimulationResult<T>::getStates() const {
    return  states;
}

}  // namespace droplet
