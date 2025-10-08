#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "simulation/entities/Fluid.hh"
#include "simulation/entities/Mixture.hh"
#include "simulation/entities/Specie.hh"

namespace py = pybind11;

using T = double;

void bind_mixture(py::module_& m) {

	py::class_<sim::Mixture<T>, py::smart_holder>(m, "Mixture")
		.def(py::self == py::self)
		.def("getId", &sim::Mixture<T>::getId, "Returns the id of the mixture.")
		.def("getName", &sim::Mixture<T>::getName, "Returns the name of the mixture.")
		.def("setName", &sim::Mixture<T>::setName, "Sets the name of the mixture.")
		.def("addSpecie", &sim::Mixture<T>::addSpecie, "Adds a specie with a given concentration to this mixture.")
		.def("getSpecie", &sim::Mixture<T>::getSpecie, "Returns the specie with the given id from this mixture, together with the concentration.")
		.def("getSpecies", &sim::Mixture<T>::getSpecie, "Returns the specie from this mixture, with the given id.")
		.def("getSpecieConcentrations", &sim::Mixture<T>::getSpecieConcentrations, "Returns a map of the specie concentrations.")
		.def("getConcentrationOfSpecie", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&>(&sim::Mixture<T>::getConcentrationOfSpecie, py::const_), 
			"Returns the concentration of a specie [g/m^3].")
		.def("getSpecieDistributions", &sim::Mixture<T>::getSpecieDistributions, "Returns the concentration distributions of all species if it's not a constant value [g/m^3].")
		.def("setSpecieConcentration", &sim::Mixture<T>::setSpecieConcentration, "Sets the concentration of a specie [g/m^3].")
		.def("getSpecieCount", &sim::Mixture<T>::getSpecieCount, "Returns the number of species listed in the mixture.")
		.def("removeSpecie", &sim::Mixture<T>::removeSpecie, "Removes a specie from the mixture.")
		.def("getDensity", &sim::Mixture<T>::getDensity, "Returns the density of the mixture [kg/m^3].")
		.def("getViscosity", &sim::Mixture<T>::getViscosity, "Returns the viscosity of the mixture [Pa s].")
		.def("getLargestMolecularSize", &sim::Mixture<T>::getLargestMolecularSize, "Returns the largest molecular size of the species in the mixture [m^3].");

	py::class_<sim::DiffusiveMixture<T>, sim::Mixture<T>, py::smart_holder>(m, "DiffusiveMixture")
		.def("setResolution", &sim::DiffusiveMixture<T>::setResolution, "Sets the spectral resolution of the distribution function.")
		.def("getResolution", &sim::DiffusiveMixture<T>::getResolution, "Returns the spectral resolution of the distribution function.")
		.def("getDistributionOfSpecie",  py::overload_cast<const std::shared_ptr<sim::Specie<T>>&>(&sim::DiffusiveMixture<T>::getDistributionOfSpecie, py::const_), 
			"Returns the concentration distribution of a species if it's not a constant value [g/m^3].");

}