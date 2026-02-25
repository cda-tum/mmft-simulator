/**
 * @file baseSimulator.h
 */
#pragma once

#include "simulation/entities/Droplet.h"
#include "simulation/entities/Fluid.h"
#include "simulation/entities/Mixture.h"
#include "simulation/entities/Specie.h"
#include "simulation/entities/Tissue.h"

#include "simulation/events/BoundaryEvent.h"
#include "simulation/events/Event.h"
#include "simulation/events/InjectionEvent.h"
#include "simulation/events/MergingEvent.h"

#include "simulation/operations/Injection.h"
#include "simulation/operations/MixtureInjection.h"

#include "simulation/models/MembraneModels.h"
#include "simulation/models/MixingModels.h"
#include "simulation/models/ResistanceModels.h"

#include "simulation/simulators/semantics/ConcentrationSemantics.h"

#include "simulation/simulators/Simulation.h"
#include "simulation/simulators/AbstractContinuous.h"
#include "simulation/simulators/AbstractConcentration.h"
#include "simulation/simulators/AbstractDroplet.h"
#include "simulation/simulators/AbstractMembrane.h"
#include "simulation/simulators/HybridContinuous.h"
#include "simulation/simulators/HybridConcentration.h"
#include "simulation/simulators/CfdContinuous.h"
#include "simulation/simulators/CfdConcentration.h"

#include "simulation/simulators/CFDSim.h"
#include "simulation/simulators/cfdHandlers/cfdSimulator.h"
#include "simulation/simulators/cfdHandlers/olbContinuous.h"
#include "simulation/simulators/cfdHandlers/olbMixing.h"
// #include "simulation/simulators/cfdHandlers/olbOoc.h"    //** TODO: HybridOocSimulation */

#include "nodalAnalysis/NodalAnalysis.h"

#include "hybridDynamics/Scheme.h"
#include "hybridDynamics/Naive.h"

#include "architecture/definitions/ChannelPosition.h"
#include "architecture/definitions/ModuleOpening.h"

#include "architecture/entities/Channel.h"
#include "architecture/entities/Edge.h"
#include "architecture/entities/FlowRatePump.h"
#include "architecture/entities/Membrane.h"
#include "architecture/entities/Module.h"
#include "architecture/entities/Node.h"
#include "architecture/entities/PressurePump.h"
#include "architecture/entities/Tank.h"

#include "architecture/Network.h"

#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"
#include "olbProcessors/saturatedFluxPostProcessor2D.h"
#include "olbProcessors/setFunctionalRegularizedHeatFlux.h"

#include "porting/jsonPorter.h"
#include "porting/jsonReaders.h"
#include "porting/jsonWriters.h"

#include "result/Results.h"

#include <architecture/pNetwork.h>
#include <stlHandler/BaseSTL.h>
#include <stlHandler/NetworkSTL.h>

#ifdef USE_ESSLBM
    #include "simulation/simulators/essContinuous.h"
    #include "simulation/simulators/essMixing.h"
#endif