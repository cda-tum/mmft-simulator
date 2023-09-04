#include "dropletSearchPostProcessor2D.h"

#include <olb2D.h>
#include <olb2D.hh>

#include <iostream>

namespace olb {
/**
* Coupling of the potentialLattice with the ADlattices (tpartners)
* Evaluate Grad( Ci Grad(Psi) ) and store result in SOURCE field of all AD lattices
*/

//======================================================================
// ======== AD coupling with Electric Potential 2D ===================//
//======================================================================

template<typename T, typename DESCRIPTOR>
DropletSearchPostProcessor2D<T,DESCRIPTOR>::DropletSearchPostProcessor2D(
    int x0_, int x1_, int y0_, int y1_,
    Vector<T,2>& center_,
    Vector<T,2>& extend_)
    : x0(x0_), x1(x1_), y0(y0_), y1(y1_), center(center_), extend(extend_),
    Left(center_[0]), Right(center_[0]), Top(center_[1]), Bottom(center_[1]) {

        this->getName() = "DropletSearchPostProcessor2D";
}

template<typename T, typename DESCRIPTOR>
void DropletSearchPostProcessor2D<T,DESCRIPTOR>::process(BlockLattice<T,DESCRIPTOR>& blockLattice) {
  processSubDomain(blockLattice, x0, x1, y0, y1);
}

template<typename T, typename DESCRIPTOR>
void DropletSearchPostProcessor2D<T,DESCRIPTOR>::processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
  int x0_, int x1_, int y0_, int y1_) {

    int newX0, newX1, newY0, newY1;

    // Horizontal search
    if ( util::intersect (
    x0, x1, center[1], center[1],
    x0_, x1_, y0_, y1_,
    newX0, newX1, newY0, newY1 ) ) {
        for (int iX=newX0; iX<=newX1; ++iX){
            auto cell = blockLattice.get(iX, center[1]);
            T rho = cell.computeRho();
            if (rho < 0.0) {
                if (iX < Left) {
                    Left = iX;
                }
                if (iX > Right) {
                    Right = iX;
                }
            }
        }
    }

    // Vertical search
    if ( util::intersect (
    center[0], center[0], y0, y1,
    x0_, x1_, y0_, y1_,
    newX0, newX1, newY0, newY1 ) ) {
        for (int iY=newY0; iY<=newY1; ++iY){
            auto cell = blockLattice.get(center[0], iY);
            T rho = cell.computeRho();
            if (rho < 0.0) {
                if (iY < Bottom) {
                    Bottom = iY;
                }
                if (iY > Top) {
                    Top = iY;
                }
            }
        }
    }
}

}   /// namespace olb