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

|               | Abstract      | Hybrid        | CFD           |
| :-----------  | :-----------: | :-----------: | :-----------: |
| Continuous    | &#x2705;      | &#x2705;      | &#x2705;      |
| Concentration | &#x2705;      |               |               |
| BigDroplet    | &#x2705;      |               |               |
| Membrane      | &#x2705;      |               |               |

</div>

### Abstraction Levels
**Abstract**: This abstraction level highly abstracts the underlying physics to an extend that it can, in most cases, provide simulation results almost instantly. It uses a method that draws an analogy between the <br>Hagen-Poiseuille law and Ohm's law, and applies analogous methods from electrical circuit engineering to <br>channel-based microfluidic devices.<br>
**Hybrid**: The Hybrid exploits simulation methods on a high level of abstraction to accelerate CFD simulations (using the LBM as implemented in the [OpenLB](https://www.openlb.net/) library) of microfluidic devices. More details can be found in the corresponding publication.<sup>[[2]](https://doi.org/10.3390/mi15010129)</sup> <br>
**CFD**: The CFD simulation simulates the microfluidic device on a low level of abstraction. This generally provides accurate results in the entire simulation domain, but is computationally expensive. The defined network is automatically converted into an STL geometry, using the [MMFT-STL-Generator](https://github.com/cda-tum/mmft-STL-generator). Alternatively, an STL file can be loaded for the CFD simulation. The [OpenLB](https://www.openlb.net/) library is used as back-end for the CFD simulation.

### Platforms
**Continuous**: The continuous platform is considered the default platform in the MMFT Simulator and describes the fluid dynamics for channel-based pressure-driven microfluidic flow. <sup>[[3]](https://doi.org/10.1039/C2LC20799K)</sup> <br>
**Concentration**: Solvents play an important role in microfluidics and on this platform, concentrations of species dissolved in the continuous phase can be simulated. There are currently **two** mixing models available:
* Instantaneous mixing model: Fluids are assumed to be fully mixed as soon as they meet.
* Diffusive mixing model: Concentration distributions of solvents are tracked and propagated according to the network topology, following an abstracted advection-diffusion equation&mdash;as described in the respective publication. <sup>[[4]](https://doi.org/10.1109/TCAD.2025.3549703)</sup> <br>

**BigDroplet**: In this platform, big droplets are considered in addition to the continuous platform. Big droplets are here described as an immiscible fluid immersed in the carrier fluid that acts as the continuous phase and are assumed to fill the entire cross-section of each channel (hence the terminology "big droplet"), generally generated in the squeezing regime. For more details, please see the respective publications. <sup>[[5]](https://doi.org/10.1145/3313867)</sup> <sup>[[6]](https://doi.org/10.1016/j.simpa.2022.100440)</sup> <br>
**Membrane**: Same as the Mixing platform, but supports tanks (e.g. for organs-on-chips designs) and membranes (connect tank to channel to allow diffusive mixture exchange between the two) and requires the instantaneous mixing model. <sup>[[7]](https://doi.org/10.1038/s41598-024-77741-8)</sup>


## System Requirements

The implementation is compatible with GCC version 13.3 or newer and a minimum CMake version 3.31.6. The python package requires Python version 3.8 or newer. The package is currently tested for Linux distributions and MacOS.

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

To use this library in python, install the python package in a virtual environment in your working directory
```python
python3 -m venv .venv
source .venv/bin/activate
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

    // define network
    auto network = arch::Network<T>::createNetwork();

    // define simulation
    sim::AbstractContinuous<T> testSimulation(network);

    // nodes
    auto node0 = network->addNode(0.0, 0.0, true);
    auto node1 = network->addNode(1e-3, 2e-3, false);
    auto node2 = network->addNode(1e-3, 1e-3, false);
    auto node3 = network->addNode(1e-3, 0.0, false);
    auto node4 = network->addNode(2e-3, 2e-3, false);
    auto node5 = network->addNode(2e-3, 1e-3, false);
    auto node6 = network->addNode(2e-3, 0.0, false);
    auto node7 = network->addNode(3e-3, 1e-3, true);

    // pressure pump
    auto pressure = 1e3;
    auto pump0 = network->addPressurePump(node0->getId(), node1->getId(), pressure);
    auto pump1 = network->addPressurePump(node0->getId(), node2->getId(), pressure);
    auto pump2 = network->addPressurePump(node0->getId(), node3->getId(), pressure);

    // channels
    auto cWidth = 100e-6;
    auto cHeight = 100e-6;
    auto cLength = 1000e-6;

    auto c1 = network->addRectangularChannel(node1->getId(), node4->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c2 = network->addRectangularChannel(node2->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c3 = network->addRectangularChannel(node3->getId(), node6->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c4 = network->addRectangularChannel(node4->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c5 = network->addRectangularChannel(node6->getId(), node5->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c6 = network->addRectangularChannel(node5->getId(), node7->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    // fluids
    auto fluid0 = testSimulation.addFluid(1e-3, 997.0);
    //--- continuousPhase ---
    testSimulation.setContinuousPhase(fluid0->getId());

    // Set the resistance model
    testSimulation.set1DResistanceModel();

    // simulate
    testSimulation.simulate();

    // results
    auto result = testSimulation.getResults();
    result->printLastState();

    return 0;
}
```

### Python

```python
from mmft import simulator

network = createNetwork()

# Nodes
n0 = network.addNode(0.0, 0.0, True)
n1 = network.addNode(1e-3, 2e-3, False)
n2 = network.addNode(1e-3, 1e-3, False)
n3 = network.addNode(1e-3, 0.0, False)
n4 = network.addNode(2e-3, 2e-3, False)
n5 = network.addNode(2e-3, 1e-3, False)
n6 = network.addNode(2e-3, 0.0, False)
n7 = network.addNode(3e-3, 1e-3, True)

# Channels
network.addPressurePump(n0, n1, 1e3)
network.addPressurePump(n0, n2, 1e3)
network.addPressurePump(n0, n3, 1e3)
network.addRectangularChannel(n1, n4, 1e-4, 1e-4, ChannelType.normal)
network.addRectangularChannel(n2, n5, 1e-4, 1e-4, ChannelType.normal)
network.addRectangularChannel(n3, n6, 1e-4, 1e-4, ChannelType.normal)
network.addRectangularChannel(n4, n5, 1e-4, 1e-4, ChannelType.normal)
network.addRectangularChannel(n6, n5, 1e-4, 1e-4, ChannelType.normal)
network.addRectangularChannel(n5, n7, 1e-4, 1e-4, ChannelType.normal)

# Simulation
simulation = AbstractContinuous(network)

# Fluid & Resistance Model
f0 = simulation.addFluid(1e-3, 997)
simulation.setContinuousPhase(f0)
simulation.set1DResistanceModel()

simulation.simulate()

# Results
results = simulation.getResults()
results.printLastState()
```

### JSON Definitions

The network and simulation objects can also be defined using a JSON file and loaded into the MMFT-Simulator:

```cpp
    // Load and set the network from a JSON file
    std::cout << "[Main] Create network object..." << std::endl;
    std::shared_ptr<arch::Network<T>> network = porting::networkFromJSON<T>("path/to/network.json");

    // Load and set the simulation from a JSON file
    std::cout << "[Main] Create simulation object..." << std::endl;
    std::unique_ptr<sim::Simulation<T>> simulation = porting::simulationFromJSON<T>("path/to/simulation.json", network);

    // Store the simulation result in a JSON file
    porting::resultToJSON("path/to/results.json", simulation);
```

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

## Related Simulators

### [mmft-droplet-simulator (deprecated)](https://github.com/cda-tum/mmft-droplet-simulator)

A microfluidic simulator that only focuses on the simulation of droplets. Its functionality has been integrated in the mmft-simulator.

### [mmft-modular-1D-simulator](https://github.com/cda-tum/mmft-modular-1D-simulator)

With the `mmft-droplet-simulator` as its starting point, this simulator adds these features to its basis:

* instantaneous mixture simulation
* membranes & tanks
* multiple channel resistance models

This simulator is the core of [this work<sup>[7]</sup>](https://doi.org/10.1038/s41598-024-77741-8) and the listed features have been integrated into the `mmft-simulator`. As the integration process is still ongoing, following features are not yet integrated in the `mmft-simulator` and the `mmft-modular-1D-simulator` should be used if they are relevant for a simulation:

* simulations with both membranes/tanks and droplets not validated
* multiple channel resistance models

#### Limitations

While many of the features of the `mmft-modular-1D-simulator` were integrated already, the unification process is ongoing.
The following limitations are still present in `mmft-simulator` and the `mmft-modular-1D-simulator` should be used if they are relevant for a simulation:

* not fully validated membrane/tank implementation for complex networks
* simulations with both membranes/tanks and droplets not validated

## References
More details about the implementation and the mechanisms behind the MMFT Simulator can be found in: 

[[1]](https://doi.org/10.3390/s22145392) M. Takken, and R. Wille. Simulation of Pressure-Driven and Channel-based Microfluidics on Different Abstract Levels: A Case Study. MDPI Sensors, 2022.

[[2]](https://doi.org/10.3390/mi15010129) M. Takken, and R. Wille. Accelerated Computational Fluid Dynamics Simulations of Microfluidic Devices by Exploiting Higher Levels of Abstraction. MDPI Micromachines, 2024.

[[3]](https://doi.org/10.1039/C2LC20799K) K. W. Oh, K. Lee, B. Ahn, and E. P. Furlani. Design of pressure-driven microfluidic networks using electric circuit analogy. Lab on a Chip 2012.

[[4]](https://doi.org/10.1109/TCAD.2025.3549703) M. Takken, M. Emmerich, and R. Wille. An Abstract Simulator for Species Concentrations in Channel-Based Microfluidic Devices. IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems (TCAD), 2025.

[[5]](https://doi.org/10.1145/3313867) A. Grimmer, M. Hamidović, W. Haselmayr, and R. Wille. Advanced Simulation of Droplet Microfluidics. ACM Journal on Emerging Technologies in Computing Systems (JETC), 2019.

[[6]](https://doi.org/10.1016/j.simpa.2022.100440) G. Fink, F. Costamoling, and R. Wille. MMFT Droplet Simulator: Efficient Simulation of Droplet-based Microfluidic Devices. Software Impacts, 2022.

[[7]](https://doi.org/10.1038/s41598-024-77741-8) M. Emmerich, F. Costamoling, and R. Wille. Modular and Extendable 1D-Simulator for Microfluidic Devices. Scientific Reports, 2024.

## How to cite
If you use this library in your research, depending on which simulator you are using, we would appreciate it if you would cite the original work of the corresponding simulator in your work.
