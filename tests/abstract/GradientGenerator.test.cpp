#include "gtest/gtest.h"

#include <exception>
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/baseSimulator.h"
#include "../test_helpers.h"
#include "../test_definitions.h"

using T = double;

class GradientGenerator : public test::definitions::GlobalTest<T> {};

/*
Continuous Fluid Simulation Tests based on
Gerold Fink et al. “Automatic Design of Microfluidic Gradient Generators”. In: IEEE Access
10 (2022), pp. 28155–28164. DOI: 10.1109/ACCESS.2022.3158327 (cit. on pp. 50, 51, 62).
*/

TEST_F(GradientGenerator, GradientGeneratorSmall) { // Result 50%
  /* Settings Gradient Generator to generate this network:
  let w = 300e-6;
  let h = 100e-6;
  let radius = w;
  let wMeanderMax = 19 * w;
  let lConnection = (wMeanderMax + w) / 2;
  const lInlet = 10 * w;
  const lOutlet = 10 * w;
  let qInlet = 0.02 * w * h;
  let mu = 1e-3;
  let tMin = 1e-10;
  let concentrations = [0.5];
  let nOutlets = concentrations.length + 2;
  let nInlets = 2;
  */
  // create the simulator
  constexpr auto cContinuousPhaseViscosity = 1e-3;
  sim::InstantaneousMixingModel<T> mixingModel;
  sim::MembraneModel9<T> membraneModel;
  auto network = arch::Network<T>::createNetwork();

  // simulator last since it has non-owning references to other objects
  sim::AbstractMembrane<T> sim(network);

  // channel parameters
  auto cWidth = 300e-6;
  auto cHeight = 100e-6;
  auto inletLength = 10 * cWidth;
  auto outletLength = 10 * cWidth;
  auto wMeanderMax = 19 * cWidth;
  auto connectionLength = (wMeanderMax + cWidth) / 2;

  // create nodes
  auto groundNode = network->addNode(1e-10, 1e-10, true);
  auto sinkNode = network->addNode(9e-8, 9e-8, true);
  auto node0 = network->addNode(2e-10, 2e-10);
  auto node1 = network->addNode(3e-10, 3e-10);
  auto node2 = network->addNode(4e-10, 4e-10);
  auto node3 = network->addNode(5e-10, 5e-10);
  auto node4 = network->addNode(6e-10, 6e-10);
  auto node5 = network->addNode(7e-10, 7e-10);
  auto node6 = network->addNode(8e-10, 8e-10);
  auto node7 = network->addNode(9e-10, 9e-10);
  auto node8 = network->addNode(1e-9, 1e-9);
  auto node9 = network->addNode(2e-9, 2e-9);

  // create channels
  // inlets
  [[maybe_unused]]
  auto *c2 = network->addChannel(node0->getId(), node3->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c3 = network->addChannel(node1->getId(), node5->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  // connections
  [[maybe_unused]]
  auto *c4 = network->addChannel(node3->getId(), node2->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c5 = network->addChannel(node3->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c6 = network->addChannel(node5->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c7 = network->addChannel(node5->getId(), node6->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  // meanders
  [[maybe_unused]]
  auto *c8 = network->addChannel(node2->getId(), node7->getId(), cHeight, cWidth, 0.003084956172616956, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c9 = network->addChannel(node4->getId(), node8->getId(), cHeight, cWidth, 0.004584956172616956, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c10 = network->addChannel(node6->getId(), node9->getId(), cHeight, cWidth, 0.003084956172616958, arch::ChannelType::NORMAL);
  // outlets
  auto *c11 = network->addChannel(node7->getId(), sinkNode->getId(), cHeight, cWidth, outletLength, arch::ChannelType::NORMAL);
  auto *c12 = network->addChannel(node8->getId(), sinkNode->getId(), cHeight, cWidth, outletLength, arch::ChannelType::NORMAL);
  auto *c13 = network->addChannel(node9->getId(), sinkNode->getId(), cHeight, cWidth, outletLength, arch::ChannelType::NORMAL);

  // flowRate pump
  auto flowRate = 6e-10;
  // create flow-rate pump from ground node to node0/node1 with the given flow-rate
  [[maybe_unused]]
  auto *pump0 = network->addFlowRatePump(groundNode->getId(), node0->getId(), flowRate);
  auto *pump1 = network->addFlowRatePump(groundNode->getId(), node1->getId(), flowRate);

  // define sink/ground node(s)
  network->setGround(groundNode->getId());
  network->setSink(sinkNode->getId());

  EXPECT_TRUE(network->isNetworkValid());
  network->sortGroups();

  // fluids
  auto waterYellow = sim.addFluid(cContinuousPhaseViscosity, 1.56e3);
  // auto water = sim.addFluid(8.65269e-4, 1.56e3, 1.0, 9e-10);  // 27 Grad
  // auto glucose = sim.addFluid(1.306489e-3, 1e3, 1.0, 9e-10);  // 27 Grad

  // define which fluid is the continuous phase
  sim.setContinuousPhase(waterYellow);

  // permanent/continuous fluid injection
  auto *waterBlue = sim.addSpecie(0.0, 0.0);
  auto *injectionMixture = sim.addMixture({{waterBlue->getId(), 1.0}});

  sim.addPermanentMixtureInjection(injectionMixture->getId(), pump1->getId(), 0.0);

  sim.setMaxEndTime(100.0);
  sim.setWriteInterval(100.0);

  sim.setMixingModel(&mixingModel);
  sim.set1DResistanceModel();
  sim.setMembraneModel(&membraneModel);

  // simulate the microfluidic network
  sim.simulate();

  porting::resultToJSON("GradientGeneratorSmall.json", &sim);

  auto const &result = *sim.getResults();
  ASSERT_FALSE(result.states.empty());
  int lastStateId = result.states.back()->id;

  auto fluidConcentrations11 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c11->getId());
  ASSERT_NEAR(fluidConcentrations11.at(-1), // -1 is continuous phase concentration (waterYellow)
              1.0, std::numeric_limits<T>::epsilon());

  auto fluidConcentrations12 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c12->getId());
  ASSERT_NEAR(fluidConcentrations12.at(-1), 0.5, 5e-14); // -1 is continuous phase
                                                         // concentration (waterYellow)
  ASSERT_NEAR(fluidConcentrations12.at(waterBlue->getId()), 0.5, 5e-14);

  auto fluidConcentrations13 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c13->getId());
  ASSERT_NEAR(fluidConcentrations13.at(waterBlue->getId()), 1.0, std::numeric_limits<T>::epsilon());
}

TEST_F(GradientGenerator, GradientGeneratorSmallDifferentPaper) { // Result Paper 38.63%
  /* Settings Gradient Generator to generate this network:
  let w = 300e-6; // mentioned in paper
  let h = 200e-6; // mentioned in paper
  let radius = w;
  let wMeanderMax = 21 * w;
  let lConnection = (wMeanderMax + w) / 2;
  const lInlet = 10 * w;
  const lOutlet = 10 * w;
  let qInlet = 5e-9 / 60; // mentioned in paper: 5uLsˆ-1
  let mu = 1e-3;
  let tMin = 60;
  let concentrations = [0.3863]; // mentioned in paper
  let nOutlets = concentrations.length + 2; // due to concentrations
  let nInlets = 2; // due to concentrations
  */
  constexpr auto cContinuousPhaseViscosity = 1e-3;
  sim::InstantaneousMixingModel<T> mixingModel;
  sim::MembraneModel9<T> membraneModel;
  auto network = arch::Network<T>::createNetwork();

  sim::AbstractMembrane<T> sim(network); // simulator last since it has non-owning references to other
                          // objects

  // channels
  auto cWidth = 300e-6;
  auto cHeight = 200e-6;
  auto inletLength = 10 * cWidth;
  auto wMeanderMax = 21 * cWidth;
  auto connectionLength = (wMeanderMax + cWidth) / 2;

  // create nodes
  auto groundNode = network->addNode(1e-10, 1e-10, true);
  auto sinkNode = network->addNode(9e-8, 9e-8, true);
  auto node0 = network->addNode(0.0, 0.0);
  auto node1 = network->addNode(0.0, 0.0);
  auto node2 = network->addNode(0.0, 0.0);
  auto node3 = network->addNode(0.0, 0.0);
  auto node4 = network->addNode(0.0, 0.0);
  auto node5 = network->addNode(0.0, 0.0);
  auto node6 = network->addNode(0.0, 0.0);
  auto node7 = network->addNode(0.0, 0.0);
  auto node8 = network->addNode(0.0, 0.0);
  auto node9 = network->addNode(0.0, 0.0);

  // create channels
  // inlets
  [[maybe_unused]]
  auto *c2 = network->addChannel(node0->getId(), node3->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c3 = network->addChannel(node1->getId(), node5->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  // connections
  [[maybe_unused]]
  auto *c4 = network->addChannel(node3->getId(), node2->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c5 = network->addChannel(node3->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c6 = network->addChannel(node5->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c7 = network->addChannel(node5->getId(), node6->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  // meanders
  [[maybe_unused]]
  auto *c8 = network->addChannel(node2->getId(), node7->getId(), cHeight, cWidth, 0.04163712059499694, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c9 = network->addChannel(node4->getId(), node8->getId(), cHeight, cWidth, 0.055556211206648096, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c10 = network->addChannel(node6->getId(), node9->getId(), cHeight, cWidth, 0.055783776606846545, arch::ChannelType::NORMAL);
  // outlets
  auto *c11 = network->addChannel(node7->getId(), sinkNode->getId(), cHeight, cWidth, 0.006899999999999999, arch::ChannelType::NORMAL);
  auto *c12 = network->addChannel(node8->getId(), sinkNode->getId(), cHeight, cWidth, 0.0009, arch::ChannelType::NORMAL);
  auto *c13 = network->addChannel(node9->getId(), sinkNode->getId(), cHeight, cWidth, 0.006899999999999999, arch::ChannelType::NORMAL);

  // flowRate pumps
  auto flowRate = 5e-9 / 60;
  // create flow-rate pump from ground node to node0/node1 with the given flow-rate
  [[maybe_unused]]
  auto *pump0 = network->addFlowRatePump(groundNode->getId(), node0->getId(), flowRate);
  auto *pump1 = network->addFlowRatePump(groundNode->getId(), node1->getId(), flowRate);

  // define sink/ground node(s)
  network->setGround(groundNode->getId());
  network->setSink(sinkNode->getId());

  EXPECT_TRUE(network->isNetworkValid());
  network->sortGroups();

  // fluids
  auto waterYellow = sim.addFluid(cContinuousPhaseViscosity, 1.56e3);
  // auto water = sim.addFluid(8.65269e-4, 1.56e3, 1.0, 9e-10);  // 27 Grad
  // auto glucose = sim.addFluid(1.306489e-3, 1e3, 1.0, 9e-10);  // 27 Grad

  // define which fluid is the continuous phase
  sim.setContinuousPhase(waterYellow);

  // permanent/continuous fluid injection
  auto *injectionSpecie = sim.addSpecie(0.0, 0.0);
  auto *waterBlue = sim.addMixture({{injectionSpecie->getId(), 1.0}});

  sim.addPermanentMixtureInjection(waterBlue->getId(), pump1->getId(), 0.0);

  sim.setMaxEndTime(100.0);
  sim.setWriteInterval(100.0);

  sim.setMixingModel(&mixingModel);
  sim.set1DResistanceModel();
  sim.setMembraneModel(&membraneModel);

  // simulate the microfluidic network
  sim.simulate();

  porting::resultToJSON("GradientGeneratorSmallDifferent.json", &sim);

  auto const &result = *sim.getResults();
  ASSERT_FALSE(result.states.empty());
  int lastStateId = result.states.back()->id;

  auto fluidConcentrations11 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c11->getId());
  ASSERT_NEAR(fluidConcentrations11.at(-1), // -1 is continuous phase concentration (waterYellow)
              1.0, std::numeric_limits<T>::epsilon());

  auto fluidConcentrations12 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c12->getId());
  ASSERT_NEAR(fluidConcentrations12.at(-1), 0.3863, 5e-14); // -1 is continuous phase concentration (waterYellow)
  ASSERT_NEAR(fluidConcentrations12.at(waterBlue->getId()), 1.0 - 0.3863, 5e-14);
  auto fluidConcentrations13 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c13->getId());
  ASSERT_NEAR(fluidConcentrations13.at(waterBlue->getId()), 1.0, std::numeric_limits<T>::epsilon());
}

TEST_F(GradientGenerator, GradientGeneratorUltraLargePaper) { // Paper 100%/88.64%/40.12%/21.82%/0%
  /* Settings Gradient Generator to generate this network:
  let w = 300e-6;
  let h = 200e-6;
  let radius = w;
  let wMeanderMax = 21 * w;
  let lConnection = (wMeanderMax + w) / 2;
  const lInlet = 10 * w;
  const lOutlet = 10 * w;
  const straightOutlets = false;
  let qInlet = 5e-9 / 60;
  let mu = 1e-3;
  let tMin = 60;
  let concentrations = [0.8864, 0.4012, 0.2182];
  let nOutlets = concentrations.length + 2;
  let nInlets = 2;
  */
  constexpr auto cContinuousPhaseViscosity = 1e-3;
  sim::InstantaneousMixingModel<T> mixingModel;
  sim::ResistanceModel1D<T> resistanceModel(cContinuousPhaseViscosity);
  sim::MembraneModel9<T> membraneModel;
  auto network = arch::Network<T>::createNetwork();

  sim::AbstractMembrane<T> sim(network); // simulator last since it has non-owning references to other objects

  // channels parameters
  auto cWidth = 300e-6;
  auto cHeight = 200e-6;
  auto inletLength = 10 * cWidth;
  auto wMeanderMax = 21 * cWidth;
  auto connectionLength = (wMeanderMax + cWidth) / 2;

  // create nodes
  auto groundNode = network->addNode(1e-10, 1e-10, true);
  auto sinkNode = network->addNode(9e-8, 9e-8, true);
  auto node0 = network->addNode(0.0, 0.0);
  auto node1 = network->addNode(0.0, 0.0);
  auto node2 = network->addNode(0.0, 0.0);
  auto node3 = network->addNode(0.0, 0.0);
  auto node4 = network->addNode(0.0, 0.0);
  auto node5 = network->addNode(0.0, 0.0);
  auto node6 = network->addNode(0.0, 0.0);
  auto node7 = network->addNode(0.0, 0.0);
  auto node8 = network->addNode(0.0, 0.0);
  auto node9 = network->addNode(0.0, 0.0);
  auto node10 = network->addNode(0.0, 0.0);
  auto node11 = network->addNode(0.0, 0.0);
  auto node12 = network->addNode(0.0, 0.0);
  auto node13 = network->addNode(0.0, 0.0);
  auto node14 = network->addNode(0.0, 0.0);
  auto node15 = network->addNode(0.0, 0.0);
  auto node16 = network->addNode(0.0, 0.0);
  auto node17 = network->addNode(0.0, 0.0);
  auto node18 = network->addNode(0.0, 0.0);
  auto node19 = network->addNode(0.0, 0.0);
  auto node20 = network->addNode(0.0, 0.0);
  auto node21 = network->addNode(0.0, 0.0);
  auto node22 = network->addNode(0.0, 0.0);
  auto node23 = network->addNode(0.0, 0.0);
  auto node24 = network->addNode(0.0, 0.0);
  auto node25 = network->addNode(0.0, 0.0);
  auto node26 = network->addNode(0.0, 0.0);
  auto node27 = network->addNode(0.0, 0.0);

  // create channels
  // inlets
  [[maybe_unused]]
  auto *c2 = network->addChannel(node0->getId(), node3->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c3 = network->addChannel(node1->getId(), node5->getId(), cHeight, cWidth, inletLength, arch::ChannelType::NORMAL);
  // connections
  [[maybe_unused]]
  auto *c4 = network->addChannel(node3->getId(), node2->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c5 = network->addChannel(node3->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c6 = network->addChannel(node5->getId(), node4->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c7 = network->addChannel(node5->getId(), node6->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  // meanders
  [[maybe_unused]]
  auto *c8 = network->addChannel(node2->getId(), node8->getId(), cHeight, cWidth, 0.0299671361633416, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c9 = network->addChannel(node4->getId(), node10->getId(), cHeight, cWidth, 0.044431241343332664, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c10 = network->addChannel(node6->getId(), node12->getId(), cHeight, cWidth, 0.03161589671051416, arch::ChannelType::NORMAL);
  // connections
  [[maybe_unused]]
  auto *c11 = network->addChannel(node8->getId(), node7->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c12 = network->addChannel(node8->getId(), node9->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c13 = network->addChannel(node10->getId(), node9->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c14 = network->addChannel(node10->getId(), node11->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c15 = network->addChannel(node12->getId(), node11->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c16 = network->addChannel(node12->getId(), node13->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  // meanders
  [[maybe_unused]]
  auto *c17 = network->addChannel(node7->getId(), node14->getId(), cHeight, cWidth, 0.03901274147675735, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c18 = network->addChannel(node9->getId(), node15->getId(), cHeight, cWidth, 0.12054175031249897, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c19 = network->addChannel(node11->getId(), node16->getId(), cHeight, cWidth, 0.047689442515671886, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c20 = network->addChannel(node13->getId(), node17->getId(), cHeight, cWidth, 0.051441945635952405, arch::ChannelType::NORMAL);
  // connections
  [[maybe_unused]]
  auto *c21 = network->addChannel(node14->getId(), node18->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c22 = network->addChannel(node14->getId(), node19->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c23 = network->addChannel(node15->getId(), node19->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c24 = network->addChannel(node15->getId(), node20->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c25 = network->addChannel(node16->getId(), node20->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c26 = network->addChannel(node16->getId(), node21->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c27 = network->addChannel(node17->getId(), node21->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c28 = network->addChannel(node17->getId(), node22->getId(), cHeight, cWidth, connectionLength, arch::ChannelType::NORMAL);
  // meanders
  [[maybe_unused]]
  auto *c29 = network->addChannel(node18->getId(), node23->getId(), cHeight, cWidth, 0.026532242716654282, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c30 = network->addChannel(node19->getId(), node24->getId(), cHeight, cWidth, 0.03333335815696055, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c31 = network->addChannel(node20->getId(), node25->getId(), cHeight, cWidth, 0.03948202927907311, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c32 = network->addChannel(node21->getId(), node26->getId(), cHeight, cWidth, 0.03355323373613861, arch::ChannelType::NORMAL);
  [[maybe_unused]]
  auto *c33 = network->addChannel(node22->getId(), node27->getId(), cHeight, cWidth, 0.02498585157999049, arch::ChannelType::NORMAL);
  // outlets
  auto *c34 = network->addChannel(node23->getId(), sinkNode->getId(), cHeight, cWidth, 0.013499999999999998, arch::ChannelType::NORMAL);
  auto *c35 = network->addChannel(node24->getId(), sinkNode->getId(), cHeight, cWidth, 0.007499999999999999, arch::ChannelType::NORMAL);
  auto *c36 = network->addChannel(node25->getId(), sinkNode->getId(), cHeight, cWidth, 0.0014999999999999998, arch::ChannelType::NORMAL);
  auto *c37 = network->addChannel(node26->getId(), sinkNode->getId(), cHeight, cWidth, 0.007499999999999999, arch::ChannelType::NORMAL);
  auto *c38 = network->addChannel(node27->getId(), sinkNode->getId(), cHeight, cWidth, 0.013499999999999998, arch::ChannelType::NORMAL);

  // flowRate pumps
  auto flowRate = 5e-9 / 60;
  // create flow-rate pump from ground node to node0/node1 with the given flow-rate
  [[maybe_unused]]
  auto *pump0 = network->addFlowRatePump(groundNode->getId(), node0->getId(), flowRate);
  auto *pump1 = network->addFlowRatePump(groundNode->getId(), node1->getId(), flowRate);

  // define sink/ground node(s)
  network->setGround(groundNode->getId());
  network->setSink(sinkNode->getId());

  EXPECT_TRUE(network->isNetworkValid());
  network->sortGroups();

  // fluids
  auto waterYellow = sim.addFluid(cContinuousPhaseViscosity, 1.56e3 /* molecular size: 9e-10*/);
  // auto water = sim.addFluid(8.65269e-4, 1.56e3, 1.0, 9e-10);  // 27 Grad
  // auto glucose = sim.addFluid(1.306489e-3, 1e3, 1.0, 9e-10);  // 27 Grad

  // define which fluid is the continuous phase
  sim.setContinuousPhase(waterYellow);

  // permanent/continuous fluid injection
  auto *injectionSpecie = sim.addSpecie(0.0, 0.0);
  auto *waterBlue = sim.addMixture({{injectionSpecie->getId(), 1.0}});

  sim.addPermanentMixtureInjection(waterBlue->getId(), pump1->getId(), 0.0);

  sim.setMaxEndTime(1000.0);
  sim.setWriteInterval(1000.0);

  sim.setMixingModel(&mixingModel);
  sim.set1DResistanceModel();
  sim.setMembraneModel(&membraneModel);

  // simulate the microfluidic network
  sim.simulate();

  porting::resultToJSON("GradientGeneratorUltraLarge.json", &sim);

  auto const &result = *sim.getResults();
  ASSERT_FALSE(result.states.empty());
  int lastStateId = result.states.back()->id;

  auto fluidConcentrations34 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c34->getId());
  EXPECT_NEAR(fluidConcentrations34.at(-1), // -1 is continuous phase concentration (waterYellow)
              1.0, std::numeric_limits<T>::epsilon());

  auto fluidConcentrations35 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c35->getId());
  // -1 is continuous phase concentration (waterYellow)
  EXPECT_NEAR(fluidConcentrations35.at(-1), 0.8864, 5e-14);
  EXPECT_NEAR(fluidConcentrations35.at(waterBlue->getId()), 1.0 - 0.8864, 5e-14);

  auto fluidConcentrations36 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c36->getId());
  // -1 is continuous phase concentration (waterYellow)
  EXPECT_NEAR(fluidConcentrations36.at(-1), 0.4012, 5e-14);
  EXPECT_NEAR(fluidConcentrations36.at(waterBlue->getId()), 1.0 - 0.4012, 5e-14);

  auto fluidConcentrations37 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c37->getId());
  // -1 is continuous phase concentration (waterYellow)
  EXPECT_NEAR(fluidConcentrations37.at(-1), 0.2182, 5e-14);
  EXPECT_NEAR(fluidConcentrations37.at(waterBlue->getId()), 1.0 - 0.2182, 5e-14);

  auto fluidConcentrations38 = test::helpers::getAverageFluidConcentrationsInEdge(result, lastStateId, c38->getId());
  EXPECT_NEAR(fluidConcentrations38.at(waterBlue->getId()), 1.0, std::numeric_limits<T>::epsilon());
}
