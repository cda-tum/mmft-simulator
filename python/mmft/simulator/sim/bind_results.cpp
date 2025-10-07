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
		.def("getFinalMixturePositions", &result::SimulationResult<T>::getFinalMixturePositions, "Returns the mixture positions at the end of the simulation.")
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