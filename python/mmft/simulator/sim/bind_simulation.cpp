#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Edge.hh"
#include "architecture/entities/FlowRatePump.hh"
#include "architecture/entities/Membrane.hh"
#include "architecture/entities/Module.hh"
#include "architecture/entities/Node.hh"
#include "architecture/entities/PressurePump.hh"
#include "architecture/entities/Tank.hh"

#include "architecture/definitions/ChannelPosition.hh"
#include "architecture/definitions/ModuleOpening.h"

#include "architecture/Network.hh"

#include "simulation/entities/Droplet.hh"
#include "simulation/entities/Fluid.hh"
#include "simulation/entities/Mixture.hh"
#include "simulation/entities/Specie.hh"
#include "simulation/entities/Tissue.hh"

#include "simulation/events/Event.h"
#include "simulation/events/BoundaryEvent.hh"
#include "simulation/events/InjectionEvent.hh"
#include "simulation/events/MergingEvent.hh"

#include "simulation/operations/Injection.hh"
#include "simulation/operations/MixtureInjection.hh"

#include "simulation/models/MembraneModels.hh"
#include "simulation/models/MixingModels.hh"
#include "simulation/models/ResistanceModels.hh"

#include "simulation/simulators/Simulation.hh"
#include "simulation/simulators/AbstractContinuous.hh"
#include "simulation/simulators/AbstractDroplet.hh"
#include "simulation/simulators/AbstractMixing.hh"
#include "simulation/simulators/AbstractMembrane.hh"
#include "simulation/simulators/HybridContinuous.hh"

#include "simulation/simulators/CFDSim.hh"
#include "simulation/simulators/cfdHandlers/cfdSimulator.hh"
#include "simulation/simulators/cfdHandlers/olbContinuous.hh"

#include "nodalAnalysis/NodalAnalysis.hh"

#include "hybridDynamics/Scheme.hh"
#include "hybridDynamics/Naive.hh"

#include "porting/jsonReaders.hh"
#include "porting/jsonWriters.hh"
#include "porting/jsonPorter.hh"

#include "result/Results.hh"

namespace py = pybind11;

using T = double;


void bind_simulation(py::module_& m) {

	py::class_<sim::Simulation<T>, py::smart_holder>(m, "Simulation")
		.def("getPlatform", &sim::Simulation<T>::getPlatform, "Returns the platform of the simulation.")
		.def("getType", &sim::Simulation<T>::getType, "Returns the type of simulation [Abstract, Hybrid CFD].")
		.def("getNetwork", &sim::Simulation<T>::getNetwork, "Returns the network on which the simulation is conducted.")
		.def("setNetwork", &sim::Simulation<T>::setNetwork, "Sets the network on which the simulation is conducted.")
		.def("set1DResistanceModel", &sim::Simulation<T>::set1DResistanceModel, "Sets the resistance model for abstract simulation to the 1D resistance model.")
		.def("setPoiseuilleResistanceModel", &sim::Simulation<T>::setPoiseuilleResistanceModel, "Sets the resistance model for abstract simulation components to the poiseuille resistance model.")
		.def("addFluid", &sim::Simulation<T>::addFluid, "Adds a fluid to the simulation.")
		.def("addMixedFluid", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&, T, const std::shared_ptr<sim::Fluid<T>>&, T>(&sim::Simulation<T>::addMixedFluid), 
			"Creates and adds a new fluid from two existing fluids.")
		.def("getFluid", &sim::Simulation<T>::getFluid, "Returns the fluid for the given id.")
		.def("readFluids", &sim::Simulation<T>::readFluids, py::return_value_policy::reference, "Returns a read-only list of fluids.")
		.def("removeFluid", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&>(&sim::Simulation<T>::removeFluid), 
			"Removes a fluid from the simulation.")
		.def("getFixtureId", &sim::Simulation<T>::getFixtureId, "Returns the fixture id.")
		.def("setFixtureId", &sim::Simulation<T>::setFixtureId, "Sets the fixture id.")
		.def("getContinuousPhase", &sim::Simulation<T>::getContinuousPhase, "Returns the continuous phase of the simulation.")
		.def("setContinuousPhase", py::overload_cast<int>(&sim::Simulation<T>::setContinuousPhase), "Set the continuous phase of the simulation to the fluid with the given id.")
		.def("setContinuousPhase", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&>(&sim::Simulation<T>::setContinuousPhase), "Set the continuous phase of the simulation to the given fluid.")
		.def("getCurrentIteration", &sim::Simulation<T>::getCurrentIteration, "Returns the current iteration number.")
		.def("getMaxIterations", &sim::Simulation<T>::getMaxIterations, "Returns the maximum number of allowed iterations. When reached the simulation ends.")
		.def("setWriteInterval", &sim::Simulation<T>::setWriteInterval, "Set the iterations interval at which the stat is saved in the results.")
		.def("getTMax", &sim::Simulation<T>::getTMax, "Returns the maximum allowed physical time. When reached the simulation ends.")
		.def("setMaxEndTime", &sim::Simulation<T>::setMaxEndTime, "Set the maximal physical time after which the simulation ends.")
		.def("getResults", &sim::Simulation<T>::getResults, "Returns the results of the simulation.")
		.def("printResults", &sim::Simulation<T>::printResults, "Prints the results of the simulation to the console.")
		.def("simulate", &sim::Simulation<T>::simulate, "Conducts the simulation.");

}

void bind_abstractContinuous(py::module_& m) {

	py::class_<sim::AbstractContinuous<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractContinuous")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractContinuous<T>>(dynamic_cast<sim::AbstractContinuous<T>*>(tmpPtr.release()));
			}));

}

void bind_abstractDroplet(py::module_& m) {

	py::class_<sim::AbstractDroplet<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractDroplet")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractDroplet<T>>(dynamic_cast<sim::AbstractDroplet<T>*>(tmpPtr.release()));
			}))
		.def("addDroplet", py::overload_cast<int, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet of the given fluid id to the simulation.")
		.def("addDroplet", py::overload_cast<const std::shared_ptr<sim::Fluid<T>>&, T>(&sim::AbstractDroplet<T>::addDroplet), "Adds a droplet to the simulation.")
		.def("addMergedDroplet", py::overload_cast<int, int>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on the ids of two existing droplets.")
		.def("addMergedDroplet", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&, const std::shared_ptr<sim::Droplet<T>>&>(&sim::AbstractDroplet<T>::addMergedDroplet), 
			"Adds a droplet to the simulator, based on two existing droplets.")
		.def("getDroplet", &sim::AbstractDroplet<T>::getDroplet, "Returns the droplet with the given id.")
		.def("getDropletAtNode", py::overload_cast<int>(&sim::AbstractDroplet<T>::getDropletAtNode, py::const_), 
			"Checks whether a droplet is present at the node with the given id. Returns true and the droplet if one is found.")
		.def("getDropletAtNode", py::overload_cast<const std::shared_ptr<arch::Node<T>>&>(&sim::AbstractDroplet<T>::getDropletAtNode, py::const_), 
			"Checks whether a droplet is present at the given node. Returns true and the droplet if one is found.")
		.def("removeDroplet", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&>(&sim::AbstractDroplet<T>::removeDroplet), "Removes a droplet from the simulation.")
		.def("addDropletInjection", py::overload_cast<int, T, int, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("addDropletInjection", py::overload_cast<const std::shared_ptr<sim::Droplet<T>>&, T, const std::shared_ptr<arch::Channel<T>>&, T>(&sim::AbstractDroplet<T>::addDropletInjection), 
			"Adds a droplet injection to the simulator of the given droplet at given time in the given channel and position.")
		.def("getDropletInjection", &sim::AbstractDroplet<T>::getDropletInjection, "Returns the droplet injection with the given id.")
		.def("removeDropletInjection", py::overload_cast<const std::shared_ptr<sim::DropletInjection<T>>&>(&sim::AbstractDroplet<T>::removeDropletInjection), 
			"Removes the droplet injection from the simulation.");

}

void bind_abstractMembrane(py::module_& m) {

	py::class_<sim::AbstractMembrane<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractMembrane")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractMembrane<T>>(dynamic_cast<sim::AbstractMembrane<T>*>(tmpPtr.release()));
			}))
		.def("setMembraneModel0", &sim::AbstractMembrane<T>::setMembraneModel0, "Sets membrane model 0.")
		.def("setMembraneModel1", &sim::AbstractMembrane<T>::setMembraneModel1, "Sets membrane model 1.")
		.def("setMembraneModel2", &sim::AbstractMembrane<T>::setMembraneModel2, "Sets membrane model 2.")
		.def("setMembraneModel3", &sim::AbstractMembrane<T>::setMembraneModel3, "Sets membrane model 3.")
		.def("setMembraneModel4", &sim::AbstractMembrane<T>::setMembraneModel4, "Sets membrane model 4.")
		.def("setMembraneModel5", &sim::AbstractMembrane<T>::setMembraneModel5, "Sets membrane model 5.")
		.def("setMembraneModel6", &sim::AbstractMembrane<T>::setMembraneModel6, "Sets membrane model 6.")
		.def("setMembraneModel7", &sim::AbstractMembrane<T>::setMembraneModel7, "Sets membrane model 7.")
		.def("setMembraneModel8", &sim::AbstractMembrane<T>::setMembraneModel8, "Sets membrane model 8.")
		.def("setMembraneModel9", &sim::AbstractMembrane<T>::setMembraneModel9, "Sets membrane model 9.")
		.def("getMembraneResistance", &sim::AbstractMembrane<T>::getMembraneResistance, "Returns the membrane resistance.");

}

void bind_abstractMixing(py::module_& m) {

	py::class_<sim::AbstractMixing<T>, sim::Simulation<T>, py::smart_holder>(m, "AbstractMixing")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::AbstractMixing<T>>(dynamic_cast<sim::AbstractMixing<T>*>(tmpPtr.release()));
			}))
		.def("setInstantaneousMixingModel", &sim::AbstractMixing<T>::setInstantaneousMixingModel, "Sets the instantaneous mixing model.")
		.def("hasInstantaneousMixingModel", &sim::AbstractMixing<T>::hasInstantaneousMixingModel, "Returns whether an instantaneous mixing model was set.")
		.def("setDiffusiveMixingModel", &sim::AbstractMixing<T>::setDiffusiveMixingModel, "Sets the diffusive mixing model.")
		.def("hasDiffusiveMixingModel", &sim::AbstractMixing<T>::hasDiffusiveMixingModel, "Returns whether a diffusive mixing model was set.")
		.def("addSpecie", &sim::AbstractMixing<T>::addSpecie, "Adds a single species to the simulator.")
		.def("getSpecie", &sim::AbstractMixing<T>::getSpecie, "Returns a species with the given id.")
		.def("removeSpecie", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&>(&sim::AbstractMixing<T>::removeSpecie), "Removes a species from the simulator.")
		.def("addMixture", py::overload_cast<const std::shared_ptr<sim::Specie<T>>&, T>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("addMixture", py::overload_cast<const std::vector<std::shared_ptr<sim::Specie<T>>>&, const std::vector<T>&>(&sim::AbstractMixing<T>::addMixture), "Adds a mixture to the simulator.")
		.def("getMixture", &sim::AbstractMixing<T>::getMixture, "Returns the mixture with the given id.")
		.def("readMixtures", &sim::AbstractMixing<T>::readMixtures, "Returns a read-only list of mixtures in the simulator.")
		.def("removeMixture", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&>(&sim::AbstractMixing<T>::removeMixture), "Removes a mixture from the simulator.")
		.def("addMixtureInjection", py::overload_cast<int, int, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixtureId"), py::arg("edgeId"), py::arg("injectionTime"), py::arg("isPermanent")=false, 
			"Add a mixture injection to the simulator.")
		.def("addMixtureInjection", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T, bool>(&sim::AbstractMixing<T>::addMixtureInjection), py::arg("mixture"), py::arg("edge"), 
			py::arg("injectionTime"), py::arg("isPermanent")=false, "Add a mixture injection to the simulator")
		.def("addPermanentMixtureInjection", py::overload_cast<int, int, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), "Add a permanent mixture injection to the simulator.")
		.def("addPermanentMixtureInjection", py::overload_cast<const std::shared_ptr<sim::Mixture<T>>&, const std::shared_ptr<arch::Edge<T>>&, T>(&sim::AbstractMixing<T>::addPermanentMixtureInjection), 
			"Add a permanent mixture injection to the simulator.")
		.def("getMixtureInjection", &sim::AbstractMixing<T>::getMixtureInjection, "Returns the mixture injection with the given id.")
		.def("removeMixtureInjection", py::overload_cast<const std::shared_ptr<sim::MixtureInjection<T>>&>(&sim::AbstractMixing<T>::removeMixtureInjection), 
			"Removes the mixture injection with the given id.");

}

void bind_hybridContinuous(py::module_& m) {

	py::class_<sim::HybridContinuous<T>, sim::Simulation<T>, py::smart_holder>(m, "HybridContinuous")
		.def(py::init<std::shared_ptr<arch::Network<T>>>())
		.def(py::init([](std::string file, std::shared_ptr<arch::Network<T>> network){
				std::unique_ptr<sim::Simulation<T>> tmpPtr = porting::simulationFromJSON<T>(file, network);
				return std::shared_ptr<sim::HybridContinuous<T>>(dynamic_cast<sim::HybridContinuous<T>*>(tmpPtr.release()));
			}))
		.def("getCharacteristicLength", &sim::HybridContinuous<T>::getCharacteristicLength, "Returns the characteristic length of the LBM simulators.")
		.def("getCharacteristicVelocity", &sim::HybridContinuous<T>::getCharacteristicVelocity, "Returns the characteristic velocity of the LBM simulators.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("name")="", "Add a LBM simulator to the hybrid simulation.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, size_t, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("resolution"), py::arg("name")="", "Add a LBM simulator to the hybrid simulation.")
		.def("addLbmSimulator", py::overload_cast<std::shared_ptr<arch::CfdModule<T>> const, size_t, T, T, T, T, std::string>(&sim::HybridContinuous<T>::addLbmSimulator),
			py::arg("module"), py::arg("resolution"), py::arg("epsilon"), py::arg("tau"), py::arg("charPhysLength"), py::arg("charPhysVelocity"), py::arg("name")="", 
			"Add a LBM simulator to the hybrid simulation.")
		.def("getLbmSimulator", &sim::HybridContinuous<T>::getLbmSimulator, "Returns the LBM simulator with the given id.")
		.def("removeLbmSimulator", &sim::HybridContinuous<T>::removeLbmSimulator, "Removes the LBM simulator from the hybrid simulation.")
		.def("setNaiveHybridScheme", py::overload_cast<T, T, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), "Set the naive update scheme for all simulators.")
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<sim::CFDSimulator<T>>&, T, T, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("setNaiveHybridScheme", py::overload_cast<const std::shared_ptr<sim::CFDSimulator<T>>&, std::unordered_map<int, T>, std::unordered_map<int, T>, int>(&sim::HybridContinuous<T>::setNaiveHybridScheme), 
			"Set the naive update scheme for the given simulator.")
		.def("getGlobalPressureBounds", &sim::HybridContinuous<T>::getGlobalPressureBounds, "Returns the global pressure bounds in the CFD simulators.")
		.def("getGlobalVelocityBounds", &sim::HybridContinuous<T>::getGlobalVelocityBounds, "Returns the global velocity bounds in the CFD simulators.")
		.def("writePressurePpm", &sim::HybridContinuous<T>::writePressurePpm, "Write the pressure field in ppm format for all simulators.")
		.def("writeVelocityPpm", &sim::HybridContinuous<T>::writeVelocityPpm, "Write the velocity field in ppm format for all simulators.");

}

void bind_cfdSimulators(py::module_& m) {

	py::class_<sim::CFDSimulator<T>, py::smart_holder>(m, "CFDSimulator")
		.def("getId", &sim::CFDSimulator<T>::getId, "Returns the id of the CFD simulator.")
		.def("getModule", &sim::CFDSimulator<T>::getModule, "Returns the module on which the simulator acts.")
		.def("getGroundNodes", &sim::CFDSimulator<T>::getGroundNodes, "Returns whether boundary nodes communicate the pressure (true) or flow rates (false) to the 1D solver.")
		.def("setVtkFolder", &sim::CFDSimulator<T>::setVtkFolder, "Sets the folder in which the vtk output should be stored.")
		.def("getVtkFile", &sim::CFDSimulator<T>::getVtkFile, "Returns the location where the last vtk file was created.")
		.def("getAlpha", &sim::CFDSimulator<T>::getAlpha, "Returns the relaxation factor for pressure updates.")
		.def("getBeta", &sim::CFDSimulator<T>::getBeta, "Returns the relaxation factor for flow rate updates.")
		.def("writeVtk", &sim::CFDSimulator<T>::writeVTK, "Write a vtk file with the current CFD simulation results.")
		.def("writePressurePpm", &sim::CFDSimulator<T>::writePressurePpm, "Write the ppm image of the pressure results.")
		.def("writeVelocityPpm", &sim::CFDSimulator<T>::writeVelocityPpm, "Write the ppm image of the velocity results.")
		.def("getPressureBounds", &sim::CFDSimulator<T>::getPressureBounds, "Returns the pressre bounds of the simulator.")
		.def("getVelocityBounds", &sim::CFDSimulator<T>::getVelocityBounds, "Returns the velocity bounds of the simulator.");

	py::class_<sim::lbmSimulator<T>, sim::CFDSimulator<T>, py::smart_holder>(m, "lbmSimulator")
		.def("getCharPhysLength", &sim::lbmSimulator<T>::getCharPhysLength, "Returns the characteristic physical length of the lbm simulator.")
		.def("getCharPhysVelocity", &sim::lbmSimulator<T>::getCharPhysVelocity, "Returns the characteristic physical velocity of the lbm simulator.")
		.def("getResolution", &sim::lbmSimulator<T>::getResolution, "Returns the resolution of the lbm simulator.")
		.def("setResolution", &sim::lbmSimulator<T>::setResolution, "Sets the resolution of the lbm simulator.")
		.def("getEpsilon", &sim::lbmSimulator<T>::getEpsilon, "Returns the epsilon for the simulator.")
		.def("setEpsilon", &sim::lbmSimulator<T>::setEpsilon, "Sets the epsilon for the simulator.")
		.def("getTau", &sim::lbmSimulator<T>::getTau, "Returns the relaxation time of the simulator.")
		.def("setTau", &sim::lbmSimulator<T>::setTau, "Sets the relaxation time of the simulator.")
		.def("getStepIter", &sim::lbmSimulator<T>::getStepIter, "Returns the number of steps used for the value tracer (default = 1000).")
		.def("hasConverged", &sim::lbmSimulator<T>::hasConverged, "Returns whether the simulator has converged or not.");

}