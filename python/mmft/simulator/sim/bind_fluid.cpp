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

void bind_fluid(py::module_& m) {

	py::class_<sim::Fluid<T>, py::smart_holder>(m, "Fluid")
		.def("getId", &sim::Fluid<T>::getId, "Returns the id of the fluid.")
		.def("setName", &sim::Fluid<T>::setName, "Sets the name of the fluid.")
		.def("getName", &sim::Fluid<T>::getName, "Returns the name of the fluid.")
		.def("setViscosity", &sim::Fluid<T>::setViscosity, "Sets the viscosity of the fluid.")
		.def("getViscosity", &sim::Fluid<T>::getViscosity, "Returns the viscosity of the fluid.")
		.def("setDensity", &sim::Fluid<T>::setDensity, "Sets the density of the fluid.")
		.def("getDensity", &sim::Fluid<T>::getDensity, "Returns the density of the fluid.");

}