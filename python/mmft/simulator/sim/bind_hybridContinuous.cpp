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
#include "simulation/simulators/cfdHandlers/olbContinuous.h"

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

void bind_hybridContinuous(py::module_& m) {

	py::class_<sim::HybridContinuous<T>, sim::Simulation<T>, py::smart_holder>(m, "HybridContinuous")
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
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<sim::CFDSimulator<T>>&, T, T, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<sim::CFDSimulator<T>>&, std::unordered_map<int, T>, std::unordered_map<int, T>, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("getGlobalPressureBounds", &sim::HybridContinuous<T>::getGlobalPressureBounds, "Returns the global pressure bounds in the CFD simulators.")
		.def("getGlobalVelocityBounds", &sim::HybridContinuous<T>::getGlobalVelocityBounds, "Returns the global velocity bounds in the CFD simulators.")
		.def("writePressurePpm", &sim::HybridContinuous<T>::writePressurePpm, "Write the pressure field in ppm format for all simulators.")
		.def("writeVelocityPpm", &sim::HybridContinuous<T>::writeVelocityPpm, "Write the velocity field in ppm format for all simulators.");

}

void bind_cfdSimulators(py::module_& m) {

	py::class_<sim::CFDSimulator<T>, py::smart_holder>(m, "CFDSimulator")
		.def("getId", &sim::CFDSimulator<T>::getId, "Returns the id of the CFD simulator.")
		.def("getModule", &sim::CFDSimulator<T>::getModule, "Returns the module on which the simulator acts.")
		.def("getGroundNodes", &sim::CFDSimulator<T>::getGroundNodes, "Returns whether boundary nodes communicate the pressure (true) or flow rates (false) to the 1D solver.")
		.def("setVtkFolder", &sim::CFDSimulator<T>::setVtkFolder, "Sets the folder in which the vtk output should be stored.")
		.def("getVtkFile", &sim::CFDSimulator<T>::getVtkFile, "Returns the location where the last vtk file was created.")
		.def("getAlpha", &sim::CFDSimulator<T>::getAlpha, "Returns the relaxation factor for pressure updates.")
		.def("getBeta", &sim::CFDSimulator<T>::getBeta, "Returns the relaxation factor for flow rate updates.")
		.def("writeVtk", &sim::CFDSimulator<T>::writeVTK, "Write a vtk file with the current CFD simulation results.")
		.def("writePressurePpm", &sim::CFDSimulator<T>::writePressurePpm, "Write the ppm image of the pressure results.")
		.def("writeVelocityPpm", &sim::CFDSimulator<T>::writeVelocityPpm, "Write the ppm image of the velocity results.")
		.def("getPressureBounds", &sim::CFDSimulator<T>::getPressureBounds, "Returns the pressre bounds of the simulator.")
		.def("getVelocityBounds", &sim::CFDSimulator<T>::getVelocityBounds, "Returns the velocity bounds of the simulator.");

	py::class_<sim::lbmSimulator<T>, sim::CFDSimulator<T>, py::smart_holder>(m, "lbmSimulator")
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

}