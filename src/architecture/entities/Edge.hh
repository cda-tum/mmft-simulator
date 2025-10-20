#include "Edge.h"

namespace arch {

template<typename T>
Edge<T>::Edge(size_t id_, size_t nodeA_, size_t nodeB_) : id(id_), nodeA(nodeA_), nodeB(nodeB_) { }

}  // namespace arch