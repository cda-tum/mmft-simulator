#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "simulation/entities/Droplet.h"
#include "simulation/entities/Fluid.h"
#include "simulation/entities/Mixture.h"
#include "simulation/entities/Specie.h"
#include "simulation/entities/Tissue.h"

#include "simulation/events/BoundaryEvent.h"
#include "simulation/events/Event.h"
#include "simulation/events/InjectionEvent.h"
#include "simulation/events/MergingEvent.h"

#include "simulation/operations/Injection.h"
#include "simulation/operations/MixtureInjection.h"

#include "simulation/models/MembraneModels.h"
#include "simulation/models/MixingModels.h"
#include "simulation/models/ResistanceModels.h"

#include "simulation/simulators/Simulation.h"
#include "simulation/simulators/AbstractContinuous.h"
#include "simulation/simulators/AbstractDroplet.h"
#include "simulation/simulators/AbstractMixing.h"
#include "simulation/simulators/AbstractMembrane.h"
#include "simulation/simulators/HybridContinuous.h"

#include "simulation/simulators/CFDSim.h"
#include "simulation/simulators/cfdHandlers/cfdSimulator.h"

#include "nodalAnalysis/NodalAnalysis.h"

#include "hybridDynamics/Scheme.h"
#include "hybridDynamics/Naive.h"

#include "architecture/definitions/ChannelPosition.h"
#include "architecture/definitions/ModuleOpening.h"

#include "architecture/entities/Channel.h"
#include "architecture/entities/Edge.h"
#include "architecture/entities/FlowRatePump.h"
#include "architecture/entities/Membrane.h"
#include "architecture/entities/Module.h"
#include "architecture/entities/Node.h"
#include "architecture/entities/PressurePump.h"
#include "architecture/entities/Tank.h"

#include "architecture/Network.h"

#include "porting/jsonPorter.h"
#include "porting/jsonReaders.h"
#include "porting/jsonWriters.h"

#include "result/Results.h"

namespace py = pybind11;

using T = double;

void bind_abstractDroplet(py::module_& m) {

	py::class_<sim::AbstractDroplet<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractDroplet")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractDroplet<T>>(dynamic_cast<sim::AbstractDroplet<T>*>(tmpPtr.release()));
			}))
		.def("addDroplet", py::overload_cast<int, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet of the given fluid id to the simulation.")
		.def("addDroplet", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet to the simulation.")
		.def("addMergedDroplet", py::overload_cast<int, int>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on the ids of two existing droplets.")
		.def("addMergedDroplet", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&, const std::shared_ptr<sim::Droplet<T>>&>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on two existing droplets.")
		.def("getDroplet", &sim::AbstractDroplet<T>::getDroplet, "Returns the droplet with the given id.")
		.def("getDropletAtNode", py::overload_cast<int>(&sim::AbstractDroplet<T>::getDropletAtNode, py::const_), 
			"Checks whether a droplet is present at the node with the given id. Returns true and the droplet if one is found.")
		.def("getDropletAtNode", py::overload_cast<const std::shared_ptr<arch::Node<T>>&>(&sim::AbstractDroplet<T>::getDropletAtNode, py::const_), 
			"Checks whether a droplet is present at the given node. Returns true and the droplet if one is found.")
		.def("removeDroplet", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&>(&sim::AbstractDroplet<T>::removeDroplet), "Removes a droplet from the simulation.")
		.def("addDropletInjection", py::overload_cast<int, T, int, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("addDropletInjection", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&, T, const std::shared_ptr<arch::Channel<T>>&, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("getDropletInjection", &sim::AbstractDroplet<T>::getDropletInjection, "Returns the droplet injection with the given id.")
		.def("removeDropletInjection", py::overload_cast<const std::shared_ptr<sim::DropletInjection<T>>&>(&sim::AbstractDroplet<T>::removeDropletInjection), 
			"Removes the droplet injection from the simulation.");

}