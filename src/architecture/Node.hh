#include "Node.h"

namespace arch {

    template<typename T>
    Node<T>::Node(int id_, T x_, T y_) : 
        id(id_) { 
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

}   // namespace arch