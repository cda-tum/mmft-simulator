#include "gtest/gtest.h"

#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/baseSimulator.h"
#include "test_helpers.h"

using T = double;

/*
Droplet Simulation Test based on
Gerold Fink et al. “Automatic Design of Droplet-Based Microfluidic Ring Networks”. In:
IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems 40.2 (2021),
pp. 339–349. DOI: 10.1109/TCAD.2020.2997000 (cit. on pp. 27, 54, 55, 56, 57).
*/

TEST(Droplet, RingNetworkE1) {
  constexpr auto cContinuousPhaseViscosity = 1e-3;
  sim::InstantaneousMixingModel<T> mixingModel;
  sim::ResistanceModel1D<T> resistanceModel(cContinuousPhaseViscosity);
  arch::Network<T> network;

  // simulator last since it has non-owning references to other objects
  sim::Simulation<T> sim;

  // droplet length
  auto lDroplet = 150e-6;

  // droplet distances
  [[maybe_unused]] double d_m1 = 881e-6;
  [[maybe_unused]] double d_m2 = 41230e-6;
  [[maybe_unused]] double d_m3 = 41867e-6;
  [[maybe_unused]] double d_m4 = 41956e-6;
  [[maybe_unused]] double d_m5 = 42047e-6;
  [[maybe_unused]] double d_m6 = 42127e-6;

  // channels
  auto cWidth = 100e-6;
  auto cHeight = 33e-6;
  auto bWidth = 200e-6;

  auto iLength = 2000e-6;

  // Node 1
  auto lIn1N1 = 469e-6;
  auto lIn2N1 = 531e-6;
  auto lByN1 = 1200e-6;
  auto lOut1N1 = 14531e-6;
  auto lOut2N1 = 24469e-6;
  // Node 2
  auto lIn1N2 = 743e-6;
  auto lIn2N2 = 757e-6;
  auto lByN2 = 1700e-6;
  auto lOut1N2 = 14257e-6;
  auto lOut2N2 = 24243e-6;
  // Node 3
  auto lIn1N3 = 1021e-6;
  auto lIn2N3 = 979e-6;
  auto lByN3 = 2200e-6;
  auto lOut1N3 = 13979e-6;
  auto lOut2N3 = 24021e-6;
  // Node 4
  auto lIn1N4 = 1300e-6;
  auto lIn2N4 = 1200e-6;
  auto lByN4 = 2700e-6;
  auto lOut1N4 = 13700e-6;
  auto lOut2N4 = 23800e-6;
  // Node 5
  auto lIn1N5 = 1580e-6;
  auto lIn2N5 = 1420e-6;
  auto lByN5 = 3200e-6;
  auto lOut1N5 = 13420e-6;
  auto lOut2N5 = 23580e-6;
  // Node 6
  auto lIn1N6 = 1862e-6;
  auto lIn2N6 = 1638e-6;
  auto lByN6 = 3700e-6;
  auto lOut1N6 = 13138e-6;
  auto lOut2N6 = 23362e-6;

  auto cLength = 500e-6;

  auto *groundNode = network.addNode(1e-10, 1e-10, true);
  auto *sinkNode = network.addNode(9e-8, 9e-8, true);
  auto *node0 = network.addNode(2e-10, 2e-10);
  auto *node1 = network.addNode(3e-10, 3e-10);
  auto *node2 = network.addNode(4e-10, 4e-10);
  auto *node3 = network.addNode(5e-10, 5e-10);
  auto *node4 = network.addNode(6e-10, 6e-10);
  auto *node21 = network.addNode(7e-10, 7e-10);
  auto *node22 = network.addNode(8e-10, 8e-10);
  auto *node23 = network.addNode(9e-10, 9e-10);
  auto *node24 = network.addNode(1e-9, 1e-9);
  auto *node31 = network.addNode(2e-9, 2e-9);
  auto *node32 = network.addNode(3e-9, 3e-9);
  auto *node33 = network.addNode(4e-9, 4e-9);
  auto *node34 = network.addNode(5e-9, 5e-9);
  auto *node41 = network.addNode(6e-9, 6e-9);
  auto *node42 = network.addNode(7e-9, 7e-9);
  auto *node43 = network.addNode(8e-9, 8e-9);
  auto *node44 = network.addNode(9e-9, 9e-9);
  auto *node51 = network.addNode(1e-8, 1e-8);
  auto *node52 = network.addNode(2e-8, 2e-8);
  auto *node53 = network.addNode(3e-8, 3e-8);
  auto *node54 = network.addNode(4e-8, 4e-8);
  auto *node61 = network.addNode(5e-8, 5e-8);
  auto *node62 = network.addNode(6e-8, 6e-8);
  auto *node63 = network.addNode(7e-8, 7e-8);
  auto *node64 = network.addNode(8e-8, 8e-8);

  auto *c1 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, iLength, arch::ChannelType::NORMAL);
  // Node 1
  [[maybe_unused]] auto *c2_n1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, lIn1N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n1 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, lIn2N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n1 = network.addChannel(node2->getId(), node4->getId(), cHeight, cWidth, lOut1N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n1 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, lOut2N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n1 = network.addChannel(node4->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n1 = network.addChannel(node2->getId(), node3->getId(), cHeight, bWidth, lByN1, arch::ChannelType::BYPASS);
  // Node 2
  [[maybe_unused]] auto *c2_n2 = network.addChannel(node21->getId(), node22->getId(), cHeight, cWidth, lIn1N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n2 = network.addChannel(node21->getId(), node23->getId(), cHeight, cWidth, lIn2N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n2 = network.addChannel(node22->getId(), node24->getId(), cHeight, cWidth, lOut1N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n2 = network.addChannel(node23->getId(), node24->getId(), cHeight, cWidth, lOut2N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n2 = network.addChannel(node24->getId(), node31->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n2 = network.addChannel(node22->getId(), node23->getId(), cHeight, bWidth, lByN2, arch::ChannelType::BYPASS);
  // Node 3
  [[maybe_unused]] auto *c2_n3 = network.addChannel(node31->getId(), node32->getId(), cHeight, cWidth, lIn1N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n3 = network.addChannel(node31->getId(), node33->getId(), cHeight, cWidth, lIn2N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n3 = network.addChannel(node32->getId(), node34->getId(), cHeight, cWidth, lOut1N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n3 = network.addChannel(node33->getId(), node34->getId(), cHeight, cWidth, lOut2N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n3 = network.addChannel(node34->getId(), node41->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n3 = network.addChannel(node32->getId(), node33->getId(), cHeight, bWidth, lByN3, arch::ChannelType::BYPASS);
  // Node 4
  [[maybe_unused]] auto *c2_n4 = network.addChannel(node41->getId(), node42->getId(), cHeight, cWidth, lIn1N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n4 = network.addChannel(node41->getId(), node43->getId(), cHeight, cWidth, lIn2N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n4 = network.addChannel(node42->getId(), node44->getId(), cHeight, cWidth, lOut1N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n4 = network.addChannel(node43->getId(), node44->getId(), cHeight, cWidth, lOut2N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n4 = network.addChannel(node44->getId(), node51->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n4 = network.addChannel(node42->getId(), node43->getId(), cHeight, bWidth, lByN4, arch::ChannelType::BYPASS);
  // Node 5
  [[maybe_unused]] auto *c2_n5 = network.addChannel(node51->getId(), node52->getId(), cHeight, cWidth, lIn1N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n5 = network.addChannel(node51->getId(), node53->getId(), cHeight, cWidth, lIn2N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n5 = network.addChannel(node52->getId(), node54->getId(), cHeight, cWidth, lOut1N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n5 = network.addChannel(node53->getId(), node54->getId(), cHeight, cWidth, lOut2N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n5 = network.addChannel(node54->getId(), node61->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n5 = network.addChannel(node52->getId(), node53->getId(), cHeight, bWidth, lByN5, arch::ChannelType::BYPASS);
  // Node 6
  [[maybe_unused]] auto *c2_n6 = network.addChannel(node61->getId(), node62->getId(), cHeight, cWidth, lIn1N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n6 = network.addChannel(node61->getId(), node63->getId(), cHeight, cWidth, lIn2N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n6 = network.addChannel(node62->getId(), node64->getId(), cHeight, cWidth, lOut1N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n6 = network.addChannel(node63->getId(), node64->getId(), cHeight, cWidth, lOut2N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n6 = network.addChannel(node64->getId(), sinkNode->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n6 = network.addChannel(node62->getId(), node63->getId(), cHeight, bWidth, lByN6, arch::ChannelType::BYPASS);

  // flowRate pump
  auto flowRate = 3e-11;
  [[maybe_unused]] auto *pump = network.addFlowRatePump(groundNode->getId(), node0->getId(), flowRate);

  //--- sink ---
  network.setSink(sinkNode->getId());
  //--- ground ---
  network.setGround(groundNode->getId());

  EXPECT_TRUE(network.isNetworkValid());
  network.sortGroups();
  sim.setNetwork(&network);

  // fluids
  auto *fluid0 = sim.addFluid(1e-3, 1e3, 1.0 /*, molecular size: 9e-10*/);
  auto *fluid1 = sim.addFluid(4e-3, 1e3, 1.0 /*, molecular size: 9e-10*/);
  //--- continuousPhase ---
  sim.setContinuousPhase(fluid0);

  // droplets
  auto dropletVolume = lDroplet * cWidth * cHeight;
  auto *droplet0 = sim.addDroplet(fluid1->getId(), dropletVolume);
  sim.addDropletInjection(droplet0->getId(), 0.0, c1->getId(), 0.5);
  // auto* droplet1 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet1->getId(), d_m1 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet2 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet2->getId(), d_m2 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet3 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet3->getId(), d_m3 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet4 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet4->getId(), d_m4 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet5 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet5->getId(), d_m5 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet6 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet6->getId(), d_m6 * flowRate / cWidth * cHeight, c1->getId(), 0.5);

  sim.setMixingModel(&mixingModel);
  sim.setResistanceModel(&resistanceModel);
  sim.setType(sim::Type::Abstract);
  sim.setPlatform(sim::Platform::BigDroplet);

  EXPECT_TRUE(network.isNetworkValid());

  // simulate the microfluidic network
  sim.simulate();

  auto &result = *sim.getSimulationResults();

  const std::vector<int> expectedDropletHeaderPath = {0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35};
  const std::vector<int> expectedDropletPayloadPath = {0, 2, 4, 5, 8, 10, 11, 14, 16, 17, 20, 22, 23, 26, 28, 29, 32, 34, 35};
  auto droplet0Path = test::helpers::getDropletPath(result, droplet0->getId());
  // auto droplet1Path = test::helpers::getDropletPath(result, droplet1->getId());
  // auto droplet2Path = test::helpers::getDropletPath(result, droplet2->getId());
  // auto droplet3Path = test::helpers::getDropletPath(result, droplet3->getId());
  // auto droplet4Path = test::helpers::getDropletPath(result, droplet4->getId());
  // auto droplet5Path = test::helpers::getDropletPath(result, droplet5->getId());
  // auto droplet6Path = test::helpers::getDropletPath(result, droplet6->getId());

  auto check_path = [](std::vector<std::vector<int>> actualDropletPath, std::vector<int> expectedDropetPath) {
    int occuranceCount = 1;
    int idx = 0;
    for (auto channels : actualDropletPath) {
      for (auto channelId : channels) {
        ASSERT_EQ(channelId, expectedDropetPath.at(idx));
        occuranceCount++;
        if (occuranceCount == 4) {
          occuranceCount = 0;
          idx++;
        }
      }
    }
    // std::cout << std::endl;
  };
  check_path(droplet0Path, expectedDropletHeaderPath);
  // check_path(droplet1Path, expectedDropletPayloadPath);
  // check_path(droplet2Path, expectedDropletHeaderPath);
  // check_path(droplet3Path, expectedDropletHeaderPath);
  // check_path(droplet4Path, expectedDropletHeaderPath);
}

TEST(Droplet, RingNetworkE2) {
  constexpr auto cContinuousPhaseViscosity = 1e-3;
  sim::InstantaneousMixingModel<T> mixingModel;
  sim::ResistanceModel1D<T> resistanceModel(cContinuousPhaseViscosity);
  arch::Network<T> network;

  // simulator last since it has non-owning references to other objects
  sim::Simulation<T> sim;

  auto *groundNode = network.addNode(1e-10, 1e-10, true);
  auto *sinkNode = network.addNode(9e-8, 9e-8, true);
  auto *node0 = network.addNode(2e-10, 2e-10);
  auto *node1 = network.addNode(3e-10, 3e-10);
  auto *node2 = network.addNode(4e-10, 4e-10);
  auto *node3 = network.addNode(5e-10, 5e-10);
  auto *node4 = network.addNode(6e-10, 6e-10);
  auto *node21 = network.addNode(7e-10, 7e-10);
  auto *node22 = network.addNode(8e-10, 8e-10);
  auto *node23 = network.addNode(9e-10, 9e-10);
  auto *node24 = network.addNode(1e-9, 1e-9);
  auto *node31 = network.addNode(2e-9, 2e-9);
  auto *node32 = network.addNode(3e-9, 3e-9);
  auto *node33 = network.addNode(4e-9, 4e-9);
  auto *node34 = network.addNode(5e-9, 5e-9);
  auto *node41 = network.addNode(6e-9, 6e-9);
  auto *node42 = network.addNode(7e-9, 7e-9);
  auto *node43 = network.addNode(8e-9, 8e-9);
  auto *node44 = network.addNode(9e-9, 9e-9);
  auto *node51 = network.addNode(1e-8, 1e-8);
  auto *node52 = network.addNode(2e-8, 2e-8);
  auto *node53 = network.addNode(3e-8, 3e-8);
  auto *node54 = network.addNode(4e-8, 4e-8);
  auto *node61 = network.addNode(5e-8, 5e-8);
  auto *node62 = network.addNode(6e-8, 6e-8);
  auto *node63 = network.addNode(7e-8, 7e-8);
  auto *node64 = network.addNode(8e-8, 8e-8);

  // flowRate pump
  auto flowRate = 3e-11;
  [[maybe_unused]] auto pump = network.addFlowRatePump(groundNode->getId(), node0->getId(), flowRate);

  // droplet length
  auto lDroplet = 150e-6;

  // droplet distances
  [[maybe_unused]] double d_m1 = 881e-6;
  [[maybe_unused]] double d_m2 = 40678e-6;
  [[maybe_unused]] double d_m3 = 42185e-6;
  [[maybe_unused]] double d_m4 = 41475e-6;

  // channels
  auto cWidth = 100e-6;
  auto cHeight = 33e-6;
  auto bWidth = 200e-6;

  auto iLength = 2000e-6;

  // Node 1
  auto lIn1N1 = 469e-6;
  auto lIn2N1 = 531e-6;
  auto lByN1 = 1200e-6;
  auto lOut1N1 = 14531e-6;
  auto lOut2N1 = 24469e-6;
  // Node 2
  auto lIn1N2 = 743e-6;
  auto lIn2N2 = 757e-6;
  auto lByN2 = 1700e-6;
  auto lOut1N2 = 14257e-6;
  auto lOut2N2 = 24243e-6;
  // Node 3
  auto lIn1N3 = 1021e-6;
  auto lIn2N3 = 979e-6;
  auto lByN3 = 2200e-6;
  auto lOut1N3 = 13979e-6;
  auto lOut2N3 = 24021e-6;
  // Node 4
  auto lIn1N4 = 1300e-6;
  auto lIn2N4 = 1200e-6;
  auto lByN4 = 2700e-6;
  auto lOut1N4 = 13700e-6;
  auto lOut2N4 = 23800e-6;
  // Node 5
  auto lIn1N5 = 1580e-6;
  auto lIn2N5 = 1420e-6;
  auto lByN5 = 3200e-6;
  auto lOut1N5 = 13420e-6;
  auto lOut2N5 = 23580e-6;
  // Node 6
  auto lIn1N6 = 1862e-6;
  auto lIn2N6 = 1638e-6;
  auto lByN6 = 3700e-6;
  auto lOut1N6 = 13138e-6;
  auto lOut2N6 = 23362e-6;

  auto cLength = 500e-6;

  auto *c1 = network.addChannel(node0->getId(), node1->getId(), cHeight, cWidth, iLength, arch::ChannelType::NORMAL);
  // Node 1
  [[maybe_unused]] auto *c2_n1 = network.addChannel(node1->getId(), node2->getId(), cHeight, cWidth, lIn1N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n1 = network.addChannel(node1->getId(), node3->getId(), cHeight, cWidth, lIn2N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n1 = network.addChannel(node2->getId(), node4->getId(), cHeight, cWidth, lOut1N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n1 = network.addChannel(node3->getId(), node4->getId(), cHeight, cWidth, lOut2N1, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n1 = network.addChannel(node4->getId(), node21->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n1 = network.addChannel(node2->getId(), node3->getId(), cHeight, bWidth, lByN1, arch::ChannelType::BYPASS);
  // Node 2
  [[maybe_unused]] auto *c2_n2 = network.addChannel(node21->getId(), node22->getId(), cHeight, cWidth, lIn1N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n2 = network.addChannel(node21->getId(), node23->getId(), cHeight, cWidth, lIn2N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n2 = network.addChannel(node22->getId(), node24->getId(), cHeight, cWidth, lOut1N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n2 = network.addChannel(node23->getId(), node24->getId(), cHeight, cWidth, lOut2N2, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n2 = network.addChannel(node24->getId(), node31->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n2 = network.addChannel(node22->getId(), node23->getId(), cHeight, bWidth, lByN2, arch::ChannelType::BYPASS);
  // Node 3
  [[maybe_unused]] auto *c2_n3 = network.addChannel(node31->getId(), node32->getId(), cHeight, cWidth, lIn1N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n3 = network.addChannel(node31->getId(), node33->getId(), cHeight, cWidth, lIn2N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n3 = network.addChannel(node32->getId(), node34->getId(), cHeight, cWidth, lOut1N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n3 = network.addChannel(node33->getId(), node34->getId(), cHeight, cWidth, lOut2N3, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n3 = network.addChannel(node34->getId(), node41->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n3 = network.addChannel(node32->getId(), node33->getId(), cHeight, bWidth, lByN3, arch::ChannelType::BYPASS);
  // Node 4
  [[maybe_unused]] auto *c2_n4 = network.addChannel(node41->getId(), node42->getId(), cHeight, cWidth, lIn1N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n4 = network.addChannel(node41->getId(), node43->getId(), cHeight, cWidth, lIn2N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n4 = network.addChannel(node42->getId(), node44->getId(), cHeight, cWidth, lOut1N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n4 = network.addChannel(node43->getId(), node44->getId(), cHeight, cWidth, lOut2N4, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n4 = network.addChannel(node44->getId(), node51->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n4 = network.addChannel(node42->getId(), node43->getId(), cHeight, bWidth, lByN4, arch::ChannelType::BYPASS);
  // Node 5
  [[maybe_unused]] auto *c2_n5 = network.addChannel(node51->getId(), node52->getId(), cHeight, cWidth, lIn1N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n5 = network.addChannel(node51->getId(), node53->getId(), cHeight, cWidth, lIn2N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n5 = network.addChannel(node52->getId(), node54->getId(), cHeight, cWidth, lOut1N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n5 = network.addChannel(node53->getId(), node54->getId(), cHeight, cWidth, lOut2N5, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n5 = network.addChannel(node54->getId(), node61->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n5 = network.addChannel(node52->getId(), node53->getId(), cHeight, bWidth, lByN5, arch::ChannelType::BYPASS);
  // Node 6
  [[maybe_unused]] auto *c2_n6 = network.addChannel(node61->getId(), node62->getId(), cHeight, cWidth, lIn1N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c3_n6 = network.addChannel(node61->getId(), node63->getId(), cHeight, cWidth, lIn2N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c4_n6 = network.addChannel(node62->getId(), node64->getId(), cHeight, cWidth, lOut1N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c5_n6 = network.addChannel(node63->getId(), node64->getId(), cHeight, cWidth, lOut2N6, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c6_n6 = network.addChannel(node64->getId(), sinkNode->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
  [[maybe_unused]] auto *c7_n6 = network.addChannel(node62->getId(), node63->getId(), cHeight, bWidth, lByN6, arch::ChannelType::BYPASS);

  //--- sink ---
  network.setSink(sinkNode->getId());
  //--- ground ---
  network.setGround(groundNode->getId());

  EXPECT_TRUE(network.isNetworkValid());
  network.sortGroups();
  sim.setNetwork(&network);

  // fluids
  auto *fluid0 = sim.addFluid(1e-3, 1e3, 1.0 /*, molecular size: 9e-10*/);
  auto *fluid1 = sim.addFluid(4e-3, 1e3, 1.0 /*, molecular size: 9e-10*/);
  //--- continuousPhase ---
  sim.setContinuousPhase(fluid0);

  // droplets
  auto dropletVolume = lDroplet * cWidth * cHeight;
  auto *droplet0 = sim.addDroplet(fluid1->getId(), dropletVolume);
  sim.addDropletInjection(droplet0->getId(), 0.0, c1->getId(), 0.5);
  // auto* droplet1 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet1->getId(), d_m1 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet2 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet2->getId(), d_m2 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet3 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet3->getId(), d_m3 * flowRate / cWidth * cHeight, c1->getId(), 0.5);
  // auto* droplet4 = sim.addDroplet(fluid1->getId(), dropletVolume);
  // sim.addDropletInjection(droplet4->getId(), d_m4 * flowRate / cWidth * cHeight, c1->getId(), 0.5);

  sim.setMixingModel(&mixingModel);
  sim.setResistanceModel(&resistanceModel);
  sim.setType(sim::Type::Abstract);
  sim.setPlatform(sim::Platform::BigDroplet);

  EXPECT_TRUE(network.isNetworkValid());

  // simulate the microfluidic network
  sim.simulate();

  auto &result = *sim.getSimulationResults();

  const std::vector<int> expectedDropletHeaderPath = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36};
  const std::vector<int> expectedDropletPayloadPath = {1, 3, 5, 6, 8, 10, 12, 15, 17, 18, 21, 23, 24, 26, 28, 30, 33, 35, 36};
  auto droplet0Path = test::helpers::getDropletPath(result, droplet0->getId());
  // auto droplet1Path = test::helpers::getDropletPath(result, droplet1->getId());
  // auto droplet2Path = test::helpers::getDropletPath(result, droplet2->getId());
  // auto droplet3Path = test::helpers::getDropletPath(result, droplet3->getId());
  // auto droplet4Path = test::helpers::getDropletPath(result, droplet4->getId());

  auto check_path = [](std::vector<std::vector<int>> actualDropletPath, std::vector<int> expectedDropetPath) {
    int occuranceCount = 1;
    int idx = 0;
    for (auto channels : actualDropletPath) {
      for (auto channelId : channels) {
        ASSERT_EQ(channelId, expectedDropetPath.at(idx));
        occuranceCount++;
        if (occuranceCount == 4) {
          occuranceCount = 0;
          idx++;
        }
      }
    }
    // std::cout << std::endl;
  };
  check_path(droplet0Path, expectedDropletHeaderPath);
  // check_path(droplet1Path, expectedDropletPayloadPath);
  // check_path(droplet2Path, expectedDropletHeaderPath);
  // check_path(droplet3Path, expectedDropletHeaderPath);
  // check_path(droplet4Path, expectedDropletHeaderPath);
}
