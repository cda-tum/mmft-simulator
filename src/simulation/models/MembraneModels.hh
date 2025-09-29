#include "MembraneModels.h"

namespace sim {

template<typename T>
MembraneModel<T>::MembraneModel() { }

//=========================================================================================
//=============================  permeability Membrane  ===================================
//=========================================================================================

// template<typename T>
// PermeabilityMembraneModel<T>::PermeabilityMembraneModel() : MembraneModel<T>() { }


//=========================================================================================
//============================  poreResistance Membrane  ==================================
//=========================================================================================

// template<typename T>
// PoreResistanceMembraneModel<T>::PoreResistanceMembraneModel() : MembraneModel<T>() { }

/** TODO: Miscellaneous
 * Fix Species and Mixture inconsistency
 */
// Ishahak2020
// template<typename T>
// MembraneModel0<T>::MembraneModel0() = default;

// template<typename T>
// T MembraneModel0<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
//     return getPoreResistance(membrane, specie) / (area * membrane->getPorosity());
// }

// template<typename T>
// T MembraneModel0<T>::getPermeabilityParameter(const arch::Membrane<T>* const membrane) const {
//     return (M_PI * membrane->getPorosity() * pow(membrane->getPoreRadius(), 4)) / 8;
// }

// template<typename T>
// T MembraneModel0<T>::getPoreResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture) const {
//     return (8 * mixture->getViscosity() * membrane->getHeight()) / (M_PI * pow(membrane->getPoreRadius(), 4));
// }

// VanDersarl2011
template<typename T>
MembraneModel1<T>::MembraneModel1(T deviceAdjustment) : deviceAdjustment(deviceAdjustment) {}

template<typename T>
T MembraneModel1<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    return deviceAdjustment * getPoreDensityDependentResistance(membrane, area, diffusionCoefficient) + (getPoreResistance(membrane, diffusionCoefficient) + getPoreExitResistance(membrane, diffusionCoefficient)) / membrane->getNumberOfPores(area);
}

template<typename T>
T MembraneModel1<T>::getPoreExitResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return membrane->getHeight() / (diffusionCoefficient * M_PI * pow(membrane->getPoreRadius(), 2));
}

template<typename T>
T MembraneModel1<T>::getPoreResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (4 * diffusionCoefficient * membrane->getPoreRadius());
}

template<typename T>
T MembraneModel1<T>::getPoreDensityDependentResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T area, T diffusionCoefficient) const {
    T proportionalityConstant = 5.3;
    return proportionalityConstant / (diffusionCoefficient * area / membrane->getWidth() * M_PI);
}

// Snyder2011
template<typename T>
MembraneModel2<T>::MembraneModel2() {}

template<typename T>
T MembraneModel2<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    T moleculeToPoreRadius = (mixture->getLargestMolecularSize() / 2) / (membrane->getPoreRadius());
    T reducedDiffusionCoefficient = (-2.81903 * moleculeToPoreRadius + 0.270788 * moleculeToPoreRadius + 1.1015 * moleculeToPoreRadius - 0.435933 * moleculeToPoreRadius) * diffusionCoefficient;
    return 1 / (1 / getPoreDiscoveryResistance(membrane, area, diffusionCoefficient) + 1 / getTransmembraneResistance(membrane, reducedDiffusionCoefficient, area));
}

template<typename T>
T MembraneModel2<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T area, T freeDiffusionCoefficient) const {
    return (2 * freeDiffusionCoefficient / membrane->getArea()) * membrane->getPoreRadius() * membrane->getNumberOfPores(area);
}

template<typename T>
T MembraneModel2<T>::getTransmembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T reducedDiffusionCoefficient, T area) const {
    return 1 / (area * membrane->getHeight()) * membrane->getNumberOfPores(area) * reducedDiffusionCoefficient * M_PI * pow(membrane->getPoreRadius(), 2);
}

// Berg1993, assume sphere
template<typename T>
MembraneModel3<T>::MembraneModel3() {}

template<typename T>
T MembraneModel3<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    return getPorePassageResistance(membrane, diffusionCoefficient, area) + getPoreDiscoveryResistance(membrane, diffusionCoefficient) / membrane->getNumberOfPores(area);
}

template<typename T>
T MembraneModel3<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (4 * membrane->getPoreRadius() * diffusionCoefficient);
}

template<typename T>
T MembraneModel3<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient, T area) const {
    return 1 / (4 * M_PI * (std::sqrt(area / M_PI)) * diffusionCoefficient);
}

// Chung1975
template<typename T>
MembraneModel4<T>::MembraneModel4() {}

template<typename T>
T MembraneModel4<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    return (getPoreDiscoveryResistance(membrane, diffusionCoefficient) + getPorePassageResistance(membrane, diffusionCoefficient)) / membrane->getNumberOfPores(area);
}

template<typename T>
T MembraneModel4<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (4 * membrane->getPoreRadius() * diffusionCoefficient);
}

template<typename T>
T MembraneModel4<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return membrane->getHeight() / (4 * M_PI * std::pow(membrane->getPoreRadius(), 2) * diffusionCoefficient);
}

// Chung1975 Appendix 1 Calculations
template<typename T>
MembraneModel5<T>::MembraneModel5() {}

template<typename T>
T MembraneModel5<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    return (getPoreDiscoveryResistance(membrane, diffusionCoefficient) + getPorePassageResistance(membrane, diffusionCoefficient)) / membrane->getNumberOfPores(area);
}

template<typename T>
T MembraneModel5<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (4 * membrane->getPoreRadius() * 2 * diffusionCoefficient);
}

template<typename T>
T MembraneModel5<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return membrane->getHeight() / (4 * M_PI * std::pow(membrane->getPoreRadius() * 2, 2) * diffusionCoefficient);
}

// Ronaldson-Bouchard 2022
template<typename T>
MembraneModel6<T>::MembraneModel6() {}

template<typename T>
T MembraneModel6<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    T diffusionCoefficient = specie->getDiffusivity();
    return (0.5 * membrane->getRectangularChannel()->getHeight() / diffusionCoefficient + membrane->getHeight() / diffusionFactorMembrane + membrane->getTank()->getHeight() * 0.5 / diffusionCoefficient);
}

// Berg1993, permeability increases twice as fast for small number of N
template<typename T>
MembraneModel7<T>::MembraneModel7() {}

template<typename T>
T MembraneModel7<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    auto diffusionCoefficient = specie->getDiffusivity();
    return 0.5 * (getPoreDiscoveryResistance(membrane, diffusionCoefficient) / membrane->getNumberOfPores(area) + getPorePassageResistance(membrane, area, diffusionCoefficient));
}

template<typename T>
T MembraneModel7<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (2 * membrane->getPoreRadius() * diffusionCoefficient);
}

template<typename T>
T MembraneModel7<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T area, T diffusionCoefficient) const {
    return membrane->getHeight() / (diffusionCoefficient * area);
}

// Berg1993, planar barrier
template<typename T>
MembraneModel8<T>::MembraneModel8() {}

template<typename T>
T MembraneModel8<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    auto diffusionCoefficient = specie->getDiffusivity();
    return (getPoreDiscoveryResistance(membrane, diffusionCoefficient) / membrane->getNumberOfPores(area) + getPorePassageResistance(membrane, area, diffusionCoefficient));
}

template<typename T>
T MembraneModel8<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (2 * membrane->getPoreRadius() * diffusionCoefficient);
}

template<typename T>
T MembraneModel8<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T area, T diffusionCoefficient) const {
    return membrane->getHeight() / (diffusionCoefficient * area);
}

//Berg1993, planar barrier with disc-absorber
template<typename T>
MembraneModel9<T>::MembraneModel9() {}

template<typename T>
T MembraneModel9<T>::getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const {
    auto diffusionCoefficient = specie->getDiffusivity();
    return (getPoreDiscoveryResistance(membrane, diffusionCoefficient) / membrane->getNumberOfPores(area) + getPorePassageResistance(membrane, area, diffusionCoefficient));
}

template<typename T>
T MembraneModel9<T>::getPoreDiscoveryResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T diffusionCoefficient) const {
    return 1 / (4 * membrane->getPoreRadius() * diffusionCoefficient);
}

template<typename T>
T MembraneModel9<T>::getPorePassageResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, T area, T diffusionCoefficient) const {
    return membrane->getHeight() / (diffusionCoefficient * area);
}
}   /// namespace sim
