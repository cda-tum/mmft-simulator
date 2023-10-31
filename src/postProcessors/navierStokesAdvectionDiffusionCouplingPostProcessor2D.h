#pragma once

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {


/**
* Class for the coupling between a Navier-Stokes (NS) lattice and an
* Advection-Diffusion (AD) lattice.
*/

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
// This coupling must be necessarily be put on the Navier-Stokes lattice!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

//======================================================================
// ========== AD coupling in Single Direction 2D =======================//
//======================================================================
template<typename T, typename DESCRIPTOR>
class NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D : public LocalPostProcessor2D<T,DESCRIPTOR> {
public:
  NavierStokesAdvectionDiffusionSingleCouplingPostProcessor2D(int x0_, int x1_, int y0_, int y1_, T velFactors,
      std::vector<BlockStructureD<2>* > partners_);
  int extent() const override
  {
    return 0;
  }
  int extent(int whichDirection) const override
  {
    return 0;
  }
  void process(BlockLattice<T,DESCRIPTOR>& blockLattice) override;
  void processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                        int x0_, int x1_, int y0_, int y1_) override;
private:
  typedef DESCRIPTOR L;
  int x0, x1, y0, y1;
  const std::vector<T> velFactors;
  std::vector<BlockLattice<T,descriptors::D2Q5<descriptors::VELOCITY,descriptors::SOURCE>>*> tPartners;
  std::vector<BlockStructureD<2>*> partners;
};

template<typename T, typename DESCRIPTOR>
class NavierStokesAdvectionDiffusionSingleCouplingGenerator2D : public LatticeCouplingGenerator2D<T,DESCRIPTOR> {
public:
  NavierStokesAdvectionDiffusionSingleCouplingGenerator2D(int x0_, int x1_, int y0_, int y1_, T velFactors_);
  PostProcessor2D<T,DESCRIPTOR>* generate(std::vector<BlockStructureD<2>* > partners) const override;
  LatticeCouplingGenerator2D<T,DESCRIPTOR>* clone() const override;
private:
  const std::vector<T> velFactors;
};

//======================================================================
// ========= AD coupling with Electro-Osmosis 2D =====================//
//======================================================================
template<typename T, typename DESCRIPTOR>
class ElectroOsmoticCouplingPostProcessor2D : public LocalPostProcessor2D<T,DESCRIPTOR> {
public:
  ElectroOsmoticCouplingPostProcessor2D(int x0, int x1, int y0, int y1,
      T E, T er, std::vector<T> dir, 
      const std::vector<T>& ionValence,
      std::vector<BlockStructureD<2>* > partners);
  int extent() const override
  {
    return 0;
  }
  int extent(int whichDirection) const override
  {
    return 0;
  }
  void process(BlockLattice<T,DESCRIPTOR>& blockLattice) override;
  void processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                        int x0_, int x1_, int y0_, int y1_) override;
private:
  typedef DESCRIPTOR L;
  int x0, x1, y0, y1;
  int component_number;
  T E;
  T er;                 // Dielectric constant (relative permittivity) of fluid medium.
  T k = 1.8095512e-8;   // Charge of electron divided by the permittivity 
                        // of vacuum k = 1.602176634e-19/8.854e-12.
  std::vector<T> dir;
  BlockLattice<T,descriptors::D2Q9<descriptors::SOURCE>> *PsiPartner;
  std::vector<BlockLattice<T,descriptors::D2Q5<descriptors::VELOCITY,descriptors::SOURCE>> *> ADPartners;
  T forcePrefactor[L::d];
  std::vector<BlockStructureD<2>*> partners;
  const std::vector<T> ionValence;
};

template<typename T, typename DESCRIPTOR>
class ElectroOsmoticCouplingGenerator2D : public LatticeCouplingGenerator2D<T,DESCRIPTOR> {
public:
  ElectroOsmoticCouplingGenerator2D(int x0, int x1, int y0, int y1,
      T E, T er, std::vector<T> dir,
      const std::vector<T>& ionValence);
  PostProcessor2D<T,DESCRIPTOR>* generate(std::vector<BlockStructureD<2>* > partners) const override;
  LatticeCouplingGenerator2D<T,DESCRIPTOR>* clone() const override;

private:
  T E;
  T er;   // Dielectric constant (relative permittivity) of fluid medium.
  std::vector<T> dir;
  const std::vector<T> ionValence;
};

}
