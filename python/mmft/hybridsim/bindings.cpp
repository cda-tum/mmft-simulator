#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <baseSimulator.h>
#include <baseSimulator.hh>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using T = double;

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pyhybridsim, m) {
	m.doc() = "Python binding for the MMFT-Simulator.";

	py::enum_<arch::ChannelType>(m, "ChannelType")
		.value("normal", arch::ChannelType::NORMAL)
		.value("bypass", arch::ChannelType::BYPASS)
		.value("cloggable", arch::ChannelType::CLOGGABLE);

	py::enum_<sim::Type>(m, "Type")
		.value("abstract", sim::Type::Abstract)
		.value("hybrid", sim::Type::Hybrid);

	py::enum_<sim::Platform>(m, "Platform")
		.value("continuous", sim::Platform::Continuous)
		.value("bigDroplet", sim::Platform::BigDroplet);

	py::class_<arch::Node<T>>(m, "Node")
		.def(py::init<int, T, T, bool>());

	py::class_<arch::Network<T>>(m, "Network")
		.def(py::init<>())
		.def("sort", &arch::Network<T>::sortGroups, "Sort the nodes, channels and modules of the network.")
		.def("valid", &arch::Network<T>::isNetworkValid, "Check if the current network is valid.")
		.def("addNode", &arch::Network<T>::addNodePyBind, "Add a new node to the network.")
		.def("addChannel", &arch::Network<T>::addChannelPyBind, "Add a new channel to the network.")
		.def("addFlowRatePump", &arch::Network<T>::addFlowRatePump, "Add a new flow rate pump to the network.")
		.def("setFlowRatePump", &arch::Network<T>::setFlowRatePump, "Turn a channel into a flow rate pump with given flow rate.")
		.def("addPressurePump", &arch::Network<T>::addPressurePump, "Add a new pressure pump to the network.")
		.def("setPressurePump", &arch::Network<T>::setPressurePump, "Turn a channel into a pressure pump with given pressure.")
		.def("addModule", &arch::Network<T>::addModulePyBind, "Add a new LBM module to the network.");;

	py::class_<sim::Fluid<T>>(m, "Fluid")
		.def(py::init<int &, T &, T &, T&>())
		.def("setName", &sim::Fluid<T>::setName, "name"_a)
		.def("getConcentration", &sim::Fluid<T>::getConcentration)
		.def("getDensity", &sim::Fluid<T>::getDensity)
		.def("getViscosity", &sim::Fluid<T>::getViscosity);
	
	py::class_<sim::Droplet<T>>(m, "Droplet")
		.def(py::init<int, T, sim::Fluid<T>*>());

	py::class_<sim::ResistanceModelPoiseuille<T>>(m, "ResistanceModelPoiseuille")
		.def(py::init<T &>());
		
	py::class_<sim::ResistanceModel1D<T>>(m, "ResistanceModelRectangular")
		.def(py::init<T &>());

	py::class_<sim::Simulation<T>>(m, "Simulation")
		.def(py::init<>())
		.def("setPlatform", &sim::Simulation<T>::setPlatform)
		.def("setType", &sim::Simulation<T>::setType)
		.def("setNetwork", &sim::Simulation<T>::setNetwork)
		.def("addFluid", &sim::Simulation<T>::addFluid)
		.def("addDroplet", &sim::Simulation<T>::addDropletPyBind)
		.def("addDropletInjection", &sim::Simulation<T>::addDropletInjection)
		.def("setContinuousPhase", py::overload_cast<sim::Fluid<T>*>(&sim::Simulation<T>::setContinuousPhase))
		.def("setResistanceModel", &sim::Simulation<T>::setResistanceModel)
		.def("simulate", &sim::Simulation<T>::simulate)
		.def("print", &sim::Simulation<T>::printResults);

	m.def("loadNetwork", py::overload_cast<std::string, arch::Network<T>*>(&porting::networkFromJSON<T>));
	m.def("loadSimulation", py::overload_cast<std::string, arch::Network<T>*>(&porting::simulationFromJSON<T>));
	m.def("saveResult", py::overload_cast<std::string, sim::Simulation<T>*>(&porting::resultToJSON<T>));

	#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
	#else
	m.attr("__version__") = "dev";
	#endif
}
