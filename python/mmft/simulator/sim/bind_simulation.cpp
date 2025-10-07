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

void bind_simulation(py::module_& m) {

	py::class_<sim::Simulation<T>, py::smart_holder>(m, "Simulation")
		.def("getPlatform", &sim::Simulation<T>::getPlatform, "Returns the platform of the simulation.")
		.def("getType", &sim::Simulation<T>::getType, "Returns the type of simulation [Abstract, Hybrid CFD].")
		.def("getNetwork", &sim::Simulation<T>::getNetwork, "Returns the network on which the simulation is conducted.")
		.def("setNetwork", &sim::Simulation<T>::setNetwork, "Sets the network on which the simulation is conducted.")
		.def("set1DResistanceModel", &sim::Simulation<T>::set1DResistanceModel, "Sets the resistance model for abstract simulation to the 1D resistance model.")
		.def("setPoiseuilleResistanceModel", &sim::Simulation<T>::setPoiseuilleResistanceModel, "Sets the resistance model for abstract simulation components to the poiseuille resistance model.")
		.def("addFluid", &sim::Simulation<T>::addFluid, "Adds a fluid to the simulation.")
		.def("addMixedFluid", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&, T, const std::shared_ptr<sim::Fluid<T>>&, T>(&sim::Simulation<T>::addMixedFluid), 
			"Creates and adds a new fluid from two existing fluids.")
		.def("getFluid", &sim::Simulation<T>::getFluid, "Returns the fluid for the given id.")
		.def("readFluids", &sim::Simulation<T>::readFluids, py::return_value_policy::reference, "Returns a read-only list of fluids.")
		.def("removeFluid", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&>(&sim::Simulation<T>::removeFluid), 
			"Removes a fluid from the simulation.")
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

}