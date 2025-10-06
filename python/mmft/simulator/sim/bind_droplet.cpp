#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Edge.h"
#include "architecture/entities/Channel.h"
#include "architecture/definitions/ChannelPosition.h"
#include "simulation/entities/Droplet.h"
#include "simulation/entities/Fluid.h"

namespace py = pybind11;

using T = double;

void bind_droplet(py::module_& m) {

	py::class_<sim::Droplet<T>, py::smart_holder>(m, "Droplet")
		.def("getId", &sim::Droplet<T>::getId, "Returns the id of the droplet.")
		.def("getName", &sim::Droplet<T>::getName, "Returns the name of the droplet.")
		.def("setName", &sim::Droplet<T>::setName, "Sets the name of the droplet.")
		.def("getVolume", &sim::Droplet<T>::getVolume, "Returns the volume of the droplet [m^3].")
		.def("setVolume", &sim::Droplet<T>::setVolume, "Sets the volume of the droplet [m^3].")
		.def("readFluid", &sim::Droplet<T>::readFluid, py::return_value_policy::reference, 
			"Returns a read-only reference to the Fluid that constitutes the droplet.")
		.def("setFluid", &sim::Droplet<T>::setFluid, "Sets the fluid that the droplet consists of.")
		.def("readBoundaries", &sim::Droplet<T>::readBoundaries, py::return_value_policy::reference, 
			"Returns a read-only list of the droplet boundaries.")
		.def("readFullyOccupiedChannels", &sim::Droplet<T>::readFullyOccupiedChannels, py::return_value_policy::reference, 
			"Returns a read-only list of the channels that the droplet occupies fully.");

}