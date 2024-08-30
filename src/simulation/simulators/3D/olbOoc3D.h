/**
 * @file olbOoc.h
 */

#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

namespace arch {

// Forward declared dependencies
template<typename T>
class Module;
template<typename T>
class Network;
template<typename T>
class Node;
template<typename T>
class Opening;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Tissue;

/**
 * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
*/
template<typename T>
class lbmOocSimulator3D : public lbmOocSimulator<T> {

using DESCRIPTOR = olb::descriptors::D3Q19<>;
using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

using ADDESCRIPTOR = olb::descriptors::D3Q7<olb::descriptors::VELOCITY>;
using ADDynamics = olb::AdvectionDiffusionBGKdynamics<T,ADDESCRIPTOR>;
using NoADDynamics = olb::NoDynamics<T,ADDESCRIPTOR>;

private:

public:

};

}   // namespace arch
