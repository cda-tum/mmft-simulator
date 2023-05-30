#include "Channel.h"

#include <Edge.h>
#include <Node.h>

#define M_PI 3.14159265358979323846

namespace arch {

    template<typename T>
    Channel<T>::Channel(int id_, int nodeA_, int nodeB_) : 
        Edge<T>(id_, nodeA_, nodeB_) { }


    template<typename T>
    void Channel<T>::setLength(T length_) {
        this->length = length_;
    }

    template<typename T>
    void Channel<T>::setPressure(T pressure_) {
        this->pressure = pressure_;
    }

    template<typename T>
    void Channel<T>::setResistance(T channelResistance_) {
        this->channelResistance = channelResistance_;
    }

    template<typename T>
    T Channel<T>::getLength() const {
        return length;
    }

    template<typename T>
    T Channel<T>::getPressure() const {
        return pressure;
    }

    template<typename T>
    T Channel<T>::getFlowRate() const {
        return getPressure() / getResistance();
    }

    template<typename T>
    T Channel<T>::getVolume() const {
        return getArea() * length;
    }

    template<typename T>
    T Channel<T>::getResistance() const {
        return channelResistance;
    }

    template<typename T>
    ChannelShape Channel<T>::getChannelShape() const {
        return shape;
    }

    //=====================================================================================
    //================================  RectangularChannel ================================
    //=====================================================================================


    template<typename T>
    RectangularChannel<T>::RectangularChannel(int id_, int nodeA_, int nodeB_, T width_, T height_) : 
        Channel<T>(id_, nodeA_, nodeB_), width(width_), height(height_) { 
            this->shape = ChannelShape::RECTANGULAR;
         }

    template<typename T>
    void RectangularChannel<T>::setWidth(T width_) {
        this->width = width_;
    }

    template<typename T>
    T RectangularChannel<T>::getWidth() const {
        return width;
    }

    template<typename T>
    void RectangularChannel<T>::setHeight(T height_) {
        this->height = height_;
    }

    template<typename T>
    T RectangularChannel<T>::getHeight() const {
        return height;
    }

    template<typename T>
    T RectangularChannel<T>::getArea() const {
        return width * height;
    }

    //=====================================================================================
    //================================  CylindricalChannel ================================
    //=====================================================================================


    template<typename T>
    CylindricalChannel<T>::CylindricalChannel(int id_, int nodeA_, int nodeB_, T radius_) : 
        Channel<T>(id_, nodeA_, nodeB_), radius(radius_) { 
            this->shape = ChannelShape::CYLINDRICAL;
    }

    template<typename T>
    void CylindricalChannel<T>::setRadius(T radius_) {
        this->radius = radius_;
    }

    template<typename T>
    T CylindricalChannel<T>::getRadius() const {
        return radius;
    }

        template<typename T>
    T CylindricalChannel<T>::getArea() const {
        return M_PI * radius * radius;
    }

}   // namespace arch