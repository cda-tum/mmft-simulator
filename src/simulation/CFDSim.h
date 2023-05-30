#pragma once

#include <olb2D.h>
#include <olb2D.hh>

#include <Network.h>
#include <Module.h>
#include <lbmModule.h>

namespace sim {

    template<typename T>
    bool conductCFDSimulation(const arch::Network<T>* network);

}   // namespace sim