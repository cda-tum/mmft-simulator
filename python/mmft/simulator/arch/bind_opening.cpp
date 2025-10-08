#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Node.hh"
#include "architecture/definitions/ModuleOpening.h"

namespace py = pybind11;

using T = double;

void bind_opening(py::module_& m) {

	py::class_<arch::Opening<T>>(m, "Opening")
        .def(py::init<std::shared_ptr<arch::Node<T>>, std::vector<T>, T,T>(),
            py::arg("node"), py::arg("normal"), py::arg("width"), py::arg("height") = 1e-4)
		.def_readwrite("node", &arch::Opening<T>::node)
		.def_readwrite("normal", &arch::Opening<T>::normal)
		.def_readwrite("tangent", &arch::Opening<T>::tangent)
		.def_readwrite("width", &arch::Opening<T>::width)
		.def_readwrite("height", &arch::Opening<T>::height)
		.def_readwrite("radial", &arch::Opening<T>::radial);

}