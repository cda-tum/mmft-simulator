#pragma once

namespace olb {

/**
* Class for the coupling between a Navier-Stokes (NS) lattice and an
* Advection-Diffusion (AD) lattice.
*/

//======================================================================
// ========== AD coupling in Single Direction 2D =======================//
//======================================================================
template<typename T, typename DESCRIPTOR>
class NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D : public LocalPostProcessor3D<T,DESCRIPTOR> {
private:
  typedef DESCRIPTOR L;
  int x0, x1, y0, y1, z0, z1;
  const std::vector<T> velFactors;
  std::vector<BlockLattice<T,descriptors::D3Q7<descriptors::VELOCITY>>*> tPartners;
  std::vector<BlockStructureD<3>*> partners;

public:
  NavierStokesAdvectionDiffusionSingleCouplingPostProcessor3D(int x0_, int x1_, int y0_, int y1_, int z0_, int z1_, std::vector<T> velFactors,
      std::vector<BlockStructureD<3>* > partners_);
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
                        int x0_, int x1_, int y0_, int y1_, int z0_, int z1_) override;
};

template<typename T, typename DESCRIPTOR>
class NavierStokesAdvectionDiffusionSingleCouplingGenerator3D : public LatticeCouplingGenerator3D<T,DESCRIPTOR> {
private:
  const std::vector<T> velFactors;

public:
  NavierStokesAdvectionDiffusionSingleCouplingGenerator3D(int x0_, int x1_, int y0_, int y1_, int z0_, int z1_, std::vector<T> velFactors_);
  PostProcessor3D<T,DESCRIPTOR>* generate(std::vector<BlockStructureD<3>* > partners) const override;
  LatticeCouplingGenerator3D<T,DESCRIPTOR>* clone() const override;
};

}
