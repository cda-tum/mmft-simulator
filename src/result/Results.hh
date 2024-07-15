#include "Results.h"

namespace result {

template<typename T>
State<T>::State(int id_, T time_) : id(id_), time(time_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_, std::unordered_map<int, std::string> vtkFiles_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_), vtkFiles(vtkFiles_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_), dropletPositions(dropletPositions_) { }

template<typename T>
State<T>::State(int id_, T time_, std::unordered_map<int, T> pressures_, std::unordered_map<int, T> flowRates_, std::unordered_map<int, std::deque<sim::MixturePosition<T>>> mixturePositions_, std::unordered_map<int, int> filledEdges_) 
    : id(id_), time(time_), pressures(pressures_), flowRates(flowRates_), mixturePositions(mixturePositions_), filledEdges(filledEdges_) { }

template<typename T>
const std::unordered_map<int, T>& State<T>::getPressures() const {
    return pressures;
}

template<typename T>
const std::unordered_map<int, T>& State<T>::getFlowRates() const {
    return flowRates;
}

template<typename T>
const std::unordered_map<int, std::string>& State<T>::getVtkFiles() const {
    return vtkFiles;
}

template<typename T>
std::unordered_map<int, sim::DropletPosition<T>>& State<T>::getDropletPositions() {
    return dropletPositions;
}

template<typename T>
std::unordered_map<int, std::deque<sim::MixturePosition<T>>>& State<T>::getMixturePositions() {
    return mixturePositions;
}

template<typename T>
T State<T>::getTime() const {
    return time;
}

template<typename T>
const void State<T>::printState() {
    std::cout << "\n";
    // print the current timestep
    std::cout << "[Result] Timestep: " << time << std::endl;
    std::cout << "\n";
    // print the pressures in all nodes
    for (auto& [key, pressure] : pressures) {
        std::cout << "\t[Result] Node " << key << " has a pressure of " << pressure << " Pa.\n";
    }
    std::cout << "\n";
    // print the flow rates in all channels
    for (auto& [key, flowRate] : flowRates) {
        std::cout << "\t[Result] Channel " << key << " has a flow rate of " << flowRate << " m^3/s.\n";
    }
    std::cout << "\n";
    // print the droplet positions
    if ( !dropletPositions.empty() ) {
        for (auto& [key, dropletPosition] : dropletPositions) {
            std::cout << "\tBoundaries:\n";
            for ( auto& boundary : dropletPosition.boundaries ) {
                std::cout << "\t\t channel " << boundary.getChannelPosition().getChannel()->getId() << "\t position "
                    << boundary.getChannelPosition().getPosition() << std::endl;
            }
            std::cout << "\n";
            std::cout << "\tChannels:";
            for ( auto& channel : dropletPosition.channelIds ) {
                std::cout << " " << channel;
            }
            std::cout << std::endl;
        }
    }
    // print the mixture positions
    if ( !mixturePositions.empty() ) {
        for (auto& [channelId, deque] : mixturePositions){
            std::cout << "\t[Result] Channel " << channelId << " contains\n";
            for (auto& mixturePosition : deque) {
                std::cout << "\t\tMixture " << mixturePosition.mixtureId << " from position "
                << mixturePosition.position1 << " to " << mixturePosition.position2 << "\n";
            }
        }
        std::cout << "\n";
    }
    // print the filled edges
    if( !filledEdges.empty() ) {
        for (auto& [edgeId, mixtureId] : filledEdges) {
            std::cout << "\t[Result] Channel " << edgeId << " is filled with " << mixtureId << "\n";
        }
        std::cout << "\n";
    }
}

template<typename T>
SimulationResult<T>::SimulationResult() { }

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
void SimulationResult<T>::addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::string> vtkFiles) {
    int id = states.size();
    std::unique_ptr<State<T>> newState = std::make_unique<State<T>>(id, time, pressures, flowRates, vtkFiles);
    states.push_back(std::move(newState));
}

template<typename T>
void SimulationResult<T>::addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, sim::DropletPosition<T>> dropletPositions) {
    int id = states.size();
    std::unique_ptr<State<T>> newState = std::make_unique<State<T>>(id, time, pressures, flowRates, dropletPositions);
    states.push_back(std::move(newState));
}

template<typename T>
void SimulationResult<T>::addState(T time, std::unordered_map<int, T> pressures, std::unordered_map<int, T> flowRates, std::unordered_map<int, std::deque<sim::MixturePosition<T>>> mixturePositions) {
    int id = states.size();
    for ( auto& [channelId, deque] : mixturePositions ) {
        if (filledEdges.count(channelId)) {
            filledEdges.at(channelId) = deque.front().mixtureId;
        } else {
            filledEdges.try_emplace(channelId, deque.back().mixtureId);
        }
    }
    std::unique_ptr<State<T>> newState = std::make_unique<State<T>>(id, time, pressures, flowRates, mixturePositions, filledEdges);
    states.push_back(std::move(newState));
}

template<typename T>
const std::vector<std::unique_ptr<State<T>>>& SimulationResult<T>::getStates() const {
    return states;
}

template<typename T>
const void SimulationResult<T>::printStates() const {
    for ( auto& state : states ) {
        state->printState();
    }
}

template<typename T>
const void SimulationResult<T>::printLastState() const {
    states.back()->printState();
}

template<typename T>
const void SimulationResult<T>::printState(int key) const {
    states.at(key)->printState();
}

template<typename T>
const void SimulationResult<T>::setMixtures(std::unordered_map<int, sim::Mixture<T>*> mixtures_) {
    mixtures = mixtures_;
}

template<typename T>
const std::unordered_map<int, sim::Mixture<T>*>& SimulationResult<T>::getMixtures() const {
    return mixtures;
}

template<typename T>
const void SimulationResult<T>::printMixtures() {

    if (mixtures.empty()) {
        throw std::invalid_argument("There are no mixture results stored.");
    } else {
        for (auto& [mixtureId, mixture] : mixtures) {
            std::cout << "\t[Result] Mixture " << mixtureId << " contains\n";
            for (auto& [specieId, concentration] : mixture->getSpecieConcentrations()) {
                std::cout << "\t\tSpecie " << specieId << " with concentration " << concentration << " kg/m^3\n";
            }
        }
        std::cout << "\n";
    }
}

template<typename T>
const void SimulationResult<T>::writeMixture(int mixtureId) {
    // TODO Maria, CSV Writer here
    // TODO get a channel pointer
    int numValues = 101; // Number of points to calculate

    std::cout << "Generating CSV files" << std::endl;

    T step = 1.0 / (numValues-1);

    auto mixture = this->mixtures.at(mixtureId);

    try {
        for (auto& [specieId, tuple] : mixture->getSpecieDistributions()) {
            std::cout << "Mixture " << mixtureId << " contains species " << specieId <<std::endl;
        }

        // std::cout << "\t[Result] Mixture " << mixtureId << " contains\n";
        std::cout << "Mixture " << mixtureId << std::endl;
        for (auto& [specieId, tuple] : mixture->getSpecieDistributions()) {
            std::cout << "Specie " << specieId << std::endl;
            std::string outputFileName = "function_mixture"+std::to_string(mixtureId)+"_species"+std::to_string(specieId)+".csv";
            std::cout << "Generating CSV file: " << outputFileName << std::endl;
            // Open a file in write mode.
            std::ofstream outputFile;
            outputFile.open(outputFileName); // TODO maybe define this inside of the loop
            // Write the header to the CSV file TODO adapt this to fit the specific mixture
            outputFile << "x,f(x)\n";
            // Calculate and write the values to the file
            for (int i = 0; i < numValues; ++i) {
                T x = i * step;
                T y;
                // y = tuple.first(x);
                y = std::get<0>(tuple)(x);
                outputFile << std::setprecision(4) << x << "," << y << "\n"; 
            }
            // Close the file
            outputFile.close();
        }
    }
    catch(...) {
        throw std::invalid_argument("Tried to access a mixture without specie distributions.");
    }
    
    std::cout << "CSV file has been generated " << std::endl;
}

}  // namespace result
