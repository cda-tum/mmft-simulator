#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "simulation/entities/Specie.hh"

namespace py = pybind11;

using T = double;

void bind_specie(py::module_& m) {

	py::class_<sim::Specie<T>, py::smart_holder>(m, "Specie")
		.def("getId", &sim::Specie<T>::getId, "Returns the id of the specie.")
		.def("setName", &sim::Specie<T>::setName, "Sets the name of the specie.")
		.def("getName", &sim::Specie<T>::getName, "Returns the name of the specie.")
		.def("setDiffusivity", &sim::Specie<T>::setDiffusivity, "Sets the diffusion coefficient of this species.")
		.def("getDiffusivity", &sim::Specie<T>::getDiffusivity, "Returns the diffusion coefficient of this species.")
		.def("setSatConc", &sim::Specie<T>::setSatConc, "Sets the saturation concentration of this specie.")
		.def("getSatConc", &sim::Specie<T>::getSatConc, "Returns the saturation concentration of this specie.");

}