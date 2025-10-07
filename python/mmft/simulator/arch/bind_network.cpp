#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.h"
#include "architecture/entities/Edge.h"
#include "architecture/entities/FlowRatePump.h"
#include "architecture/entities/Membrane.h"
#include "architecture/entities/Module.h"
#include "architecture/entities/Node.h"
#include "architecture/entities/PressurePump.h"
#include "architecture/entities/Tank.h"
#include "architecture/definitions/ModuleOpening.h"
#include "architecture/Network.h"

namespace py = pybind11;

using T = double;

void bind_network(py::module_& m) {

	py::class_<arch::Network<T>, py::smart_holder>(m, "Network")
		.def("isNetworkValid", &arch::Network<T>::isNetworkValid, "Check if the current network is valid.")
		.def("toJson", &arch::Network<T>::toJson, "Store the network object in a JSON file.")
		.def("print", &arch::Network<T>::print, "Print the content of this network.")
		// Nodes
		.def("addNode", py::overload_cast<T, T, bool>(&arch::Network<T>::addNode), 
			py::arg("x"), py::arg("y"), py::arg("ground")=false, "Add a new node to the network.")
		.def("getNode", &arch::Network<T>::getNode, "Returns the node with the given id.")
		.def("getNodes", &arch::Network<T>::getNodes, "Returns all nodes in the network.")
		.def("getGroundNodes", &arch::Network<T>::getGroundNodes, "Returns the set of nodes that are ground nodes.")
		.def("getGroundNodeIds", &arch::Network<T>::getGroundNodeIds, "Returns the ids of the set of ground nodes.")
		.def("getVirtualNodes", &arch::Network<T>::getVirtualNodes, "Returns the amount of virtual nodes in the network.")
		.def("setVirtualNodes", &arch::Network<T>::setVirtualNodes, "Sets the amount of virtual nodes in the network.")
		.def("hasNode", py::overload_cast<size_t>(&arch::Network<T>::hasNode, py::const_), "Returns true if the network contains the node with the given id.")
		.def("hasNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::hasNode, py::const_), "Returns true if the network contains the given node.")
		.def("setSink", py::overload_cast<size_t>(&arch::Network<T>::setSink), "Sets the node with given id to be a sink node.")
		.def("setSink", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::setSink), "Sets the given node to be a sink node.")
		.def("isSink", py::overload_cast<size_t>(&arch::Network<T>::isSink, py::const_), "Returns true if the node of the given id is a sink node.")
		.def("isSink", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::isSink, py::const_), "Returns true if the given node is a sink node.")
		.def("setGround", py::overload_cast<size_t>(&arch::Network<T>::setGround), "Sets the node with given id to be a ground node.")
		.def("setGround", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::setGround), "Sets the given node to be a ground node.")
		.def("isGround", py::overload_cast<size_t>(&arch::Network<T>::isGround, py::const_), "Returns true if the node of the given id is a ground node.")
		.def("isGround", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::isGround, py::const_), "Returns true if the given node is a ground node.")
		.def("calculateNodeDistance", py::overload_cast<size_t, size_t>(&arch::Network<T>::calculateNodeDistance, py::const_), 
			"Calculates the Euclidean distance between the nodes of the given ids.")
		.def("calculateNodeDistance", py::overload_cast<std::shared_ptr<arch::Node<T>>, std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::calculateNodeDistance, py::const_), 
			"Calculates the Euclidean distance between the two given nodes.")
		.def("removeNode", &arch::Network<T>::removeNode, "Removes the given node from the network.")
		// Channels
		.def("addRectangularChannel", py::overload_cast<size_t, size_t, T, T, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("addRectangularChannel", py::overload_cast<const std::shared_ptr<arch::Node<T>>&, const std::shared_ptr<arch::Node<T>>&, T, T, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("addRectangularChannel", py::overload_cast<size_t, size_t, T, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("addRectangularChannel", py::overload_cast<const std::shared_ptr<arch::Node<T>>&, const std::shared_ptr<arch::Node<T>>&, T, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("addRectangularChannel", py::overload_cast<size_t, size_t, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("addRectangularChannel", py::overload_cast<const std::shared_ptr<arch::Node<T>>&, const std::shared_ptr<arch::Node<T>>&, T, arch::ChannelType>(&arch::Network<T>::addRectangularChannel), 
			"Add a new channel with rectangular cross-section to the network.")
		.def("getChannel", &arch::Network<T>::getChannel, "Returns the channel with the given id.")
		.def("getRectangularChannel", &arch::Network<T>::getRectangularChannel, "Returns the channel with the given id, if it is a rectangular channel.")
		.def("getChannels", &arch::Network<T>::getChannels, "Returns all channels in the network.")
		.def("getChannelsAtNode", py::overload_cast<size_t>(&arch::Network<T>::getChannelsAtNode, py::const_), 
				"Returns all channels in the network that are connected to the node with the given id.")
		.def("getChannelsAtNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::getChannelsAtNode, py::const_), 
			"Returns all channels in the network that are connected to the given node.")
		.def("isChannel", py::overload_cast<int>(&arch::Network<T>::isChannel, py::const_), "Returns true if the edge with the given id is a channel.")
		.def("isChannel", py::overload_cast<const std::shared_ptr<arch::Edge<T>>&>(&arch::Network<T>::isChannel, py::const_), "Returns true if the given edge is a channel.")
		.def("removeChannel", &arch::Network<T>::removeChannel, "Remove the given channel from the network.")
		// Pumps
		.def("addFlowRatePump", &arch::Network<T>::addFlowRatePump, "Add a flow rate pump to the network.")
		.def("getFlowRatePump", &arch::Network<T>::getFlowRatePump, "Returns the flow rate pump with the given id.")
		.def("setFlowRatePump", &arch::Network<T>::setFlowRatePump, "Set the edge with the given id to be a flow rate pump.")
		.def("isFlowRatePump", &arch::Network<T>::isFlowRatePump, "Returns true if the provided edge id is from a flow rate pump.")
		.def("getFlowRatePumps", &arch::Network<T>::getFlowRatePumps, "Returns all flow rate pumps in the network.")
		.def("removeFlowRatePump", &arch::Network<T>::removeFlowRatePump, "Removes the given flow rate pump from the network.")
		.def("addPressurePump", &arch::Network<T>::addPressurePump, "Add a pressure pump to the network.")
		.def("getPressurePump", &arch::Network<T>::getPressurePump, "Returns the pressure pump with the given id.")
		.def("setPressurePump", &arch::Network<T>::setPressurePump, "Set the edge with the given id to be a pressure pump.")
		.def("isPressurePump", &arch::Network<T>::isPressurePump, "Returns true if the provided edge id is from a pressure pump.")
		.def("getPressurePumps", &arch::Network<T>::getPressurePumps, "Returns all pressure pumps in the network.")
		.def("removePressurePump", &arch::Network<T>::removePressurePump, "Removes the given pressure pump from the network.")
		// Modules
		.def("addCfdModule", &arch::Network<T>::addCfdModule, "Adds a CFD module to the network.")
		.def("getCfdModule", &arch::Network<T>::getCfdModule, "Returns the CFD module with the given id.")
		.def("getCfdModules", &arch::Network<T>::getCfdModules, "Returns all CFD modules in the network.")
		.def("removeModule", &arch::Network<T>::removeModule, "Removes the given module from the network")
		// Membrane
		.def("addMembraneToChannel", py::overload_cast<size_t, T, T, T, T>(&arch::Network<T>::addMembraneToChannel), "Adds a membrane to a channel in the network.")
		.def("addMembraneToChannel", py::overload_cast<const std::shared_ptr<arch::Channel<T>>&, T, T, T, T>(&arch::Network<T>::addMembraneToChannel), 
			"Adds a membrane to a channel in the network.")
		.def("getMembrane", &arch::Network<T>::getMembrane, "Returns the membrane with the given id.")
		.def("getMembraneBetweenNodes", py::overload_cast<size_t, size_t>(&arch::Network<T>::getMembraneBetweenNodes, py::const_), 
			"Returns the membrane between two given nodes.")
		.def("getMembraneBetweenNodes", py::overload_cast<const std::shared_ptr<arch::Node<T>>&, const std::shared_ptr<arch::Node<T>>&>(&arch::Network<T>::getMembraneBetweenNodes, py::const_), 
			"Returns the membrane between two given nodes.")
		.def("getMembranes", &arch::Network<T>::getMembranes, "Returns all membranes in the network.")
		.def("getMembranesAtNode", py::overload_cast<size_t>(&arch::Network<T>::getMembranesAtNode, py::const_), 
			"Returns the set of membranes that are at the given node.")
		.def("getMembranesAtNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::getMembranesAtNode, py::const_), 
			"Returns the set of membranes that are at the given node.")
		.def("isMembrane", &arch::Network<T>::isMembrane, "Returns true if the given edge id belongs to a membrane.")
		// Tank
		.def("addTankToMembrane", py::overload_cast<size_t, T, T>(&arch::Network<T>::addTankToMembrane), "Adds a tank to a membrane in the network.")
		.def("addTankToMembrane", py::overload_cast<const std::shared_ptr<arch::Membrane<T>>&, T, T>(&arch::Network<T>::addTankToMembrane), 
			"Adds a tank to a membrane in the network.")
		.def("getTank", &arch::Network<T>::getTank, "Returns the tank with the given id.")
		.def("getTankBetweenNodes", py::overload_cast<size_t, size_t>(&arch::Network<T>::getTankBetweenNodes, py::const_), 
			"Returns the tank between two given nodes.")
		.def("getTankBetweenNodes", py::overload_cast<const std::shared_ptr<arch::Node<T>>&, const std::shared_ptr<arch::Node<T>>&>(&arch::Network<T>::getTankBetweenNodes, py::const_), 
			"Returns the tank between two given nodes.")
		.def("getTanks", &arch::Network<T>::getTanks, "Returns all tanks in the network.");
		
	m.def("createNetwork", py::overload_cast<>(&arch::Network<T>::createNetwork), "Create a Network object.");
	m.def("createNetwork", py::overload_cast<std::unordered_map<size_t, std::shared_ptr<arch::Node<T>>>>
		(&arch::Network<T>::createNetwork), "Create a Network object.");
	m.def("createNetwork", py::overload_cast<std::unordered_map<size_t, std::shared_ptr<arch::Node<T>>>, 
											std::unordered_map<size_t, std::shared_ptr<arch::Channel<T>>>>
		(&arch::Network<T>::createNetwork), "Create a Network object.");
	m.def("createNetwork", py::overload_cast<std::unordered_map<size_t, std::shared_ptr<arch::Node<T>>>, 
                                            std::unordered_map<size_t, std::shared_ptr<arch::Channel<T>>>,
                                            std::unordered_map<size_t, std::shared_ptr<arch::FlowRatePump<T>>>,
                                            std::unordered_map<size_t, std::shared_ptr<arch::PressurePump<T>>>,
                                            std::unordered_map<size_t, std::shared_ptr<arch::CfdModule<T>>>>
		(&arch::Network<T>::createNetwork), "Create a Network object.");

}