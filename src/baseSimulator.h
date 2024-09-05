/**
 * @file baseSimulator.h
 */
#pragma once

#include "simulation/CFDSim.h"
#include "simulation/Droplet.h"
#include "simulation/Fluid.h"
#include "simulation/Injection.h"
#include "simulation/MembraneModels.h"
#include "simulation/Mixture.h"
#include "simulation/MixtureInjection.h"
#include "simulation/MixingModels.h"
#include "simulation/ResistanceModels.h"
#include "simulation/simulators/cfdSimulator.h"
#include "simulation/simulators/olbSim.h"
#include "simulation/Specie.h"
#include "simulation/Tissue.h"
#include "simulation/events/BoundaryEvent.h"
#include "simulation/events/Event.h"
#include "simulation/events/InjectionEvent.h"
#include "simulation/events/MergingEvent.h"

#if DIMENSION == 2
#include "simulation/simulators/2D/2D.h"
#include "simulation/simulators/2D/olbContinuous2D.h"
#include "simulation/simulators/2D/olbMixing2D.h"
#include "simulation/simulators/2D/olbOoc2D.h"
#endif
#if DIMENSION == 3
#include "simulation/simulators/3D/3D.h"
#include "simulation/simulators/3D/olbContinuous3D.h"
//#include "simulation/simulators/3D/olbMixing3D.h"
//#include "simulation/simulators/3D/olbOoc3D.h"
#endif

#include "nodalAnalysis/NodalAnalysis.h"

#include "hybridDynamics/Scheme.h"
#include "hybridDynamics/Naive.h"

#include "architecture/Channel.h"
#include "architecture/ChannelPosition.h"
#include "architecture/Edge.h"
#include "architecture/FlowRatePump.h"
#include "architecture/Module.h"
#include "architecture/ModuleOpening.h"
#include "architecture/Network.h"
#include "architecture/Node.h"
#include "architecture/PressurePump.h"

#if DIMENSION == 2
#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"
#include "olbProcessors/saturatedFluxPostProcessor2D.h"
#include "olbProcessors/setFunctionalRegularizedHeatFlux2D.h"
#endif
#if DIMENSION == 3
#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor3D.h"
#include "olbProcessors/saturatedFluxPostProcessor2D.h"
//#include "olbProcessors/setFunctionalRegularizedHeatFlux3D.h"
#endif

#include "porting/jsonPorter.h"
#include "porting/jsonReaders.h"
#include "porting/jsonWriters.h"

#include "result/Results.h"

#ifdef USE_ESSLBM
    #include "simulation/simulators/essContinuous.h"
    #include "simulation/simulators/essMixing.h"
#endif

#include "simulation/Simulation.h"