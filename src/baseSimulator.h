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

#include "simulation/simulators/Simulation.h"
#include "simulation/simulators/AbstractContinuous.h"
#include "simulation/simulators/AbstractDroplet.h"
#include "simulation/simulators/AbstractMixing.h"
#include "simulation/simulators/AbstractMembrane.h"
#include "simulation/simulators/HybridContinuous.h"

#include "simulation/simulators/CFDSim.h"
#include "simulation/simulators/cfdHandlers/cfdSimulator.h"
#include "simulation/simulators/cfdHandlers/olbContinuous.h"
// #include "simulation/simulators/cfdHandlers/olbMixing.h" //** TODO: HybridMixingSimulation */
// #include "simulation/simulators/cfdHandlers/olbOoc.h"    //** TODO: HybridOocSimulation */

#include "nodalAnalysis/NodalAnalysis.h"

#include "hybridDynamics/Scheme.h"
#include "hybridDynamics/Naive.h"

#include "architecture/Channel.h"
#include "architecture/ChannelPosition.h"
#include "architecture/Edge.h"
#include "architecture/FlowRatePump.h"
#include "architecture/Membrane.h"
#include "architecture/Module.h"
#include "architecture/ModuleOpening.h"
#include "architecture/Network.h"
#include "architecture/Node.h"
#include "architecture/PressurePump.h"
#include "architecture/Tank.h"

#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"
#include "olbProcessors/saturatedFluxPostProcessor2D.h"
#include "olbProcessors/setFunctionalRegularizedHeatFlux.h"

#include "porting/jsonPorter.h"
#include "porting/jsonReaders.h"
#include "porting/jsonWriters.h"

#include "result/Results.h"

#ifdef USE_ESSLBM
    #include "simulation/simulators/essContinuous.h"
    #include "simulation/simulators/essMixing.h"
#endif