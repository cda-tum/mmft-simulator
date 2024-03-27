#include "Node.h"

namespace arch {

template<typename T>
Node<T>::Node(int id_, T x_, T y_, bool ground_) : 
    id(id_), ground(ground_) { 
        pos.push_back(x_);
        pos.push_back(y_);
    }

template<typename T>
void Node<T>::setPosition(std::vector<T> pos_) {
    this->pos =  pos_;
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
std::vector<T> Node<T>::getPosition() const {
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