/**
 * @file jsonReaders.h
 */

#pragma once

#include <memory>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "../architecture/Channel.h"
#include "../architecture/FlowRatePump.h"
#include "../architecture/lbmModule.h"
#include "../architecture/Network.h"
#include "../architecture/Node.h"
#include "../architecture/PressurePump.h"

#include "../simulation/Simulation.h"

#include "../result/Results.h"

#include "nlohmann/json.hpp"

namespace porting {

    template<typename T>
    void readNodes (json jsonString, arch::Network<T>& network);

    template<typename T>
    void readChannels (json jsonString, arch::Network<T>& network);

    template<typename T>
    sim::Platform readPlatform (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    sim::Type readType (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    void readFluids (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    void readDroplets (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    void readSpecies (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    void readDropletInjections (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

    template<typename T>
    void readMixtureInjections (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

    template<typename T>
    void readBoundaryConditions (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

    template<typename T>
    void readContinuousPhase (json jsonString, sim::Simulation<T>& simulation, int activeFixture);

    template<typename T>
    void readSimulators (json jsonString, arch::Network<T>* network);

    template<typename T>
    void readPumps (json jsonString, arch::Network<T>* network);

    template<typename T>
    void readResistanceModel (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    void readMixingModel (json jsonString, sim::Simulation<T>& simulation);

    template<typename T>
    int readActiveFixture (json jsonString);

}   // namespace porting