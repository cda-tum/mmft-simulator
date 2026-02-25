from mmft.simulator import *

# Continuous CFD
def cfdContinuous():

    network = createNetwork()

    # Nodes
    n1 = network.addNode(1e-3, 2e-3, True)
    n2 = network.addNode(1e-3, 1e-3, True)
    n3 = network.addNode(1e-3, 0.0, True)
    n4 = network.addNode(2e-3, 2e-3, False)
    n5 = network.addNode(2e-3, 1e-3, False)
    n6 = network.addNode(2e-3, 0.0, False)
    n7 = network.addNode(3e-3, 1e-3, True)

    # Channels
    network.addRectangularChannel(n1, n4, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n2, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n3, n6, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n4, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n6, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n5, n7, 1e-4, 1e-4, ChannelType.normal)

    # Simulation
    simulation = CfdContinuous(network)

    # Fluid & Resistance Model
    f0 = simulation.addFluid(1e-3, 1e3)
    simulation.setContinuousPhase(f0)
    
    # Define Boundary Conditions
    simulation.addPressureBC(n1, 1e2)
    simulation.addPressureBC(n2, 1e2)
    simulation.addPressureBC(n3, 1e2)
    simulation.addPressureBC(n7, 0.0)

    simulation.simulate()

def main():
    cfdContinuous()

if __name__ == "__main__":
    main()