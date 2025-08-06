#pragma once

#include "../src/baseSimulator.h"

#include "gtest/gtest.h"

namespace test::definitions {

template<typename T>
class GlobalTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << "I'm being called" << std::endl;
        sim::Fluid<T>::resetFluidCounter();
    }
};

}
