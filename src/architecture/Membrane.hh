#include "Membrane.h"

namespace arch {

template<typename T>
Membrane<T>::Membrane(int id, Node<T>* nodeA, Node<T>* nodeB, T height, T width, T length, T poreRadius, T porosity) : Edge<T>(id, nodeA->getId(), nodeB->getId()),
    height(height), width(width), length(length), poreRadius(poreRadius), porosity(porosity) {}

template<typename T>
Membrane<T>::Membrane(int id, Node<T>* nodeA, Node<T>* nodeB, T resistance) : Edge<T>(id, nodeA->getId(), nodeB->getId()),
    membraneResistance(resistance) {}

template<typename T>
void Membrane<T>::setDimensions(T width_, T height_, T length_) {
    this->height = height_;
    this->width = width_;
    this->length = length_;
}

template<typename T>
void Membrane<T>::setHeight(T height_) {
    this->height = height_;
}

template<typename T>
void Membrane<T>::setWidth(T width_) {
    this->width = width_;
}

template<typename T>
void Membrane<T>::setLength(T length_) {
    this->length = length_;
}

template<typename T>
void Membrane<T>::setPoreRadius(T poreRadius_) {
    this->poreRadius = poreRadius_;
}

template<typename T>
void Membrane<T>::setPorosity(T porosity_) {
    this->porosity = porosity_;
}

template<typename T>
void Membrane<T>::setChannel(RectangularChannel<T>* channel_) {
    this->channel = channel_;
}

template<typename T>
void Membrane<T>::setTank(Tank<T>* tank_) {
    this->tank = tank_;
}

template<typename T>
T Membrane<T>::getHeight() const {
    return height;
}

template<typename T>
T Membrane<T>::getWidth() const {
    return width;
}

template<typename T>
T Membrane<T>::getLength() const {
    return length;
}

template<typename T>
RectangularChannel<T>* Membrane<T>::getChannel() const {
    return channel;
}

template<typename T>
Tank<T>* Membrane<T>::getTank() const {
    return tank;
}

template<typename T>
T Membrane<T>::getPoreRadius() const {
    return poreRadius;
}

template<typename T>
T Membrane<T>::getPoreDiameter() const {
    return poreRadius * 2;
}

template<typename T>
T Membrane<T>::getPorosity() const {
    return porosity;
}

template<typename T>
T Membrane<T>::getNumberOfPores(T area) const {
    return (porosity * area) / (M_PI * pow(poreRadius, 2));
}

template<typename T>
T Membrane<T>::getPoreDensity() const {
    return (porosity * M_PI * pow(poreRadius, 2));
}

template<typename T>
T Membrane<T>::getArea() const {
    return width * length;
}

template<typename T>
T Membrane<T>::getVolume() const {
    return width * height * length;
}

template<typename T>
T Membrane<T>::getPressure() const {
    return channel->getPressure();
}

template<typename T>
T Membrane<T>::getFlowRate() const {
    return channel->getFlowRate();
}

template<typename T>
T Membrane<T>::getResistance() const {
    return membraneResistance;
}

template<typename T>
T Membrane<T>::getConcentrationChange(T resistance, T timeStep, T concentrationDifference) const {
    // adapted Runge-Kutta 4 method
    auto f = [](T concentration, T permeability) { return permeability * concentration; };

    T permeability = 1 / resistance;
    T k1 = timeStep * (f(concentrationDifference, permeability));
    T k2 = timeStep * (f((concentrationDifference + k1 / 2), permeability));
    T k3 = timeStep * (f((concentrationDifference + k2 / 2), permeability));
    T k4 = timeStep * (f((concentrationDifference + k3), permeability));
    T concentrationChange = (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    return concentrationChange;
}

}  // namespace arch
