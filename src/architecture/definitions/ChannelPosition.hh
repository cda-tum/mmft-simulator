#include "ChannelPosition.h"

namespace arch {

template<typename T>
ChannelPosition<T>::ChannelPosition(RectangularChannel<T>* channel, T position) : channel(channel), position(position) {}

template<typename T>
void ChannelPosition<T>::setChannel(RectangularChannel<T>* const channel) {
    this->channel = channel;
}

template<typename T>
void ChannelPosition<T>::setPosition(T position) {
    // ensure that position stays in range (e.g., due to rounding errors)
    if (position < 0.0) {
        this->position = 0.0;
    } else if (position > 1.0) {
        this->position = 1.0;
    } else {
        this->position = position;
    }
}

template<typename T>
void ChannelPosition<T>::addToPosition(T volumeShift) {
    T newPosition = position + volumeShift / channel->getVolume();
    setPosition(newPosition);
}

template<typename T>
RectangularChannel<T>* ChannelPosition<T>::getChannel() const {
    return channel;
}

template<typename T>
T ChannelPosition<T>::getPosition() const {
    return position;
}

template<typename T>
T ChannelPosition<T>::getAbsolutePosition() const {
    return position * channel->getLength();
}

template<typename T>
T ChannelPosition<T>::getVolumeA() const {
    return position * channel->getVolume();
}

template<typename T>
T ChannelPosition<T>::getVolumeB() const {
    return (1.0 - position) * channel->getVolume();
}

}  // namespace arch