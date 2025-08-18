#include "ResistanceModels.h"

namespace sim {

// ### ResistanceModel ###
template<typename T>
ResistanceModel<T>::ResistanceModel(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

// ### ResistanceModel1D ###
template<typename T>
ResistanceModel1D<T>::ResistanceModel1D(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModel1D<T>::getChannelResistance(arch::Channel<T> const* const channel) const {
    T a = computeFactorA(channel);

    return channel->getLength() * a * this->continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));
}

template<typename T>
T ResistanceModel1D<T>::getDropletLength(arch::Channel<T> const* const channel, T volumeInsideChannel) const {
    T len_ = volumeInsideChannel / (channel->getArea());
    return len_;
}

template<typename T>
T ResistanceModel1D<T>::computeFactorA(arch::Channel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    return 12. / (1. - 192. * height * tanh(M_PI * width / (2. * height)) / (pow(M_PI, 5.) * width));
}

template<typename T>
T ResistanceModel1D<T>::getDropletResistance(arch::Channel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    T a = computeFactorA(channel);
    T dropletLength = volumeInsideChannel / (channel->getHeight() * channel->getWidth());
    T resistance = 3 * dropletLength * a * this->continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));

    if (resistance < 0.0) {
        std::cout << "droplet length: \t" << dropletLength << std::endl;
        std::cout << "volume inside channel: \t" << volumeInsideChannel << std::endl;
        std::cout << "a: \t\t" << a << std::endl;
        std::cout << "continuous phase viscosity: \t" << this->continuousPhaseViscosity << std::endl;
        std::cout << "channel width: \t" << channel->getWidth() << std::endl;
        std::cout << "channel height: \t" << channel->getHeight() << std::endl;
        throw std::invalid_argument("Negative droplet resistance. Droplet " + std::to_string(droplet->getId()) + " has a resistance of " + std::to_string(resistance));
    }

    return resistance;
}

template<typename T>
T ResistanceModel1D<T>::getRelativeDropletVelocity(arch::Channel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

template<typename T>
T ResistanceModel1D<T>::computeH_inf(arch::Channel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

// ### ResistanceModelPoiseuille ###
template<typename T>
ResistanceModelPoiseuille<T>::ResistanceModelPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelPoiseuille<T>::getChannelResistance(arch::Channel<T> const* const channel) const {
    
    return (12.0 * this->continuousPhaseViscosity * channel->getLength()/channel->getWidth() * pow(channel->getHeight(), 3.0));
    
}

template<typename T>
T ResistanceModelPoiseuille<T>::computeFactorA(arch::Channel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    T a = 1 - (192 * height)/(pow(M_PI, 5) * width) * tanh ((M_PI * width)/(2 * height));
    return a;
}

template<typename T>
T ResistanceModelPoiseuille<T>::getDropletResistance(arch::Channel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}
template<typename T>
T ResistanceModelPoiseuille<T>::getDropletLength(arch::Channel<T> const* const channel, T volumeInsideChannel) const {
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

template<typename T>
T ResistanceModelPoiseuille<T>::computeH_inf(arch::Channel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

template<typename T>
T ResistanceModelPoiseuille<T>::getRelativeDropletVelocity(arch::Channel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

//Resistance model BiPoiseuille 
template<typename T>
ResistanceModelBiPoiseuille<T>::ResistanceModelBiPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelBiPoiseuille<T>::getChannelResistance(arch::Channel<T> const* const channel) const{
    if (channel->getChannelShape() != arch::ChannelShape::CYLINDRICAL) {                       //either specified as rectangular or default rectangular          
        T h = channel->getHeight();
        T w = channel->getWidth();
        if (h/w < 0.01) {                                                             //high aspect ratio h<<w i.e. almost planar
            return (12.0 * this->continuousPhaseViscosity * channel->getLength())/(w * pow(h, 3));
        }else {                                                                       //h and w are comparable 
            T a = computeFactorA(channel);
            return (12.0 * this->continuousPhaseViscosity * channel->getLength())/(a * w * pow(h, 3));
        }
    } else {                                                                           //cylindrical channel
        T r = channel->getRadius();
        return (8.0 * this->continuousPhaseViscosity * channel->getLength()) /(M_PI * pow(r, 4.0));
    }
}

/** 
template<typename T>
T ResistanceModelBiPoiseuille<T>::getDropletLength(arch::Channel<T> const* const channel, T) const {
    T len_ = droplet->getVolume() / (channel->getArea());
    return len_;
}
*/

 
template<typename T>
T ResistanceModelBiPoiseuille<T>::getDropletLength(arch::Channel<T> const* const channel, T volumeInsideChannel) const {
    T len = volumeInsideChannel / (channel->getArea());
    return len;
}


template<typename T>
T ResistanceModelBiPoiseuille<T>::computeFactorA(arch::Channel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    T a = 1 - (192 * height)/(pow(M_PI, 5) * width) * tanh ((M_PI * width)/(2 * height));
    return a;
}

template<typename T>
T ResistanceModelBiPoiseuille<T>::getDropletResistance(arch::Channel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const{
    if (channel->getChannelShape() != arch::ChannelShape::CYLINDRICAL) {                      //defined rectangular or default rectangular channel
        T h = channel->getHeight();
        T w = channel->getWidth();
        T r_hydro;
        if (h==w) { 
            r_hydro = w / 2.0;
        }else {
            r_hydro = (w * h) / (w + h);
        }
        T dynamicViscosity= droplet->getViscosity() / this->continuousPhaseViscosity;
        //T Ca_inf = droplet->getCa();
        T H_inf = computeH_inf(channel, droplet);   
        //T droplet_radius= r_hydro - H_inf;
        T droplet_length_inside_channel = volumeInsideChannel / (channel->getArea());
        T droplet_length = droplet->getVolume()/ (channel->getArea());

        T dropletResistance = computeDropletResistance_rect(dynamicViscosity, droplet->getViscosity(), droplet_length_inside_channel, r_hydro, H_inf);
        //if (dropletResistance < 0.0) {
            std::cout << "droplet length: \t" << droplet_length << std::endl;
            std::cout << "droplet length inside the channel: \t" << droplet_length_inside_channel << std::endl;
            std::cout << "volume inside channel: \t" << volumeInsideChannel << std::endl;
            std::cout << "a: \t\t" << 3 << std::endl;
            std::cout << "continuous phase viscosity: \t" << this->continuousPhaseViscosity << std::endl;
            std::cout << "lambda = " << dynamicViscosity <<std::endl;
            std::cout << "channel height: \t" << channel->getHeight() << std::endl;
            std::cout << "channel width: \t" << channel->getWidth() << std::endl;
            std::cout << "uniform thick film: \t" << H_inf << std::endl;
            //throw std::invalid_argument("Negative droplet resistance. Droplet " + std::to_string(droplet->getId()) + " has a resistance of " + std::to_string(dropletResistance));
        //} 
        //somehow this was throwing an error need to vheckout in detail

        if (dropletResistance < 0.0 || std::isnan(dropletResistance) || std::isinf(dropletResistance)) {
            throw std::invalid_argument(
                "Invalid droplet resistance: Droplet " +
                std::to_string(droplet->getId()) +
                " has resistance = " + std::to_string(dropletResistance));
        }
        return dropletResistance;
    } else {                                                                             //cylindrical channel    
        T dynamicViscosity = droplet->getViscosity() / this-> continuousPhaseViscosity;
        T H_inf = computeH_inf(channel, droplet);
        T droplet_radius = channel->getRadius() - H_inf; 
        T droplet_length = droplet->getVolume() / channel->getArea();
        T droplet_length_inside_channel= volumeInsideChannel / channel->getArea();
        T dropletResistance = computeDropletResistance_cyl(dynamicViscosity, droplet->getViscosity(), droplet_length_inside_channel, channel->getRadius(), H_inf);


        //if(dropletResistance < 0.0){
            std::cout << "droplet length: \t" << droplet_length << std::endl;
            std::cout << "droplet length inside channel: \t" << droplet_length_inside_channel << std::endl;
            std::cout << "droplet radius: \t" << droplet_radius << std::endl;
            std::cout << "volume inside channel: \t" << volumeInsideChannel<< std::endl;
            std::cout << "a: \t\t" << 12 << std::endl;
            std::cout << "continuous phase viscosity: \t" << this->continuousPhaseViscosity << std::endl;
            std::cout << "channel radius: \t" << channel->getRadius() << std::endl;
            std::cout << "uniform thick film: \t" << H_inf << std::endl;
            //throw std::invalid_argument("Negative droplet resistance. Droplet " + std::to_string(droplet->getId()) + " has a resistance of " + std::to_string(dropletResistance));
        //}

        if (dropletResistance < 0.0 ) {
            throw std::invalid_argument(
                "Invalid droplet resistance: Droplet " +
                std::to_string(droplet->getId()) +
                " has resistance = " + std::to_string(dropletResistance));
        }
        return dropletResistance;
    }
}

template<typename T>
T ResistanceModelBiPoiseuille<T>::computeDropletResistance_rect(T lambda, T dropletviscosity, T droplet_length_inside_channel, T channelradius, T uniform_H) const{
    T num = 3.0 * dropletviscosity * droplet_length_inside_channel;
    T den = uniform_H * ((2 * channelradius * channelradius) - ((4 - (3 * lambda))* uniform_H * channelradius) + (2*(1 - lambda)*uniform_H*uniform_H));
    T result = num/den;
    return result;
}

template<typename T>
T ResistanceModelBiPoiseuille<T>::computeDropletResistance_cyl(T lambda, T dropletviscosity, T droplet_length_inside_channel, T channelradius, T uniform_H) const{
    T num = 12.0 * dropletviscosity * droplet_length_inside_channel;
    T den = M_PI * channelradius * uniform_H * ((3.0 * channelradius * channelradius) + (6.0 * lambda * uniform_H * channelradius) + (4.0 * lambda * uniform_H * uniform_H));
    return num/den;
}

template<typename T>
T ResistanceModelBiPoiseuille<T>::computeH_inf(arch::Channel<T> const* const channel, Droplet<T>* droplet) const {
    T lambda = droplet->getViscosity() / this->continuousPhaseViscosity;                       //dynamic viscosity
    T Ca = droplet->getCa();
    // c, c1, c2, c3, c4, d1, d2, d3 for RECTANGULAR (idx 0) and CYLINDRICAL (idx 1)
    static const std::array<std::array<T, 8>, 2> constants = {{
        { T(2.52), T(98.76), T(146.42), T(70.42), T(1.45), T(45.04), T(89.61), T(77.03) }, // rectangular
        { T(2.36), T(2.21),  T(111.25), T(33.84), T(1.37), T(0.89),  T(44.86), T(54.50) }  // cylindrical
    }};
    T r_hydro;
    if (channel->getChannelShape() != arch::ChannelShape::CYLINDRICAL) {                      //defined rectangular or default rectangular channel
        T h = channel->getHeight();
        T w = channel->getWidth();
        if (h==w) { 
            r_hydro = w / 2.0;
        }else {
            r_hydro = (w * h) / (w + h);
        }
    } else {
        r_hydro = channel->getRadius();
    }
    

    const std::size_t idx = (channel->getChannelShape() == arch::ChannelShape::CYLINDRICAL) ? 1u : 0u;
    const auto& C = constants[idx];

    const T c  = C[0];
    const T c1 = C[1];
    const T c2 = C[2];
    const T c3 = C[3];
    const T c4 = C[4];
    const T d1 = C[5];
    const T d2 = C[6];
    const T d3 = C[7];

    T two_bar = pow(2.0, 0.6666);
    std::cout << "T raised to power 2/3 = " <<two_bar << std::endl;
    T tan_coeff = two_bar - 1;
    // Compute P_bar(lambda)
    T logVal = 1.28*log10(lambda);
    T P_bar= (0.3215) * (1 + two_bar +
                   (tan_coeff * tanh(logVal- c)));
    std::cout<< "P_bar = " << P_bar <<std::endl;
    // Q is a fitting function.
    T num = c1 + (c2*lambda) + (c3*pow(lambda, 2.0)) + (c4*pow(lambda, 3.0));
    T den = d1 + (d2*lambda) + (d3*pow(lambda, 2.0)) + (pow(lambda, 3.0));
    T Q = num/den;
    std::cout<< "Q function = "<< Q << std::endl;
    T H = 0;

    // Compute H_inf / R
    T Ca_= pow(3.0 * Ca, 2.0/3.0);
    if (Ca < 1e-3){
        H = P_bar * Ca_ * r_hydro;
    } else if ((Ca >= 1e-3) && (Ca <=1.0)){
        H = ((P_bar * Ca_ ) / (1 + (P_bar * Q * Ca_))) * r_hydro;
    }
    return H;
}

template<typename T>
T ResistanceModelBiPoiseuille<T>::getRelativeDropletVelocity(arch::Channel<T> const* const channel, Droplet<T>* droplet) const{
    T dynamicViscosity= droplet->getViscosity() / this->continuousPhaseViscosity;
    T H_inf = computeH_inf(channel, droplet);
    if (channel->getChannelShape() != arch::ChannelShape::CYLINDRICAL) {
        T h = channel->getHeight();
        T w = channel->getWidth();
        T R;
        if (h==w) { 
            R = w / 2.0;
        }else {
            R = (w * h) / (w + h);
        }
        T R_drop = R - H_inf;
        T num = R * ((2.0 * pow(R_drop, 2.0)) + (3.0 * H_inf * ((2.0 * R) - H_inf)) * dynamicViscosity);
        T den = (2.0 * pow(R_drop, 3.0)) + (2.0 * H_inf * (3.0 * pow(R, 2.0) - (3.0 * H_inf * R) + pow(H_inf, 2)) * dynamicViscosity); 
        T U_inf_by_U_d = den/num; // we do this to match the balestra graphs
        T relative_velocity = 1 - U_inf_by_U_d; //graph 16 balestra
        //T beta = num / den ;  /// = U_d_by_U_inffor jakiela graphs
        return relative_velocity;
    } else {
        T R = channel->getRadius();
        T R_drop = R - H_inf;
        T num = pow(R, 2.0) * (pow(R_drop, 2.0) + (2.0 * H_inf * ((2.0 * R) - H_inf) * dynamicViscosity));
        T den = pow(R_drop, 4.0) + (H_inf * ((2.0 * R) - H_inf) * ((2.0 * pow(R, 2.0)) - (2.0 * H_inf * R) + pow(H_inf, 2.0)) * dynamicViscosity);
        T U_inf_by_U_d = den/num; // we do this to match the balestra graphs
        T relative_velocity = 1 - U_inf_by_U_d; //graph 16 balestra
        //T beta = num / den ;  /// = U_d_by_U_inffor jakiela graphs
        return relative_velocity;
    }
}
}// namespace sim