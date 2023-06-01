#include <pybind11/pybind11.h>

#include <baseSimulator.h>
#include <baseSimulator.hh>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using T = double;

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pyhybridsim, m) {
  m.doc() = "Python binding for the closed channel-based baseSimulator.";

  py::class_<arch::Network<T>>(m, "Network")
      .def(py::init<std::string &>())
      .def("setPressurePump", &arch::Network<T>::setPressurePump);

  py::class_<sim::Fluid<T>>(m, "Fluid")
      .def(py::init<int &, T &, T &>())
      .def("setName", &sim::Fluid<T>::setName, "name"_a)
      .def("getViscosity", &sim::Fluid<T>::getViscosity);

  py::class_<sim::ResistanceModel1D<T>>(m, "ResistanceModel")
      .def(py::init<T &>());

  py::class_<sim::Simulation<T>>(m, "Simulation")
      .def(py::init<>())
      .def("setNetwork", &sim::Simulation<T>::setNetwork)
      .def("setContinuousPhase", &sim::Simulation<T>::setContinuousPhase)
      .def("setResistanceModel", &sim::Simulation<T>::setResistanceModel)
      .def("simulate", &sim::Simulation<T>::simulate)
      .def("print", &sim::Simulation<T>::printResults);

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
