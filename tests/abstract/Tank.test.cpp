#include "../src/baseSimulator.h"

#include "gtest/gtest.h"
#include "../test_definitions.h"

#include <cmath>

#include "../test_helpers.h"

using T = double;

class Tank : public test::definitions::GlobalTest<T> {};

// Membrane and Tank Example based on
//
// Alan Chramiec et al. “Integrated human organ-on-a-chip model for predictive
// studies of anti-tumor drug efficacy and cardiac safety”. In: Lab Chip 20 (23
// 2020), pp. 4357–4372. DOI: 10.1039/D0LC00424C. URL: http://dx.doi.org/10.1039/
// D0LC00424C (cit. on pp. 32, 59, 60, 61).
//
// executed with MembraneModel9.
TEST_F(Tank, TwoTank) {
  // create the simulator
  constexpr auto cContinuousPhaseViscosity = 0.7e-3;
  sim::MembraneModel9<T> membraneModel;
  auto network = arch::Network<T>::createNetwork();

  // simulator last since it has non-owning references to other objects
  sim::AbstractMembrane<T> sim(network);

  // network setup
  auto cWidth = 5e-3;
  auto cHeight = 0.3e-3;         // mentioned in paper
  auto cConnectionLength = 5e-3; // mentioned in paper

  auto cMembraneLength = 8e-3;   // mentioned in paper
  auto mHeight = 55e-6;          // membrane as stated in personal communication with the authors and according to specification on website
  auto mWidth = 4e-3;            // mentioned in paper
  auto pillarArea = 6.28318e-6;  // area covered by pillars
  auto poreRadius = 20e-6 / 2;   // pore size 20e-6 mentioned in paper, millipore size specifies size as diameter
  auto porosity = 0.14;          // membrane as stated in personal communication with the authors and according to specification on membrane manufacturers website

  auto oHeight = 13e-3;
  auto oWidth = 1.5e-6 / (oHeight * cMembraneLength); // 1.5mL organ volume mentioned in paper

  // add nodes
  auto groundSinkNode = network->addNode(0.0, 0.0, true);
  auto node0 = network->addNode(cConnectionLength, 0.0);
  auto node1 = network->addNode(cConnectionLength, cMembraneLength);
  auto node2 = network->addNode(0.0, cMembraneLength);

  // create channels
  auto *c1 = network->addChannel(node0->getId(), node1->getId(), cHeight, cWidth, cMembraneLength, arch::ChannelType::NORMAL); // bone tumor tissue
  [[maybe_unused]] auto *c2 = network->addChannel(node1->getId(), node2->getId(), cHeight, cWidth, cConnectionLength, arch::ChannelType::NORMAL);
  auto *c3 = network->addChannel(node2->getId(), groundSinkNode->getId(), cHeight, cWidth, cMembraneLength, arch::ChannelType::NORMAL); // cardiac tissue

  // create membrane and organ
  auto *m4 = network->addMembraneToChannel(c1->getId(), mHeight, mWidth, poreRadius, porosity);
  auto *o5 = network->addTankToMembrane(m4->getId(), oHeight, oWidth);

  auto *m6 = network->addMembraneToChannel(c3->getId(), mHeight, mWidth - pillarArea / cMembraneLength, poreRadius, porosity);
  auto *o7 = network->addTankToMembrane(m6->getId(), oHeight, oWidth);

  // define that node -1 is a sink
  network->setSink(groundSinkNode->getId());
  // define that node -1 is the ground node
  network->setGround(groundSinkNode->getId());

  // flowRate pump
  constexpr auto flowRate = 5.5e-8; // 3.3mL/min
  // create flow-rate pump from ground node to node 0 with the given flow-rate
  auto *pump0 = network->addFlowRatePump(node0->getId(), groundSinkNode->getId(), flowRate);

  EXPECT_TRUE(network->isNetworkValid());
  network->sortGroups();

  // fluids
  auto continuousPhaseFluid = sim.addFluid(cContinuousPhaseViscosity, 0.993e3 /*molecular size: 9e-10, diffusivity: 4.4e-10, saturation: 0.0*/);

  // define which fluid is the continuous phase
  sim.setContinuousPhase(continuousPhaseFluid);

  auto injectionSpecie = sim.addSpecie(4.4e-10, 3.894e-3);
  auto injectionMixture = sim.addMixture(injectionSpecie, 1.0);

  std::cout << "We have: " <<injectionMixture->getConcentrationOfSpecie(injectionSpecie) << std::endl;

  // Set simuation models
  sim.setInstantaneousMixingModel();
  sim.set1DResistanceModel();
  sim.setMembraneModel(&membraneModel);

  // permanent/continuous fluid injection
  sim.addPermanentMixtureInjection(injectionMixture->getId(), pump0->getId(), 0.0);

  sim.setMaxEndTime(86'400.0);  // 24h
  sim.setWriteInterval(1800.0); // 0.5h

  // simulate the microfluidic network
  sim.simulate();

  auto const &result = *sim.getResults();

  auto fluidConcentrations3 = test::helpers::getAverageFluidConcentrationsInEdge(result, 3 / 0.5, o5->getId());
  ASSERT_NEAR(fluidConcentrations3.at(injectionSpecie->getId()), 0.7, 0.15);
  auto fluidConcentrations6 = test::helpers::getAverageFluidConcentrationsInEdge(result, 6 / 0.5, o5->getId());
  ASSERT_NEAR(fluidConcentrations6.at(injectionSpecie->getId()), 0.9, 0.1);
  auto fluidConcentrations12 = test::helpers::getAverageFluidConcentrationsInEdge(result, 12 / 0.5, o5->getId());
  ASSERT_NEAR(fluidConcentrations12.at(injectionSpecie->getId()), 0.96, 0.05);
  auto fluidConcentrations24 = test::helpers::getAverageFluidConcentrationsInEdge(result, 24 / 0.5, o5->getId());
  ASSERT_NEAR(fluidConcentrations24.at(injectionSpecie->getId()), 1.0, 0.05);

  fluidConcentrations3 = test::helpers::getAverageFluidConcentrationsInEdge(result, 3 / 0.5, o7->getId());
  ASSERT_NEAR(fluidConcentrations3.at(injectionSpecie->getId()), 0.6, 0.1);
  fluidConcentrations6 = test::helpers::getAverageFluidConcentrationsInEdge(result, 6 / 0.5, o7->getId());
  ASSERT_NEAR(fluidConcentrations6.at(injectionSpecie->getId()), 0.8, 0.1);
  fluidConcentrations12 = test::helpers::getAverageFluidConcentrationsInEdge(result, 12 / 0.5, o7->getId());
  ASSERT_NEAR(fluidConcentrations12.at(injectionSpecie->getId()), 0.92, 0.05);
  fluidConcentrations24 = test::helpers::getAverageFluidConcentrationsInEdge(result, 24 / 0.5, o7->getId());
  ASSERT_NEAR(fluidConcentrations24.at(injectionSpecie->getId()), 1.0, 0.05);
}
