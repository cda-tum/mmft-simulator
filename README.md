# MMFT Simulator

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Ubuntu-build](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_ubuntu.yml/badge.svg)](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_ubuntu.yml)
[![MacOS-build](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_macos.yml/badge.svg)](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_macos.yml)

<p align="center">
  <picture>
    <img src="https://www.cda.cit.tum.de/research/microfluidics/logo-microfluidics-toolkit.png" width="60%">
  </picture>
</p>

The MMFT Simulator is a library that contains a collection of simulators for microfluidic devices on different levels of abstraction for different applications, and is currently distributed as a C++ library, as well as a [python package](https://pypi.org/project/mmft.simulator/). The MMFT Simulator is developed by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/) as part of the [Munich MicroFluidic Toolkit (MMFT)](https://www.cda.cit.tum.de/research/microfluidics/munich-microfluidics-toolkit/). For more information about our work on Microfluidics, please visit https://www.cda.cit.tum.de/research/microfluidics/.  If you have any questions, or if you would like to suggest new features, feel free to contact us via microfluidics.cda@xcit.tum.de or by creating an issue on GitHub. 


## Simulators

The MMFT Simulator supports simulations for different platforms of microfluidic devices, i.e., the means (physics) by which a microfluidic device operates in an application. These simulations can be further categorized into levels of abstraction, i.e., the grade at which the underlying physics have been simplified. Overall, this leads to a matrix of simulations for various platforms on different levels of abstraction. The following matrix illustrates the simulations that are currently supported by the MMFT Simulator:

<div align="center">

|               | Abstract      | Hybrid        |
| :-----------  | :-----------: | :-----------: |
| Continuous    | &#x2705;      | &#x2705;      |
| BigDroplet    | &#x2705;      |               |

</div>

### Abstraction Levels
**Abstract**: This abstraction level highly abstracts the underlying physics to an extend that it can, in most cases, provide simulation results almost instantly. It uses a method that draws an analogy between the <br>Hagen-Poiseuille law and Ohm's law, and applies analogous methods from electrical circuit engineering to <br>channel-based microfluidic devices.<br>
**Hybrid**: The Hybrid exploits simulation methods on a high level of abstraction to accelerate CFD simulations (using the LBM as implemented in the OpenLB library) of microfluidic devices.<sup>[[1]](https://doi.org/10.3390/mi15010129)</sup>

### Platforms
**Continuous**: The continuous platform is considered the default platform in the MMFT Simulator and describes the fluid dynamics for channel-based pressure-driven microfluidic flow. <br>
**BigDroplet**: In this platform, droplets are considered in addition to the continuous platform. Droplets are here described as an immiscible fluid immersed in the carrier fluid that acts as the continuous phase and are assumed to fill the entire cross-section of each channel.<sup>[[2]](https://doi.org/10.1016/j.simpa.2022.100440)</sup>


## System Requirements

The implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version 3.21. The python package requires Python version 3.8 or newer. The package is currently tested for Linux distributions and MacOS.

## Usage
### C++
To use this library, include the following code in your cmake file: 
```cmake
include(FetchContent)
FetchContent_Declare(
    mmftsim
    GIT_REPOSITORY https://github.com/cda-tum/mmft-simulator.git
    GIT_TAG master
)
FetchContent_MakeAvailable(mmftsim)

target_link_libraries(${TARGET} PRIVATE mmftsim)
```
and include the library API header in your project file:
```cpp
#include <baseSimulator.h>
#include <baseSimulator.hh>
```

### Python

Install the python package
```python
pip install mmft.simulator
```
and import the MMFT simulator in your code
```python
from mmft import simulator
```

## JSON Definitions

Additional to defining a use case using the MMFT Simulator API, it is also possible to load and store use cases and results with the JSON file format. To simulate a case, the JSON definitions for the `Network` and `Simulation` are necessary. The results can be stored in a JSON file. 

**Network Definition**<br>
A network is defined as a set of `Nodes` and `Channels`. A `Node` contains the x and y position on a Cartesian coordinate system, where the origin is the bottom-left corner of the microfluidic device. If the node is a ground node (i.e. the reference pressure p<sub>0</sub> acts on the node), `ground` must be set `true`. For the BigDroplet simulation platform, the sinks of the domain must additionally be defined. 
```JSON
{
    "x": 2e-3,
    "y": 1e-3,
    "ground": true
}
```
A `Channel` connects two nodes (node1 and node2) and has a width and a height:
```JSON
{
    "node1": 1,
    "node2": 2,
    "width": 1e-4,
    "height": 1e-4
}
```
Examples of JSON definitions of networks can be found in the `examples` folder.

**Simulation Definition**

To define a simulation, the simulation platform, simulation type and the resistance model must be set. The supported platforms are `Continuous` and `BigDroplet`, and the supported simulation types are `Abstract` and `Hybrid`. The `resistanceModel` should be set to `1D` for the `abstract` simulation type, and to `Poiseuille` for the `Hybrid` simulation type.
```JSON
{
    "platform": "Continuous",
    "type": "Abstract",
    "resistanceModel": "1D",
}   
```
A simulation requires a fluid that acts as continuous phase and pumps. The definition of a `fluid` is given below, and the continuous phase is set in `fixtures`. Pumps can be either a pressure pump (`PumpPressure`) with a pressure difference `deltaP`, or a flow rate pump (`PumpFlowRate`) with a flow rate value `flowRate`. A pump is set on a channel of the network, which are indexed sequentially.
```JSON
{
    "fluids": [
        {	
            "name": "Water",
            "concentration": 1,
            "density": 1000,           
            "viscosity": 0.001
        }
    ],
    "pumps": [
        {	
            "channel": 0,
            "type": "PumpPressure",
            "deltaP": 1000
        },
        {	
            "channel": 1,
            "type": "PumpPressure",
            "deltaP": 1000
        },
        {	
            "channel": 2,
            "type": "PumpPressure",
            "deltaP": 1000
        }
    ],
    "fixtures": [
        {
            "name": "Setup #1",
            "phase": 0
        }
    ],
    "activeFixture": 0,
}   
```
A CFD Module is defined with type "LBM" and contains paramaters for the LBM solver instance and information on the geometry of the CFD instance. The geometry of the CFD `Module` is described by a .STL file. The in-/outflow boundaries of the CFD `Module` are described by the `Openings`. Each opening is coupled to a single `Node` (located in the middle of the opening) of the Network, has a normal direction and a width.
```JSON
{
    "settings": {
        "simulators": [
            {	
                "Type": "LBM",
                "name": "Test1-cross-0",
                "stlFile": "/path/to/cross.stl",
                "charPhysLength": 1e-4,
                "charPhysVelocity": 1e-1,
                "alpha": 0.1,
                "resolution": 20,
                "epsilon": 1e-1,
                "tau": 0.55,
                "posX": 1.75e-3,
                "posY": 0.75e-3,
                "sizeX": 5e-4,
                "sizeY": 5e-4,
                "Openings": [
                    {	
                        "node": 5,
                        "normal": {
                            "x": 1.0,
                            "y": 0.0,
                            "z": 0.0
                        },
                        "width": 1e-4,
                        "height": 1e-4
                    },
                    {	
                        "node": 7,
                        "normal": {
                            "x": 0.0,
                            "y": -1.0,
                            "z": 0.0
                        },
                        "width": 1e-4,
                        "height": 1e-4
                    },
                    {	
                        "node": 8,
                        "normal": {
                            "x": 0.0,
                            "y": 1.0,
                            "z": 0.0
                        },
                        "width": 1e-4,
                        "height": 1e-4
                    },
                    {	
                        "node": 9,
                        "normal": {
                            "x": -1.0,
                            "y": 0.0,
                            "z": 0.0
                        },
                        "width": 1e-4,
                        "height": 1e-4
                    }
                ]
            }
        ]
    }
}   
```

Examples of JSON definitions of simulations can be found in the `examples` folder.

## Examples

For the C++ and python API, examples of simulations for different platforms on different levels of abstraction are provided in the `examples` folder of this repository. Examples of JSON definitions for various simulation cases can also be found in the `examples` folder. The two code-snippets below show how the API can be used to load and store JSON files, and to simulate the cases in C++ and python.

### C++

```cpp
#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    std::string networkFile = "/path/to/Network.JSON";
    std::string simulationFile = "/path/to/Simulation.JSON";
    
    // Load and set the network from a JSON file
    arch::Network<T> network = porting::networkFromJSON<T>(networkFile);

    // Check validity
    network.isNetworkValid();

    // Load and set the simulation from a JSON file
    sim::Simulation<T> simulation = porting::simulationFromJSON<T>(simulationFile, &network);
    
    // Simulate
    simulation.simulate();

    // Store results
    porting::resultToJSON<T>("/path/to/Result.JSON", &simulation );
}
```

### Python

```python
from mmft import simulator

# Initialize Network object and load from JSON file
network = simulator.Network()
network.loadNetwork("/path/to/Network.JSON")

# Check validity
network.sort()
network.valid()

# Initialize Simulation object and load from JSON file
simulation = simulator.Simulation()
simulation.loadSimulation(network, "/path/to/Simulation.JSON")

# Perform simulation 
simulation.simulate()

# Store results
simulation.saveResult("/path/to/Result.JSON")
```

## References
More details about the implementation and the mechanisms behind the MMFT Simulator can be found in: 

[[1]](https://doi.org/10.3390/mi15010129) M. Takken, and R. Wille. Accelerated Computational Fluid Dynamics Simulations of Microfluidic Devices by Exploiting Higher Levels of Abstraction. MDPI Micromachines, 2024.

[[2]](https://doi.org/10.1016/j.simpa.2022.100440) G. Fink, F. Costamoling, and R. Wille. MMFT Droplet Simulator: Efficient Simulation of Droplet-based Microfluidic Devices. Software Impacts, 2022.