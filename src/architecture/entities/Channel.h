/**
 * @file Channel.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace arch{

// Forward declared dependencies
template<typename T>
class Edge;

template<typename T>
class Node;

/**
 * @brief An enum to specify the type of channel.
*/
enum class ChannelType {
    NORMAL,     ///< A normal channel is te regular channel in which flow flows.
    BYPASS,     ///< A bypass channel allows droplets to bypass another channel, e.g., if a droplet is trapped in that channel.
    CLOGGABLE   ///< A cloggable channel will be clogged during the time a droplet passes by one of its ends.
};

/**
* @brief An enum to specify the shape of channel.
*/
enum class ChannelShape {
    NONE,
    RECTANGULAR,    ///< A channel with a rectangular cross-section
    CYLINDRICAL     ///< A channel with a circular cross-section
};

/**
 * @brief A struct that defines a straight channel segment
*/
template<typename T, int DIM>
struct Line_segment {
    std::vector<T> start;
    std::vector<T> end;

    Line_segment(std::vector<T> start, std::vector<T> end);
    
    /**
     * @brief Returns the length of this line segment.
     * @returns Length of line segment in m.
    */
    T getLength();
}; 

/**
 * @brief A struct that defines an arc channel segment
*/
template<typename T, int DIM>
struct Arc {
    bool right;
    std::vector<T> start;
    std::vector<T> end;
    std::vector<T> center;

    Arc(bool right, std::vector<T> start, std::vector<T> end, std::vector<T> center);

    /**
     * @brief Returns the length of this arc.
     * @returns Length of arc in m.
    */
    T getLength();
};

template<typename T>
class Channel : public Edge<T>{
private:
    T length = 0;                               ///< Length of the channel in m.
    T area = 0;                                 ///< Area of the channel cross-section in m^2.
    T pressure = 0;                             ///< Pressure of a channel in Pa.
    T channelResistance = 0;                    ///< Resistance of a channel in Pas/L.
    T dropletResistance = 0;                    ///< Additional resistance of present droplets in the channel in Pas/L.
    ChannelShape shape = ChannelShape::NONE;    ///< The cross-section shape of this channel is rectangular.
    ChannelType type = ChannelType::NORMAL;     ///< What kind of channel it is.
    
    std::vector<std::unique_ptr<Line_segment<T,2>>> line_segments;      ///< Straight line segments in the channel.
    std::vector<std::unique_ptr<Arc<T,2>>> arcs;                        ///< Arcs in the channel.

protected:
    /**
     * @brief Constructor of a rectangular channel with line segments and arcs connecting two-nodes.
     * @param[in] id Id of the channel.
     * @param[in] nodeA Node at one end of the channel.
     * @param[in] nodeB Node at the other end of the channel.
     * @param[in] line_segments The straight line segments of this channel.
     * @param[in] arcs The arcs of this channel.
    */
    Channel(size_t id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, 
            std::vector<Line_segment<T,2>*> line_segments,
            std::vector<Arc<T,2>*> arcs);

    /**
     * @brief Constructor of a channel connecting two-nodes.
     * @param[in] id Id of the channel.
     * @param[in] nodeA Node at one end of the channel.
     * @param[in] nodeB Node at the other end of the channel.
    */
    Channel(size_t id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB);

public:
    /**
     * @brief Set which kind of channel it is.
     * @param[in] channelType Which kind of channel it is.
     */
    inline void setChannelType(ChannelType channelType) { this->type = channelType; }

    /**
     * @brief Returns the shape of channel.
     * @returns What shape the channel has.
     */
    [[nodiscard]] inline ChannelShape getChannelShape() const { return shape; }

    /**
     * @brief Returns the type of channel.
     * @returns What kind of channel it is.
     */
    [[nodiscard]] inline ChannelType getChannelType() const { return type; }

    /**
     * @brief Set length of channel.
     * @param[in] length New length of this channel in m.
    */
    inline void setLength(T length) { this->length = length; }

    /**
     * @brief Returns the length of this channel.
     * @returns Length of channel in m.
    */
    [[nodiscard]] inline T getLength() const { return length; }

    /**
     * @brief Set the pressure difference over a channel.
     * @param[in] Pressure in Pa.
     */
    inline void setPressure(T pressure) { this->pressure = pressure; }

    /**
     * @brief Calculates and returns pressure difference over a channel.
     * @returns Pressure in Pa.
     */
    [[noidscard]] inline T getPressure() const override { return pressure; }

    /**
     * @brief Set resistance of a channel without droplets.
     * @param[in] channelResistance Resistance of a channel without droplets in Pas/L.
     */
    inline void setResistance(T channelResistance) { this->channelResistance = channelResistance; }

    /**
     * @brief Returns resistance caused by the channel itself.
     * @returns Resistance caused by the channel itself in Pas/L.
     */
    [[nodiscard]] inline T getResistance() const override { return channelResistance + dropletResistance; }

    /**
     * @brief Set resistance caused by droplets within channel.
     * @param[in] dropletResistance Resistance caused by droplets within channel in Pas/L.
     */
    inline void setDropletResistance(T dropletResistance) { this->dropletResistance = dropletResistance; }

    /**
     * @brief Add resistance caused by a droplet to droplet resistance of channel that is caused by all droplets currently in the channel.
     * @param[in] dropletResistance Resistance caused by a droplet in Pas/L.
     */
    inline void addDropletResistance(T dropletResistance) { this->dropletResistance += dropletResistance; }

    /**
     * @brief Calculate flow rate within the channel.
     * @returns Flow rate in m^3/s.
     */
    [[nodiscard]] inline T getFlowRate() const override { return getPressure() / getResistance(); }

    /**
     * @brief Returns area of a channel.
     * @returns Area in m^2.
     */
    virtual T getArea() const = 0;

    /**
     * @brief Calculates and returns volume of the channel.
     * @returns Volume of a channel in m^3.
     */
    [[nodiscard]] inline T getVolume() const { return this->getArea() * length; }

    /**
     * TODO:
     */
    [[nodiscard]] virtual bool isRectangular() const { return false; }

    /**
     * TODO:
     */
    [[nodiscard]] virtual bool isCylindrical() const { return false; }

};

template<typename T>
class RectangularChannel final : public Channel<T> {
private:
    T width;                                        ///< Width of a channel in m.
    T height;                                       ///< Height of a channel in m.

    /**
     * @brief Constructor of a channel with rectangular cross-section
     * @param[in] id Id of the channel.
     * @param[in] nodeA Node at one end of the channel.
     * @param[in] nodeB Node at the other end of the channel.
     * @param[in] width The width of the channel.
     * @param[in] height The height of the channel.
    */
    RectangularChannel(size_t id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T width, T height);

    /**
     * @brief Constructor of a rectangular channel with line segments and arcs connecting two-nodes.
     * @param[in] id Id of the channel.
     * @param[in] nodeA Node at one end of the channel.
     * @param[in] nodeB Node at the other end of the channel.
     * @param[in] line_segments The straight line segments of this channel.
     * @param[in] arcs The arcs of this channel.
     * @param[in] width The width of the channel cross-section.
     * @param[in] height The height of the channel cross-section.
    */
    RectangularChannel(size_t id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, 
            std::vector<Line_segment<T,2>*> line_segments,
            std::vector<Arc<T,2>*> arcs, T width, T height);

public:
    /**
     * @brief Set width of rectangular channel.
     * @param[in] width The width of the channel.
    */
    inline void setWidth(T width) { this->width = width; }

    /**
     * @brief Returns the width of this channel.
     * @returns Width of channel in m.
    */
    [[nodiscard]] inline T getWidth() const { return width; }

    /**
     * @brief Set height of rectangular channel.
     * @param[in] height The height of the channel.
    */
    inline void setHeight(T height) { this->height = height; }

    /**
     * @brief Returns the height of this channel.
     * @returns Height of channel in m.
    */
    [[nodiscard]] inline T getHeight() const { return height; }

    /**
     * @brief Returns area of a channel.
     * @returns Area in m^2.
     */
    [[nodiscard]] inline T getArea() const override { return width * height; }

    /**
     * TODO:
     */
    [[nodiscard]] virtual bool isRectangular() const override { return true; }
};

template<typename T>
class CylindricalChannel final : public Channel<T> {
private:
    T radius;                                       ///< Radius of a channel in m.

    /**
     * @brief Constructor of a channel with circular cross-section             
     * @param[in] id Id of the channel.
     * @param[in] nodeA One node of the channel.
     * @param[in] nodeB The other node of the channel.
     * @param[in] radius The radius of the channel.
    */
    CylindricalChannel(size_t id, int nodeA, int nodeB, T radius);

public:    
    /**
     * @brief Set radius of cilyndrical channel.
     * @param[in] radius The radius of the channel.
    */
    inline void setRadius(T radius) { this->radius = radius; }

    /**
     * @brief Returns the radius of this channel.
     * @returns Radius of channel in m.
    */
    [[nodiscard]] inline T getRadius() const { return radius; }

    /**
     * @brief Returns area of a channel.
     * @returns Area in m^2.
     */
    [[nodiscard]] T getArea() const override;

    /**
     * TODO:
     */
    [[nodiscard]] virtual bool isCylindrical() const override { return true; }
};

}   // namespace arch