from pysimulator import *

# Continuous Abstract
def abstractContinuous():

    network = Network()

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
    network.addChannel(n1, n4, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n2, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n3, n6, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n4, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n6, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n5, n7, 1e-4, 1e-4, ChannelType.normal)

    network.sort()
    network.valid()

    simulation = Simulation()
    
    # Simulation meta-data
    simulation.setType(Type.abstract)
    simulation.setPlatform(Platform.continuous)
    simulation.setNetwork(network)

    # Fluid & Resistance Model
    f0 = simulation.addFluid(997, 1e-3, 1.0)
    simulation.setContinuousPhase(f0)
    simulation.setRectangularResistanceModel()

    simulation.simulate()

    simulation.saveResult("contAbst.JSON")

# Droplet Abstract
def abstractDroplet():

    network = Network()

    # Nodes
    n0 = network.addNode(0.0, 0.0, False)
    n1 = network.addNode(1e-3, 0.0, False)
    n2 = network.addNode(2e-3, 0.0, False)
    n3 = network.addNode(2.5e-3, 0.86602540378e-3, False)
    n4 = network.addNode(3e-3, 0.0, False)
    n5 = network.addNode(4e-3, 0.0, True, True)

    # Channels
    c0 = network.addChannel(n0, n1, 1e-4, 3e-5, ChannelType.normal)
    network.addChannel(n1, n2, 1e-4, 3e-5, ChannelType.normal)
    network.addChannel(n2, n3, 1e-4, 3e-5, ChannelType.normal)
    network.addChannel(n2, n4, 1e-4, 3e-5, ChannelType.normal)
    network.addChannel(n3, n4, 1e-4, 3e-5, ChannelType.normal)
    network.addChannel(n4, n5, 1e-4, 3e-5, ChannelType.normal)
    network.addFlowRatePump(n5, n0, 3e-11)

    network.sort()
    network.valid()

    simulation = Simulation()
    
    # Simulation meta-data
    simulation.setType(Type.abstract)
    simulation.setPlatform(Platform.bigDroplet)
    simulation.setNetwork(network)

    # Fluid & Resistance Model
    water = simulation.addFluid(1e3, 1e-3, 1.0)
    oil = simulation.addFluid(1e3, 3e-3, 1.0)
    simulation.setContinuousPhase(water)
    simulation.setRectangularResistanceModel()
    d1 = simulation.addDroplet(oil, 4.5e-13)
    simulation.injectDroplet(d1, 0.0, c0, 0.5)

    simulation.simulate()

    simulation.saveResult("dropletAbst.JSON")

# Continuous Hybrid
def hybridContinuous():

    network = Network()

    # Nodes
    n0 = network.addNode(0.0, 0.0, True)
    n1 = network.addNode(1e-3, 2e-3, False)
    n2 = network.addNode(1e-3, 1e-3, False)
    n3 = network.addNode(1e-3, 0.0, False)
    n4 = network.addNode(2e-3, 2e-3, False)
    n5 = network.addNode(1.75e-3, 1e-3, False)
    n6 = network.addNode(2e-3, 0.0, False)
    n7 = network.addNode(2e-3, 1.25e-3, False)
    n8 = network.addNode(2e-3, 0.75e-3, False)
    n9 = network.addNode(2.25e-3, 1e-3, False)
    n10 = network.addNode(3e-3, 1e-3, True)

    # Channels
    network.addChannel(n0, n1, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n0, n2, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n0, n3, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n1, n4, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n2, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n3, n6, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n4, n7, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n6, n8, 1e-4, 1e-4, ChannelType.normal)
    network.addChannel(n9, n10, 1e-4, 1e-4, ChannelType.normal)

    # Module
    network.addModule("test", "../../../../examples/STL/cross.stl", [1.75e-3, 0.75e-3], [5e-4, 5e-4], [n5, n7, n8, n9], \
                      [[1.0, 0.0], [0.0, -1.0], [0.0, 1.0], [-1.0, 0.0]], [1e-4, 1e-4, 1e-4, 1e-4], \
                        1e-4, 1e-1, 0.1, 20, 1e-1, 0.55)

    # Pressure Pumps
    network.addPressurePump(n0, n1, 1e3)
    network.addPressurePump(n0, n2, 1e3)
    network.addPressurePump(n0, n3, 1e3)

    network.sort()
    network.valid()

    simulation = Simulation()
    
    # Simulation meta-data
    simulation.setType(Type.hybrid)
    simulation.setPlatform(Platform.continuous)
    simulation.setNetwork(network)

    # Fluid & Resistance Model
    f0 = simulation.addFluid(1e3, 1e-3, 1.0)
    simulation.setContinuousPhase(f0)
    simulation.setPoiseuilleResistanceModel()

    simulation.simulate()

    simulation.saveResult("contHybrid.JSON")

def abstractContinuousJSON():
    
    network = Network()
    network.loadNetwork("../../../../examples/Abstract/Continuous/Network1.JSON")

    network.sort()
    network.valid()

    simulation = Simulation()
    simulation.loadSimulation(network, "../../../../examples/Abstract/Continuous/Network1.JSON")

    simulation.simulate()

    simulation.saveResult("contAbstjson.JSON")

def abstractDropletJSON():

    network = Network()
    network.loadNetwork("../../../../examples/Abstract/Droplet/Network1.JSON")

    network.sort()
    network.valid()

    simulation = Simulation()
    simulation.loadSimulation(network, "../../../../examples/Abstract/Droplet/Network1.JSON")

    simulation.simulate()

    simulation.saveResult("dropletAbstjson.JSON")

def hybridContinuousJSON():

    network = Network()
    network.loadNetwork("../../../../examples/Hybrid/Network1a.JSON")

    simulation = Simulation()
    simulation.loadSimulation(network, "../../../../examples/Hybrid/Network1a.JSON")

    network.valid()

    simulation.simulate()

    simulation.saveResult("contHybridjson.JSON")

def main():
    abstractContinuous()
    abstractContinuousJSON()
    abstractDroplet()
    abstractDropletJSON()
    #hybridContinuous()
    hybridContinuousJSON()

if __name__ == "__main__":
    main()