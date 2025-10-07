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

void bind_abstractMixing(py::module_& m) {

	py::class_<sim::AbstractMixing<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractMixing")
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
		.def("removeSpecie", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&>(&sim::AbstractMixing<T>::removeSpecie), "Removes a species from the simulator.")
		.def("addMixture", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&, T>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("addMixture", py::overload_cast<const std::vector<std::shared_ptr<sim::Specie<T>>>&, const std::vector<T>&>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("getMixture", &sim::AbstractMixing<T>::getMixture, "Returns the mixture with the given id.")
		.def("readMixtures", &sim::AbstractMixing<T>::readMixtures, "Returns a read-only list of mixtures in the simulator.")
		.def("removeMixture", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&>(&sim::AbstractMixing<T>::removeMixture), "Removes a mixture from the simulator.")
		.def("addMixtureInjection", py::overload_cast<int, int, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixtureId"), py::arg("edgeId"), py::arg("injectionTime"), py::arg("isPermanent")=false, 
			"Add a mixture injection to the simulator.")
		.def("addMixtureInjection", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixture"), py::arg("edge"), 
			py::arg("injectionTime"), py::arg("isPermanent")=false, "Add a mixture injection to the simulator")
		.def("addPermanentMixtureInjection", py::overload_cast<int, int, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), "Add a permanent mixture injection to the simulator.")
		.def("addPermanentMixtureInjection", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), 
			"Add a permanent mixture injection to the simulator.")
		.def("getMixtureInjection", &sim::AbstractMixing<T>::getMixtureInjection, "Returns the mixture injection with the given id.")
		.def("removeMixtureInjection", py::overload_cast<const std::shared_ptr<sim::MixtureInjection<T>>&>(&sim::AbstractMixing<T>::removeMixtureInjection), 
			"Removes the mixture injection with the given id.");

}