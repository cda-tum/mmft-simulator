#include "navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"

namespace olb {

//=====================================================================================
//===========  NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D ============
//=====================================================================================

template<typename T, typename DESCRIPTOR>
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D<T,DESCRIPTOR>::
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D(int x0_, int x1_, int y0_, int y1_, std::vector<T> velFactors_, std::vector<BlockStructureD<2>* > partners_)
  :  x0(x0_), x1(x1_), y0(y0_), y1(y1_), velFactors(velFactors_), partners(partners_)
{
  this->getName() = "NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D";
  for (long unsigned int i = 0; i<partners_.size(); i++) {
    tPartners.emplace_back(static_cast<BlockLattice<T,descriptors::D2Q5<descriptors::VELOCITY>> *>(partners[i]));
  }
}

template<typename T, typename DESCRIPTOR>
void NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D<T,DESCRIPTOR>::
processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                 int x0_, int x1_, int y0_, int y1_)
{
  int newX0, newX1, newY0, newY1;
  if ( util::intersect (
         x0, x1, y0, y1,
         x0_, x1_, y0_, y1_,
         newX0, newX1, newY0, newY1 ) ) {

    for (long unsigned int i = 0; i<partners.size(); i++) {
      for (int iX=newX0; iX<=newX1; ++iX) {
        for (int iY=newY0; iY<=newY1; ++iY) {
          auto cell = blockLattice.get(iX,iY);
          auto partnerCell = tPartners[i]->get(iX,iY);
          // Velocity coupling
          T u[DESCRIPTOR::d] { };
          cell.computeU(u);
          for ( int d = 0; d<DESCRIPTOR::d; d++ ) {
            u[d] *= velFactors[i];
          }
          partnerCell.template setField<descriptors::VELOCITY>(u);
        }
      }
    }
  }
}

template<typename T, typename DESCRIPTOR>
void NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D<T,DESCRIPTOR>::
process(BlockLattice<T,DESCRIPTOR>& blockLattice)
{
  processSubDomain(blockLattice, x0, x1, y0, y1);
}

/// LatticeCouplingGenerator for advectionDiffusion coupling

template<typename T, typename DESCRIPTOR>
NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR>::
NavierStokesAdvectionDiffusionSingleCouplingGenerator2D(int x0_, int x1_, int y0_, int y1_, std::vector<T> velFactors_)
  : LatticeCouplingGenerator2D<T,DESCRIPTOR>(x0_, x1_, y0_, y1_), velFactors(velFactors_)
{ }

template<typename T, typename DESCRIPTOR>
PostProcessor2D<T,DESCRIPTOR>* NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR>::generate (
  std::vector<BlockStructureD<2>* > partners) const
{
  return new NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D<T,DESCRIPTOR>(
           this->x0,this->x1,this->y0,this->y1, this->velFactors, partners);
}

template<typename T, typename DESCRIPTOR>
LatticeCouplingGenerator2D<T,DESCRIPTOR>* NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR>::clone() const
{
  return new NavierStokesAdvectionDiffusionSingleCouplingGenerator2D<T,DESCRIPTOR>(*this);
}

}  // namespace olb
