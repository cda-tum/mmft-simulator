#pragma once

#include <olb2D.h>
#include <olb2D.hh>

namespace olb {

/**
 * Searching and updating droplet locations based on initial locations.
*/

//======================================================================
// =================== Search for droplets ===========================//
//======================================================================

template<typename T, typename DESCRIPTOR>
class DropletSearchPostProcessor2D : public LocalPostProcessor2D<T, DESCRIPTOR> {

public:
    DropletSearchPostProcessor2D(int x0, int x1, int y0, int y1,
    Vector<T,2>& center,
    Vector<T,2>& extend);

    int extent() const override {
        return 0;
    }

    int extend(int whichDirection) const override {
        return 0;
    }

    void process(BlockLattice<T,DESCRIPTOR>& blockLattice) override;

    void processSubDomain(BlockLattice<T,DESCRIPTOR>& blockLattice,
        int x0, int x1, int y0, int y1) override;

    Vector<T,4> getSize() {
        return Vector<T,4> {Left, Right, Top, Bottom};
    }

private:
    int x0, x1, y0, y1;
    Vector<T,2> center;
    Vector<T,2> extend;
    int Left, Right, Bottom, Top
    
};

}   /// namespace olb