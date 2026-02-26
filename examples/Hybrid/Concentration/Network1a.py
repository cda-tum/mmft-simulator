from mmft.simulator import *

# Continuous Hybrid
def hybridConcentration():

    network = createNetwork()

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
    c0 = network.addRectangularChannel(n0, n1, 1e-4, 1e-4, ChannelType.normal)
    c1 = network.addRectangularChannel(n0, n2, 1e-4, 1e-4, ChannelType.normal)
    c2 = network.addRectangularChannel(n0, n3, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n1, n4, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n2, n5, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n3, n6, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n4, n7, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n6, n8, 1e-4, 1e-4, ChannelType.normal)
    network.addRectangularChannel(n9, n10, 1e-4, 1e-4, ChannelType.normal)

    # Openings
    Opening1 = Opening(n5, [1.0, 0.0], 1e-4)
    Opening2 = Opening(n7, [0.0, -1.0], 1e-4)
    Opening3 = Opening(n8, [0.0, 1.0], 1e-4)
    Opening4 = Opening(n9, [-1.0, 0.0], 1e-4);

    # Module
    position = [1.75e-3, 0.75e-3]
    size = [5e-4, 5e-4]
    stlFile = "../../STL/cross.stl"
    openings = {n5.getId(): Opening1, 
                n7.getId(): Opening2, 
                n8.getId(): Opening3, 
                n9.getId(): Opening4}
    m0 = network.addCfdModule(position, size, stlFile, openings)

    # Pressure Pumps
    network.setPressurePump(c0.getId(), 1e3)
    network.setPressurePump(c1.getId(), 1e3)
    network.setPressurePump(c2.getId(), 1e3)

    # Simulation
    simulation = HybridConcentration(network)

    # Fluid & Resistance Model
    f0 = simulation.addFluid(1e-3, 1e3)
    simulation.setContinuousPhase(f0)
    simulation.setPoiseuilleResistanceModel()

    simulation.setDiffusiveMixingModel()

    s1 = simulation.addSpecie(1e-9, 2.0, 1.0)

    m1 = simulation.addMixture(s1, 2.0)
    m2 = simulation.addMixture(s1, 1.0)
    simulation.addMixtureInjection(m1, c0, 0.0, 1.0)
    simulation.addMixtureInjection(m2, c1, 0.0, 1.0)
    simulation.addMixtureInjection(m2, c2, 0.0, 1.0)

    simulation.addLbmSimulator(m0, 20, 1e-1, 0.55, 0.55, 1e-4, 1e-1, "1a")

    simulation.setNaiveHybridScheme(0.1, 0.5, 10)

    simulation.simulate()

    # Results
    result = simulation.getResults()
    result.printLastState()

def main():
    hybridConcentration()

if __name__ == "__main__":
    main()