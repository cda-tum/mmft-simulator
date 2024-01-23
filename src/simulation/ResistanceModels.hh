#include "ResistanceModels.h"

namespace sim {

// ### ResistanceModel ###
template<typename T>
ResistanceModel<T>::ResistanceModel(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

// ### ResistanceModel1D ###
template<typename T>
ResistanceModel1D<T>::ResistanceModel1D(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModel1D<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());

    return channel->getLength() * a * this->continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));
}

template<typename T>
T ResistanceModel1D<T>::computeFactorA(T width, T height) const {
    return 12. / (1. - 192. * height * tanh(M_PI * width / (2. * height)) / (pow(M_PI, 5.) * width));
}

template<typename T>
T ResistanceModel1D<T>::getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());
    T dropletLength = volumeInsideChannel / (channel->getWidth() * channel->getHeight());
    T resistance = 3 * dropletLength * a * this->continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));

    if (resistance < 0.0) {
        std::cout << "droplet length: \t" << dropletLength << std::endl;
        std::cout << "volume inside channel: \t" << volumeInsideChannel << std::endl;
        std::cout << "a: \t\t" << a << std::endl;
        std::cout << "continuous phase viscosity: \t" << this->continuousPhaseViscosity << std::endl;
        std::cout << "channel width: \t" << channel->getWidth() << std::endl;
        std::cout << "channel height: \t" << channel->getHeight() << std::endl;
        throw std::invalid_argument("Negative droplet resistance. Droplet " + std::to_string(droplet->getId()) + " has a resistance of " + std::to_string(resistance));
    }

    return resistance;
}

// ### ResistanceModelPoiseuille ###
template<typename T>
ResistanceModelPoiseuille<T>::ResistanceModelPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelPoiseuille<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());

    return channel->getLength() * a * this->continuousPhaseViscosity / (channel->getHeight() * pow(channel->getWidth(), 3));
}

template<typename T>
T ResistanceModelPoiseuille<T>::computeFactorA(T width, T height) const {
    return 12.;
}

template<typename T>
T ResistanceModelPoiseuille<T>::getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}
}  // namespace sim