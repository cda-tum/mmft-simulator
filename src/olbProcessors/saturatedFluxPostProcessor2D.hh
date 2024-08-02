#include "saturatedFluxPostProcessor2D.h"

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

////////  SaturationFluxProcessor2D ////////////////////////////

template<typename T, typename DESCRIPTOR>
SaturatedFluxProcessor2D<T,DESCRIPTOR>::
SaturatedFluxProcessor2D(int x0_, int x1_, int y0_, int y1_, int discreteNormalX_, int discreteNormalY_, T *Vmax_, T *Km_)
  : x0(x0_), x1(x1_), y0(y0_), y1(y1_),
    discreteNormalX(discreteNormalX_), discreteNormalY(discreteNormalY_),
    Vmax(Vmax_), Km(Km_)
{
  this->getName() = "SaturatedFluxProcessor2D";
}

template<typename T, typename DESCRIPTOR>
void SaturatedFluxProcessor2D<T,DESCRIPTOR>::
processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice, int x0_, int x1_, int y0_, int y1_)
{
  int newX0, newX1, newY0, newY1;
  if ( util::intersect (
         x0, x1, y0, y1,
         x0_, x1_, y0_, y1_,
         newX0, newX1, newY0, newY1 ) ) {

    T u[2];
    for (int iX=newX0; iX<=newX1; ++iX) {
      for (int iY=newY0; iY<=newY1; ++iY) {
        T tempFluid = blockLattice.get(iX-discreteNormalX, iY-discreteNormalY).computeRho();
        T flux = ((*Vmax) * tempFluid) / ((*Km) + tempFluid);
        u[0] = flux*discreteNormalX;
        u[1] = flux*discreteNormalY;
        blockLattice.get(iX,iY).defineU(u);
      }
    }
  }
}

template<typename T, typename DESCRIPTOR>
void SaturatedFluxProcessor2D<T,DESCRIPTOR>::
process(BlockLattice<T,DESCRIPTOR>& blockLattice)
{
  processSubDomain(blockLattice, x0, x1, y0, y1);
}

template<typename T, typename DESCRIPTOR>
SaturatedFluxProcessorGenerator2D<T,DESCRIPTOR>::
SaturatedFluxProcessorGenerator2D(int x0_, int x1_, int y0_, int y1_, int discreteNormalX_,
                                   int discreteNormalY_, T *Vmax_, T *Km_)
  : PostProcessorGenerator2D<T,DESCRIPTOR>(x0_, x1_, y0_, y1_), discreteNormalX(discreteNormalX_), 
    discreteNormalY(discreteNormalY_), Vmax(Vmax_), Km(Km_)
{ }

template<typename T, typename DESCRIPTOR>
PostProcessor2D<T,DESCRIPTOR>*
SaturatedFluxProcessorGenerator2D<T,DESCRIPTOR>::generate() const
{
  return new SaturatedFluxProcessor2D<T,DESCRIPTOR>
         (this->x0, this->x1, this->y0, this->y1, discreteNormalX, discreteNormalY, Vmax, Km);
}

template<typename T, typename DESCRIPTOR>
PostProcessorGenerator2D<T,DESCRIPTOR>*
SaturatedFluxProcessorGenerator2D<T,DESCRIPTOR>::clone() const
{
  return new SaturatedFluxProcessorGenerator2D<T,DESCRIPTOR>
         (this->x0, this->x1, this->y0, this->y1, discreteNormalX, discreteNormalY, Vmax, Km);
}

}
