/**
 * @file baseSimulator.h
 */
#pragma once

#include "simulation/CFDSim.h"
#include "simulation/Droplet.h"
#include "simulation/Fluid.h"
#include "simulation/Injection.h"
#include "simulation/ResistanceModels.h"
#include "simulation/Simulation.h"
#include "simulation/events/BoundaryEvent.h"
#include "simulation/events/Event.h"
#include "simulation/events/InjectionEvent.h"
#include "simulation/events/MergingEvent.h"

#include "nodalAnalysis/NodalAnalysis.h"

#include "architecture/Channel.h"
#include "architecture/ChannelPosition.h"
#include "architecture/Edge.h"
#include "architecture/FlowRatePump.h"
#include "architecture/lbmModule.h"
#include "architecture/Module.h"
#include "architecture/Network.h"
#include "architecture/Node.h"
#include "architecture/PressurePump.h"

#include "porting/jsonPorter.h"
#include "porting/jsonReaders.h"
#include "porting/jsonWriters.h"

#include "result/Results.h"