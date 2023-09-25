#include "Channel.h"

#include "Edge.h"
#include "Node.h"

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
    Channel<T>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_) : 
    Edge<T>(id_, nodeA_->getId(), nodeB_->getId()) { 
        std::unique_ptr<Line_segment<T,2>> line = std::make_unique<Line_segment<T,2>> (nodeA_->getPosition(), nodeB_->getPosition());
        this->length = line->getLength();
        line_segments.push_back(std::move(line));
    }

    template<typename T>
    Channel<T>::Channel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, 
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
    RectangularChannel<T>::RectangularChannel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, T width_, T height_) : 
    Channel<T>(id_, nodeA_, nodeB_), width(width_), height(height_) { 
        this->area = width*height;
    }

    template<typename T>
    RectangularChannel<T>::RectangularChannel(int id_, std::shared_ptr<Node<T>> nodeA_, std::shared_ptr<Node<T>> nodeB_, 
                        std::vector<Line_segment<T,2>*> line_segments_,
                        std::vector<Arc<T,2>*> arcs_, T width_, T height_) : 
    Channel<T>(id_, nodeA_, nodeB_, line_segments_, arcs_), width(width_), height(height_) { 
        this->area = width*height;
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