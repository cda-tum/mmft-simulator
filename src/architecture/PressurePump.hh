#include "PressurePump.h"

#include <Edge.h>
#include <Node.h>

namespace arch {

    template<typename T>
    PressurePump<T>::PressurePump(int id_, int nodeA_, int nodeB_, T pressure_) : 
        Edge<T>(id_, nodeA_, nodeB_), pressure(pressure_) { }

    template<typename T>
    void PressurePump<T>::setPressure(T pressure_) {
        this->pressure = pressure_;
    }

    template<typename T>
    void PressurePump<T>::setFlowRate(T flowRate_) {
        this->flowRate = flowRate_;
    }

    template<typename T>
    T PressurePump<T>::getPressure() const {
        return pressure;
    }

    template<typename T>
    T PressurePump<T>::getFlowRate() const {
        return flowRate;
    }

    template<typename T>
    T PressurePump<T>::getResistance() const {
        return getPressure() / getFlowRate();
    }

}  // namespace arch
