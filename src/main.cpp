#include <iostream>

#include <baseSimulator.h>
#include <baseSimulator.hh>

using T = double;

int main(int argc, char const* argv []) {

    // Define Network
    arch::Network<T> network;

    // nodes
    auto n_01 = network.addNode(0, 0, false);
    auto n_02 = network.addNode(0, 0, false);
    auto n_03 = network.addNode(0, 0, false);

    auto n_11 = network.addNode(0, 0, false);
    auto n_12 = network.addNode(0, 0, false);
    auto n_13 = network.addNode(0, 0, false);
    auto n_14 = network.addNode(0, 0, false);
    auto n_15 = network.addNode(0, 0, false);

    auto n_21 = network.addNode(0, 0, false);
    auto n_22 = network.addNode(0, 0, false);
    auto n_23 = network.addNode(0, 0, false);
    auto n_24 = network.addNode(0, 0, false);
    auto n_25 = network.addNode(0, 0, false);
    auto n_26 = network.addNode(0, 0, false);
    auto n_27 = network.addNode(0, 0, false);

    auto n_31 = network.addNode(0, 0, false);
    auto n_32 = network.addNode(0, 0, false);
    auto n_33 = network.addNode(0, 0, false);
    auto n_34 = network.addNode(0, 0, false);
    auto n_35 = network.addNode(0, 0, false);
    auto n_36 = network.addNode(0, 0, false);
    auto n_37 = network.addNode(0, 0, false);
    auto n_38 = network.addNode(0, 0, false);
    auto n_39 = network.addNode(0, 0, false);

    auto n_41 = network.addNode(0, 0, false);
    auto n_42 = network.addNode(0, 0, false);
    auto n_43 = network.addNode(0, 0, false);
    auto n_44 = network.addNode(0, 0, false);
    auto n_45 = network.addNode(0, 0, false);
    auto n_46 = network.addNode(0, 0, false);
    auto n_47 = network.addNode(0, 0, false);
    auto n_48 = network.addNode(0, 0, false);
    auto n_49 = network.addNode(0, 0, false);
    auto n_410 = network.addNode(0, 0, false);
    auto n_411 = network.addNode(0, 0, false);

    auto n_51 = network.addNode(0, 0, false);
    auto n_52 = network.addNode(0, 0, false);
    auto n_53 = network.addNode(0, 0, false);
    auto n_54 = network.addNode(0, 0, false);
    auto n_55 = network.addNode(0, 0, false);
    auto n_56 = network.addNode(0, 0, false);
    auto n_57 = network.addNode(0, 0, false);
    auto n_58 = network.addNode(0, 0, false);
    auto n_59 = network.addNode(0, 0, false);
    auto n_510 = network.addNode(0, 0, false);
    auto n_511 = network.addNode(0, 0, false);
    auto n_512 = network.addNode(0, 0, false);
    auto n_513 = network.addNode(0, 0, false);
    
    auto n_61 = network.addNode(0, 0, false);
    auto n_62 = network.addNode(0, 0, false);
    auto n_63 = network.addNode(0, 0, false);
    auto n_64 = network.addNode(0, 0, false);
    auto n_65 = network.addNode(0, 0, false);
    auto n_66 = network.addNode(0, 0, false);
    auto n_67 = network.addNode(0, 0, false);
    auto n_68 = network.addNode(0, 0, false);
    auto n_69 = network.addNode(0, 0, false);
    auto n_610 = network.addNode(0, 0, false);
    auto n_611 = network.addNode(0, 0, false);
    auto n_612 = network.addNode(0, 0, false);
    auto n_613 = network.addNode(0, 0, false);
    auto n_614 = network.addNode(0, 0, false);
    auto n_615 = network.addNode(0, 0, false);

    auto n_71 = network.addNode(0, 0, false);
    auto n_72 = network.addNode(0, 0, false);
    auto n_73 = network.addNode(0, 0, false);

    auto n_81 = network.addNode(0, 0, false);

    auto n_91 = network.addNode(0, 0, true);

    auto n_1001 = network.addNode(0, 0, true);
    auto n_1002 = network.addNode(0, 0, true);
    auto n_1003 = network.addNode(0, 0, true);

    // channels
    auto cWidth = 50e-6;
    auto cHeight = 100e-6;
    auto mLength = 10e-3;   // Meander length
    auto cLength = 570e-6;  // Channel length

    auto c_01 = network.addChannel(n_01->getId(), n_11->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_02 = network.addChannel(n_02->getId(), n_12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_03 = network.addChannel(n_03->getId(), n_13->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_11 = network.addChannel(n_11->getId(), n_12->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_12 = network.addChannel(n_12->getId(), n_13->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_13 = network.addChannel(n_13->getId(), n_14->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_14 = network.addChannel(n_14->getId(), n_15->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_21 = network.addChannel(n_11->getId(), n_21->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);
    auto c_22 = network.addChannel(n_12->getId(), n_23->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_23 = network.addChannel(n_14->getId(), n_25->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_24 = network.addChannel(n_15->getId(), n_27->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);

    auto c_31 = network.addChannel(n_21->getId(), n_22->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_32 = network.addChannel(n_22->getId(), n_23->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_33 = network.addChannel(n_23->getId(), n_24->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_34 = network.addChannel(n_24->getId(), n_25->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_35 = network.addChannel(n_25->getId(), n_26->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_36 = network.addChannel(n_26->getId(), n_27->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_41 = network.addChannel(n_21->getId(), n_31->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);
    auto c_42 = network.addChannel(n_22->getId(), n_33->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_43 = network.addChannel(n_24->getId(), n_35->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_44 = network.addChannel(n_26->getId(), n_37->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_45 = network.addChannel(n_27->getId(), n_39->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);

    auto c_51 = network.addChannel(n_31->getId(), n_32->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_52 = network.addChannel(n_32->getId(), n_33->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_53 = network.addChannel(n_33->getId(), n_34->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_54 = network.addChannel(n_34->getId(), n_35->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_55 = network.addChannel(n_35->getId(), n_36->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_56 = network.addChannel(n_36->getId(), n_37->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_57 = network.addChannel(n_37->getId(), n_38->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_58 = network.addChannel(n_38->getId(), n_39->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_61 = network.addChannel(n_31->getId(), n_41->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);
    auto c_62 = network.addChannel(n_32->getId(), n_43->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_63 = network.addChannel(n_34->getId(), n_45->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_64 = network.addChannel(n_36->getId(), n_47->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_65 = network.addChannel(n_38->getId(), n_49->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_66 = network.addChannel(n_39->getId(), n_411->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);

    auto c_71 = network.addChannel(n_41->getId(), n_42->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_72 = network.addChannel(n_42->getId(), n_43->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_73 = network.addChannel(n_43->getId(), n_44->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_74 = network.addChannel(n_44->getId(), n_45->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_75 = network.addChannel(n_45->getId(), n_46->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_76 = network.addChannel(n_46->getId(), n_47->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_77 = network.addChannel(n_47->getId(), n_48->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_78 = network.addChannel(n_48->getId(), n_49->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_79 = network.addChannel(n_49->getId(), n_410->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_710 = network.addChannel(n_410->getId(), n_411->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_81 = network.addChannel(n_41->getId(), n_51->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);
    auto c_82 = network.addChannel(n_42->getId(), n_53->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_83 = network.addChannel(n_44->getId(), n_55->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_84 = network.addChannel(n_46->getId(), n_57->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_85 = network.addChannel(n_48->getId(), n_59->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_86 = network.addChannel(n_410->getId(), n_511->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_87 = network.addChannel(n_411->getId(), n_513->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);

    auto c_91 = network.addChannel(n_51->getId(), n_52->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_92 = network.addChannel(n_52->getId(), n_53->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_93 = network.addChannel(n_53->getId(), n_54->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_94 = network.addChannel(n_54->getId(), n_55->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_95 = network.addChannel(n_55->getId(), n_56->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_96 = network.addChannel(n_56->getId(), n_57->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_97 = network.addChannel(n_57->getId(), n_58->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_98 = network.addChannel(n_58->getId(), n_59->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_99 = network.addChannel(n_59->getId(), n_510->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_910 = network.addChannel(n_510->getId(), n_511->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_911 = network.addChannel(n_511->getId(), n_512->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_912 = network.addChannel(n_512->getId(), n_513->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_101 = network.addChannel(n_51->getId(), n_61->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);
    auto c_102 = network.addChannel(n_52->getId(), n_63->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_103 = network.addChannel(n_54->getId(), n_65->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_104 = network.addChannel(n_56->getId(), n_67->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_105 = network.addChannel(n_58->getId(), n_69->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_106 = network.addChannel(n_510->getId(), n_611->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_107 = network.addChannel(n_512->getId(), n_613->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_108 = network.addChannel(n_513->getId(), n_615->getId(), cHeight, cWidth, cLength+mLength, arch::ChannelType::NORMAL);

    auto c_111 = network.addChannel(n_61->getId(), n_62->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_112 = network.addChannel(n_62->getId(), n_63->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_113 = network.addChannel(n_63->getId(), n_64->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_114 = network.addChannel(n_64->getId(), n_65->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_115 = network.addChannel(n_65->getId(), n_66->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_116 = network.addChannel(n_66->getId(), n_67->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_117 = network.addChannel(n_67->getId(), n_68->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_118 = network.addChannel(n_68->getId(), n_69->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_119 = network.addChannel(n_69->getId(), n_610->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_1110 = network.addChannel(n_610->getId(), n_611->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_1111 = network.addChannel(n_611->getId(), n_612->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_1112 = network.addChannel(n_612->getId(), n_613->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_1113 = network.addChannel(n_613->getId(), n_614->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);
    auto c_1114 = network.addChannel(n_614->getId(), n_615->getId(), cHeight, cWidth, cLength, arch::ChannelType::NORMAL);

    auto c_121 = network.addChannel(n_61->getId(), n_71->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_122 = network.addChannel(n_62->getId(), n_71->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_123 = network.addChannel(n_64->getId(), n_71->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_124 = network.addChannel(n_66->getId(), n_72->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_125 = network.addChannel(n_68->getId(), n_72->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_126 = network.addChannel(n_610->getId(), n_72->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_127 = network.addChannel(n_612->getId(), n_73->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_128 = network.addChannel(n_614->getId(), n_73->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    auto c_129 = network.addChannel(n_615->getId(), n_73->getId(), cHeight, cWidth, mLength, arch::ChannelType::NORMAL);
    
    auto c_131 = network.addChannel(n_71->getId(), n_81->getId(), cHeight, 3*cWidth, 1e-6, arch::ChannelType::NORMAL);
    auto c_132 = network.addChannel(n_72->getId(), n_81->getId(), cHeight, 3*cWidth, 1e-6, arch::ChannelType::NORMAL);
    auto c_133 = network.addChannel(n_73->getId(), n_81->getId(), cHeight, 3*cWidth, 1e-6, arch::ChannelType::NORMAL);

    auto c_141 = network.addChannel(n_81->getId(), n_91->getId(), cHeight, 900e-6, 500e-6, arch::ChannelType::NORMAL);

    // flowRate pump
    auto flowRate = 5e-12;   // 1 mm/s
    auto p_1 = network.addFlowRatePump(n_1001->getId(), n_01->getId(), flowRate);
    auto p_2 = network.addFlowRatePump(n_1002->getId(), n_02->getId(), flowRate);
    auto p_3 = network.addFlowRatePump(n_1003->getId(), n_03->getId(), flowRate);

    // Define simulations
    sim::Simulation<T> sim;
    sim.setType(sim::Type::_1D);
    sim.setPlatform(sim::Platform::MIXING);
    sim.setNetwork(&network);

    // fluids   
    auto f_0 = sim.addFluid(1e-3, 1e3, 1.0);
    sim.setContinuousPhase(f_0->getId());

    // Define and set the resistance model
    sim::ResistanceModel1D<T> resistanceModel = sim::ResistanceModel1D<T>(sim.getContinuousPhase()->getViscosity());
    sim.setResistanceModel(&resistanceModel);

    // species
    auto s_1 = sim.addSpecie(5e-9, 1.0);

    // mixture and injection
    std::unordered_map<int, sim::Specie<T>*> species;
    std::unordered_map<int, T> concentrations;
    species.try_emplace(s_1->getId(), s_1);
    concentrations.try_emplace(s_1->getId(), 1.0);
    auto m_0 = sim.addDiffusiveMixture(species, concentrations);
    sim.addMixtureInjection(m_0->getId(), c_02->getId(), 0.0);

    // Define and set the mixing model
    sim::DiffusionMixingModel<T> diffMixingModel;
    sim.setMixingModel(&diffMixingModel);
    sim.diffusiveMixing = true;

    // Check if network is valid
    network.isNetworkValid();
    network.sortGroups();

    // simulate
    sim.simulate();

    // results
    result::SimulationResult<T>* result = sim.getSimulationResults();
    result->printLastState();

    result->writeDiffusiveMixtures(565);

    return 0;
}