#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Edge.hh"
#include "architecture/entities/FlowRatePump.hh"
#include "architecture/entities/Membrane.hh"
#include "architecture/entities/Module.hh"
#include "architecture/entities/Node.hh"
#include "architecture/entities/PressurePump.h"
#include "architecture/entities/Tank.hh"

#include "architecture/definitions/ChannelPosition.hh"
#include "architecture/definitions/ModuleOpening.h"

#include "architecture/Network.hh"

#include "simulation/entities/Droplet.hh"
#include "simulation/entities/Fluid.hh"
#include "simulation/entities/Mixture.hh"
#include "simulation/entities/Specie.hh"
#include "simulation/entities/Tissue.hh"

#include "simulation/events/BoundaryEvent.hh"
#include "simulation/events/Event.h"
#include "simulation/events/InjectionEvent.hh"
#include "simulation/events/MergingEvent.hh"

#include "simulation/operations/Injection.hh"
#include "simulation/operations/MixtureInjection.hh"

#include "simulation/models/MembraneModels.hh"
#include "simulation/models/MixingModels.hh"
#include "simulation/models/ResistanceModels.hh"

#include "simulation/simulators/Simulation.hh"
#include "simulation/simulators/AbstractContinuous.hh"
#include "simulation/simulators/AbstractDroplet.hh"
#include "simulation/simulators/AbstractConcentration.hh"
#include "simulation/simulators/AbstractMembrane.hh"
#include "simulation/simulators/HybridContinuous.hh"

#include "simulation/simulators/CFDSim.hh"
#include "simulation/simulators/cfdHandlers/cfdSimulator.hh"

#include "nodalAnalysis/NodalAnalysis.hh"

#include "hybridDynamics/Scheme.hh"
#include "hybridDynamics/Naive.hh"

#include "result/Results.hh"

namespace py = pybind11;

using T = double;

void bind_results(py::module_& m) {

	py::class_<result::State<T>, py::smart_holder>(m, "State")
		.def("getPressures", &result::State<T>::getPressures, "Get read-only references to all pressures that were obtained during the simulation.")
		.def("getFlowRates", &result::State<T>::getFlowRates, "Get read-only references to all flow rates that were obtained during the simulation.")
		.def("getVtkFiles", &result::State<T>::getVtkFiles, "Get the locations of all vtk files that were written during the simulation.")
		.def("getDropletPositions", &result::State<T>::getDropletPositions, "Get copies of all droplet positions that were calculated during the simulation.")
		.def("getMixturePositions", &result::State<T>::getMixturePositions, "Get copies of all mixture positions that were calculated during the simulation.")
		.def("getTime", &result::State<T>::getTime, "Get the simulation timestamp of this state.")
		.def("printState", &result::State<T>::printState, "Print the state.");

	py::class_<result::SimulationResult<T>, py::smart_holder>(m, "SimulationResult")
		.def("getStates", &result::SimulationResult<T>::getStates, "Get the simulated states that were stored during simulation.")
		.def("getLastState", &result::SimulationResult<T>::getLastState, "Get the last state.")
		.def("getState", &result::SimulationResult<T>::getState, "Get state with given key.")
		.def("printStates", &result::SimulationResult<T>::printStates, "Print all states.")
		.def("printLastState", &result::SimulationResult<T>::printLastState, "Print the last state.")
		.def("printState", &result::SimulationResult<T>::printState, "Print state with given key.")
		.def("getMixtures", &result::SimulationResult<T>::getMixtures, "Get read-only references to all mixtures that were defined during the simulation.")
		.def("printMixtures", &result::SimulationResult<T>::printMixtures, "Print all mixtures that were defined during the simulation.")
		.def("writeMixture", &result::SimulationResult<T>::writeMixture, "Write the concentration profile of the mixture with given id to a CSV file.");

}