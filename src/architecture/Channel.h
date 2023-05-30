#pragma once

#include <Edge.h>
#include <Node.h>

namespace arch{

    /**
    * @brief An enum to specify the shape of channel.
    */
    enum class ChannelShape {
        NONE,
        RECTANGULAR,    ///< A channel with a rectangular cross-section
        CYLINDRICAL     ///< A channel with a circular cross-section
    };

    template<typename T>
    class Channel : public Edge<T>{
        protected:
            T length = 0;               ///< Length of the channel in m.
            T pressure = 0;             ///< Pressure of a channel in Pa.
            T channelResistance = 0;    ///< Resistance of a channel in Pas/L.
            ChannelShape shape = ChannelShape::NONE; ///< The cross-section shape of this channel is rectangular
        
        public:
            /**
             * @brief Constructor of a channel connecting two-nodes.
            */
            Channel(int id, int nodeA, int nodeB);

            /**
             * @brief Set length of channel.
             * @param[in] length New length of this channel in m.
            */
            void setLength(T length);

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
             * @brief Returns the length of this channel.
             * @returns Length of channel in m.
            */
            T getLength() const;

            /**
             * @brief Calculates and returns pressure difference over a channel.
             * @returns Pressure in Pa.
             */
            T getPressure() const override;

            /**
             * @brief Calculate flow rate within the channel.
             * @returns Flow rate in m^3/s.
             */
            T getFlowRate() const override;

            /**
             * @brief Returns resistance caused by the channel itself.
             * @returns Resistance caused by the channel itself in Pas/L.
             */
            T getResistance() const override;

            /**
             * @brief Returns area of a channel.
             * @returns Area in m^2.
             */
            virtual T getArea() const = 0;

            /**
             * @brief Calculates and returns volume of the channel.
             * @returns Volumne of a channel in m^3.
             */
            T getVolume() const;

            /**
             * @brief Returns the shape of channel.
             * @returns What shape the channel has.
             */
            ChannelShape getChannelShape() const;
    };

    template<typename T>
    class RectangularChannel : public Channel<T> {
        private:
            T width;                                        ///< Width of a channel in m.
            T height;                                       ///< Height of a channel in m.
        
        public:
            /**
             * @brief Constructor of a channel with rectangular cross-section
            */
            RectangularChannel(int id, int nodeA, int nodeB, T width, T height);

            /**
             * @brief Set width of rectangular channel.
            */
            void setWidth(T width);

            /**
             * @brief Returns the width of this channel.
             * @returns Width of channel in m.
            */
            T getWidth() const;

            /**
             * @brief Set height of rectangular channel.
            */
            void setHeight(T height);

            /**
             * @brief Returns the height of this channel.
             * @returns Height of channel in m.
            */
            T getHeight() const;

            /**
             * @brief Returns area of a channel.
             * @returns Area in m^2.
             */
            T getArea() const override;
    };

    template<typename T>
    class CylindricalChannel : public Channel<T> {
        private:
            T radius;                                       ///< Radius of a channel in m.

        public:
            /**
             * @brief Constructor of a channel with circular cross-section
            */
            CylindricalChannel(int id, int nodeA, int nodeB, T radius);
            
            /**
             * @brief Set radius of cilyndrical channel.
            */
            void setRadius(T radius);

            /**
             * @brief Returns the radius of this channel.
             * @returns Radius of channel in m.
            */
            T getRadius() const;

            /**
             * @brief Returns area of a channel.
             * @returns Area in m^2.
             */
            T getArea() const override;
    };

}   // namespace arch