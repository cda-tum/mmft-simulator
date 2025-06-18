# MMFT Simulator

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Ubuntu-build](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_ubuntu.yml/badge.svg)](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_ubuntu.yml)
[![MacOS-build](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_macos.yml/badge.svg)](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_macos.yml)
<!-- [![Windows-build](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/cda-tum/mmft-simulator/actions/workflows/cmake_windows.yml) -->

<p align="center">
  <picture>
    <img src="https://www.cda.cit.tum.de/research/microfluidics/logo-microfluidics-toolkit.png" width="60%">
  </picture>
</p>

The MMFT Simulator is a library that contains a collection of simulators for closed channel-based microfluidic devices on different levels of abstraction for different applications, and is currently distributed as a C++ library, as well as a [python package](https://pypi.org/project/mmft.simulator/). The MMFT Simulator is developed by the [Chair for Design Automation](https://www.cda.cit.tum.de/) at the [Technical University of Munich](https://www.tum.de/) as part of the [Munich Microfluidic Toolkit (MMFT)](https://www.cda.cit.tum.de/research/microfluidics/munich-microfluidics-toolkit/). For more information about our work on Microfluidics, please visit https://www.cda.cit.tum.de/research/microfluidics/.  If you have any questions, or if you would like to suggest new features, feel free to contact us via microfluidics.cda@xcit.tum.de or by creating an issue on GitHub. 


## Simulators

The MMFT Simulator supports simulations for different platforms of microfluidic devices, i.e., the means (physics) by which a microfluidic device operates in an application. These simulations can be further categorized into levels of abstraction, i.e., the grade at which the underlying physics have been simplified. More information regarding microfluidic simulations on different levels of abstraction are provided in the respective publication.<sup>[[1]](https://doi.org/10.3390/s22145392)</sup> Overall, this leads to a matrix of simulations for various platforms on different levels of abstraction. The matrix below illustrates the simulations that are currently supported by the MMFT Simulator. In this matrix, the rows describe the microfluidic platform and the columns the level of abstraction of the simulation.

<div align="center">

|               | Abstract      | Hybrid        |
| :-----------  | :-----------: | :-----------: |
| Continuous    | &#x2705;      | &#x2705;      |
| BigDroplet    | &#x2705;      |               |
| Mixing        | &#x2705;      |               |

</div>

### Abstraction Levels
**Abstract**: This abstraction level highly abstracts the underlying physics to an extend that it can, in most cases, provide simulation results almost instantly. It uses a method that draws an analogy between the <br>Hagen-Poiseuille law and Ohm's law, and applies analogous methods from electrical circuit engineering to <br>channel-based microfluidic devices.<br>
**Hybrid**: The Hybrid exploits simulation methods on a high level of abstraction to accelerate CFD simulations (using the LBM as implemented in the OpenLB library) of microfluidic devices. More details can be found in the corresponding publication.<sup>[[2]](https://doi.org/10.3390/mi15010129)</sup>

### Platforms
**Continuous**: The continuous platform is considered the default platform in the MMFT Simulator and describes the fluid dynamics for channel-based pressure-driven microfluidic flow. <sup>[[3]](https://doi.org/10.1039/C2LC20799K)</sup> <br>
**BigDroplet**: In this platform, big droplets are considered in addition to the continuous platform. Big droplets are here described as an immiscible fluid immersed in the carrier fluid that acts as the continuous phase and are assumed to fill the entire cross-section of each channel (hence the terminology "big droplet"), generally generated in the squeezing regime. For more details, please see the respective publications. <sup>[[4]](https://doi.org/10.1145/3313867)</sup> <sup>[[5]](https://doi.org/10.1016/j.simpa.2022.100440)</sup> <br>
**Mixing**: Solvents play an important role in microfluidics and on this platform, concentrations of species dissolved in the continuous phase can be simulated. There are currently **two** mixing models available:
* Instantaneous mixing model: Fluids are assumed to be fully mixed as soon as they meet.
* Diffusive mixing model: Concentration distributions of solvents are tracked and propagated according to the network topology.



## System Requirements

The implementation should be compatible with any current C++ compiler supporting C++17 and a minimum CMake version 3.21. The python package requires Python version 3.8 or newer. The package is currently tested for Linux distributions and MacOS.

## Usage

This library is currently supported for use in C++ and python projects. The respective usage is described below.

### C++
To use this library in c++, include the following code in your cmake file: 
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

To use this library in python, install the python package
```python
pip install mmft.simulator
```
and import the MMFT simulator in your code
```python
from mmft import simulator
```

## API

Simulations can be defined using the C++ and Python APIs. Additionally, simulations can also be defined using the JSON file format. Please see the `examples` folder of this repository for examples of various simulations using the C++ or Python API, or JSON files. The two code-snippets below show how the API can be used to load and store JSON files, and to simulate the cases in C++ and python. Afterwards, a more detailed description of the JSON file formats is given.

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

### JSON Definitions

The JSON file formats provide an accessible way for loading and storing simulation cases and results. To simulate a case, the JSON definitions for the `Network` and `Simulation` are necessary. Once a simulation is finished, the `Result` can be stored in a JSON file (see code-snippets above). 

**Network Definition**<br>
The geometry of a simulation case must be described as a network, which is here defined as a set of `Nodes` and `Channels`. A `Node` contains the x and y position on a Cartesian coordinate system, where the origin is the bottom-left corner of the microfluidic device. If the node is a ground node (i.e. the reference pressure p<sub>0</sub> acts on the node), `ground` must be set `true`. For the BigDroplet simulation platform, the sinks of the domain must additionally be set at the nodes where a droplet can exit the domain. 
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
A `module` defines a "black box" region on which a `simulator` acts. This region is defined by it's position, size, and the nodes that are present on its interface. An example use for the `module` is to define a CFD region for a Hybrid simulation.
```JSON
{
    "position": [2.75e-3, 1.75e-3],
    "size": [5e-4, 5e-4],
    "nodes": [5, 7, 8, 9]
}
```

Please, see the `examples` folder for examples of JSON definitions of networks for various simulations.

**Simulation Definition**

To define a simulation, the simulation platform, simulation type and the resistance model must be set. The supported platforms are `Continuous` and `BigDroplet`, and the supported simulation types are `Abstract` and `Hybrid`. The `resistanceModel` should be set to `1D` for the `abstract` simulation type, and to `Poiseuille` for the `Hybrid` simulation type.
```JSON
{
    "platform": "Continuous",
    "type": "Abstract",
    "resistanceModel": "Rectangular",
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
For simulations that are of `hybrid` type, at least one CFD `simulator` must be defined on a `module`, which is the subset of the domain that will be simulated using CFD. A CFD `simulator` is currently only supported for "LBM" type and contains paramaters for the LBM solver instance and a reference to the module it acts on. The geometry of the CFD `simulator` is described by a .STL file, which is contained by the bounding-box given by the `module`. The in-/outflow boundaries of the CFD `simulator` are described by the `Openings`. Each opening is coupled to a single `Node` (located in the middle of the opening) of the Network, has a normal direction and a width. These nodes are the interface nodes of the `module` on which the `simulator` is defined, and the node ids must correspond.
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
                "moduleId": 0,
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
For examples of JSON definitions of simulations for various simulations and definitions of CFD modules, please see the `examples` folder.

## References
More details about the implementation and the mechanisms behind the MMFT Simulator can be found in: 

[[1]](https://doi.org/10.3390/s22145392) M. Takken, and R. Wille. Simulation of Pressure-Driven and Channel-based Microfluidics on Different Abstract Levels: A Case Study. MDPI Sensors, 2022.

[[2]](https://doi.org/10.3390/mi15010129) M. Takken, and R. Wille. Accelerated Computational Fluid Dynamics Simulations of Microfluidic Devices by Exploiting Higher Levels of Abstraction. MDPI Micromachines, 2024.

[[3]](https://doi.org/10.1039/C2LC20799K) K. W. Oh, K. Lee, B. Ahn, and E. P. Furlani. Design of pressure-driven microfluidic networks using electric circuit analogy. Lab on a Chip 2012.

[[4]](https://doi.org/10.1145/3313867) A. Grimmer, M. Hamidović, W. Haselmayr, and R. Wille. Advanced Simulation of Droplet Microfluidics. ACM Journal on Emerging Technologies in Computing Systems (JETC), 2019.

[[5]](https://doi.org/10.1016/j.simpa.2022.100440) G. Fink, F. Costamoling, and R. Wille. MMFT Droplet Simulator: Efficient Simulation of Droplet-based Microfluidic Devices. Software Impacts, 2022.

## How to cite
If you use this library in your research, depending on which simulator you are using, we would appreciate it if you would cite the original work of the corresponding simulator in your work.