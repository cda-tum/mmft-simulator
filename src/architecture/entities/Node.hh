#include "Node.h"

namespace arch {

template<typename T>
Node<T>::Node(size_t id_, T x_, T y_, bool ground_) : 
    id(id_), ground(ground_) { 
    pos.push_back(x_);
    pos.push_back(y_);
}

}   // namespace arch