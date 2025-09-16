#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <baseSimulator.h>
#include <baseSimulator.hh>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using T = double;

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pysimulator, m) {
	m.doc() = "Python binding for the MMFT-Simulator.";

	py::enum_<arch::ChannelType>(m, "ChannelType")
		.value("normal", arch::ChannelType::NORMAL)
		.value("bypass", arch::ChannelType::BYPASS)
		.value("cloggable", arch::ChannelType::CLOGGABLE);

	py::enum_<arch::ModuleType>(m, "ModuleType")
		.value("normal", arch::ModuleType::NORMAL)
		.value("lbm", arch::ModuleType::LBM);

	py::enum_<sim::Type>(m, "Type")
		.value("abstract", sim::Type::Abstract)
		.value("hybrid", sim::Type::Hybrid);

	py::enum_<sim::Platform>(m, "Platform")
		.value("continuous", sim::Platform::Continuous)
		.value("bigDroplet", sim::Platform::BigDroplet);

	py::class_<arch::Opening>(m, "Opening")
        .def(py::init<std::shared_ptr<NodeT>, std::vector<T>, T,T>(),
            py::arg("node"), py::arg("normal"), py::arg("width"), py::arg("height") = 1e-4)
		.def_readwrite("node", &arch::Opening::node)
		.def_readwrite("normal", &arch::Opening::normal)
		.def_readwrite("tangent", &arch::Opening::tangent)
		.def_readwrite("width", &arch::Opening::width)
		.def_readwrite("height", &arch::Opening::height)
		.def_readwrite("radial", &arch::Opening::radial)

	py::class_<arch::Node<T>, py::smart_holder>(m, "Node")
		.def("getId", &arch::Node<T>::getId, "Returns the Id of the node.")
		.def("setPosition", &arch::Node<T>::setPosition, "Set the position of the node.")
		.def("getPosition", &arch::Node<T>::getPosition, "Returns the position of the node.")
		.def("getPressure", &arch::Node<T>::getPressure, "Returns the pressure at the node.")
		.def("setSink", &arch::Node<T>::setSink, "Set the node as a sink node.")
		.def("getSink", &arch::Node<T>::getSink, "Returns true if the node is a sink node.")
		.def("setGround", &arch::Node<T>::setGround, "Set the node as a ground node.")
		.def("getGround", &arch::Node<T>::getGround, "Returns true if the node is a ground node.");

	py::class_<arch::Edge<T>, py::smart_holder>(m, "Edge")
		.def("getId", &arch::Edge<T>::getId, "Returns the Id of the edge.")
		.def("getNodeAId", &arch::Edge<T>::getNodeAId, "Returns the id of the node at the start of the edge.")
		.def("getNodeBId", &arch::Edge<T>::getNodeBId, "Returns the id of the node at the end of the edge.")
		.def("getNodeIds", &arch::Edge<T>::getNodeIds, "Returns the ids of the edge's nodes.")
		.def("getPressure", &arch::Edge<T>::getPressure, "Returns the pressure of the edge [Pa].")
		.def("getFlowRate", &arch::Edge<T>::getFlowRate, "Returns the flow rate within the edge [m^3/s].")
		.def("getResistance", &arch::Edge<T>::getResistance, "Returns the inherent resistance of the edge [Pa s].");

	py::class_<arch::Channel<T>, arch::Edge<T>, py::smart_holder>(m, "Channel")
		.def("setChannelType", &arch::Channel<T>::setChannelType, "Set the type of channel.")
		.def("getChannelShape", &arch::Channel<T>::getChannelShape, "Returns the shape of the channel cross section.")
		.def("getChannelType", &arch::Channel<T>::getChannelType, "Returns the type of the channel.")
		.def("setLength", &arch::Channel<T>::setLength, "Set the length of the channel [m].")
		.def("getLength", &arch::Channel<T>::getLength, "Returns the length of the channel [m].")
		.def("getArea", &arch::Channel<T>::getArea, "Returns the area of the channel cross-section [m^2].")
		.def("getVolume", &arch::Channel<T>::getVolume, "Returns the volume of the channel [m^3].")
		.def("isChannelValid", &arch::Channel<T>::isChannelValid, "Returns true if the channel parameters are valid.")
		.def("isRectangular", &arch::Channel<T>::isRectangular, "Returns true if the channel has a rectangular cross-section.")
		.def("isCylindrical", &arch::Channel<T>::isCylindrical, "Returns true if the channel has a cylindrical cross-section.");

	py:class_<arch::RectangularChannel<T>, arch::Channel<T>, py::smart_holder>(m, "RectangularChannel")
		.def("get2DFlowRate", &arch::RectangularChannel<T>::get2DFlowRate, "Maps and returns the 2-dimensional flow rate within the channel [m^2/s].")
		.def("setWidth", &arch::RectangularChannel<T>::setWidth, "Set the width of the channel cross-section [m].")
		.def("getWidth", &arch::RectangularChannel<T>::getWidth, "Returns the width of the channel cross-section [m].")
		.def("setHeight", &arch::RectangularChannel<T>::setHeight, "Set the height of the channel cross-section [m].")
		.def("getHeight", &arch::RectangularChannel<T>::getHeight, "Returns the height of the channel cross-section [m]");

	py:class_<arch::CylindricalChannel<T>, arch::Channel<T>, py::smart_holder>(m, "CylindricalChannel")
		.def("setRadius", &arch::CylindricalChannel<T>::setRadius, "Set the radius of the channel cross-section [m].")
		.def("getRadius", &arch::CylindricalChannel<T>::getRadius, "Returns the radius of the channel cross-section [m].")

	py::class_<arch::FlowRatePump<T>, arch::Edge<T>, py::smart_holder>(m, "FlowRatePump")
		.def("setFlowRate", &arch::CylindricalChannel<T>::setFlowRate, "Set the flow rate of the pump [m^3/s].")

	py::class_<arch::PressurePump<T>, arch::Edge<T>, py::smart_holder>(m, "PressurePump")
		.def("setPressure", &arch::CylindricalChannel<T>::setPressure, "Set the pressure across the pump [Pa].")

	py::class_<arch::Membrane<T>, arch::Edge<T>, py::smart_holder>(m, "Membrane")
		.def("setDimensions", &arch::Membrane::setDimensions, "Set the dimensions of the membrane [w, h, l] [m].")
		.def("setHeight", &arch::Membrane::setHeight, "Set the height of the membrane [m].")
		.def("setWidth", &arch::Membrane::setWidth, "Set the width of the membrane [m].")
		.def("setLength", &arch::Membrane::setLength, "Set the length of the membrane [m].")
		.def("setPoreRadius", &arch::Membrane::setPoreRadius, "Set the pore radius of the pores of the membrane [m].")
		.def("setPorosity", &arch::Membrane::setPorosity, "Set the porosity of the membrane [0.0-1.0].")
		.def("setChannel", &arch::Membrane::setChannel, "Set the channel the membrane is connected to.")
		.def("setTank", &arch::Membrane::setTank, "Set the tank the membrane is connected to.")
		.def("getConcentrationChange", &arch::Membrane::getConcentrationChange, "Returns the concentration change caused by the membrane [mol].")
		.def("getHeight", &arch::Membrane::getHeight, "Returns the height of the membrane [m].")
		.def("getWidth", &arch::Membrane::getWidth, "Returns the width of the membrane [m].")
		.def("getLength", &arch::Membrane::getLength, "Returns the length of the membrane [m].")
		.def("getChannel", &arch::Membrane::getChannel, "Returns the channel that the membrane is connected to.")
		.def("getTank", &arch::Membrane::getTank, "Returns the tank that the membrane is connected to.")
		.def("getPoreRadius", &arch::Membrane::getPoreRadius, "Returns the pore radius of the membrane [m].")
		.def("getPoreDiameter", &arch::Membrane::getPoreDiameter, "Returns the diameter of the pores of the membrane [m].")
		.def("getPorosity", &arch::Membrane::getPorosity, "Returns the porosity of the membrane [0.0-1.0].")
		.def("getNumberOfPores", &arch::Membrane::getNumberOfPores, "Returns the number of pores in a given area [m^2] of the membrane.")
		.def("getPoreDensity", &arch::Membrane::getPoreDensity, "Returns the density of the pores in the membrane.")
		.def("getArea", &arch::Membrane::getArea, "Returns the area of the membrane [m^2].")
		.def("getVolume", &arch::Membrane::getVolume, "Returns the volume of the membrane [m^3].");

	py::class_<arch::Tank<T>, arch::Edge<T>, py::smart_holder>(m, "Tank")
		.def("setDimensions", &arch::Tank::setDimensions, "Set the dimensions of the tank [w, h, l] [m].")
		.def("setHeight", &arch::Tank::setHeight, "Set the height of the tank [m].")
		.def("setWidth", &arch::Tank::setWidth, "Set the width of the tank [m].")
		.def("setLength", &arch::Tank::setLength, "Set the length of the tank [m].")
		.def("getHeight", &arch::Tank::getHeight, "Returns the height of the tank [m].")
		.def("getWidth", &arch::Tank::getWidth, "Returns the width of the tank [m].")
		.def("getLength", &arch::Tank::getLength, "Returns the length of the tank [m].")
		.def("getArea", &arch::Tank::getArea, "Returns the area of the tank [m^2].")
		.def("getVolume", &arch::Tank::getVolume, "Returns the volume of the tank [m^3].");

	py::class_<arch::Module<T>, py::smart_holder>(m, "Module")
		.def("getId", &arch::Module::getId, "Returns the id of the module.")
		.def("getPosition", &arch::Module::getPosition, "Returns the position [x, y] of the bottom left corner w.r.t. the device [m].")
		.def("getSize", &arch::Module::getSize, "Returns the size [x, y] of the module [m].")
		.def("getNodes", &arch::Module::getNodes, "Returns a map of nodes that are on the module's edges.")
		.def("setModuleTypeLbm", &arch::Module::setModuleTypeLbm, "Set the type of the module to lbm simulator.")
		.def("getModuleType", &arch::Module::getModuleType, "Returns the type of the module.");

	py::class_<arch::CfdModule<T>, arch::Module<T>, py::smart_holder>(m, "CfdModule")
		.def("assertInitialized", &arch::CfdModule::assertInitialized, "Checks whether the CFD module has valid openings and STL definitions.")
		.def("getNetwork", &arch::CfdModule::getNetwork, "Returns a graph network that fully connects the module's boundary nodes.")
		.def("getOpenings", &arch::CfdModule::getOpenings, "Returns a map of the openings of the module.")
		.def("getStlFile", &arch::Module::getStlFile, "Returns the STL file that defines the CFD domain.");

	py::class_<arch::Network<T>, py::smart_holder>(m, "Network")
		.def("isNetworkValid", &arch::Network<T>::isNetworkValid, "Check if the current network is valid.")
		.def("", &arch::Network<T>::, "")
		.def("print", &arch::Network<T>::print, "Print the content of this network.")
		// Nodes
		.def("addNode", py::overload_cast<T, T, bool>(&arch::Network<T>::addNode), "Add a new node to the network.")
		.def("getNode", &arch::Network<T>::getNode, "Returns the node with the given id.")
		.def("getNodes", &arch::Network<T>::getNodes, "Returns all nodes in the network.")
		.def("getGroundNodes", &arch::Network<T>::getGroundNodes, "Returns the set of nodes that are ground nodes.")
		.def("getGroundNodeIds", &arch::Network<T>::getGroundNodeIds, "Returns the ids of the set of ground nodes.")
		.def("getVirtualNodes", &arch::Network<T>::getVirtualNodes, "Returns the amount of virtual nodes in the network.")
		.def("setVirtualNodes", &arch::Network<T>::setVirtualNodes, "Sets the amount of virtual nodes in the network.")
		.def("hasNode", py::overload_cast<size_t>(&arch::Network<T>::addNode), "Returns true if the network contains the node with the given id.")
		.def("hasNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::addNode), "Returns true if the network contains the given node.")
		.def("setSink", py::overload_cast<size_t>(&arch::Network<T>::setSink), "Sets the node with given id to be a sink node.")
		.def("setSink", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::setSink), "Sets the given node to be a sink node.")
		.def("isSink", py::overload_cast<size_t>(&arch::Network<T>::isSink), "Returns true if the node of the given id is a sink node.")
		.def("isSink", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::isSink), "Returns true if the given node is a sink node.")
		.def("setGround", py::overload_cast<size_t>(&arch::Network<T>::setGround), "Sets the node with given id to be a ground node.")
		.def("setGround", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::setGround), "Sets the given node to be a ground node.")
		.def("isGround", py::overload_cast<size_t>(&arch::Network<T>::isGround), "Returns true if the node of the given id is a ground node.")
		.def("isGround", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::isGround), "Returns true if the given node is a ground node.")
		.def("calculateNodeDistance", py::overload_cast<size_t, size_t>(&arch::Network<T>::calculateNodeDistance), 
			"Calculates the Euclidean distance between the nodes of the given ids.")
		.def("calculateNodeDistance", py::overload_cast<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>>(&arch::Network<T>::calculateNodeDistance), 
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
		.def("getChannelsAtNode", py::overload_cast<size_t>(&arch::Network<T>::getChannelsAtNode), 
				"Returns all channels in the network that are connected to the node with the given id.")
		.def("getChannelsAtNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::getChannelsAtNode), 
			"Returns all channels in the network that are connected to the given node.")
		.def("isChannel", py::overload_cast<int>(&arch::Network<T>::isChannel), "Returns true if the edge with the given id is a channel.")
		.def("isChannel", py::overload_cast<const std::shared_ptr<arch::Edge<T>>&>(&arch::Network<T>::isChannel), "Returns true if the given edge is a channel.")
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
		.def("getMembraneBetweenNodes", py::overload_cast<size_t, size_t>(&arch::Network<T>::getMembraneBetweenNodes), 
			"Returns the membrane between two given nodes.")
		.def("getMembraneBetweenNodes", py::overload_cast<const std::shared_ptr<Node<T>>&, const std::shared_ptr<Node<T>>&>(&arch::Network<T>::getMembraneBetweenNodes), 
			"Returns the membrane between two given nodes.")
		.def("getMembranes", &arch::Network<T>::getMembranes, "Returns all membranes in the network.")
		.def("getMembranesAtNode", py::overload_cast<size_t>(&arch::Network<T>::getMembranesAtNode), 
			"Returns the set of membranes that are at the given node.")
		.def("getMembranesAtNode", py::overload_cast<std::shared_ptr<arch::Node<T>>>(&arch::Network<T>::getMembranesAtNode), 
			"Returns the set of membranes that are at the given node.")
		.def("isMembrane", &arch::Network<T>::isMembrane, "Returns true if the given edge id belongs to a membrane.")
		// Tank
		.def("addTankToMembrane", py::overload_cast<size_t, T, T, T, T>(&arch::Network<T>::addTankToMembrane), "Adds a tank to a membrane in the network.")
		.def("addTankToMembrane", py::overload_cast<const std::shared_ptr<arch::Channel<T>>&, T, T, T, T>(&arch::Network<T>::addTankToMembrane), 
			"Adds a tank to a membrane in the network.")
		.def("getTank", &arch::Network<T>::getTank, "Returns the tank with the given id.")
		.def("getTankBetweenNodes", py::overload_cast<size_t, size_t>(&arch::Network<T>::getTankBetweenNodes), 
			"Returns the tank between two given nodes.")
		.def("getTankBetweenNodes", py::overload_cast<const std::shared_ptr<Node<T>>&, const std::shared_ptr<Node<T>>&>(&arch::Network<T>::getTankBetweenNodes), 
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

	py::class_<sim::Fluid<T>, py::smart_holder>(m, "Fluid")
		.def("getId", &sim::Fluid<T>::getId, "Returns the id of the fluid.")
		.def("setName", &sim::Fluid<T>::setName, "Sets the name of the fluid.")
		.def("getName", &sim::Fluid<T>::getName, "Returns the name of the fluid.")
		.def("setViscosity", &sim::Fluid<T>::setViscosity, "Sets the viscosity of the fluid.")
		.def("getViscosity", &sim::Fluid<T>::getViscosity, "Returns the viscosity of the fluid.")
		.def("setDensity", &sim::Fluid<T>::setDensity, "Sets the density of the fluid.")
		.def("getDensity", &sim::Fluid<T>::getDensity, "Returns the density of the fluid.");

	py::class_<sim::Droplet<T>, py::smart_holder>(m, "Droplet")
		.def("getId", &sim::Droplet<T>::getId, "Returns the id of the droplet.")
		.def("getName", &sim::Droplet<T>::getName, "Returns the name of the droplet.")
		.def("setName", &sim::Droplet<T>::setName, "Sets the name of the droplet.")
		.def("getVolume", &sim::Droplet<T>::getVolume, "Returns the volume of the droplet [m^3].")
		.def("setVolume", &sim::Droplet<T>::setVolume, "Sets the volume of the droplet [m^3].")
		.def("readFluid", &sim::Droplet<T>::readFluid, "Returns a read-only reference to the Fluid that constitutes the droplet.")
		.def("setFluid", &sim::Droplet<T>::setFluid, "Sets the fluid that the droplet consists of.")
		.def("readBoundaries", &sim::Droplet<T>::readBoundaries, "Returns a read-only list of the droplet boundaries.")
		.def("readFullyOccupiedChannels", &sim::Droplet<T>::readFullyOccupiedChannels, 
			"Returns a read-only list of the channels that the droplet occupies fully.");

	py::class_<sim::Specie<T>, py::smart_holder>(m, "Specie")
		.def("getId", &sim::Specie<T>::getId, "Returns the id of the specie.")
		.def("setName", &sim::Specie<T>::setName, "Sets the name of the specie.")
		.def("getName", &sim::Specie<T>::getName, "Returns the name of the specie.")
		.def("setDiffusivity", &sim::Specie<T>::setDiffusivity, "Sets the diffusion coefficient of this species.")
		.def("getDiffusivity", &sim::Specie<T>::getDiffusivity, "Returns the diffusion coefficient of this species.")
		.def("setSatConc", &sim::Specie<T>::setSatConc, "Sets the saturation concentration of this specie.")
		.def("getSatConc", &sim::Specie<T>::getSatConc, "Returns the saturation concentration of this specie.");
	
	py::class_<sim::Mixture<T>, py::smart_holder>(m, "Mixture")
		.def(py::self == py::self)
		.def("getId", &sim::Mixture<T>::getId, "Returns the id of the mixture.")
		.def("getName", &sim::Mixture<T>::getName, "Returns the name of the mixture.")
		.def("setName", &sim::Mixture<T>::setName, "Sets the name of the mixture.")
		//.def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		.def("addSpecie", &sim::Mixture<T>::addSpecie, "Adds a specie with a given concentration to this mixture.")
		// .def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		// .def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		.def("readSpecieConcentrations", &sim::Mixture<T>::readSpecieConcentrations, "Returns a read-only map of the specie concentrations.")
		// .def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		.def("getConcentrationOfSpecie", &sim::Mixture<T>::getConcentrationOfSpecie, "Returns the concentration of a specie [TODO].")
		.def("getSpecieDistributions", &sim::Mixture<T>::getSpecieDistributions, "Returns the concentration distribution of a species if it's not a constant value [TODO].")
		// .def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		.def("setSpecieConcentration", &sim::Mixture<T>::setSpecieConcentration, "Sets the concentration of a specie [TODO].")
		.def("getSpecieCount", &sim::Mixture<T>::getSpecieCount, "Returns the number of species listed in the mixture.")
		// .def("", &sim::Mixture<T>::, "") <- Shouldn't be able to own a raw pointer
		.def("removeSpecie", &sim::Mixture<T>::removeSpecie, "Removes a specie from the mixture.")
		.def("getDensity", &sim::Mixture<T>::getDensity, "Returns the density of the mixture [kg/m^3].")
		.def("getViscosity", &sim::Mixture<T>::getViscosity, "Returns the viscosity of the mixture [Pa s].")
		.def("getLargestMolecularSize", &sim::Mixture<T>::getLargestMolecularSize, "Returns the largest molecular size of the species in the mixture.");

	py::class_<sim::DiffusiveMixture<T>, sim::Mixture<T>, py::smart_holder>(m, "DiffusiveMixture")
		.def("setResolution", &sim::DiffusiveMixture<T>::setResolution, "Sets the spectral resolution of the distribution function.")
		.def("getResolution", &sim::DiffusiveMixture<T>::getResolution, "Returns the spectral resolution of the distribution function.")

	py::class_<sim::DropletInjection<T>, py::smart_holder>(m, "DropletInjection")
		.def("getId", &sim::DropletInjection<T>getId, "Returns the id of the droplet injection.")
		.def("getName", &sim::DropletInjection<T>getName, "Returns the name of the droplet injection.")
		.def("setName", &sim::DropletInjection<T>setName, "Sets the name of the droplet inkection.")
		.def("getInjectionTime", &sim::DropletInjection<T>getInjectionTime, "Returns the time at which the droplet is injected [s].")
		.def("setInjectionTime", &sim::DropletInjection<T>setInjectionTime, "Sets the time at which the droplet should be injected [s].")
		// Encapsulation -> Why do we set and return the ChannelPosition object, and not just channel and position parameters?
		.def("getInjectionPosition", &sim::DropletInjection<T>getInjectionPosition, "Returns the channel and position at which the droplet is injected.")
		.def("setInjectionPosition", &sim::DropletInjection<T>setInjectionPosition, "Sets the channel and position at which the droplet is injected.");

	py::class_<sim::MixtureInjection<T>, py::smart_holder>(m, "MixtureInjection")
		.def("getId", &sim::MixtureInjection<T>getId, "Returns the id of the mixture injection.")
		.def("getName", &sim::MixtureInjection<T>getName, "Returns the name of the mixture injection.")
		.def("setName", &sim::MixtureInjection<T>setName, "Sets the name of the mixture inkection.")
		.def("getInjectionTime", &sim::MixtureInjection<T>getInjectionTime, "Returns the time at which the mixture is injected [s].")
		.def("setInjectionTime", &sim::MixtureInjection<T>setInjectionTime, "Sets the time at which the mixture should be injected [s].")
		.def("getInjectionChannel", &sim::MixtureInjection<T>getInjectionChannel, "Returns the channel into which the mixture is injected.")
		.def("setInjectionChannel", &sim::MixtureInjection<T>setInjectionChannel, "Sets the channel into which the mixture is injected.");

	py::class_<sim::CFDSimulator<T>, py::smart_holder>(m, "CFDSimulator")
		.def("getId", &sim::CFDSimulator<T>::getId, "Returns the id of the CFD simulator.")
		.def("getModule", &sim::CFDSimulator<T>::getModule, "Returns the module on which the simulator acts.")
		.def("getGroundNodes", &sim::CFDSimulator<T>::getGroundNodes, "Returns whether boundary nodes communicate the pressure (true) or flow rates (false) to the 1D solver.")
		// .def("getInitialized", &sim::CFDSimulator<T>::getInitialized, "Returns whether the simulator is initialized or not.")
		// .def("setInitialized", &sim::CFDSimulator<T>::, "")
		.def("setVtkFolder", &sim::CFDSimulator<T>::setVtkFolder, "Sets the folder in which the vtk output should be stored.")
		.def("getVtkFile", &sim::CFDSimulator<T>::getVtkFile, "Returns the location where the last vtk file was created.")
		.def("getAlpha", &sim::CFDSimulator<T>::getAlpha, "Returns the relaxation factor for pressure updates.")
		.def("getBeta", &sim::CFDSimulator<T>::getBeta, "Returns the relaxation factor for flow rate updates.")
		// .def("storeConcentrations", &sim::CFDSimulator<T>::, "")
		// .def("getConcentrations", &sim::CFDSimulator<T>::, "")
		// .def("setBoundaryValues", &sim::CFDSimulator<T>::, "")
		.def("writeVtk", &sim::CFDSimulator<T>::writeVTK, "Write a vtk file with the current CFD simulation results.")
		.def("writePressurePpm", &sim::CFDSimulator<T>::writePressurePpm, "Write the ppm image of the pressure results.")
		.def("writeVelocityPpm", &sim::CFDSimulator<T>::writeVelocityPpm, "Write the ppm image of the velocity results.")
		.def("getPressureBounds", &sim::CFDSimulator<T>::getPressureBounds, "Returns the pressre bounds of the simulator.")
		.def("getVelocityBounds", &sim::CFDSimulator<T>::getVelocityBounds, "Returns the velocity bounds of the simulator.")
		// .def("storeCfdResults", &sim::CFDSimulator<T>::, "");

	py::class_<sim::lbmSimulator, sim::CFDSimulator<T>, py::smart_holder>(m, "lbmSimulator")
		.def("getCharPhysLength", &sim::lbmSimulator<T>::getCharPhysLength, "Returns the characteristic physical length of the lbm simulator.")
		.def("getCharPhysVelocity", &sim::lbmSimulator<T>::getCharPhysVelocity, "Returns the characteristic physical velocity of the lbm simulator.")
		.def("getResolution", &sim::lbmSimulator<T>::getResolution, "Returns the resolution of the lbm simulator.")
		.def("setResolution", &sim::lbmSimulator<T>::setResolution, "Sets the resolution of the lbm simulator.")
		.def("getEpsilon", &sim::lbmSimulator<T>::getEpsilon, "Returns the epsilon for the simulator.")
		.def("setEpsilon", &sim::lbmSimulator<T>::setEpsilon, "Sets the epsilon for the simulator.")
		.def("getTau", &sim::lbmSimulator<T>::getTau, "Returns the relaxation time of the simulator.")
		.def("setTau", &sim::lbmSimulator<T>::setTau, "Sets the relaxation time of the simulator.")
		.def("getStepIter", &sim::lbmSimulator<T>::getStepIter, "Returns the number of steps used for the value tracer (default = 1000).")
		.def("hasConverged", &sim::lbmSimulator<T>::hasConverged, "Returns whether the simulator has converged or not.");

	py::class_<sim::Simulation, py::smart_holder>(m, "Simulation")
		.def("getPlatform", &sim::Simulation<T>::getPlatorm, "Returns the platform of the simulation.")
		.def("getType", &sim::Simulation<T>::getType, "Returns the type of simulation [Abstract, Hybrid CFD].")
		.def("getNetwork", &sim::Simulation<T>::getNetwork, "Returns the network on which the simulation is conducted.")
		.def("setNetwork", &sim::Simulation<T>::setNetwork, "Sets the network on which the simulation is conducted.")
		.def("set1DResistanceModel", &sim::Simulation<T>::set1DResistanceModel, "Sets the resistance model for abstract simulation to the 1D resistance model.")
		.def("setPoiseuilleResistanceModel", &sim::Simulation<T>::setPoiseuilleResistanceModel, "Sets the resistance model for abstract simulation components to the poiseuille resistance model.")
		.def("addFluid", &sim::Simulation<T>::addFluid, "Adds a fluid to the simulation.")
		.def("addMixedFluid", &sim::Simulation<T>::addMixedFluid, "Creates and adds a new fluid from two existing fluids.")
		.def("getFluid", &sim::Simulation<T>::getFluid, "Returns the fluid for the given id.")
		.def("readFluids", &sim::Simulation<T>::readFluids, "Returns a read-only list of fluids.")
		.def("removeFluid", &sim::Simulation<T>::removeFluid, "Removes a fluid from the simulation.")
		.def("getFixtureId", &sim::Simulation<T>::getFixtureId, "Returns the fixture id.")
		.def("setFixtureId", &sim::Simulation<T>::setFixtureId, "Sets the fixture id.")
		.def("getContinuousPhase", &sim::Simulation<T>::getContinuousPhase, "Returns the continuous phase of the simulation.")
		.def("setContinuousPhase", py::overload_cast<int>(&sim::Simulation<T>::setContinuousPhase), "Set the continuous phase of the simulation to the fluid with the given id.")
		.def("setContinuousPhase", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&>(&sim::Simulation<T>::setContinuousPhase), "Set the continuous phase of the simulation to the given fluid.")
		.def("getCurrentIteration", &sim::Simulation<T>::getCurrentIteration, "Returns the current iteration number.")
		.def("getMaxIterations", &sim::Simulation<T>::getMaxIterations, "Returns the maximum number of allowed iterations. When reached the simulation ends.")
		.def("setWriteInterval", &sim::Simulation<T>::setWriteInterval, "Set the iterations interval at which the stat is saved in the results.")
		.def("getTMax", &sim::Simulation<T>::getTMax, "Returns the maximum allowed physical time. When reached the simulation ends.")
		.def("setMaxEndTime", &sim::Simulation<T>::setMaxEndTime, "Set the maximal physical time after which the simulation ends.")
		.def("getResults", &sim::Simulation<T>::getResults, "Returns the results of the simulation.")
		.def("printResults", &sim::Simulation<T>::printResults, "Prints the results of the simulation to the console.")
		.def("simulate", &sim::Simulation<T>::simulate, "Conducts the simulation.");

	py::class_<sim::AbstractContinuous, sim::Simulation, py::smart_holder>(m, "AbstractContinuous")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractContinuous<T>>(dynamic_cast<sim::AbstractContinuous<T>*>(tmpPtr.release()));
			}));

	py::class_<sim::AbstractDroplet, sim::Simulation, py::smart_holder>(m, "AbstractDroplet")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractDroplet<T>>(dynamic_cast<sim::AbstractDroplet<T>*>(tmpPtr.release()));
			}))
		.def("addDroplet", py::overload_cast<int, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet of the given fluid id to the simulation.")
		.def("addDroplet", py::overload_cast<const std::shared_ptr<Fluid<T>>&, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet to the simulation.")
		.def("addMergedDroplet", py::overload_cast<int, int>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on the ids of two existing droplets.")
		.def("addMergedDroplet", py::overload_cast<const std::shared_ptr<Droplet<T>>&, const std::shared_ptr<Droplet<T>>&>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on two existing droplets.")
		.def("getDroplet", &sim::AbstractDroplet<T>::getDroplet, "Returns the droplet with the given id.")
		.def("getDropletAtNode", py::overload_cast<int>(&sim::AbstractDroplet<T>::getDropletAtNode), 
			"Checks whether a droplet is present at the node with the given id. Returns true and the droplet if one is found.")
		.def("getDropletAtNode", py::overload_cast<const std::shared_ptr<arch::Node<T>>&>(&sim::AbstractDroplet<T>::getDropletAtNode), 
			"Checks whether a droplet is present at the given node. Returns true and the droplet if one is found.")
		.def("removeDroplet", &sim::AbstractDroplet<T>::removeDroplet, "Removes a droplet from the simulation.")
		.def("addDropletInjection", py::overload_cast<int, T, int, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("addDropletInjection", py::overload_cast<const std::shared_ptr<Droplet<T>>&, T, const std::shared_ptr<arch::Channel<T>>&, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("getDropletInjection", &sim::AbstractDroplet<T>::getDropletInjection, "Returns the droplet injection with the given id.")
		.def("removeDropletInjection", &sim::AbstractDroplet<T>::removeDropletInjection, "Removes the droplet injection from the simulation.");

	py::class_<sim::AbstractMembrane, sim::Simulation, py::smart_holder>(m, "AbstractMembrane")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractMembrane<T>>(dynamic_cast<sim::AbstractMembrane<T>*>(tmpPtr.release()));
			}))
		.def("setPermeabilityMembraneModel", &sim::AbstractMembrane<T>::setPermeabilityMembraneModel, "Sets the permeability membrane model.")
		.def("setPoreResistanceMembraneModel", &sim::AbstractMembrane<T>::setPoreResistanceMembraneModel, "Sets the pore resistance membrane model.")
		.def("setMembraneModel0", &sim::AbstractMembrane<T>::setMembraneModel0, "Sets membrane model 0.")
		.def("setMembraneModel1", &sim::AbstractMembrane<T>::setMembraneModel1, "Sets membrane model 1.")
		.def("setMembraneModel2", &sim::AbstractMembrane<T>::setMembraneModel2, "Sets membrane model 2.")
		.def("setMembraneModel3", &sim::AbstractMembrane<T>::setMembraneModel3, "Sets membrane model 3.")
		.def("setMembraneModel4", &sim::AbstractMembrane<T>::setMembraneModel4, "Sets membrane model 4.")
		.def("setMembraneModel5", &sim::AbstractMembrane<T>::setMembraneModel5, "Sets membrane model 5.")
		.def("setMembraneModel6", &sim::AbstractMembrane<T>::setMembraneModel6, "Sets membrane model 6.")
		.def("setMembraneModel7", &sim::AbstractMembrane<T>::setMembraneModel7, "Sets membrane model 7.")
		.def("setMembraneModel8", &sim::AbstractMembrane<T>::setMembraneModel8, "Sets membrane model 8.")
		.def("setMembraneModel9", &sim::AbstractMembrane<T>::setMembraneModel9, "Sets membrane model 9.")
		.def("getMembraneResistance", &sim::AbstractMembrane<T>::getMembraneResistance, "Returns the membrane resistance.")

	py::class_<sim::AbstractMixing, sim::Simulation, py::smart_holder>(m, "AbstractMixing")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractMixing<T>>(dynamic_cast<sim::AbstractMixing<T>*>(tmpPtr.release()));
			}))
		.def("setInstantaneousMixingModel", &sim::AbstractMixing<T>::setInstantaneousMixingModel, "Sets the instantaneous mixing model.")
		.def("hasInstantaneousMixingModel", &sim::AbstractMixing<T>::hasInstantaneousMixingModel, "Returns whether an instantaneous mixing model was set.")
		.def("setDiffusiveMixingModel", &sim::AbstractMixing<T>::setDiffusiveMixingModel, "Sets the diffusive mixing model.")
		.def("hasDiffusiveMixingModel", &sim::AbstractMixing<T>::hasDiffusiveMixingModel, "Returns whether a diffusive mixing model was set.")
		.def("addSpecie", &sim::AbstractMixing<T>::addSpecie, "Adds a single species to the simulator.")
		.def("getSpecie", &sim::AbstractMixing<T>::getSpecie, "Returns a species with the given id.")
		.def("removeSpecie", &sim::AbstractMixing<T>::removeSpecies, "Removes a species from the simulator.")
		.def("addMixture", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&, T>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("addMixture", py::overload_cast<const std::vector<std::shared_ptr<sim::Specie<T>>>&, const std::vector<T>&>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("getMixture", &sim::AbstractMixing<T>::getMixture, "Returns the mixture with the given id.")
		.def("readMixtures", &sim::AbstractMixing<T>::readMixtures, "Returns a read-only list of mixtures in the simulator.")
		.def("removeMixture", &sim::AbstractMixing<T>::removeMixture, "Removes a mixture from the simulator.")
		.def("addMixtureInjection", py::overload_cast<int, int, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixtureId"), py::arg("edgeId"), py::arg("injectionTime"), py::arg("isPermanent")=false, 
			"Add a mixture injection to the simulator.")
		.def("addMixtureInjection", py::overload_cast<const std::shared_ptr<Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixture"), py::arg("edge"), 
			py::arg("injectionTime"), py::arg("isPermanent")=false, "Add a mixture injection to the simulator")
		.def("addPermanentMixtureInjection", py::overload_cast<int, int, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), "Add a permanent mixture injection to the simulator.")
		.def("addPermanentMixtureInjection", py::overload_cast<const std::shared_ptr<Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), 
			"Add a permanent mixture injection to the simulator.")
		.def("getMixtureInjection", &sim::AbstractMixing<T>::getMixtureInjection, "Returns the mixture injection with the given id.")
		.def("removeMixtureInjection", &sim::AbstractMixing<T>::removeMixtureInjection, "Removes the mixture injection with the given id.");

	py::class_<sim::HybridContinuous, sim::Simulation, py::smart_holder>(m, "HybridContinuous")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::HybridContinuous<T>>(dynamic_cast<sim::HybridContinuous<T>*>(tmpPtr.release()));
			}))
		.def("getCharacteristicLength", &sim::HybridContinuous<T>::getCharacteristicLength, "Returns the characteristic length of the LBM simulators.")
		.def("getCharacteristicVelocity", &sim::HybridContinuous<T>::getCharacteristicVelocity, "Returns the characteristic velocity of the LBM simulators.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("name")="", "Add a LBM simulator to the hybrid simulation.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, size_t, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("resolution"), py::arg("name")="", "Add a LBM simulator to the hybrid simulation.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, size_t, T, T, T, T, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("resolution"), py::arg("epsilon"), py::arg("tau"), py::arg("charPhysLength"), py::arg("charPhysVelocity"), py::arg("name")="", 
			"Add a LBM simulator to the hybrid simulation.")
		.def("getLbmSimulator", &sim::HybridContinuous<T>::getLbmSimulator, "Returns the LBM simulator with the given id.")
		.def("removeLbmSimulator", &sim::HybridContinuous<T>::removeLbmSimulator, "Removes the LBM simulator from the hybrid simulation.")
		.def("setNaiveHybridScheme", py::overload_cast<T, T, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), "Set the naive update scheme for all simulators.")
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<CFDSimulator<T>>&, T, T, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<CFDSimulator<T>>&, std::unordered_map<int, T>, std::unordered_map<int, T>, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("getGlobalPressureBounds", &sim::HybridContinuous<T>::getGlobalPressureBounds, "Returns the global pressure bounds in the CFD simulators.")
		.def("getGlobalVelocityBounds", &sim::HybridContinuous<T>::getGlobalVelocityBounds, "Returns the global velocity bounds in the CFD simulators.")
		.def("writePressurePpm", &sim::HybridContinuous<T>::writePressurePpm, "Write the pressure field in ppm format for all simulators.")
		.def("writeVelocityPpm", &sim::HybridContinuous<T>::writeVelocityPpm, "Write the velocity field in ppm format for all simulators.");
		
	#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
	#else
	m.attr("__version__") = "dev";
	#endif
}
