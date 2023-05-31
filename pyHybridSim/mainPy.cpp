#include <pybind11/pybind11.h>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

namespace py = pybind11;

namespace pySim {

    PYBIND11_MODULE(hybridSim, m) {

        using T = double;

        m.doc() = "Python binding for the closed channel-based baseSimulator.";

        py::class_<arch::Network<T>>(m, "Network")
            .def(py::init<std::string &>())
            .def("setPressurePump", &arch::Network<T>::setPressurePump);

        py::class_<sim::Fluid<T>>(m, "Fluid")
            .def(py::init<int &, T &, T &>())
            .def("setName", &sim::Fluid<T>::setName)
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

    }

}