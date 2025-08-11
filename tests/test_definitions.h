#pragma once

#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

namespace test::definitions {

template<typename T>
class GlobalTest : public ::testing::Test {
protected:
    void SetUp() override {
        sim::Fluid<T>::resetFluidCounter();
        sim::DropletImplementation<T>::resetDropletCounter();
        sim::DropletInjection<T>::resetDropletInjectionCounter();
    }
};

}
