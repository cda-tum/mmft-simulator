from mmft.simulator import *

# Droplet Abstract
def abstractDroplet():

    network = createNetwork()

    # Nodes
    n0 = network.addNode(0.0, 0.0, False)
    n1 = network.addNode(1e-3, 0.0, False)
    n2 = network.addNode(2e-3, 0.0, False)
    n3 = network.addNode(2.5e-3, 0.86602540378e-3, False)
    n4 = network.addNode(3e-3, 0.0, False)
    n5 = network.addNode(4e-3, 0.0, True, True)

    # Channels
    c0 = network.addRectangularChannel(n0, n1, 1e-4, 3e-5, ChannelType.normal)
    network.addRectangularChannel(n1, n2, 1e-4, 3e-5, ChannelType.normal)
    network.addRectangularChannel(n2, n3, 1e-4, 3e-5, ChannelType.normal)
    network.addRectangularChannel(n2, n4, 1e-4, 3e-5, ChannelType.normal)
    network.addRectangularChannel(n3, n4, 1e-4, 3e-5, ChannelType.normal)
    network.addRectangularChannel(n4, n5, 1e-4, 3e-5, ChannelType.normal)
    network.addFlowRatePump(n5, n0, 3e-11)

    # Simulation
    simulation = AbstractDroplet(network)

    # Fluid & Resistance Model
    water = simulation.addFluid(1e-3, 1e3)
    oil = simulation.addFluid(3e-3, 1e3)
    simulation.setContinuousPhase(water)
    simulation.set1DResistanceModel()
    d1 = simulation.addDroplet(oil, 4.5e-13)
    simulation.addDropletInjection(d1, 0.0, c0, 0.5)

    simulation.simulate()

    # Results
    result = simulation.getResults()
    result.printLastState()

def main():
    abstractDroplet()

if __name__ == "__main__":
    main()