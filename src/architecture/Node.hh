#include "Node.h"

namespace arch {

template<typename T>
Node<T>::Node(int id_, T x_, T y_, bool ground_) : 
    id(id_), ground(ground_) 
{ 
    pos[0] = x_;
    pos[1] = y_;
    pos[2] = T(0.0);
}

template<typename T>
Node<T>::Node(int id_, T x_, T y_, T z_, bool ground_) : 
    id(id_), ground(ground_) 
{ 
    pos[0] = x_;
    pos[1] = y_;
    pos[2] = z_;
}

template<typename T>
void Node<T>::setPosition(std::vector<T> pos_) {
    if (pos_.size() == 2) {
        std::copy(pos_.begin(), pos_.begin() + 1, pos.begin());
        pos[2] = T(0.0);
    } else if (pos_.size() == 3) {
        std::copy(pos_.begin(), pos_.begin() + 2, pos.begin());
    } else {
        throw std::domain_error("Node position should have two or three elements.");
    }
}

template<typename T>
void Node<T>::setPosition(std::array<T,3> pos_) {
    this->pos = pos_;
}

template<typename T>
void Node<T>::setPressure(T pressure_) {
    this->pressure = pressure_;
}

template<typename T>
int Node<T>::getId() const {
    return id;
}

template<typename T>
std::array<T,3> Node<T>::getPosition() const {
    return pos;
}

template<typename T>
T Node<T>::getPressure() const {
    return pressure;
}

template<typename T>
void Node<T>::setSink(bool sink_) {
    sink = sink_;
}

template<typename T>
void Node<T>::setGround(bool ground_) {
    ground = ground_;
}

template<typename T>
bool Node<T>::getGround() {
    return ground;
}

template<typename T>
bool Node<T>::getSink() {
    return sink;
}

}   // namespace arch