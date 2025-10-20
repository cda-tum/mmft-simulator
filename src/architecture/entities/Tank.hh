#include "Tank.h"

namespace arch {

template<typename T>
Tank<T>::Tank(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T height, T width, T length) : Edge<T>(id, nodeA->getId(), nodeB->getId()),
    height(height), width(width), length(length), pressureDifference(nodeA->getPressure() - nodeB->getPressure()) {}

template<typename T>
Tank<T>::Tank(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T resistance) : Edge<T>(id, nodeA->getId(), nodeB->getId()),
    tankResistance(resistance), pressureDifference(nodeA->getPressure() - nodeB->getPressure()) {}

template<typename T>
void Tank<T>::setDimensions(T width_, T height_, T length_) {
    this->height = height_;
    this->width = width_;
    this->length = length_;
}

template<typename T>
void Tank<T>::setHeight(T height_) {
    this->height = height_;
}

template<typename T>
void Tank<T>::setWidth(T width_) {
    this->width = width_;
}

template<typename T>
void Tank<T>::setLength(T length_) {
    this->length = length_;
}

template<typename T>
T Tank<T>::getHeight() const {
    return height;
}

template<typename T>
T Tank<T>::getWidth() const {
    return width;
}

template<typename T>
T Tank<T>::getLength() const {
    return length;
}

template<typename T>
T Tank<T>::getArea() const {
    return width * length;
}

template<typename T>
T Tank<T>::getVolume() const {
    return width * height * length;
}

template<typename T>
T Tank<T>::getPressure() const {
    return pressureDifference;
}

template<typename T>
T Tank<T>::getFlowRate() const {
    // there is no flow in a tank, thus the tank does not have a flow-rate
    // to ensure time-accurate concentration changes
    // mixtures move through the tank based on the flow-rate of the connected channel
    throw std::logic_error("Tank has no flow rate!");
}

template<typename T>
T Tank<T>::getResistance() const {
    return tankResistance;
}

}  // namespace arch
