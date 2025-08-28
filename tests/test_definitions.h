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
        sim::Specie<T>::resetSpecieCounter();
        sim::Mixture<T>::resetMixtureCounter();
        sim::MixtureInjection<T>::resetMixtureInjectionCounter();
        sim::CFDSimulator<T>::resetSimulatorCounter();
    }
};

template<typename T>
class GeometryTest : public GlobalTest<T> {
protected:
    sim::lbmSimulator<T> createLbmGeometry(std::string name, std::shared_ptr<arch::CfdModule<T>> cfdModule, T resolution) { 
        return sim::lbmSimulator(0, name, cfdModule, 0.0, 0.0, resolution, 0.0, 0.932); 
    }

    void readGeometryStl(sim::lbmSimulator<T>& simulator, T dx) { simulator.readGeometryStl(dx, false); }

    void readOpenings(sim::lbmSimulator<T>& simulator, T dx) { simulator.readOpenings(dx); }

    size_t readVoxels(sim::lbmSimulator<T>& simulator, unsigned char m) { return simulator.readGeometry().getStatistics().getNvoxel(m); }
};

}
