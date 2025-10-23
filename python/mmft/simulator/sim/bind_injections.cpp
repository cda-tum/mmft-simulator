#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Edge.hh"
#include "architecture/entities/Channel.hh"
#include "architecture/definitions/ChannelPosition.hh"

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

namespace py = pybind11;

using T = double;

void bind_injections(py::module_& m) {

	py::class_<sim::DropletInjection<T>, py::smart_holder>(m, "DropletInjection")
		.def("getId", &sim::DropletInjection<T>::getId, "Returns the id of the droplet injection.")
		.def("getName", &sim::DropletInjection<T>::getName, "Returns the name of the droplet injection.")
		.def("setName", &sim::DropletInjection<T>::setName, "Sets the name of the droplet inkection.")
		.def("getInjectionTime", &sim::DropletInjection<T>::getInjectionTime, "Returns the time at which the droplet is injected [s].")
		.def("setInjectionTime", &sim::DropletInjection<T>::setInjectionTime, "Sets the time at which the droplet should be injected [s].")
		.def("getInjectionPosition", &sim::DropletInjection<T>::getInjectionPosition, "Returns the relative channel position at which the droplet is injected [0.0-1.0].")
		.def("setInjectionPosition", &sim::DropletInjection<T>::setInjectionPosition, "Sets the relative channel position at which the droplet is injected [0.0-1.0].")
		.def("getInjectionChannel", &sim::DropletInjection<T>::getInjectionPosition, "Returns the channel into which the droplet is injected.")
		.def("setInjectionChannel", &sim::DropletInjection<T>::setInjectionPosition, "Sets the channel into which the droplet is injected.");

	py::class_<sim::MixtureInjection<T>, py::smart_holder>(m, "MixtureInjection")
		.def("getId", &sim::MixtureInjection<T>::getId, "Returns the id of the mixture injection.")
		.def("getName", &sim::MixtureInjection<T>::getName, "Returns the name of the mixture injection.")
		.def("setName", &sim::MixtureInjection<T>::setName, "Sets the name of the mixture inkection.")
		.def("getInjectionTime", &sim::MixtureInjection<T>::getInjectionTime, "Returns the time at which the mixture is injected [s].")
		.def("setInjectionTime", &sim::MixtureInjection<T>::setInjectionTime, "Sets the time at which the mixture should be injected [s].")
		.def("getInjectionChannel", &sim::MixtureInjection<T>::getInjectionChannel, "Returns the channel into which the mixture is injected.")
		.def("setInjectionChannel", &sim::MixtureInjection<T>::setInjectionChannel, "Sets the channel into which the mixture is injected.");

}