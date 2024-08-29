#include <pybind11/pybind11.h>
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

	py::enum_<sim::Type>(m, "Type")
		.value("abstract", sim::Type::Abstract)
		.value("hybrid", sim::Type::Hybrid);

	py::enum_<sim::Platform>(m, "Platform")
		.value("continuous", sim::Platform::Continuous)
		.value("bigDroplet", sim::Platform::BigDroplet);

	py::class_<arch::Network<T>>(m, "Network")
		.def(py::init<>())
		.def("sort", &arch::Network<T>::sortGroups, "Sort the nodes, channels and modules of the network.")
		.def("valid", &arch::Network<T>::isNetworkValid, "Check if the current network is valid.")
		.def("addNode", [](arch::Network<T> &network, T x, T y, bool ground) {
			return network.addNode(x, y, ground)->getId();
			}, "Add a new node to the network.")
		.def("addNode", [](arch::Network<T> &network, T x, T y, bool ground, bool sink) {
			int id = network.addNode(x, y, ground)->getId();
			network.setSink(id);
			return id;
			}, "Add a new node to the network.")
		.def("addChannel", [](arch::Network<T> &network, int nodeAId, int nodeBId, T width, T height, arch::ChannelType type) {
			return network.addChannel(nodeAId, nodeBId, height, width, type)->getId();
			}, "Add a new channel to the network.")
		.def("addFlowRatePump", [](arch::Network<T> &network, int nodeAId, int nodeBId, T flowRate) {
			return network.addFlowRatePump(nodeAId, nodeBId, flowRate)->getId();
			}, "Add a new flow rate pump to the network.")
		.def("setFlowRatePump", &arch::Network<T>::setFlowRatePump, "Turn a channel into a flow rate pump with given flow rate.")
		.def("addPressurePump", [](arch::Network<T> &network, int nodeAId, int nodeBId, T pressure) {
			return network.addPressurePump(nodeAId, nodeBId, pressure)->getId();
			}, "Add a new pressure pump to the network.")
		.def("setPressurePump", &arch::Network<T>::setPressurePump, "Turn a channel into a pressure pump with given pressure.")
		.def("addModule", [](arch::Network<T> &network,
								std::vector<T> position,
								std::vector<T> size,
								std::vector<int> nodes) {

			std::unordered_map<int, std::shared_ptr<arch::Node<T>>> newNodes;

			for (long unsigned int i=0; i<nodes.size(); ++i) {
				newNodes.try_emplace(nodes[i], network.getNode(nodes[i]));
			}

			return network.addModule(position, size, newNodes)->getId();

			}, "Add a new module to the network.")
		.def("loadNetwork", [](arch::Network<T> &network, std::string file) { 
			porting::networkFromJSON(file, network);
		});

	py::class_<sim::Simulation<T>>(m, "Simulation")
		.def(py::init<>())
		.def("setPlatform", &sim::Simulation<T>::setPlatform)
		.def("setType", &sim::Simulation<T>::setType)
		.def("setNetwork", &sim::Simulation<T>::setNetwork)
		.def("addFluid", [](sim::Simulation<T> &simulation, T density, T viscosity, T concentration) {
				return simulation.addFluid(viscosity, density, concentration)->getId();
			})
		.def("addDroplet", [](sim::Simulation<T> &simulation, int fluidId, T volume) {
				return simulation.addDroplet(fluidId, volume)->getId();
			})
		.def("addLbmSimulator", [](	sim::Simulation<T> &simulation, 
									std::string name,
									std::string stlFile,
									int moduleId,
									std::vector<int> nodes,
									std::vector<std::vector<T>> normals,
									std::vector<T> widths,
									T charPhysLength,
									T charPhysVelocity,
									T resolution,
									T epsilon,
									T tau) {

			std::unordered_map<int, arch::Opening<T>> openings;

			// ASSERT equal length for nodes, normals, widths and heights
			if (nodes.size() != normals.size() || nodes.size() != widths.size()) {
				throw std::invalid_argument("There should be an equal amount of nodes, normals, and widths");
			}

			// ASSERT equal length for nodes, normals, widths and heights
			if (nodes.size() != simulation.getNetwork()->getModule(moduleId)->getNodes().size()) {
				throw std::invalid_argument("There should be an equal amount of nodes passed as in the provided module.");
			}

			for (long unsigned int i=0; i<nodes.size(); ++i) {
				openings.try_emplace(nodes[i], arch::Opening<T>{simulation.getNetwork()->getNode(nodes[i]), normals[i], widths[i]});
			}
			
			return simulation.addLbmSimulator(	name, stlFile, simulation.getNetwork()->getModule(moduleId), openings, charPhysLength,
												charPhysVelocity, resolution, epsilon, tau)->getId();

			}, "Add a LBM simulator to the simulation.")
		.def("injectDroplet", [](sim::Simulation<T> &simulation, int dropletId, T injectionTime, int channelId, T injectionPosition) {
				simulation.addDropletInjection(dropletId, injectionTime, channelId, injectionPosition);
			})
		.def("setContinuousPhase", py::overload_cast<int>(&sim::Simulation<T>::setContinuousPhase))
		.def("setRectangularResistanceModel", [](sim::Simulation<T> & simulation) {
				sim::ResistanceModel1D<T>* resistanceModel = new sim::ResistanceModel1D<T>(simulation.getContinuousPhase()->getViscosity());
				simulation.setResistanceModel(resistanceModel);
			})
		.def("setPoiseuilleResistanceModel", [](sim::Simulation<T> & simulation) {
				sim::ResistanceModelPoiseuille<T>* resistanceModel = new sim::ResistanceModelPoiseuille<T>(simulation.getContinuousPhase()->getViscosity());
				simulation.setResistanceModel(resistanceModel);
			})
		.def("setNaiveScheme", [](sim::Simulation<T> & simulation, T alpha, T beta, int theta) {
				simulation.setNaiveHybridScheme(alpha, beta, theta);
			})
		.def("simulate", &sim::Simulation<T>::simulate)
		.def("print", &sim::Simulation<T>::printResults)
		.def("loadSimulation", [](sim::Simulation<T> &simulation, arch::Network<T> &network, std::string file) { 
				porting::simulationFromJSON(file, &network, simulation);
			})
		.def("saveResult", [](sim::Simulation<T> &simulation, std::string file) {
				porting::resultToJSON(file, &simulation);
			});

	#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
	#else
	m.attr("__version__") = "dev";
	#endif
}
