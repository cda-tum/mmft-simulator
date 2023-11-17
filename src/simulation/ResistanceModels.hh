#include "ResistanceModels.h"

#include <math.h>

#include "../architecture/Channel.h"
#include "../architecture/lbmModule.h"

namespace sim {

// ### ResistanceModel1D ###
template<typename T>
ResistanceModel1D<T>::ResistanceModel1D(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModel1D<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());

    return channel->getLength() * a * continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));
}

template<typename T>
T ResistanceModel1D<T>::computeFactorA(T width, T height) const {
    return 12. / (1. - 192. * height * tanh(M_PI * width / (2. * height)) / (pow(M_PI, 5.) * width));
}

template<typename T>
T ResistanceModel1D<T>::getDropletResistance(arch::Channel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());
    T dropletLength = volumeInsideChannel / (channel->getWidth() * channel->getHeight());

    return 3 * dropletLength * a * continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));
}

// ### ResistanceModelPoiseuille ###
template<typename T>
ResistanceModelPoiseuille<T>::ResistanceModelPoiseuille(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelPoiseuille<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());

    return channel->getLength() * a * continuousPhaseViscosity / (channel->getHeight() * pow(channel->getWidth(), 3));
}

template<typename T>
T ResistanceModelPoiseuille<T>::computeFactorA(T width, T height) const {
    return 12.;
}

}  // namespace sim