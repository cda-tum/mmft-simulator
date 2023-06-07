#include "ResistanceModels.h"

#include <math.h>

#include <Channel.h>
#include <lbmModule.h>

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

// ### ResistanceModel2DPoiseuille ###
template<typename T>
ResistanceModel2DPoiseuille<T>::ResistanceModel2DPoiseuille(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModel2DPoiseuille<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel->getWidth(), channel->getHeight());

    return channel->getLength() * a * continuousPhaseViscosity / (channel->getHeight() * pow(channel->getWidth(), 3));
}

template<typename T>
T ResistanceModel2DPoiseuille<T>::computeFactorA(T width, T height) const {
    return 12.;
}

}  // namespace sim