from mmft.simulator import *

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
    m0 = network.addModule( [1.75e-3, 0.75e-3], [5e-4, 5e-4], [n5, n7, n8, n9])

    # Pressure Pumps
    network.addPressurePump(n0, n1, 1e3)
    network.addPressurePump(n0, n2, 1e3)
    network.addPressurePump(n0, n3, 1e3)

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

    s1 = simulation.addLbmSimulator("1a", "../STL/cross.stl", m0, [n5, n7, n8, n9], \
                                    [[1.0, 0.0], [0.0, -1.0], [0.0, 1.0], [-1.0, 0.0]], [1e-4, 1e-4, 1e-4, 1e-4], \
                                    1e-4, 1e-1, 20, 1e-1, 0.55)
    
    s1.setNaiveScheme(0.1, 0.5, 10)

    network.sort()

    simulation.simulate()

    simulation.saveResult("continuousHybrid.JSON")

def main():
    hybridContinuous()

if __name__ == "__main__":
    main()