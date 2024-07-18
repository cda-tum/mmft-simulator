#include "setFunctionalRegularizedHeatFlux.h"

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

/**
* Class for setting a regularized head flux boundary condition on 
* advection diffusion fields, depending on the temperature (concentration)
* value of the neighbouring node(s).
*/

// Initialising the FunctionalRegularizedHeatFluxBoundary on the superLattice domain
template<typename T, typename DESCRIPTOR, typename MixinDynamics>
void setFunctionalRegularizedHeatFluxBoundary(SuperLattice<T, DESCRIPTOR>& sLattice, T omega, SuperGeometry<T,2>& superGeometry, int material, T *Vmax, T *Km) 
{
  setFunctionalRegularizedHeatFluxBoundary<T,DESCRIPTOR,MixinDynamics>(sLattice, omega, superGeometry.getMaterialIndicator(material), Vmax, Km);
}

// Initializing the FunctionalRegularizedHeatFluxBoundary on the superLattice domain
template<typename T, typename DESCRIPTOR, typename MixinDynamics>
void setFunctionalRegularizedHeatFluxBoundary(SuperLattice<T, DESCRIPTOR>& sLattice, T omega, FunctorPtr<SuperIndicatorF2D<T>>&& indicator, T *Vmax, T *Km)
{
  int _overlap = 1;
  for (int iCloc = 0; iCloc < sLattice.getLoadBalancer().size(); ++iCloc) {
    setFunctionalRegularizedHeatFluxBoundary<T,DESCRIPTOR,MixinDynamics>(sLattice.getBlock(iCloc), omega,
    indicator->getBlockIndicatorF(iCloc), Vmax, Km);
  }
  // Adds needed Cells to the Communicator _commBC in SuperLattice
  addPoints2CommBC<T, DESCRIPTOR>(sLattice, std::forward<decltype(indicator)>(indicator), _overlap);
}


// Set FunctionalRegularizedHeatFluxBoundary for indicated cells inside the block domain
template<typename T, typename DESCRIPTOR, typename MixinDynamics>
void setFunctionalRegularizedHeatFluxBoundary(BlockLattice<T,DESCRIPTOR>& block, T omega, BlockIndicatorF2D<T>& indicator, T *Vmax, T *Km)
{
  using namespace boundaryhelper;
  auto& blockGeometryStructure = indicator.getBlockGeometry();
  OstreamManager clout(std::cout, "setFunctionalRegularizedHeatFluxBoundary");
  /*
   *x0,x1,y0,y1 Range of cells to be traversed
   **/
  int x0 = 0;
  int y0 = 0;
  int x1 = blockGeometryStructure.getNx()-1;
  int y1 = blockGeometryStructure.getNy()-1;
  std::vector<int> discreteNormal(3,0);
  for (int iX = x0; iX <= x1; ++iX) {
    for (int iY = y0; iY <= y1; ++iY) {
      Dynamics<T,DESCRIPTOR>* dynamics = nullptr;
      if (indicator(iX, iY)) {
        discreteNormal = indicator.getBlockGeometry().getStatistics().getType(iX,iY);
        if (discreteNormal[0] == 0) { // set momenta, dynamics and posprocessor on indicated FunctionalRegularizedHeatFluxBoundary cells
          dynamics = block.getDynamics(PlainMixinDynamicsForDirectionOrientationMomenta<T,DESCRIPTOR,
            CombinedAdvectionDiffusionRLBdynamics,
            MixinDynamics,momenta::RegularizedHeatFluxBoundaryTuple
          >::construct(Vector<int,2>(discreteNormal.data() + 1)));
          setBoundary(block, iX, iY, dynamics);

          auto fluxPostProcessor = std::unique_ptr<PostProcessorGenerator2D<T, DESCRIPTOR>>{
            new SaturatedFluxProcessorGenerator2D<T, DESCRIPTOR>(
            iX, iX, iY, iY, discreteNormal[1], discreteNormal[2], Vmax, Km )
          };

          block.addPostProcessor(*fluxPostProcessor);
        }
        else if (discreteNormal[0] == 1) { // set momenta, dynamics and postProcessor on indicated FunctionalRegularizedHeatFluxBoundary external corner cells
          dynamics = block.getDynamics(NormalMixinDynamicsForPlainMomenta<T,DESCRIPTOR,
            AdvectionDiffusionCornerDynamics2D,
            MixinDynamics,momenta::EquilibriumBoundaryTuple
          >::construct(Vector<int,2>(discreteNormal.data() + 1)));
          setBoundary(block, iX, iY, dynamics);
        }
        if (dynamics) {
          dynamics->getParameters(block).template set<descriptors::OMEGA>(omega);
        }
      }
    }
  }
}



}
