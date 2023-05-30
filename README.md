# MMFT Hybrid Simulator

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Ubuntu-build](https://github.com/micheltakken/Base-Simulator/actions/workflows/cmake_ubuntu.yml/badge.svg)](https://github.com/micheltakken/Base-Simulator/actions/workflows/cmake_ubuntu.yml) 
[![MacOS-build](https://github.com/micheltakken/Base-Simulator/actions/workflows/cmake_macos.yml/badge.svg)](https://github.com/micheltakken/Base-Simulator/actions/workflows/cmake_macos.yml)

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="images/mmft_dark.png" width="60%">
    <img src="images/mmft_dark.png" width="60%">
  </picture>
</p>

A Hybrid Simulator for Microfluidic Devices developed by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/) as part of the Munich MicroFluidic Toolkit (MMFT). This simulator exploits the Modified Nodal Analysis (which is a simulation method on a high level of abstraction) to accelerate CFD simulations (using the LBM as implemented in the OpenLB library) of microfluidic devices. 

For more information about our work on Microfluidics, please visit https://www.cda.cit.tum.de/research/microfluidics/. 

If you have any questions, feel free to contact us via microfluidics.cda@xcit.tum.de or by creating an issue on GitHub. 

## System Requirements

The implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version 3.21. The python package requires Python version 3.7 or newer.

## Usage
#### C++
To use this library, include the following code in your cmake file: 
```cmake
include(FetchContent)
FetchContent_Declare(
    hybridSim
    GIT_REPOSITORY https://github.com/cda-tum/mmft-hybrid-simulator.git
    GIT_TAG master
)
FetchContent_MakeAvailable(hybridSim)

target_link_libraries(${TARGET} PRIVATE hybridSim)
```
and include the library API header in your project file:
```cpp
#include <baseSimulator.h>
#include <baseSimulator.hh>
```

#### Python

Install the python package
```python
pip install mmft.hybridSim
```

## Example

#### C++

```cpp
#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    // New simulation object
    std::cout << "[Main] Create simulation object..." << std::endl;
    sim::Simulation<T> simulation = sim::Simulation<T>();

    // Load and set the network from a JSON file
    std::cout << "[Main] Load the JSON network..." << std::endl;
    std::string file = "/path/to/Network.JSON";
    arch::Network<T>* network = new arch::Network<T>(file);
    simulation.setNetwork(network);

    // Add Pressure and/or Flow Rate Pumps
    std::cout << "[Main] Add pressure and Flow rate pumps..." << std::endl;
    network->setPressurePump(0, T(1e3));

    // Define and set the continuous phase fluid
    std::cout << "[Main] Set the continuous phase fluid..." << std::endl;
    sim::Fluid<T>* fluid = new sim::Fluid<T>(0, T(1000), T(1e-3));
    fluid->setName("Water");
    simulation.setContinuousPhase(fluid);

    // Define and set the resistance model
    std::cout << "[Main] Set the resistance model..." << std::endl;
    sim::ResistanceModel1D<T>* resistanceModel = new sim::ResistanceModel1D<T>(fluid->getViscosity());
    simulation.setResistanceModel(resistanceModel);

    // Perform simulation and store results
    std::cout << "[Main] Simulation..." << std::endl;
    simulation.simulate();

    // Print the results
    std::cout << "[Main] Results..." << std::endl;
    simulation.printResults();

    return 0;
}
```

#### Python

```python
import mmft.hybridSim as hybridSim

# New simulation object
simulation = hybridSim.Simulation()

# Load and set the network from a JSON file
network = hybridSim.Network("/path/to/Network.JSON")
simulation.setNetwork(network)

# Add Pressure and/or Flow Rate Pumps
network.setPressurePump(0, 1e3)

# Define and set the continuous phase fluid
fluid = hybridSim.Fluid(0, 1000, 1e-3)
simulation.setContinuousPhase(fluid)

# Define and set the resistance model
resistanceModel = hybridSim.ResistanceModel(fluid.getViscosity())
simulation.setResistanceModel(resistanceModel)

# Perform simulation and store results
simulation.simulate()

# Print the results
simulation.print()
```
