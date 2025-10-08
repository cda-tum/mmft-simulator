#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Module.hh"
#include "simulation/models/ResistanceModels.hh"
#include "simulation/simulators/Simulation.hh"
#include "simulation/simulators/CFDSim.hh"
#include "simulation/simulators/HybridContinuous.hh"
#include "simulation/simulators/cfdHandlers/cfdSimulator.hh"

namespace py = pybind11;

using T = double;

void bind_enums(py::module_& m) {

	py::enum_<arch::ChannelType>(m, "ChannelType")
		.value("normal", arch::ChannelType::NORMAL)
		.value("bypass", arch::ChannelType::BYPASS)
		.value("cloggable", arch::ChannelType::CLOGGABLE);

	py::enum_<arch::ModuleType>(m, "ModuleType")
		.value("normal", arch::ModuleType::NORMAL)
		.value("lbm", arch::ModuleType::LBM);

	py::enum_<sim::Type>(m, "Type")
		.value("abstract", sim::Type::Abstract)
		.value("hybrid", sim::Type::Hybrid);

	py::enum_<sim::Platform>(m, "Platform")
		.value("continuous", sim::Platform::Continuous)
		.value("bigDroplet", sim::Platform::BigDroplet);

}