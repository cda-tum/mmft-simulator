#include "PressurePump.h"

namespace arch {

template<typename T>
PressurePump<T>::PressurePump(size_t id_, size_t nodeA_, size_t nodeB_, T pressure_) : 
    Edge<T>(id_, nodeA_, nodeB_), pressure(pressure_) { }

}  // namespace arch
