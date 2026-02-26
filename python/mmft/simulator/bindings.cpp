#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "bindings.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using T = double;

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pysimulator, m) {
	m.doc() = "Python binding for the MMFT-Simulator.";

	// Architecture bindings
	bind_enums(m);
	bind_opening(m);
	bind_node(m);
	bind_edge(m);
	bind_channel(m);
	bind_pumps(m);
	bind_membrane(m);
	bind_tank(m);
	bind_module(m);
	bind_network(m);

	// Simulator bindings
	bind_fluid(m);
	bind_droplet(m);
	bind_specie(m);
	bind_mixture(m);
	bind_injections(m);
	bind_cfdSimulators(m);
	bind_simulation(m);
	bind_concentrationSemantics(m);
	bind_abstractContinuous(m);
	bind_abstractDroplet(m);
	bind_abstractConcentration(m);
	bind_abstractMembrane(m);
	bind_hybridContinuous(m);
	bind_hybridConcentration(m);
	bind_cfdContinuous(m);
	bind_cfdConcentration(m);
	bind_porter(m);
	bind_results(m);
		
	#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
	#else
	m.attr("__version__") = "dev";
	#endif
}
