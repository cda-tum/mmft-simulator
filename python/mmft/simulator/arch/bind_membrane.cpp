#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Membrane.hh"
#include "architecture/entities/Edge.hh"

namespace py = pybind11;

using T = double;

void bind_membrane(py::module_& m) {

	py::class_<arch::Membrane<T>, arch::Edge<T>, py::smart_holder>(m, "Membrane")
		.def("setDimensions", &arch::Membrane<T>::setDimensions, "Set the dimensions of the membrane [w, h, l] [m].")
		.def("setHeight", &arch::Membrane<T>::setHeight, "Set the height of the membrane [m].")
		.def("setWidth", &arch::Membrane<T>::setWidth, "Set the width of the membrane [m].")
		.def("setLength", &arch::Membrane<T>::setLength, "Set the length of the membrane [m].")
		.def("setPoreRadius", &arch::Membrane<T>::setPoreRadius, "Set the pore radius of the pores of the membrane [m].")
		.def("setPorosity", &arch::Membrane<T>::setPorosity, "Set the porosity of the membrane [0.0-1.0].")
		.def("setChannel", &arch::Membrane<T>::setChannel, "Set the channel the membrane is connected to.")
		.def("setTank", &arch::Membrane<T>::setTank, "Set the tank the membrane is connected to.")
		.def("getConcentrationChange", &arch::Membrane<T>::getConcentrationChange, "Returns the concentration change caused by the membrane [mol].")
		.def("getHeight", &arch::Membrane<T>::getHeight, "Returns the height of the membrane [m].")
		.def("getWidth", &arch::Membrane<T>::getWidth, "Returns the width of the membrane [m].")
		.def("getLength", &arch::Membrane<T>::getLength, "Returns the length of the membrane [m].")
		.def("getChannel", &arch::Membrane<T>::getChannel, "Returns the channel that the membrane is connected to.")
		.def("getRectangularChannel", &arch::Membrane<T>::getRectangularChannel, "Returns the rectangular channel that the membrane is connected to.")
		.def("getTank", &arch::Membrane<T>::getTank, "Returns the tank that the membrane is connected to.")
		.def("getPoreRadius", &arch::Membrane<T>::getPoreRadius, "Returns the pore radius of the membrane [m].")
		.def("getPoreDiameter", &arch::Membrane<T>::getPoreDiameter, "Returns the diameter of the pores of the membrane [m].")
		.def("getPorosity", &arch::Membrane<T>::getPorosity, "Returns the porosity of the membrane [0.0-1.0].")
		.def("getNumberOfPores", &arch::Membrane<T>::getNumberOfPores, "Returns the number of pores in a given area [m^2] of the membrane.")
		.def("getPoreDensity", &arch::Membrane<T>::getPoreDensity, "Returns the density of the pores in the membrane.")
		.def("getArea", &arch::Membrane<T>::getArea, "Returns the area of the membrane [m^2].")
		.def("getVolume", &arch::Membrane<T>::getVolume, "Returns the volume of the membrane [m^3].");

}