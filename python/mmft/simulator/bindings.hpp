// bindings.hpp
#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations of all binding functions

// arch bindings
void bind_enums(py::module_& m);
void bind_opening(py::module_& m);
void bind_node(py::module_& m);
void bind_edge(py::module_& m);
void bind_channel(py::module_& m);
void bind_pumps(py::module_& m);
void bind_membrane(py::module_& m);
void bind_tank(py::module_& m);
void bind_module(py::module_& m);
void bind_network(py::module_& m);

// sim bindings
void bind_fluid(py::module_& m);
void bind_droplet(py::module_& m);
void bind_specie(py::module_& m);
void bind_mixture(py::module_& m);
void bind_injections(py::module_& m);
void bind_cfdSimulators(py::module_& m);
void bind_simulation(py::module_& m);
void bind_concentrationSemantics(py::module_& m);
void bind_abstractContinuous(py::module_& m);
void bind_abstractDroplet(py::module_& m);
void bind_AbstractConcentration(py::module_& m);
void bind_hybridContinuous(py::module_& m);
void bind_hybridMixing(py::module_& m);
void bind_cfdContinuous(py::module_& m);
void bind_cfdMixing(py::module_& m);
void bind_porter(py::module_& m);
void bind_results(py::module_& m);
