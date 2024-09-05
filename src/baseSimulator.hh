#include "simulation/CFDSim.hh"
#include "simulation/Droplet.hh"
#include "simulation/Fluid.hh"
#include "simulation/Injection.hh"
#include "simulation/MembraneModels.hh"
#include "simulation/Mixture.hh"
#include "simulation/MixtureInjection.hh"
#include "simulation/MixingModels.hh"
#include "simulation/ResistanceModels.hh"
#include "simulation/simulators/cfdSimulator.hh"
#include "simulation/Specie.hh"
#include "simulation/Tissue.hh"
#include "simulation/events/BoundaryEvent.hh"
#include "simulation/events/InjectionEvent.hh"
#include "simulation/events/MergingEvent.hh"

#if DIMENSION == 2
#include "simulation/simulators/2D/olbContinuous2D.hh"
#include "simulation/simulators/2D/olbMixing2D.hh"
#include "simulation/simulators/2D/olbOoc2D.hh"
#endif
#if DIMENSION == 3
#include "simulation/simulators/3D/olbContinuous3D.hh"
//#include "simulation/simulators/3D/olbMixing3D.hh"
//#include "simulation/simulators/3D/olbOoc3D.hh"
#endif

#include "nodalAnalysis/NodalAnalysis.hh"

#include "hybridDynamics/Scheme.hh"
#include "hybridDynamics/Naive.hh"

#include "architecture/Channel.hh"
#include "architecture/ChannelPosition.hh"
#include "architecture/Edge.hh"
#include "architecture/FlowRatePump.hh"
#include "architecture/Module.hh"
#include "architecture/Network.hh"
#include "architecture/Node.hh"
#include "architecture/PressurePump.hh"

#if DIMENSION == 2
#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.hh"
#include "olbProcessors/saturatedFluxPostProcessor2D.hh"
#include "olbProcessors/setFunctionalRegularizedHeatFlux2D.hh"
#endif
#if DIMENSION == 3
#include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor3D.hh"
#include "olbProcessors/saturatedFluxPostProcessor2D.hh"
//#include "olbProcessors/setFunctionalRegularizedHeatFlux3D.hh"
#endif

#include "porting/jsonPorter.hh"
#include "porting/jsonReaders.hh"
#include "porting/jsonWriters.hh"

#include "result/Results.hh"

#ifdef USE_ESSLBM
    #include "simulation/simulators/essContinuous.hh"
    #include "simulation/simulators/essMixing.hh"
#endif

#include "simulation/Simulation.hh"