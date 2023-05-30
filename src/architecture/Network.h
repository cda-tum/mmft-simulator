#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <fstream>

#include <lbmModule.h>
#include <Node.h>
#include <Channel.h>
#include <FlowRatePump.h>
#include <Module.h>
#include <Platform.h>
#include <PressurePump.h>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace arch {
/**
 * @brief Class to specify a Network of Nodes, Channels, and Models for a Platform on a Chip.
*/
template<typename T>
class Network {
    private:
        std::unordered_map<int, std::shared_ptr<Node<T>>> nodes;        ///< Nodes the network consists of.
        std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels;  ///< Map of ids and channel pointers to channels in the network.
        std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePumps;  ///< Map of ids and channel pointers to flow rate pumps in the network.
        std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePumps;  ///< Map of ids and channel pointers to pressure pumps in the network.
        std::unordered_map<int, std::unique_ptr<lbmModule<T>>> modules;    ///< Map of ids and module pointers to modules in the network.
        //Platform* platform;                                             ///< The microfluidic platform that operates on this network.

    public:
    /**
     * @brief Constructor of the Network
    */
    Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
            std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>> channels,
            std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>> flowRatePump,
            std::unordered_map<int, std::unique_ptr<PressurePump<T>>> pressurePump,
            std::unordered_map<int, std::unique_ptr<lbmModule<T>>> modules);

    Network(std::unordered_map<int, std::shared_ptr<Node<T>>> nodes);

    /**
     * @brief Constructor of the Network from a JSON string
     * @param json json string
     * @return SimulationResult struct
    */
    Network(std::string jsonFile);

    /**
     * @brief Adds a new node to the network.
    */
    int addNode();

    /**
     * @brief Get a pointer to the node with the specific id.
    */
    std::shared_ptr<Node<T>>& getNode(int nodeId) const;

    /**
     * @brief Adds a new channel to the network.
    */
    int addChannel();

    /**
     * @brief Get a pointer to the channel with the specific id.
    */
    Channel<T>* getChannel(int channelId) const;

    /**
     * @brief Adds a new module to the network.
    */
    int addModule();

    /**
     * @brief Get a pointer to the module with the specidic id.
    */
    Module<T>* getModule(int moduleId) const;

    const std::unordered_map<int, std::shared_ptr<Node<T>>>& getNodes() const;

    const std::unordered_map<int, std::unique_ptr<RectangularChannel<T>>>& getChannels() const;

    const std::unordered_map<int, std::unique_ptr<lbmModule<T>>>& getModules() const;

    const std::unordered_map<int, std::unique_ptr<FlowRatePump<T>>>& getFlowRatePumps() const;

    const std::unordered_map<int, std::unique_ptr<PressurePump<T>>>& getPressurePumps() const;

    /**
     * @brief Turns a channel with the specific id into a pressurepump with given pressure.
     * @param channelID id of the channel.
     * @param pressure pressure value of the pressure pump.
    */
    void setPressurePump(int channelId, T pressure);

    /**
     * @brief Store the network object in a JSON file.
    */
    void toJson(std::string jsonString) const;
};

}   // namespace arch