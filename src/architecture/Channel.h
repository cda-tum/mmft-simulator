#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Edge.h"
#include "Node.h"

namespace arch{

    template<typename T, int dim>
    struct Line_segment {
        T start[dim];
        T end[dim];

        Line_segment(T start[dim], T end[dim]);
        
        /**
         * @brief Returns the length of this line segment.
         * @returns Length of line segment in m.
        */
        T getLength();
    };

    template<typename T, int dim>
    struct Arc {
        bool right;
        T start[dim];
        T end[dim];
        T center[dim];

        Arc(bool right, T start[dim], T end[dim], T center[dim]);

        /**
         * @brief Returns the length of this arc.
         * @returns Length of arc in m.
        */
        T getLength();
    };

    template<typename T, int dim=2>
    class Channel : public Edge<T>{
        public:
        /**
        * @brief An enum to specify the shape of channel.
        */
        enum class ChannelShape {
            NONE,
            RECTANGULAR,    ///< A channel with a rectangular cross-section
            CYLINDRICAL     ///< A channel with a circular cross-section
        };

        /**
        * @brief An enum to specify the shape of channel.
        */
        enum class PumpType {
            NONE,           ///< A regular channel with hydraulic resistance.
            PRESSUREPUMP,   ///< A channel with a pre-set pressure difference.
            FLOWRATEPUMP    ///< A channel with a pre-set flow rate.
        };

        protected:
            T width;                                    ///< Width of the channel in m.
            T height;                                   ///< Height of the channel in m.
            T radius;                                   ///< Radius of the channel in m.
            T length;                                   ///< Length of the channel in m.
            T area;                                     ///< Area of the channel cross-section in m^2.
            T flowRate;                                 ///< Volumetric flow rate of the pump in m^3/s.
            T pressure;                                 ///< Pressure of a channel in Pa.
            T channelResistance;                        ///< Resistance of a channel in Pas/L.
            ChannelShape shape = ChannelShape::NONE;    ///< The cross-section shape of this channel is rectangular
            PumpType pump = PumpType::NONE;             ///< The pump type of the channel.

            std::unordered_map<int, std::unique_ptr<line_segment<T,dim>>> line_segments;
            std::unordered_map<int, std::unique_ptr<arc<T,dim>>> arcs;
        
        public:
            /**
             * @brief Constructor of a rectangular channel with one line segment connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] width The width of the channel cross-section.
             * @param[in] height The height of the channel cross-section.
             * 
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T width, T height);
            
            /**
             * @brief Constructor of a cylindrical channel with one line segment connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] radius The radius of the channel cross-section.
             * 
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, T radius);

            /**
             * @brief Constructor of a rectangular channel with one arc connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] width The width of the channel cross-section.
             * @param[in] height The height of the channel cross-section.
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, bool right, T center[dim], T width, T height);

            /**
             * @brief Constructor of a cylindrical channel with one arc connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] radius The radius of the channel cross-section.
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, bool right, T center[dim], T radius);

            /**
             * @brief Constructor of a rectangular channel with line segments and arcs connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] width The width of the channel cross-section.
             * @param[in] height The height of the channel cross-section.
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, 
                    std::unordered_map<int, std::unique_ptr<Line_segment<T,dim>>> line_segments,
                    std::unordered_map<int, std::unique_ptr<Arc<T,dim>>> arcs, T width, T height);

            /**
             * @brief Constructor of a cylindrical channel with line segments and arcs connecting two-nodes.
             * @param[in] id Id of the channel.
             * @param[in] nodeA Node at one end of the channel.
             * @param[in] nodeB Node at the other end of the channel.
             * @param[in] radius The radius of the channel cross-section.
            */
            Channel(int id, std::shared_ptr<Node<T>> nodeA, std::shared_ptr<Node<T>> nodeB, 
                    std::unordered_map<int, std::unique_ptr<Line_segment<T,dim>>> line_segments,
                    std::unordered_map<int, std::unique_ptr<Arc<T,dim>>> arcs, T radius);

            /**
             * @brief Set width of rectangular channel.
             * @param[in] width The width of the channel.
            */
            void setWidth(T width);

            /**
             * @brief Set height of rectangular channel.
             * @param[in] height The height of the channel.
            */
            void setHeight(T height);

            /**
             * @brief Set radius of cilyndrical channel.
             * @param[in] radius The radius of the channel.
            */
            void setRadius(T radius);

            /**
             * @brief Set volumetric flow rate of the pump.
             * @param[in] flowRate New volumetric flow rate to set in m^3/s.
             */
            void setFlowRate(T flowRate);

            /**
             * @brief Set the pressure difference over a channel.
             * @param[in] Pressure in Pa.
             */
            void setPressure(T pressure);

            /**
             * @brief Set resistance of a channel without droplets.
             * @param[in] channelResistance Resistance of a channel without droplets in Pas/L.
             */
            void setResistance(T channelResistance);

            /**
             * @brief Returns the shape of channel.
             * @returns What shape the channel has.
             */
            void setChannelPumpType(PumpType type);

            /**
             * @brief Returns the length of this channel.
             * @returns Length of channel in m.
            */
            T getLength() const;

            /**
             * @brief Returns area of a channel.
             * @returns Area in m^2.
             */
            T getArea() const;

            /**
             * @brief Returns the width of this channel.
             * @returns Width of channel in m.
            */
            T getWidth() const;

            /**
             * @brief Returns the height of this channel.
             * @returns Height of channel in m.
            */
            T getHeight() const;

            /**
             * @brief Returns the radius of this channel.
             * @returns Radius of channel in m.
            */
            T getRadius() const;

            /**
             * @brief Calculates and returns pressure difference over a channel.
             * @returns Pressure in Pa.
             */
            T getPressure() const;

            /**
             * @brief Calculate flow rate within the channel.
             * @returns Flow rate in m^3/s.
             */
            T getFlowRate() const;

            /**
             * @brief Returns resistance caused by the channel itself.
             * @returns Resistance caused by the channel itself in Pas/L.
             */
            T getResistance() const;

            /**
             * @brief Returns the shape of channel.
             * @returns What shape the channel has.
             */
            ChannelShape getChannelShape() const;

            /**
             * @brief Returns the shape of channel.
             * @returns What shape the channel has.
             */
            PumpType getChannelPumpType() const;
    };

}   // namespace arch