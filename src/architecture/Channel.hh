#include "Channel.h"

#include <math.h>

#include "Edge.h"
#include "Node.h"

#define M_PI 3.14159265358979323846

namespace arch {

//=====================================================================================
//================================  Line_segment  =====================================
//=====================================================================================

template<typename T, int DIM>
Line_segment<T,DIM>::Line_segment(T start_[DIM], T end_[DIM]) :
    start(start_), end(end_) { }

template<typename T, int DIM>
T Line_segment<T,DIM>::getLength() {
    T dx = start[0] - end[0];
    T dy = start[1] - end[1];
    T dz = 0.0;
    if (DIM == 3) {
        T dz = start[2] - end[2];
    }
    T length = sqrt(dx*dx + dy*dy + dz*dz);

    return length;
}

//=====================================================================================
//================================  Arc ===============================================
//=====================================================================================

template<typename T, int DIM>
Arc<T,DIM>::Arc(bool right_, T start_[DIM], T end_[DIM], T center_[DIM]) :
    right(right_), start(start_), end(end_), center(center_) { }

template<typename T, int DIM>
T Arc<T,DIM>::getLength() {
    T dx1 = start[0] - center[0];
    T dy1 = start[1] - center[1];
    T dz1 = 0.0;
    if (DIM == 3) {
        T dz1 = start[2] - center[2];
    }
    T dx2 = start[0] - end[0];
    T dy2 = start[1] - end[1];
    T dz2 = 0.0;
    if (DIM == 3) {
        T dz2 = start[2] - end[2];
    }

    T radius = sqrt(dx1*dx1 + dy1*dy1 + dz1*dz1);
    T chord = sqrt(dx2*dx2 + dy2*dy2 + dz2*dz2);
    T theta = 2*asin(chord/(2*radius));
    T length = theta*radius;

    return length;
} 

//=====================================================================================
//================================  Channel ===========================================
//=====================================================================================

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, T width_, T height_) : 
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), width(width_), height(height_), shape(ChannelShape::RECTANGULAR) { 
    Line_segment<T,DIM> line = std::make_unique<Line_segment<T,DIM>> (nodeA_->getPosition(), nodeB_->getPosition());
    length = line->getLength();
    area = width*height;
    line_segments.try_emplace(int 0, std::move(line));
}

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, T radius_) : 
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), radius(radius_), shape(ChannelShape::CYLINDRICAL) { 
    Line_segment<T,DIM> line = std::make_unique<Line_segment<T,DIM>> (nodeA_->getPosition(), nodeB_->getPosition());
    length = line->getLength();
    area = M_PI*radius*radius;
    line_segments.try_emplace(int 0, std::move(line));
}

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, bool right, T center_[DIM], T width_, T height_) : 
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), width(width_), height(height_), shape(ChannelShape::RECTANGULAR) { 
    Arc<T,DIM> arc = std::make_unique<Arc<T,DIM>> (right, nodeA_->getPosition(), nodeB_->getPosition(), center_);
    length = arc->getLength();
    area = width*height;
    arcs.try_emplace(int 0, std::move(arc));
}

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, bool right, T center_[DIM], T radius_) : 
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), radius(radius_), shape(ChannelShape::CYLINDRICAL) { 
    Arc<T,DIM> arc = std::make_unique<Arc<T,DIM>> (right, nodeA_->getPosition(), nodeB_->getPosition(), center_);
    length = arc->getLength();
    area = M_PI*radius*radius;
    arcs.try_emplace(int 0, std::move(arc));
}

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, 
                        std::unordered_map<int, std::unique_ptr<Line_segment<T,DIM>>> line_segments_,
                        std::unordered_map<int, std::unique_ptr<Arc<T,DIM>>> arcs_, T width_, T height_) :
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), line_segments(line_segments_), arcs(arcs_), width(width_), height(height_), 
shape(ChannelShape::RECTANGULAR) {
    for (auto& [key, line] : this->line_segments) {
        length += line->getLength();
    }
    for (auto& [key, arc] : this->arcs) {
        length += line->getLength();
    }
    area = width*height;
}  

template<typename T, int DIM>
Channel<T,DIM>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, 
                        std::unordered_map<int, std::unique_ptr<Line_segment<T,DIM>>> line_segments_,
                        std::unordered_map<int, std::unique_ptr<Arc<T,DIM>>> arcs_, T radius_) :
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()), line_segments(line_segments_), arcs(arcs_), radius(radius_), shape(ChannelShape::CYLINDRICAL) {
    for (auto& [key, line] : this->line_segments) {
        length += line->getLength();
    }
    for (auto& [key, arc] : this->arcs) {
        length += line->getLength();
    }
    area = M_PI*radius*radius;
}  

template<typename T, int DIM>
void Channel<T,DIM>::setWidth(T width_) {
    assert(this->shape == ChannelShape::RECTANGULAR);
    this->width = width_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setHeight(T height_) {
    assert(this->shape == ChannelShape::RECTANGULAR);
    this->height = height_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setRadius(T radius_) {
    assert(this->shape == ChannelShape::CYLINDRICAL);
    this->radius = radius_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setFlowRate(T flowRate_) {
    assert(this->pump == PumpType::FLOWRATEPUMP);
    this->flowRate = flowRate_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setPressure(T pressure_) {
    assert(this->pump == PumpType::PRESSUREPUMP);
    this->pressure = pressure_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setResistance(T channelResistance_) {
    assert(this->pump == PumpType::NONE);
    this->channelResistance = channelResistance_;
}

template<typename T, int DIM>
void Channel<T,DIM>::setChannelPumpType(PumpType type_) {
    this->pump = type_;
}

template<typename T, int DIM>
T Channel<T,DIM>::getLength() const {
    return length;
}

template<typename T, int DIM>
T Channel<T,DIM>::getArea() const {
    return area;
}

template<typename T, int DIM>
T Channel<T,DIM>::getWidth() const {
    assert(this->shape == ChannelShape::RECTANGULAR);
    return width;
}

template<typename T, int DIM>
T Channel<T,DIM>::getHeight() const {
    assert(this->shape == ChannelShape::RECTANGULAR);
    return height;
}

template<typename T, int DIM>
T Channel<T,DIM>::getRadius() const {
    assert(this->shape == ChannelShape::CYLINDRICAL);
    return radius;
}

template<typename T, int DIM>
T Channel<T,DIM>::getPressure() const {
    return pressure;
}

template<typename T, int DIM>
T Channel<T,DIM>::getFlowRate() const {
    if (type == PumpType::FLOWRATEPUMP) {
        return flowRate;
    } else {
        return getPressure() / getResistance();
    }
}

template<typename T, int DIM>
T Channel<T,DIM>::getResistance() const {
    return channelResistance;
}

template<typename T, int DIM>
Channel<T,DIM>::ChannelShape Channel<T,DIM>::getChannelShape() const {
    return shape;
}

template<typename T, int DIM>
Channel<T,DIM>::PumpType Channel<T,DIM>::getChannelPumpType() const {
    return pump;
}

}   // namespace arch