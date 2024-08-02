#pragma once

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

////////  SaturationFluxProcessor2D ////////////////////////////

template<typename T, typename DESCRIPTOR>
class SaturatedFluxProcessor2D : public LocalPostProcessor2D<T, DESCRIPTOR> {
public:
  SaturatedFluxProcessor2D(int x0_, int x1_, int y0_, int y1_,
                            int discreteNormalX_, int discreteNormalY_, T *Vmax, T *Km);
  int extent() const override
  {
    return 1;
  }
  int extent(int whichDirection) const override
  {
    return 1;
  }
  void process(BlockLattice<T,DESCRIPTOR>& blockLattice) override;
  void processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
                        int x0_,int x1_,int y0_,int y1_ ) override;
private:
  int x0, x1, y0, y1;
  int discreteNormalX, discreteNormalY;
  T *Vmax;
  T *Km;
};

/// Generator class for the FreeEnergyWall PostProcessor handling the wetting boundary condition.
template<typename T, typename DESCRIPTOR>
class SaturatedFluxProcessorGenerator2D : public PostProcessorGenerator2D<T, DESCRIPTOR> {
public:
  SaturatedFluxProcessorGenerator2D(int x0_, int x1_, int y0_, int y1_,
                                     int discreteNormalX_, int discreteNormalY_, T *Vmax_, T *Km_);
  PostProcessor2D<T,DESCRIPTOR>* generate() const override;
  PostProcessorGenerator2D<T,DESCRIPTOR>*  clone() const override;
private:
  int discreteNormalX;
  int discreteNormalY;
  T *Vmax;
  T *Km;
};
}
