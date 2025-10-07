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

void bind_abstractMembrane(py::module_& m) {

	py::class_<sim::AbstractMembrane<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractMembrane")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractMembrane<T>>(dynamic_cast<sim::AbstractMembrane<T>*>(tmpPtr.release()));
			}))
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
		.def("getMembraneResistance", &sim::AbstractMembrane<T>::getMembraneResistance, "Returns the membrane resistance.");

}