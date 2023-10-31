#include "navierStokesAdvectionDiffusionCouplingPostProcessor2D.h"

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

//=====================================================================================
//===========  NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D ============
//=====================================================================================

template<typename T, typename DESCRIPTOR>
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D<T,DESCRIPTOR>::
NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D(int x0_, int x1_, int y0_, int y1_, T velFactors_, std::vector<BlockStructureD<2>* > partners_)
  :  x0(x0_), x1(x1_), y0(y0_), y1(y1_), velFactors(velFactors_), partners(partners_)
{
  this->getName() = "NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D";
  for (int i = 0; i<partners_.size(); i++) {
    tPartners[i] = static_cast<BlockLattice<T,descriptors::D2Q5<descriptors::VELOCITY,descriptors::SOURCE>> *>(partners[i]);
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

    for (int i = 0; i<partners.size(); i++) {
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
NavierStokesAdvectionDiffusionSingleCouplingGenerator2D(int x0_, int x1_, int y0_, int y1_, T velFactors_)
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

//=====================================================================================
//======================  ElectroOsmoticCouplingPostProcessor2D =======================
//=====================================================================================

template<typename T, typename DESCRIPTOR>
ElectroOsmoticCouplingPostProcessor2D<T,DESCRIPTOR>::
ElectroOsmoticCouplingPostProcessor2D(int x0_, int x1_, int y0_, int y1_,
    T E_, T er_, std::vector<T> dir_, 
    const std::vector<T>& ionValence_,
    std::vector<BlockStructureD<2>* > partners_)
  :  x0(x0_), x1(x1_), y0(y0_), y1(y1_),
     E(E_), er(er_), dir(dir_), ionValence(ionValence_), partners(partners_)
{
  this->getName() = "ElectroOsmoticCouplingPostProcessor2D";
  // we normalize the direction of force vector
  T normDir = T();
  for (unsigned iD = 0; iD < dir.size(); ++iD) {
    normDir += dir[iD]*dir[iD];
  }
  normDir = util::sqrt(normDir);
  for (unsigned iD = 0; iD < dir.size(); ++iD) {
    dir[iD] /= normDir;
  }

  for (unsigned iD = 0; iD < dir.size(); ++iD) {
    forcePrefactor[iD] = E * dir[iD];
  }

  PsiPartner = static_cast<BlockLattice<T,descriptors::D2Q9<descriptors::SOURCE>> *>(partners[0]);
  for (int i = 1; i<this->component_number-1;i++){
    ADPartners.emplace_back(
    static_cast<BlockLattice<T,descriptors::D2Q5<descriptors::VELOCITY,descriptors::SOURCE>> *>(partners[i]));
  }
}

template<typename T, typename DESCRIPTOR>
void ElectroOsmoticCouplingPostProcessor2D<T,DESCRIPTOR>::
processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                 int x0_, int x1_, int y0_, int y1_)
{
  int newX0, newX1, newY0, newY1;
  if ( util::intersect (
         x0, x1, y0, y1,
         x0_, x1_, y0_, y1_,
         newX0, newX1, newY0, newY1 ) ) {

    for (int iX=newX0; iX<=newX1; ++iX) {
      for (int iY=newY0; iY<=newY1; ++iY) {

        auto cell = blockLattice.get(iX,iY);
        auto psiCell = PsiPartner->get(iX,iY);
        
        // Computation of the charge density
        T temp_charge_den = 0.;
        for (int iter_component = 0; iter_component<this->component_number; ++ iter_component){
          T conc = ADPartners[iter_component]->get(iX,iY).computeRho();
          temp_charge_den += k * ionValence[iter_component] * conc / er;
        }

        // Charge density coupling
        psiCell.template setField<descriptors::SOURCE>(temp_charge_den);
        
        // Force computation
        auto force = cell.template getFieldPointer<descriptors::FORCE>();
        for (unsigned iD = 0; iD < L::d; ++iD) {
          force[iD] = forcePrefactor[iD] * temp_charge_den;
        }       

        // Velocity coupling
        T u[DESCRIPTOR::d] { };
        cell.computeU(u);
        for (int iter_component = 0; iter_component<this->component_number; ++ iter_component){
          ADPartners[iter_component]->get(iX,iY).template setField<descriptors::VELOCITY>(u);
        }
      }
    }
  }
}

template<typename T, typename DESCRIPTOR>
void ElectroOsmoticCouplingPostProcessor2D<T,DESCRIPTOR>::
process(BlockLattice<T,DESCRIPTOR>& blockLattice)
{
  processSubDomain(blockLattice, x0, x1, y0, y1);
}

/// LatticeCouplingGenerator for advectionDiffusion coupling

template<typename T, typename DESCRIPTOR>
ElectroOsmoticCouplingGenerator2D<T,DESCRIPTOR>::
ElectroOsmoticCouplingGenerator2D(int x0_, int x1_, int y0_, int y1_,
    T E_, T er_, std::vector<T> dir_, const std::vector<T>& ionValence_)
  : LatticeCouplingGenerator2D<T,DESCRIPTOR>(x0_, x1_, y0_, y1_),
    E(E_), er(er_), dir(dir_), ionValence(ionValence_)
{ }

template<typename T, typename DESCRIPTOR>
PostProcessor2D<T,DESCRIPTOR>* ElectroOsmoticCouplingGenerator2D<T,DESCRIPTOR>::generate (
  std::vector<BlockStructureD<2>* > partners) const
{
  return new ElectroOsmoticCouplingPostProcessor2D<T,DESCRIPTOR>(
           this->x0,this->x1,this->y0,this->y1, E, er, dir, ionValence, partners);
}

template<typename T, typename DESCRIPTOR>
LatticeCouplingGenerator2D<T,DESCRIPTOR>* ElectroOsmoticCouplingGenerator2D<T,DESCRIPTOR>::clone() const
{
  return new ElectroOsmoticCouplingGenerator2D<T,DESCRIPTOR>(*this);
}

}  // namespace olb
