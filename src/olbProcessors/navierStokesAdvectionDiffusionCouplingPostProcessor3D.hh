#include "navierStokesAdvectionDiffusionCouplingPostProcessor3D.h"

namespace olb {

//=====================================================================================
//===========  NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D ============
//=====================================================================================

template<typename T, typename DESCRIPTOR>
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D<T,DESCRIPTOR>::
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D(int x0_, int x1_, int y0_, int y1_, int z0_, int z1_, std::vector<T> velFactors_, std::vector<BlockStructureD<3>* > partners_)
  :  x0(x0_), x1(x1_), y0(y0_), y1(y1_), z0(z0_), z1(z1_), velFactors(velFactors_), partners(partners_)
{
  this->getName() = "NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D";
  for (long unsigned int i = 0; i<partners_.size(); i++) {
    tPartners.emplace_back(static_cast<BlockLattice<T,descriptors::D3Q7<descriptors::VELOCITY>> *>(partners[i]));
  }
}

template<typename T, typename DESCRIPTOR>
void NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D<T,DESCRIPTOR>::
processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                 int x0_, int x1_, int y0_, int y1_, int z0_, int z1_)
{
  int newX0, newX1, newY0, newY1, newZ0, newZ1;
  if ( util::intersect (
         x0, x1, y0, y1, z0, z1,
         x0_, x1_, y0_, y1_, z0_, z1_,
         newX0, newX1, newY0, newY1, newZ0, newZ1 ) ) {

    for (long unsigned int i = 0; i<partners.size(); i++) {
      for (int iX=newX0; iX<=newX1; ++iX) {
        for (int iY=newY0; iY<=newY1; ++iY) {
          for (int iZ=newZ0; iZ<=newZ1; ++iZ) {
            auto cell = blockLattice.get(iX,iY,iZ);
            auto partnerCell = tPartners[i]->get(iX,iY,iZ);
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
}

template<typename T, typename DESCRIPTOR>
void NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D<T,DESCRIPTOR>::
process(BlockLattice<T,DESCRIPTOR>& blockLattice)
{
  processSubDomain(blockLattice, x0, x1, y0, y1, z0, z1);
}

/// LatticeCouplingGenerator for advectionDiffusion coupling

template<typename T, typename DESCRIPTOR>
NavierStokesAdvectionDiffusionSingleCouplingGenerator3D<T,DESCRIPTOR>::
NavierStokesAdvectionDiffusionSingleCouplingGenerator3D(int x0_, int x1_, int y0_, int y1_, int z0_, int z1_, std::vector<T> velFactors_)
  : LatticeCouplingGenerator3D<T,DESCRIPTOR>(x0_, x1_, y0_, y1_, z0_, z1_), velFactors(velFactors_)
{ }

template<typename T, typename DESCRIPTOR>
PostProcessor3D<T,DESCRIPTOR>* NavierStokesAdvectionDiffusionSingleCouplingGenerator3D<T,DESCRIPTOR>::generate (
  std::vector<BlockStructureD<3>* > partners) const
{
  return new NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D<T,DESCRIPTOR>(
           this->x0,this->x1,this->y0,this->y1,this->z0,this->z1, this->velFactors, partners);
}

template<typename T, typename DESCRIPTOR>
LatticeCouplingGenerator3D<T,DESCRIPTOR>* NavierStokesAdvectionDiffusionSingleCouplingGenerator3D<T,DESCRIPTOR>::clone() const
{
  return new NavierStokesAdvectionDiffusionSingleCouplingGenerator3D<T,DESCRIPTOR>(*this);
}

}  // namespace olb
