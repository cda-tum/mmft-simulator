#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Edge.h"
#include "architecture/entities/Tank.h"

namespace py = pybind11;

using T = double;

void bind_tank(py::module_& m) {

	py::class_<arch::Tank<T>, arch::Edge<T>, py::smart_holder>(m, "Tank")
		.def("setDimensions", &arch::Tank<T>::setDimensions, "Set the dimensions of the tank [w, h, l] [m].")
		.def("setHeight", &arch::Tank<T>::setHeight, "Set the height of the tank [m].")
		.def("setWidth", &arch::Tank<T>::setWidth, "Set the width of the tank [m].")
		.def("setLength", &arch::Tank<T>::setLength, "Set the length of the tank [m].")
		.def("getHeight", &arch::Tank<T>::getHeight, "Returns the height of the tank [m].")
		.def("getWidth", &arch::Tank<T>::getWidth, "Returns the width of the tank [m].")
		.def("getLength", &arch::Tank<T>::getLength, "Returns the length of the tank [m].")
		.def("getArea", &arch::Tank<T>::getArea, "Returns the area of the tank [m^2].")
		.def("getVolume", &arch::Tank<T>::getVolume, "Returns the volume of the tank [m^3].");

}