#include "gtest/gtest.h"
#include "benchmark/benchmark.h"

#include "../src/baseSimulator.h"
#include "../src/baseSimulator.hh"

using T = double;

void BM_empty(benchmark::State& state) {
  std::vector<int> x_vec;
  for (auto _ : state) {
    int x;
    for (x = 0; x < 200; ++x) {
      x *= 2;
    }
    x_vec.push_back(x);
  }
}
BENCHMARK(BM_empty);


void BM_simRun(benchmark::State& state) {

  std::string file = "../examples/Hybrid/Network4a.JSON";

  // Load and set the network from a JSON file
  arch::Network<T> network = porting::networkFromJSON<T>(file);

  // Load and set the simulation from a JSON file
  sim::Simulation<T> testSimulation = porting::simulationFromJSON<T>(file, &network);
  for (auto _ : state) {
    testSimulation.simulate();
  }
}
BENCHMARK(BM_simRun);


BENCHMARK_MAIN(); 