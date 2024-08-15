#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"

using T = double;

TEST(Cube, deg0) {

    std::string stlFile = "../examples/STL/cube0.stl";
    int resolution = 50;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-5e-5, 0.0);
    network.addNode(0.0, -5e-5);
    network.addNode(5e-5, 0.0);
    network.addNode(0.0, 5e-5);

    // module
    network.addModule(std::vector<T>({-5e-5, -5e-5}), std::vector<T>({1e-4, 1e-4}), std::vector<int>({0, 1, 2, 3}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(1, arch::Opening<T>(network.getNode(1), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(2, arch::Opening<T>(network.getNode(2), std::vector<T>({-1.0, 0.0}), 1e-4));
    Openings.try_emplace(3, arch::Opening<T>(network.getNode(3), std::vector<T>({0.0, -1.0}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube0", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, 0.0, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -1.5708, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, -3.14159, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 1.5708, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        EXPECT_EQ(s0.readGeometry().getStatistics().getNvoxel(m), resolution);
    }
}

TEST(Cube, deg10) {

    std::string stlFile = "../examples/STL/cube10.stl";
    int resolution = 50;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.92404e-5, -8.6824e-6);
    network.addNode(8.6824e-6, -4.92404e-5);
    network.addNode(4.92404e-5, 8.6824e-6);
    network.addNode(-8.6824e-6, 4.92404e-5);

    // module
    network.addModule(std::vector<T>({-5.79228e-5, -5.79228e-5}), std::vector<T>({1.15846e-4, 1.15846e-4}), std::vector<int>({0, 1, 2, 3}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.984807753, 0.173648178}), 1e-4));
    Openings.try_emplace(1, arch::Opening<T>(network.getNode(1), std::vector<T>({-0.173648178, 0.984807753}), 1e-4));
    Openings.try_emplace(2, arch::Opening<T>(network.getNode(2), std::vector<T>({-0.984807753, -0.173648178}), 1e-4));
    Openings.try_emplace(3, arch::Opening<T>(network.getNode(3), std::vector<T>({0.173648178, -0.984807753}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube10", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.174533, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -1.74533, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, 2.96706, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 1.39626, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        unsigned int voxels = s0.readGeometry().getStatistics().getNvoxel(m);
        EXPECT_GE(voxels, resolution);
        EXPECT_LE(voxels, 2*resolution);
    }
}

TEST(Cube, deg20) {

    std::string stlFile = "../examples/STL/cube20.stl";
    int resolution = 50;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.698460e-05, -1.71010e-05);
    network.addNode(1.71010e-05, -4.698460e-05);
    network.addNode(4.698460e-05, 1.71010e-05);
    network.addNode(-1.71010e-05, 4.698460e-05);

    // module
    network.addModule(std::vector<T>({-6.40856e-5, -6.40856e-5}), std::vector<T>({1.28171e-4, 1.28171e-4}), std::vector<int>({0, 1, 2, 3}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.9396929621, 0.342020143}), 1e-4));
    Openings.try_emplace(1, arch::Opening<T>(network.getNode(1), std::vector<T>({-0.342020143, 0.9396929621}), 1e-4));
    Openings.try_emplace(2, arch::Opening<T>(network.getNode(2), std::vector<T>({-0.9396929621, -0.342020143}), 1e-4));
    Openings.try_emplace(3, arch::Opening<T>(network.getNode(3), std::vector<T>({0.342020143, -0.9396929621}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube20", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.349066, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -1.91986, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, 2.79253, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 1.22173, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        unsigned int voxels = s0.readGeometry().getStatistics().getNvoxel(m);
        EXPECT_GE(voxels, resolution);
        EXPECT_LE(voxels, 2*resolution);
    }
}

TEST(Cube, deg30) {

    std::string stlFile = "../examples/STL/cube30.stl";
    int resolution = 50;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.330130e-05, -2.5e-05);
    network.addNode(2.5e-05, -4.330130e-05);
    network.addNode(4.330130e-05, 2.5e-05);
    network.addNode(-2.5e-05, 4.330130e-05);

    // module
    network.addModule(std::vector<T>({-6.83013e-5, -6.83013e-5}), std::vector<T>({1.36603e-4, 1.36603e-4}), std::vector<int>({0, 1, 2, 3}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.866025404, 0.5}), 1e-4));
    Openings.try_emplace(1, arch::Opening<T>(network.getNode(1), std::vector<T>({-0.5, 0.866025404}), 1e-4));
    Openings.try_emplace(2, arch::Opening<T>(network.getNode(2), std::vector<T>({-0.866025404, -0.5}), 1e-4));
    Openings.try_emplace(3, arch::Opening<T>(network.getNode(3), std::vector<T>({0.5, -0.866025404}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube30", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.523599, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -2.0944, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, 2.61799, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 1.0472, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        unsigned int voxels = s0.readGeometry().getStatistics().getNvoxel(m);
        EXPECT_GE(voxels, resolution);
        EXPECT_LE(voxels, 2*resolution);
    }
}

TEST(Cube, deg45) {

    std::string stlFile = "../examples/STL/cube45.stl";
    int resolution = 50;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-3.535535e-05, -3.535535e-05);
    network.addNode(3.535535e-05, -3.535535e-05);
    network.addNode(3.535535e-05, 3.535535e-05);
    network.addNode(-3.535535e-05, 3.535535e-05);

    // module
    network.addModule(std::vector<T>({-7.07107e-5, -7.07107e-5}), std::vector<T>({1.41421e-4, 1.41421e-4}), std::vector<int>({0, 1, 2, 3}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.707106781, 0.707106781}), 1e-4));
    Openings.try_emplace(1, arch::Opening<T>(network.getNode(1), std::vector<T>({-0.707106781, 0.707106781}), 1e-4));
    Openings.try_emplace(2, arch::Opening<T>(network.getNode(2), std::vector<T>({-0.707106781, -0.707106781}), 1e-4));
    Openings.try_emplace(3, arch::Opening<T>(network.getNode(3), std::vector<T>({0.707106781, -0.707106781}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube45", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.785398, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -2.35619, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, 2.35619, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 0.785398, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        unsigned int voxels = s0.readGeometry().getStatistics().getNvoxel(m);
        EXPECT_GE(voxels, resolution);
        EXPECT_LE(voxels, 2*resolution);
    }
}

TEST(Cross, deg10) {

    std::string stlFile = "../examples/STL/cross10.stl";
    int resolution = 20;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(3.79807e-6, 2.06588e-4);

    // module
    network.addModule(std::vector<T>({-4.88436e-06, -4.88436e-06}), std::vector<T>({5.09769e-4, 5.09769e-4}), std::vector<int>({0}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.984807753, 0.173648178}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube0", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.174533, 1e-5);
    EXPECT_GE(s0.readGeometry().getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(s0.readGeometry().getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Cross, deg20) {

    std::string stlFile = "../examples/STL/cross20.stl";
    int resolution = 20;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(1.507681e-5, 1.64495e-4);

    // module
    network.addModule(std::vector<T>({-2.02418e-06, -2.02418e-06}), std::vector<T>({5.04049e-4, 5.04049e-4}), std::vector<int>({0}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.93969261, 0.342020143}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube0", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.349066, 1e-5);
    EXPECT_GE(s0.readGeometry().getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(s0.readGeometry().getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Cross, deg30) {

    std::string stlFile = "../examples/STL/cross30.stl";
    int resolution = 20;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(3.349362e-5, 1.25e-4);

    // module
    network.addModule(std::vector<T>({8.49364e-06, 8.49364e-06}), std::vector<T>({4.83014e-4, 4.83014e-4}), std::vector<int>({0}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.866025404, 0.5}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube0", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.523599, 1e-5);
    EXPECT_GE(s0.readGeometry().getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(s0.readGeometry().getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Cross, deg45) {

    std::string stlFile = "../examples/STL/cross45.stl";
    int resolution = 20;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(7.3223e-5, 7.3223e-5);

    // module
    network.addModule(std::vector<T>({3.78679e-05, 3.78679e-05}), std::vector<T>({4.24264e-4, 4.24264e-4}), std::vector<int>({0}));

    // openings
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(0, arch::Opening<T>(network.getNode(0), std::vector<T>({0.707106781, 0.707106781}), 1e-4));

    // simulator
    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(1e-6);
    sim::lbmSimulator<T> s0 (0, "cube0", stlFile, network.getModule(0), Openings, &resistanceModel, 0.0, 0.0, 0.0, resolution, 0.0);
    s0.readGeometryStl(dx, false);
    s0.readOpenings(dx);

    // Writes geometry to file system
    // olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // olb::SuperLatticeGeometry2D<T> writeGeometry (s0.readGeometry());
    // vtmWriter.write(writeGeometry);

    EXPECT_NEAR(Openings.at(0).radial, -0.785398, 1e-5);
    EXPECT_GE(s0.readGeometry().getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(s0.readGeometry().getStatistics().getNvoxel(3), 2*resolution);
}