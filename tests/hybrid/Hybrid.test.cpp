#include "../src/baseSimulator.h"
#ifdef USE_ESSLBM
    #include <mpi.h>
#endif
#include "gtest/gtest.h"
#include <iostream>

using T = double;

TEST(Opening, Cube0) {

    std::string stlFile = "../examples/STL/cube0.stl";
    int resolution = 100;
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

    EXPECT_NEAR(Openings.at(0).radial, 0.0, 1e-5);
    EXPECT_NEAR(Openings.at(1).radial, -1.5708, 1e-5);
    EXPECT_NEAR(Openings.at(2).radial, -3.14159, 1e-5);
    EXPECT_NEAR(Openings.at(3).radial, 1.5708, 1e-5);

    for (unsigned char m : {3, 4, 5, 6}) {
        EXPECT_EQ(s0.readGeometry().getStatistics().getNvoxel(m), resolution);
    }
}

TEST(Opening, Cube10) {

    std::string stlFile = "../examples/STL/cube10.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.92404e-5, -8.6824e-6);
    network.addNode(8.6824e-6, -4.92404e-5);
    network.addNode(4.92404e-5, 8.6824e-6);
    network.addNode(-8.6824e-6, 4.92404e-5);

    // module
    network.addModule(std::vector<T>({-5e-5, -5e-5}), std::vector<T>({1e-4, 1e-4}), std::vector<int>({0, 1, 2, 3}));

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

TEST(Opening, Cube20) {

    std::string stlFile = "../examples/STL/cube20.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.698460e-05, -1.71010e-05);
    network.addNode(1.71010e-05, -4.698460e-05);
    network.addNode(4.698460e-05, 1.71010e-05);
    network.addNode(-1.71010e-05, 4.698460e-05);

    // module
    network.addModule(std::vector<T>({-5e-5, -5e-5}), std::vector<T>({1e-4, 1e-4}), std::vector<int>({0, 1, 2, 3}));

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

TEST(Opening, Cube30) {

    std::string stlFile = "../examples/STL/cube30.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-4.330130e-05, -2.5e-05);
    network.addNode(2.5e-05, -4.330130e-05);
    network.addNode(4.330130e-05, 2.5e-05);
    network.addNode(-2.5e-05, 4.330130e-05);

    // module
    network.addModule(std::vector<T>({-5e-5, -5e-5}), std::vector<T>({1e-4, 1e-4}), std::vector<int>({0, 1, 2, 3}));

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

TEST(Opening, Cube45) {

    std::string stlFile = "../examples/STL/cube45.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;

    // define network
    arch::Network<T> network;

    // nodes
    network.addNode(-3.535535e-05, -3.535535e-05);
    network.addNode(3.535535e-05, -3.535535e-05);
    network.addNode(3.535535e-05, 3.535535e-05);
    network.addNode(-3.535535e-05, 3.535535e-05);

    // module
    network.addModule(std::vector<T>({-5e-5, -5e-5}), std::vector<T>({1e-4, 1e-4}), std::vector<int>({0, 1, 2, 3}));

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

/*
TEST(Opening, Cube0) {

    std::string stlFile = "../examples/STL/cube0.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx-0.5*dx, min[1]-margin*dx-0.5*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx+dx, max[1]-min[1]+2*margin*dx+dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    // Nodes on the STL domain edges
    auto node0 = std::make_shared<arch::Node<T>>(0, -5e-5, 0.0);
    auto node1 = std::make_shared<arch::Node<T>>(1, 0.0, -5e-5);
    auto node2 = std::make_shared<arch::Node<T>>(2, 5e-5, 0.0);
    auto node3 = std::make_shared<arch::Node<T>>(3, 0.0, 5e-5);

    std::vector<T> normal0({1.0, 0.0});
    std::vector<T> normal1({0.0, 1.0});
    std::vector<T> normal2({-1.0, 0.0});
    std::vector<T> normal3({0.0, -1.0});

    auto opening0 = arch::Opening<T>(node0, normal0, 1e-4);
    auto opening1 = arch::Opening<T>(node1, normal1, 1e-4);
    auto opening2 = arch::Opening<T>(node2, normal2, 1e-4);
    auto opening3 = arch::Opening<T>(node3, normal3, 1e-4);

    T extendMargin = 4;

    T x_extend = extendMargin*dx;
    T y_extend = 1e-4;  // opening.width

    T x_origin0 = opening0.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin0 = opening0.node->getPosition()[1] - 0.5*opening0.width;
    T x_origin1 = opening1.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin1 = opening1.node->getPosition()[1] - 0.5*opening1.width;
    T x_origin2 = opening2.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin2 = opening2.node->getPosition()[1] - 0.5*opening2.width;
    T x_origin3 = opening3.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin3 = opening3.node->getPosition()[1] - 0.5*opening3.width;

    olb::Vector<T,2> originO0 (x_origin0, y_origin0);
    olb::Vector<T,2> originO1 (x_origin1, y_origin1);
    olb::Vector<T,2> originO2 (x_origin2, y_origin2);
    olb::Vector<T,2> originO3 (x_origin3, y_origin3);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd0(extendO, originO0);
    olb::IndicatorCuboid2D<T> openingInd1(extendO, originO1, -1.5708);
    olb::IndicatorCuboid2D<T> openingInd2(extendO, originO2, 3.14159);
    olb::IndicatorCuboid2D<T> openingInd3(extendO, originO3, 1.5708);
    
    geometry->rename(2, 3, openingInd0);
    geometry->rename(2, 4, openingInd1);
    geometry->rename(2, 5, openingInd2);
    geometry->rename(2, 6, openingInd3);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_EQ(geometry->getStatistics().getNvoxel(2), int(4));
    EXPECT_EQ(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_EQ(geometry->getStatistics().getNvoxel(4), resolution);
    EXPECT_EQ(geometry->getStatistics().getNvoxel(5), resolution);
    EXPECT_EQ(geometry->getStatistics().getNvoxel(6), resolution);
}

TEST(Opening, Cube10) {

    std::string stlFile = "../examples/STL/cube10.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    // Nodes on the STL domain edges
    auto node0 = std::make_shared<arch::Node<T>>(0, -4.92404e-5, -8.6824e-6);
    auto node1 = std::make_shared<arch::Node<T>>(1, 8.6824e-6, -4.92404e-5);
    auto node2 = std::make_shared<arch::Node<T>>(2, 4.92404e-5, 8.6824e-6);
    auto node3 = std::make_shared<arch::Node<T>>(3, -8.6824e-6, 4.92404e-5);

    std::vector<T> normal0({0.984807753, 0.173648178});
    std::vector<T> normal1({-0.173648178, 0.984807753});
    std::vector<T> normal2({-0.984807753, -0.173648178});
    std::vector<T> normal3({0.173648178, -0.984807753});

    auto opening0 = arch::Opening<T>(node0, normal0, 1e-4);
    auto opening1 = arch::Opening<T>(node1, normal1, 1e-4);
    auto opening2 = arch::Opening<T>(node2, normal2, 1e-4);
    auto opening3 = arch::Opening<T>(node3, normal3, 1e-4);

    T extendMargin = 4;

    T x_extend = extendMargin*dx;
    T y_extend = 1e-4;  // opening.width

    T x_origin0 = opening0.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin0 = opening0.node->getPosition()[1] - 0.5*opening0.width;
    T x_origin1 = opening1.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin1 = opening1.node->getPosition()[1] - 0.5*opening1.width;
    T x_origin2 = opening2.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin2 = opening2.node->getPosition()[1] - 0.5*opening2.width;
    T x_origin3 = opening3.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin3 = opening3.node->getPosition()[1] - 0.5*opening3.width;

    olb::Vector<T,2> originO0 (x_origin0, y_origin0);
    olb::Vector<T,2> originO1 (x_origin1, y_origin1);
    olb::Vector<T,2> originO2 (x_origin2, y_origin2);
    olb::Vector<T,2> originO3 (x_origin3, y_origin3);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd0(extendO, originO0, -0.174533);
    olb::IndicatorCuboid2D<T> openingInd1(extendO, originO1, -1.74533);
    olb::IndicatorCuboid2D<T> openingInd2(extendO, originO2, 2.96706);
    olb::IndicatorCuboid2D<T> openingInd3(extendO, originO3, 1.39626);
    
    geometry->rename(2, 3, openingInd0);
    geometry->rename(2, 4, openingInd1);
    geometry->rename(2, 5, openingInd2);
    geometry->rename(2, 6, openingInd3);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_EQ(geometry->getStatistics().getNvoxel(2), int(4));
    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(4), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(4), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(5), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(5), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(6), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(6), 2*resolution);
}

TEST(Opening, Cube20) {

    std::string stlFile = "../examples/STL/cube20.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    // Nodes on the STL domain edges
    auto node0 = std::make_shared<arch::Node<T>>(0, -4.698460e-05, -1.71010e-05);
    auto node1 = std::make_shared<arch::Node<T>>(1, 1.71010e-05, -4.698460e-05);
    auto node2 = std::make_shared<arch::Node<T>>(2, 4.698460e-05, 1.71010e-05);
    auto node3 = std::make_shared<arch::Node<T>>(3, -1.71010e-05, 4.698460e-05);

    std::vector<T> normal0({0.9396929621, 0.342020143});
    std::vector<T> normal1({-0.342020143, 0.9396929621});
    std::vector<T> normal2({-0.9396929621, -0.342020143});
    std::vector<T> normal3({0.342020143, -0.9396929621});

    auto opening0 = arch::Opening<T>(node0, normal0, 1e-4);
    auto opening1 = arch::Opening<T>(node1, normal1, 1e-4);
    auto opening2 = arch::Opening<T>(node2, normal2, 1e-4);
    auto opening3 = arch::Opening<T>(node3, normal3, 1e-4);

    T extendMargin = 4;

    T x_extend = extendMargin*dx;
    T y_extend = 1e-4;  // opening.width

    T x_origin0 = opening0.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin0 = opening0.node->getPosition()[1] - 0.5*opening0.width;
    T x_origin1 = opening1.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin1 = opening1.node->getPosition()[1] - 0.5*opening1.width;
    T x_origin2 = opening2.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin2 = opening2.node->getPosition()[1] - 0.5*opening2.width;
    T x_origin3 = opening3.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin3 = opening3.node->getPosition()[1] - 0.5*opening3.width;

    olb::Vector<T,2> originO0 (x_origin0, y_origin0);
    olb::Vector<T,2> originO1 (x_origin1, y_origin1);
    olb::Vector<T,2> originO2 (x_origin2, y_origin2);
    olb::Vector<T,2> originO3 (x_origin3, y_origin3);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd0(extendO, originO0, -0.349066);
    olb::IndicatorCuboid2D<T> openingInd1(extendO, originO1, -1.91986);
    olb::IndicatorCuboid2D<T> openingInd2(extendO, originO2, 2.79253);
    olb::IndicatorCuboid2D<T> openingInd3(extendO, originO3, 1.22173);
    
    geometry->rename(2, 3, openingInd0);
    geometry->rename(2, 4, openingInd1);
    geometry->rename(2, 5, openingInd2);
    geometry->rename(2, 6, openingInd3);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(4), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(4), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(5), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(5), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(6), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(6), 2*resolution);
}

TEST(Opening, Cube30) {

    std::string stlFile = "../examples/STL/cube30.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    // Nodes on the STL domain edges
    auto node0 = std::make_shared<arch::Node<T>>(0, -4.330130e-05, -2.5e-05);
    auto node1 = std::make_shared<arch::Node<T>>(1, 2.5e-05, -4.330130e-05);
    auto node2 = std::make_shared<arch::Node<T>>(2, 4.330130e-05, 2.5e-05);
    auto node3 = std::make_shared<arch::Node<T>>(3, -2.5e-05, 4.330130e-05);

    std::vector<T> normal0({0.866025404, 0.5});
    std::vector<T> normal1({-0.5, 0.866025404});
    std::vector<T> normal2({-0.866025404, -0.5});
    std::vector<T> normal3({0.5, -0.866025404});

    auto opening0 = arch::Opening<T>(node0, normal0, 1e-4);
    auto opening1 = arch::Opening<T>(node1, normal1, 1e-4);
    auto opening2 = arch::Opening<T>(node2, normal2, 1e-4);
    auto opening3 = arch::Opening<T>(node3, normal3, 1e-4);

    T extendMargin = 4;

    T x_extend = extendMargin*dx;
    T y_extend = 1e-4;  // opening.width

    T x_origin0 = opening0.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin0 = opening0.node->getPosition()[1] - 0.5*opening0.width;
    T x_origin1 = opening1.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin1 = opening1.node->getPosition()[1] - 0.5*opening1.width;
    T x_origin2 = opening2.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin2 = opening2.node->getPosition()[1] - 0.5*opening2.width;
    T x_origin3 = opening3.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin3 = opening3.node->getPosition()[1] - 0.5*opening3.width;

    olb::Vector<T,2> originO0 (x_origin0, y_origin0);
    olb::Vector<T,2> originO1 (x_origin1, y_origin1);
    olb::Vector<T,2> originO2 (x_origin2, y_origin2);
    olb::Vector<T,2> originO3 (x_origin3, y_origin3);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd0(extendO, originO0, -0.523599);
    olb::IndicatorCuboid2D<T> openingInd1(extendO, originO1, -2.0944);
    olb::IndicatorCuboid2D<T> openingInd2(extendO, originO2, 2.61799);
    olb::IndicatorCuboid2D<T> openingInd3(extendO, originO3, 1.0472);
    
    geometry->rename(2, 3, openingInd0);
    geometry->rename(2, 4, openingInd1);
    geometry->rename(2, 5, openingInd2);
    geometry->rename(2, 6, openingInd3);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(4), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(4), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(5), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(5), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(6), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(6), 2*resolution);
}

TEST(Opening, Cube45) {

    std::string stlFile = "../examples/STL/cube45.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    // Nodes on the STL domain edges
    auto node0 = std::make_shared<arch::Node<T>>(0, -3.535535e-05, -3.535535e-05);
    auto node1 = std::make_shared<arch::Node<T>>(1, 3.535535e-05, -3.535535e-05);
    auto node2 = std::make_shared<arch::Node<T>>(2, 3.535535e-05, 3.535535e-05);
    auto node3 = std::make_shared<arch::Node<T>>(3, -3.535535e-05, 3.535535e-05);

    std::vector<T> normal0({0.707106781, 0.707106781});
    std::vector<T> normal1({-0.707106781, 0.707106781});
    std::vector<T> normal2({-0.707106781, -0.707106781});
    std::vector<T> normal3({0.707106781, -0.707106781});

    auto opening0 = arch::Opening<T>(node0, normal0, 1e-4);
    auto opening1 = arch::Opening<T>(node1, normal1, 1e-4);
    auto opening2 = arch::Opening<T>(node2, normal2, 1e-4);
    auto opening3 = arch::Opening<T>(node3, normal3, 1e-4);

    T extendMargin = 4;

    T x_extend = extendMargin*dx;
    T y_extend = 1e-4;  // opening.width

    T x_origin0 = opening0.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin0 = opening0.node->getPosition()[1] - 0.5*opening0.width;
    T x_origin1 = opening1.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin1 = opening1.node->getPosition()[1] - 0.5*opening1.width;
    T x_origin2 = opening2.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin2 = opening2.node->getPosition()[1] - 0.5*opening2.width;
    T x_origin3 = opening3.node->getPosition()[0] - 0.5*extendMargin*dx;
    T y_origin3 = opening3.node->getPosition()[1] - 0.5*opening3.width;

    olb::Vector<T,2> originO0 (x_origin0, y_origin0);
    olb::Vector<T,2> originO1 (x_origin1, y_origin1);
    olb::Vector<T,2> originO2 (x_origin2, y_origin2);
    olb::Vector<T,2> originO3 (x_origin3, y_origin3);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd0(extendO, originO0, -0.785398);
    olb::IndicatorCuboid2D<T> openingInd1(extendO, originO1, -2.35619);
    olb::IndicatorCuboid2D<T> openingInd2(extendO, originO2, 2.35619);
    olb::IndicatorCuboid2D<T> openingInd3(extendO, originO3, 0.785398);
    
    geometry->rename(2, 3, openingInd0);
    geometry->rename(2, 4, openingInd1);
    geometry->rename(2, 5, openingInd2);
    geometry->rename(2, 6, openingInd3);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(4), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(4), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(5), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(5), 2*resolution);
    EXPECT_GE(geometry->getStatistics().getNvoxel(6), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(6), 2*resolution);
}
*/

/** TODO:
 * ->   extendMargin is probably a function of how diagonal the normal of the face is (45 deg most extreme?)
 * ->   cube0 has additional 0.5*dx margin, to have the edge between cells. Todo: add an if function for whether
 *      the bounding box has one or multiple boundaries exactly on the lattice cell. If so, add artificial margin.
 */

/*
TEST(Opening, Deg10) {

    std::string stlFile = "../examples/STL/cross10.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    std::cout << "Bounding box is \n[" << min[0] << " " << min[1] << " " << min[2] << "]"
    <<  "\n[" << max[0] << " " << max[1] << " " << max[2] << "] " << std::endl;
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    auto node = std::make_shared<arch::Node<T>>(0, 3.79807e-6, 2.06588e-4);
    std::vector<T> normal({0.98, 0.17});
    auto opening = arch::Opening<T>(node, normal, 1e-4);

    T x_origin = opening.node->getPosition()[0] - 2*dx;
    T y_origin = opening.node->getPosition()[1] - 0.5*opening.width;

    T x_extend = 2*dx;
    T y_extend = opening.width;

    olb::Vector<T,2> originO (x_origin, y_origin);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd(extendO, originO, -0.174533);
    
    geometry->rename(2, 3, openingInd);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Opening, Deg20) {

    std::string stlFile = "../examples/STL/cross20.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    std::cout << "Bounding box is \n[" << min[0] << " " << min[1] << " " << min[2] << "]"
    <<  "\n[" << max[0] << " " << max[1] << " " << max[2] << "] " << std::endl;
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    auto node = std::make_shared<arch::Node<T>>(0, 1.507681e-5, 1.64495e-4);
    std::vector<T> normal({0.94, 0.342});
    auto opening = arch::Opening<T>(node, normal, 1e-4);

    T x_origin = opening.node->getPosition()[0] - 2*dx;
    T y_origin = opening.node->getPosition()[1] - 0.5*opening.width;

    T x_extend = 2*dx;
    T y_extend = opening.width;

    olb::Vector<T,2> originO (x_origin, y_origin);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd(extendO, originO, -0.349066);
    
    geometry->rename(2, 3, openingInd);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Opening, Deg30) {

    std::string stlFile = "../examples/STL/cross30.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    std::cout << "Bounding box is \n[" << min[0] << " " << min[1] << " " << min[2] << "]"
    <<  "\n[" << max[0] << " " << max[1] << " " << max[2] << "] " << std::endl;
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    auto node = std::make_shared<arch::Node<T>>(0, 3.349362e-5, 1.25e-4);
    std::vector<T> normal({0.866, 0.5});
    auto opening = arch::Opening<T>(node, normal, 1e-4);

    T x_origin = opening.node->getPosition()[0] - 2*dx;
    T y_origin = opening.node->getPosition()[1] - 0.5*opening.width;

    T x_extend = 2*dx;
    T y_extend = opening.width;

    olb::Vector<T,2> originO (x_origin, y_origin);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd(extendO, originO, -0.523599);
    
    geometry->rename(2, 3, openingInd);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
}

TEST(Opening, Deg45) {

    std::string stlFile = "../examples/STL/cross45.stl";
    int resolution = 100;
    T dx = 1e-4/resolution;
    int margin = 1;

    auto stlReader = std::make_shared<olb::STLreader<T>>(stlFile, dx);
    auto min = stlReader->getMesh().getMin();
    auto max = stlReader->getMesh().getMax();
    std::cout << "Bounding box is \n[" << min[0] << " " << min[1] << " " << min[2] << "]"
    <<  "\n[" << max[0] << " " << max[1] << " " << max[2] << "] " << std::endl;
    auto stl2Dindicator = std::make_shared<olb::IndicatorF2DfromIndicatorF3D<T>>(*stlReader);

    olb::Vector<T,2> origin(min[0]-margin*dx, min[1]-margin*dx);
    olb::Vector<T,2> extend(max[0]-min[0]+2*margin*dx, max[1]-min[1]+2*margin*dx);
    olb::IndicatorCuboid2D<T> cuboid(extend, origin);
    auto cuboidGeometry = std::make_shared<olb::CuboidGeometry2D<T>> (cuboid, dx, 1);
    auto loadBalancer = std::make_shared<olb::HeuristicLoadBalancer<T>> (*cuboidGeometry);
    auto geometry = std::make_shared<olb::SuperGeometry<T,2>> (*cuboidGeometry, *loadBalancer);

    geometry->rename(0, 2);
    geometry->rename(2, 1, *stl2Dindicator);

    geometry->clean();

    auto node = std::make_shared<arch::Node<T>>(0, 7.3223e-5, 7.3223e-5);
    std::vector<T> normal({0.707106781, 0.707106781});
    auto opening = arch::Opening<T>(node, normal, 1e-4);

    T x_origin = opening.node->getPosition()[0] - 3*dx;
    T y_origin = opening.node->getPosition()[1] - 0.5*opening.width;

    T x_extend = 3*dx;
    T y_extend = opening.width;

    olb::Vector<T,2> originO (x_origin, y_origin);
    olb::Vector<T,2> extendO (x_extend, y_extend);
    olb::IndicatorCuboid2D<T> openingInd(extendO, originO, -0.785398);
    
    geometry->rename(2, 3, openingInd);

    olb::SuperVTMwriter2D<T> vtmWriter( "testGeometry" );
    // Writes geometry to file system
    olb::SuperLatticeGeometry2D<T> writeGeometry (*geometry);
    vtmWriter.write(writeGeometry);

    EXPECT_GE(geometry->getStatistics().getNvoxel(3), resolution);
    EXPECT_LE(geometry->getStatistics().getNvoxel(3), 2*resolution);
}

/*
TEST(Hybrid, Case1a) {
    // define simulation
    sim::Simulation<T> testSimulation;
    testSimulation.setType(sim::Type::Hybrid);
    testSimulation.setPlatform(sim::Platform::Continuous);

    // define network
    arch::Network<T> network;
    
    // nodes
    auto node0 = network.addNode(0.0, 0.0, true);
    auto node1 = network.addNode(1e-3, 2e-3, false);
    auto node2 = network.addNode(1e-3, 1e-3, false);
    auto node3 = network.addNode(1e-3, 0.0, false);
    auto node4 = network.addNode(2e-3, 2e-3, false);
    auto node5 = network.addNode(1.75e-3, 1e-3, false);
    auto node6 = network.addNode(2e-3, 0.0, false);
    auto node7 = network.addNode(2e-3, 1.25e-3, false);
    auto node8 = network.addNode(2e-3, 0.75e-3, false);
    auto node9 = network.addNode(2.25e-3, 1e-3, false);
    auto node10 = network.addNode(3e-3, 1e-3, true);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c0 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c1 = network.addChannel(node0->getId(), node2->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network.addChannel(node0->getId(), node3->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node4->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node6->getId(), node8->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    network.addChannel(node9->getId(), node10->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // module
    std::vector<T> position = { 1.75e-3, 0.75e-3 };
    std::vector<T> size = { 5e-4, 5e-4 };
    std::unordered_map<int, std::shared_ptr<arch::Node<T>>> nodes;

    nodes.try_emplace(5, network.getNode(5));
    nodes.try_emplace(7, network.getNode(7));
    nodes.try_emplace(8, network.getNode(8));
    nodes.try_emplace(9, network.getNode(9));

    auto m0 = network.addModule(position, size, nodes);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 1e3, 1.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    sim::ResistanceModelPoiseuille<T> resistanceModel = sim::ResistanceModelPoiseuille<T>(testSimulation.getContinuousPhase()->getViscosity());
    testSimulation.setResistanceModel(&resistanceModel);

    // simulator
    std::string name = "Paper1a-cross-0";
    std::string stlFile = "../examples/STL/cross.stl";
    T charPhysLength = 1e-4;
    T charPhysVelocity = 1e-1;
    T alpha = 0.1;
    T resolution = 20;
    T epsilon = 1e-1;
    T tau = 0.55;
    std::unordered_map<int, arch::Opening<T>> Openings;
    Openings.try_emplace(5, arch::Opening<T>(network.getNode(5), std::vector<T>({1.0, 0.0}), 1e-4));
    Openings.try_emplace(7, arch::Opening<T>(network.getNode(7), std::vector<T>({0.0, -1.0}), 1e-4));
    Openings.try_emplace(8, arch::Opening<T>(network.getNode(8), std::vector<T>({0.0, 1.0}), 1e-4));
    Openings.try_emplace(9, arch::Opening<T>(network.getNode(9), std::vector<T>({-1.0, 0.0}), 1e-4));

    testSimulation.addLbmSimulator(name, stlFile, network.getModule(m0->getId()), Openings, charPhysLength, charPhysVelocity, alpha, resolution, epsilon, tau);
    network.sortGroups();

    // pressure pump
    auto pressure = 1e3;
    network.setPressurePump(c0->getId(), pressure);
    network.setPressurePump(c1->getId(), pressure);
    network.setPressurePump(c2->getId(), pressure);

    network.isNetworkValid();
    
    // Simulate
    testSimulation.setNetwork(&network);
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);
}

#ifdef USE_ESSLBM
TEST(Hybrid, esstest) {

    MPI_Init(NULL, NULL);
    std::string file = "../examples/Hybrid/Network1a_ESS.JSON";

    // Load and set the network from a JSON file
    arch::Network<float> network = porting::networkFromJSON<float>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<float> testSimulation = porting::simulationFromJSON<float>(file, &network);

    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);

    MPI_Finalize();
}
#endif
*/
/*
TEST(Hybrid, Case1aJSON) {
    
    std::string file = "../examples/Hybrid/Network1a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);

    network.isNetworkValid();
    
    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 791.962, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 859.216, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 753.628, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 422.270, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.31153e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.1732e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 4.69188e-9, 1e-14);
}

TEST(Hybrid, testCase2a) {
    
std::string file = "../examples/Hybrid/Network2a.JSON";

    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 577.014, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 729.934, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 491.525, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 469.468, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 641.131, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 607.210, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 427.408, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 270.394, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 3.00073e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), -7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 2.2957e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 7.12409e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 3.00438e-9, 1e-14);

}

TEST(Hybrid, testCase3a) {
    
        std::string file = "../examples/Hybrid/Network3a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 687.204, 1e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 801.008, 1e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 625.223, 1e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 601.422, 1e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 428.435, 1e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 428.007, 1e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 331.775, 1e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 733.690, 1e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 703.349, 1e-2);
    EXPECT_NEAR(network.getNodes().at(11)->getPressure(), 578.736, 1e-2);
    EXPECT_NEAR(network.getNodes().at(12)->getPressure(), 525.094, 1e-2);
    EXPECT_NEAR(network.getNodes().at(13)->getPressure(), 529.993, 1e-2);
    EXPECT_NEAR(network.getNodes().at(14)->getPressure(), 326.021, 1e-2);
    EXPECT_NEAR(network.getNodes().at(15)->getPressure(), 198.163, 1e-2);
    EXPECT_NEAR(network.getNodes().at(16)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.21102e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), -5.16509e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 5.16509e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 1.69878e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 5.16517e-10, 1e-14);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.07399e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.13318e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.07399e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(9)->getFlowRate(), 1.13318e-9, 1e-14);
    EXPECT_NEAR(network.getChannels().at(10)->getFlowRate(), 2.20181e-9, 1e-14);

}

TEST(Hybrid, testCase4a) {
    
        std::string file = "../examples/Hybrid/Network4a.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(file);

    // Load and set the simulation from a JSON file
    sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
    
    network.isNetworkValid();

    // Simulate
    testSimulation.simulate();

    EXPECT_NEAR(network.getNodes().at(0)->getPressure(), 0, 1e-2);
    EXPECT_NEAR(network.getNodes().at(1)->getPressure(), 1000, 1e-2);
    EXPECT_NEAR(network.getNodes().at(2)->getPressure(), 781.127, 2e-2);
    EXPECT_NEAR(network.getNodes().at(3)->getPressure(), 616.803, 2e-2);
    EXPECT_NEAR(network.getNodes().at(4)->getPressure(), 604.131, 2e-2);
    EXPECT_NEAR(network.getNodes().at(5)->getPressure(), 535.190, 2e-2);
    EXPECT_NEAR(network.getNodes().at(6)->getPressure(), 447.980, 2e-2);
    EXPECT_NEAR(network.getNodes().at(7)->getPressure(), 371.030, 2e-2);
    EXPECT_NEAR(network.getNodes().at(8)->getPressure(), 698.415, 2e-2);
    EXPECT_NEAR(network.getNodes().at(9)->getPressure(), 695.906, 2e-2);
    EXPECT_NEAR(network.getNodes().at(10)->getPressure(), 552.139, 2e-2);
    EXPECT_NEAR(network.getNodes().at(11)->getPressure(), 540.383, 2e-2);
    EXPECT_NEAR(network.getNodes().at(12)->getPressure(), 522.320, 2e-2);
    EXPECT_NEAR(network.getNodes().at(13)->getPressure(), 454.955, 2e-2);
    EXPECT_NEAR(network.getNodes().at(14)->getPressure(), 355.576, 2e-2);
    EXPECT_NEAR(network.getNodes().at(15)->getPressure(), 217.852, 2e-2);
    EXPECT_NEAR(network.getNodes().at(16)->getPressure(), 0, 1e-2);

    EXPECT_NEAR(network.getChannels().at(1)->getFlowRate(), 2.43192e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(2)->getFlowRate(), 9.06803e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(3)->getFlowRate(), 1.52958e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(4)->getFlowRate(), 9.06793e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(5)->getFlowRate(), 4.96979e-10, 1e-13);
    EXPECT_NEAR(network.getChannels().at(6)->getFlowRate(), 1.02671e-9, 1e-13);
    EXPECT_NEAR(network.getChannels().at(7)->getFlowRate(), 1.39887e-9, 1e-12);
    EXPECT_NEAR(network.getChannels().at(8)->getFlowRate(), 1.02671e-9, 1e-12);
    EXPECT_NEAR(network.getChannels().at(9)->getFlowRate(), 2.42036e-9, 1e-12);

}
*/