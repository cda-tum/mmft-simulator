#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"
#include "../test_definitions.h"

using T = double;

class Cfd : public test::definitions::GlobalTest<T> {};

TEST_F(Cfd, Case1a) {

}
