#include "FlowRatePump.h"

namespace arch {

template<typename T>
FlowRatePump<T>::FlowRatePump(size_t id, size_t nodeA_, size_t nodeB_, T flowRate_) : 
    Edge<T>(id, nodeA_, nodeB_), flowRate(flowRate_) { }

}  // namespace arch
