#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Edge.hh"

namespace py = pybind11;
using T = double;

void bind_channel(py::module_& m) {

	py::class_<arch::Channel<T>, arch::Edge<T>, py::smart_holder>(m, "Channel")
		.def("setChannelType", &arch::Channel<T>::setChannelType, "Set the type of channel.")
		.def("getChannelShape", &arch::Channel<T>::getChannelShape, "Returns the shape of the channel cross section.")
		.def("getChannelType", &arch::Channel<T>::getChannelType, "Returns the type of the channel.")
		.def("setLength", &arch::Channel<T>::setLength, "Set the length of the channel [m].")
		.def("getLength", &arch::Channel<T>::getLength, "Returns the length of the channel [m].")
		.def("getArea", &arch::Channel<T>::getArea, "Returns the area of the channel cross-section [m^2].")
		.def("getVolume", &arch::Channel<T>::getVolume, "Returns the volume of the channel [m^3].")
		.def("isChannelValid", &arch::Channel<T>::isChannelValid, "Returns true if the channel parameters are valid.")
		.def("isRectangular", &arch::Channel<T>::isRectangular, "Returns true if the channel has a rectangular cross-section.")
		.def("isCylindrical", &arch::Channel<T>::isCylindrical, "Returns true if the channel has a cylindrical cross-section.");

	py::class_<arch::RectangularChannel<T>, arch::Channel<T>, py::smart_holder>(m, "RectangularChannel")
		.def("get2DFlowRate", &arch::RectangularChannel<T>::get2DFlowRate, "Maps and returns the 2-dimensional flow rate within the channel [m^2/s].")
		.def("setWidth", &arch::RectangularChannel<T>::setWidth, "Set the width of the channel cross-section [m].")
		.def("getWidth", &arch::RectangularChannel<T>::getWidth, "Returns the width of the channel cross-section [m].")
		.def("setHeight", &arch::RectangularChannel<T>::setHeight, "Set the height of the channel cross-section [m].")
		.def("getHeight", &arch::RectangularChannel<T>::getHeight, "Returns the height of the channel cross-section [m]");

	py::class_<arch::CylindricalChannel<T>, arch::Channel<T>, py::smart_holder>(m, "CylindricalChannel")
		.def("setRadius", &arch::CylindricalChannel<T>::setRadius, "Set the radius of the channel cross-section [m].")
		.def("getRadius", &arch::CylindricalChannel<T>::getRadius, "Returns the radius of the channel cross-section [m].");

}