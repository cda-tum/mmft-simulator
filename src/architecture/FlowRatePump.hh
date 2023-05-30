#include "FlowRatePump.h"

#include <Edge.h>
#include <Node.h>

namespace arch {

template<typename T>
FlowRatePump<T>::FlowRatePump(int id, int nodeA_, int nodeB_, T flowRate_) : 
    Edge<T>(id, nodeA_, nodeB_), flowRate(flowRate_) { }

template<typename T>
void FlowRatePump<T>::setPressure(T pressure_) {
    this->pressure = pressure_;
}

template<typename T>
void FlowRatePump<T>::setFlowRate(T flowRate_) {
    this->flowRate = flowRate_;
}

template<typename T>
T FlowRatePump<T>::getPressure() const {
    return pressure;
}

template<typename T>
T FlowRatePump<T>::getFlowRate() const {
    return flowRate;
}

template<typename T>
T FlowRatePump<T>::getResistance() const {
    return getPressure() / getFlowRate();
}

}  // namespace arch
