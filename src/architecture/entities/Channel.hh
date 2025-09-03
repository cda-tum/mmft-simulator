#include "Channel.h"

#define M_PI 3.14159265358979323846

namespace arch {

//=====================================================================================
//================================  Line_segment  =====================================
//=====================================================================================

template<typename T, int DIM>
Line_segment<T,DIM>::Line_segment(std::vector<T> start_, std::vector<T> end_) :
    start(start_), end(end_) { }

template<typename T, int DIM>
T Line_segment<T,DIM>::getLength() {
    T dx = start[0] - end[0];
    T dy = start[1] - end[1];
    T dz = 0.0;
    if (DIM == 3) {
        dz = start[2] - end[2];
    }
    T length = sqrt(dx*dx + dy*dy + dz*dz);

    return length;
}

//=====================================================================================
//================================  Arc ===============================================
//=====================================================================================

template<typename T, int DIM>
Arc<T,DIM>::Arc(bool right_, std::vector<T> start_, std::vector<T> end_, std::vector<T> center_) :
    right(right_), start(start_), end(end_), center(center_) { }

template<typename T, int DIM>
T Arc<T,DIM>::getLength() {
    T dx1 = start[0] - center[0];
    T dy1 = start[1] - center[1];
    T dz1 = 0.0;
    if (DIM == 3) {
        dz1 = start[2] - center[2];
    }
    T dx2 = start[0] - end[0];
    T dy2 = start[1] - end[1];
    T dz2 = 0.0;
    if (DIM == 3) {
        dz2 = start[2] - end[2];
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

template<typename T>
Channel<T>::Channel(size_t id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_) : 
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()) { 
    std::unique_ptr<Line_segment<T,2>> line = std::make_unique<Line_segment<T,2>> (nodeA_->getPosition(), nodeB_->getPosition());
    this->length = line->getLength();
    line_segments.push_back(std::move(line));
}

template<typename T>
Channel<T>::Channel(size_t id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_,
                    std::vector<Line_segment<T,2>*> line_segments_,
                    std::vector<Arc<T,2>*> arcs_) :
Edge<T>(id_, nodeA_->getId(), nodeB_->getId()) {
    for (auto& line : line_segments_) {
        this->length += line->getLength();
        std::unique_ptr<Line_segment<T,2>> uLine(line);
        line_segments.push_back(std::move(uLine));
    }
    for (auto& arc : arcs_) {
        this->length += arc->getLength();
        std::unique_ptr<Arc<T,2>> uArc(arc);
        arcs.push_back(std::move(uArc));
    }
}

//=====================================================================================
//================================  RectangularChannel ================================
//=====================================================================================

template<typename T>
RectangularChannel<T>::RectangularChannel(size_t id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, T width_, T height_) : 
Channel<T>(id_, nodeA_, nodeB_), width(width_), height(height_) { 
    this->area = width*height;
}

template<typename T>
RectangularChannel<T>::RectangularChannel(size_t id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, 
                    std::vector<Line_segment<T,2>*> line_segments_,
                    std::vector<Arc<T,2>*> arcs_, T width_, T height_) : 
Channel<T>(id_, nodeA_, nodeB_, line_segments_, arcs_), width(width_), height(height_) { 
    this->area = width*height;
}

//=====================================================================================
//================================  CylindricalChannel ================================
//=====================================================================================


template<typename T>
CylindricalChannel<T>::CylindricalChannel(size_t id_, int nodeA_, int nodeB_, T radius_) : 
    Channel<T>(id_, nodeA_, nodeB_), radius(radius_) { 
        this->shape = ChannelShape::CYLINDRICAL;
}

template<typename T>
T CylindricalChannel<T>::getArea() const {
    return M_PI * radius * radius;
}

}   // namespace arch