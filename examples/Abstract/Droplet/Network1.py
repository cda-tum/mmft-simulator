from mmft.simulator import *

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

    simulation.saveResult("dropletAbstract.JSON")

def main():
    abstractDroplet()

if __name__ == "__main__":
    main()