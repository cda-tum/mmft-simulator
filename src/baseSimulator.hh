#include "simulation/entities/Droplet.hh"
#include "simulation/entities/Fluid.hh"
#include "simulation/entities/Mixture.hh"
#include "simulation/entities/Specie.hh"
#include "simulation/entities/Tissue.hh"

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
// #include "simulation/simulators/cfdHandlers/olbMixing.h" //** TODO: HybridMixingSimulation */
// #include "simulation/simulators/cfdHandlers/olbOoc.h"    //** TODO: HybridOocSimulation */


#include "nodalAnalysis/NodalAnalysis.hh"

#include "hybridDynamics/Scheme.hh"
#include "hybridDynamics/Naive.hh"

#include "architecture/definitions/ChannelPosition.hh"

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Edge.hh"
#include "architecture/entities/FlowRatePump.hh"
#include "architecture/entities/Membrane.hh"
#include "architecture/entities/Module.hh"
#include "architecture/entities/Node.hh"
#include "architecture/entities/PressurePump.hh"
#include "architecture/entities/Tank.hh"

#include "architecture/Network.hh"

//** TODO: HybridOocSimulation */
// #include "olbProcessors/navierStokesAdvectionDiffusionCouplingPostProcessor2D.hh"
// #include "olbProcessors/saturatedFluxPostProcessor2D.hh"
// #include "olbProcessors/setFunctionalRegularizedHeatFlux.hh"

#include "porting/jsonPorter.hh"
#include "porting/jsonReaders.hh"
#include "porting/jsonWriters.hh"

#include "result/Results.hh"

#ifdef USE_ESSLBM
    #include "simulation/simulators/essContinuous.hh"
    #include "simulation/simulators/essMixing.hh"
#endif