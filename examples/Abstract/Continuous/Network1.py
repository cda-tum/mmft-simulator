from mmft.simulator import *

# Continuous Abstract
def abstractContinuous():

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

    # simulation.saveResult("continuousAbstract.JSON")

def main():
    abstractContinuous()

if __name__ == "__main__":
    main()