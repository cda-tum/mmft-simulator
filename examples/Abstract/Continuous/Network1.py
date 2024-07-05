from mmft.simulator import *

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

    simulation.saveResult("continuousAbstract.JSON")

def main():
    abstractContinuous()

if __name__ == "__main__":
    main()