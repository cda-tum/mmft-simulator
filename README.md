# MMFT Hybrid Simulator

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Ubuntu-build](https://github.com/cda-tum/mmft-hybrid-simulator/actions/workflows/cmake_ubuntu.yml/badge.svg)](https://github.com/cda-tum/mmft-hybrid-simulator/actions/workflows/cmake_ubuntu.yml)
[![MacOS-build](https://github.com/cda-tum/mmft-hybrid-simulator/actions/workflows/cmake_macos.yml/badge.svg)](https://github.com/cda-tum/mmft-hybrid-simulator/actions/workflows/cmake_macos.yml)

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://www.cda.cit.tum.de/research/microfluidics/logo-microfluidics-toolkit.png" width="60%">
    <img src="images/mmft_dark.png" width="60%">
  </picture>
</p>

A Hybrid Simulator for Microfluidic Devices developed by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/) as part of the Munich MicroFluidic Toolkit (MMFT). This simulator exploits the Modified Nodal Analysis (which is a simulation method on a high level of abstraction) to accelerate CFD simulations (using the LBM as implemented in the OpenLB library) of microfluidic devices. 

For more information about our work on Microfluidics, please visit https://www.cda.cit.tum.de/research/microfluidics/. 

If you have any questions, feel free to contact us via microfluidics.cda@xcit.tum.de or by creating an issue on GitHub. 

## System Requirements

The implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version 3.21. The python package requires Python version 3.7 or newer. The package is currently tested for Linux distributions.

## Usage
### C++
To use this library, include the following code in your cmake file: 
```cmake
include(FetchContent)
FetchContent_Declare(
    hybridsim
    GIT_REPOSITORY https://github.com/cda-tum/mmft-hybrid-simulator.git
    GIT_TAG master
)
FetchContent_MakeAvailable(hybridsim)

target_link_libraries(${TARGET} PRIVATE hybridsim)
```
and include the library API header in your project file:
```cpp
#include <baseSimulator.h>
#include <baseSimulator.hh>
```

### Python

Install the python package
```python
pip install mmft.hybridsim
```
and import the hybrid simulator in your code
```python
from mmft import hybridsim
```

## Example

To use the hybrid simulator, the network must be defined in a Network.JSON file. 
A network is defined as a set of `Nodes`, `Channels` and CFD `Modules`. 

A `Node` contains the x and y position on a Cartesian coordinate system, where the origin is the bottom-left corner of the microfluidic device:
```JSON
{
    "iD": 1,
    "x": 2e-3,
    "y": 1e-3
}
```
A `Channel` connects two nodes (nA and nB) and has a width and a height:
```JSON
{
    "iD": 1,
    "nA": 1,
    "nB": 2,
    "width": 1e-4,
    "height": 1e-4
}
```

A CFD `Module` is defined with type `"LBM"` and contains paramaters for the LBM solver instance and information on the geometry of the CFD instance:
```JSON
{
    "iD": 0,
    "Type":"LBM",
    "name": "Test1-cross-0",
    "stlFile": "/path/to/cross.stl",
    "charPhysLength": 1e-4,
    "charPhysVelocity": 1e-2,
    "alpha": 0.1,
    "resolution": 20,
    "epsilon": 0.5,
    "posX": 3.75e-3,
    "posY": 0.75e-3,
    "sizeX": 5e-4,
    "sizeY": 5e-4,
    "Openings":
    [
        {
            "nodeId": 4,
            "normalX": 1.0,
            "normalY": 0.0,
            "width": 1e-4
        },
        {
            "nodeId": 8,
            "normalX": 0.0,
            "normalY": -1.0,
            "width": 1e-4
        },
        {
            "nodeId": 9,
            "normalX": 0.0,
            "normalY": 1.0,
            "width": 1e-4
        },
        {
            "nodeId": 10,
            "normalX": -1.0,
            "normalY": 0.0,
            "width": 1e-4
        }
    ]
}
```
Most importantly, the geometry of the CFD `Module` is described by a .STL file. The in-/outflow boundaries of the CFD `Module` are described by the `Openings`. Each opening is coupled to a single `Node` (located in the middle of the opening), has a normal direction and a width.

Examples of networks can be found in the `examples` folder.

### C++

The simulation case is defined in `main.cpp`. An example of a simulation case in c++ is given here:

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

### Python

The simulation case can be defined once the `mmft.hybridsim` package is installed. An example for a simulation case in python is given here:

```python
from mmft import hybridsim

# New simulation object
simulation = hybridsim.Simulation()

# Load and set the network from a JSON file
network = hybridsim.Network("/path/to/Network.JSON")
simulation.setNetwork(network)

# Add Pressure and/or Flow Rate Pumps
network.setPressurePump(0, 1e3)

# Define and set the continuous phase fluid
fluid = hybridsim.Fluid(0, 1000, 1e-3)
simulation.setContinuousPhase(fluid)

# Define and set the resistance model
resistanceModel = hybridsim.ResistanceModel(fluid.getViscosity())
simulation.setResistanceModel(resistanceModel)

# Perform simulation and store results
simulation.simulate()

# Print the results
simulation.print()
```
