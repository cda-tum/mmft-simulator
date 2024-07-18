#pragma once

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

///Initialising the FunctionalRegularizedHeatFluxBoundary on the superLattice domain
///This is an advection diffusion boundary -->MixinDynamics = AdvectionDiffusionRLBdynamics
template<typename T, typename DESCRIPTOR, typename MixinDynamics=AdvectionDiffusionRLBdynamics<T,DESCRIPTOR>>
void setFunctionalRegularizedHeatFluxBoundary(SuperLattice<T, DESCRIPTOR>& sLattice,T omega, SuperGeometry<T,2>& superGeometry, int material, T *Vmax=nullptr, T *Km=nullptr);

///Initialising the RegularizedHeatFluxBoundary on the superLattice domain
template<typename T, typename DESCRIPTOR, typename MixinDynamics>
void setFunctionalRegularizedHeatFluxBoundary(SuperLattice<T, DESCRIPTOR>& sLattice,T omega, FunctorPtr<SuperIndicatorF2D<T>>&& indicator,  T *Vmax=nullptr, T *Km=nullptr);


///Set RegularizedHeatFluxBoundary for indicated cells inside the block domain
template<typename T, typename DESCRIPTOR, typename MixinDynamics>
void setFunctionalRegularizedHeatFluxBoundary(BlockLattice<T,DESCRIPTOR>& block, T omega, BlockIndicatorF2D<T>& indicator, T *Vmax, T *Km);

}//namespace olb
