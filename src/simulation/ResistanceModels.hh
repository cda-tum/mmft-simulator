#include "ResistanceModels.h"

namespace sim {

// ### ResistanceModel ###
template<typename T>
ResistanceModel<T>::ResistanceModel(T continuousPhaseViscosity_) : continuousPhaseViscosity(continuousPhaseViscosity_) {}

// ### ResistanceModel1D ###
template<typename T>
ResistanceModel1D<T>::ResistanceModel1D(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModel1D<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    T a = computeFactorA(channel);

    return channel->getLength() * a * this->continuousPhaseViscosity / (channel->getWidth() * pow(channel->getHeight(), 3));
}

template<typename T>
T ResistanceModel1D<T>::getDropletLength(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const {
    T len_ = droplet->getVolume() / (channel->getWidth() * channel->getHeight());
    return len_;
}

template<typename T>
T ResistanceModel1D<T>::computeFactorA(arch::RectangularChannel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    return 12. / (1. - 192. * height * tanh(M_PI * width / (2. * height)) / (pow(M_PI, 5.) * width));
}

template<typename T>
T ResistanceModel1D<T>::getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
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
T ResistanceModel1D<T>::getRelativeDropletVelocity(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

// ### ResistanceModelPoiseuille ###
template<typename T>
ResistanceModelPoiseuille<T>::ResistanceModelPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelPoiseuille<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const {
    
    return (12.0 * this->continuousPhaseViscosity * channel->getLength()/channel->getWidth() * pow(channel->getHeight(), 3.0));
    
}

template<typename T>
T ResistanceModelPoiseuille<T>::computeFactorA(arch::RectangularChannel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    T a = 1 - (192 * height)/(pow(M_PI, 5) * width) * tanh ((M_PI * width)/(2 * height));
    return a;
}

template<typename T>
T ResistanceModelPoiseuille<T>::getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}
template<typename T>
T ResistanceModelPoiseuille<T>::getDropletLength(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const {
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}

template<typename T>
T ResistanceModelPoiseuille<T>::getRelativeDropletVelocity(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const{
    throw std::invalid_argument("The resistance model is not compatible with droplet simulations.");
}



//Resistance model PlanarPoiseuille : for poiseuille flow in rectangular channel 
template<typename T>
ResistanceModelPlanarPoiseuille<T>::ResistanceModelPlanarPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::getChannelResistance(arch::RectangularChannel<T> const* const channel) const{
    std::cout<< "correct function found" << std::endl;
    T h = channel->getHeight();
    T w = channel->getWidth();
    if (h==w){
        std::cout<<"square case initiated" <<std::endl;
        T r_hydro = (w * h) /(w + h);
        return (8.0 * this->continuousPhaseViscosity * channel->getLength())/(M_PI * pow(r_hydro, 4.0));
        
    } else if (h/w < 0.01) {
        return (12.0 * this->continuousPhaseViscosity * channel->getLength())/(w * pow(h, 3));
    } else {
        T a = computeFactorA(channel);
        return (12.0 * this->continuousPhaseViscosity * channel->getLength())/(a * w * pow(h, 3));
    }
}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::getDropletLength(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const {
    T len_ = droplet->getVolume() / (channel->getWidth() * channel->getHeight());
    return len_;
}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::computeFactorA(arch::RectangularChannel<T> const* const channel) const {
    T width = channel->getWidth();
    T height = channel->getHeight();
    T a = 1 - (192 * height)/(pow(M_PI, 5) * width) * tanh ((M_PI * width)/(2 * height));
    return a;
}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::getDropletResistance(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const{
    std::cout<< "droplet resistance calculation initiated" <<std::endl;
    T h = channel->getHeight();
    T w = channel->getWidth();
    T r_hydro;
    if (h==w) { 
        r_hydro = (w * h)/(w + h);
    }else {
        r_hydro = w / 2.0;
    }
    T dynamicViscosity= droplet->getViscosity() / this->continuousPhaseViscosity;
    T Ca_inf = droplet->getCa();
    T H_inf = computeH_inf_rect(dynamicViscosity, Ca_inf, r_hydro);
    T droplet_radius= r_hydro - H_inf;
    T droplet_length = volumeInsideChannel / (channel->getHeight() * channel->getWidth());
    //T continuousResistance = computeContinuousPhaseResistance_rect(this->continuousPhaseViscosity, droplet_length, channel->getWidth(), channel->getHeight());

    T dropletResistance = computeDropletResistance_rect(dynamicViscosity, droplet->getViscosity(), droplet_length, r_hydro, H_inf);
    //T netResistance = dropletResistance - continuousResistance
    //if (dropletResistance < 0.0) {
        std::cout << "droplet length: \t" << droplet_length << std::endl;
        std::cout << "volume inside channel: \t" << volumeInsideChannel << std::endl;
        std::cout << "a: \t\t" << 3 << std::endl;
        std::cout << "continuous phase viscosity: \t" << this->continuousPhaseViscosity << std::endl;
        std::cout << "channel height: \t" << channel->getHeight() << std::endl;
        std::cout << "channel width: \t" << channel->getWidth() << std::endl;
        std::cout << "uniform thick film: \t" << H_inf << std::endl;
        //throw std::invalid_argument("Negative droplet resistance. Droplet " + std::to_string(droplet->getId()) + " has a resistance of " + std::to_string(dropletResistance));
    //}

    if (dropletResistance < 0.0 || std::isnan(dropletResistance) || std::isinf(dropletResistance)) {
        throw std::invalid_argument(
            "Invalid droplet resistance: Droplet " +
            std::to_string(droplet->getId()) +
            " has resistance = " + std::to_string(dropletResistance));
    }
    return dropletResistance;
    /**if (netResistance < 0.0 || std::isnan(netResistance) || std::isinf(netResistance)) {
        throw std::invalid_argument(
            "Invalid droplet resistance: Droplet " +
            std::to_string(droplet->getId()) +
            " has resistance = " + std::to_string(dropletResistance));
    }
    return netResistance;
    */
}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::getRelativeDropletVelocity(arch::RectangularChannel<T> const* const channel, Droplet<T>* droplet) const{
    T dynamicViscosity= droplet->getViscosity() / this->continuousPhaseViscosity;
    T Ca_inf = droplet->getCa();
    T h = channel->getHeight();
    T w = channel->getWidth();
    T R;
    if (h==w) { 
        R = (w * h)/(w + h);
    }else {
        R = w / 2.0;
    }
    T H_inf = computeH_inf_rect(dynamicViscosity, Ca_inf, R);
    T R_drop = R - H_inf;
    T num = R * ((2.0 * pow(R_drop, 2.0)) + 3.0 * H_inf * (2.0 * R - H_inf) * dynamicViscosity);
    T den = (2.0 * pow(R_drop, 3.0)) + 2.0 * H_inf * (3.0 * pow(R, 2.0) - (3.0 * H_inf * R) + pow(H_inf, 2)) * dynamicViscosity; 
    T U_inf_by_U_d = den/num; // we do this to match the balestra graphs
    T relative_velocity = 1 - U_inf_by_U_d; //graph 16 balestra
    return relative_velocity;
}

//template<typename T>
//T ResistanceModelPoiseuille<T>::computeContinuousPhaseResistance_rect(T viscosity, T droplet_length, T channel_width, T channel_height) const {
//    T resistance = (12.0 * viscosity * droplet_length) / (channel_width * pow(channel_height, 3));

//    return resistance;
//}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::computeH_inf_rect(T lambda, T Ca, T radius) const {
    std::cout << "H_inf being computed" << std::endl;  
    std::cout<< "lambda = " <<lambda<<std::endl;

    // Compute P_bar(lambda)
    T P_bar= (0.643 / 2.0) * (1 + pow(2.0, 2.0/3.0) +
                   (pow(2.0, 2.0/3.0) - 1) * tanh((1.28 * log10(lambda))- 2.52));
    std::cout<< "P_bar = " << P_bar <<std::endl;
    // Q is a fitting function.
    T c1 = 98.76, c2 = 146.42, c3 = 70.42, c4= 1.45;
    T d1 = 45.04, d2 = 89.61, d3 =77.03;
    T num = c1 + (c2*lambda) + (c3*pow(lambda, 2.0)) + (c4*pow(lambda, 3.0));
    T den = d1 + (d2*lambda) + (d3*pow(lambda, 2.0)) + (pow(lambda, 3.0));
    T Q = num/den;
    std::cout<<"Q = " << Q <<std::endl;
    T H;

    // Compute H_inf / R
    T Ca_= pow(3.0 * Ca, 2.0/3.0);
    std::cout<< "Ca_ = "<< Ca_<< std::endl;

    if (Ca < 1e-3){
        H = P_bar * Ca_ * radius;
        std::cout<< "Ca<1e-3; H = "<< H << std::endl;
    } else if ((Ca >= 1e-3) && (Ca <=1.0)){
        H = ((P_bar * Ca_ ) / (1 + (P_bar * Q * Ca_))) * radius;
        std::cout<< "Ca>01e-3; H = "<< H << std::endl;
    }
    std::cout<< "H being returned =" << H << std::endl;
    return H;
}

template<typename T>
T ResistanceModelPlanarPoiseuille<T>::computeDropletResistance_rect(T lambda, T dropletviscosity, T droplet_length, T channelradius, T uniform_H) const{
    std::cout<< "compute droplet resistance initiated" <<std::endl;
    T num = 3.0 * dropletviscosity * droplet_length;
    std::cout << "num = "<< num <<std::endl;
    //T den = uniform_H * ((2.0 * pow(channelradius, 2.0)) -  ((4.0 - (3.0 * lambda)) * uniform_H * channelradius) + (2.0 * (1.0 - lambda) * pow(uniform_H, 2.0)));
    T den = uniform_H * channelradius * ((2.0 * channelradius) -  ((4.0 - (3.0 * lambda)) * uniform_H)); 
    std::cout << "den = "<< den <<std::endl;
    T result = num/den;
    std::cout<< "droplet resistance being returned = "<<result<<std::endl;
    return result;
}


//Resistance model CylindricalPoiseuille : for poiseuille flow in cylindrical channel

template<typename T>
ResistanceModelCylindricalPoiseuille<T>::ResistanceModelCylindricalPoiseuille(T continuousPhaseViscosity_) : ResistanceModel<T>(continuousPhaseViscosity_) {}

template <typename T>
T ResistanceModelCylindricalPoiseuille<T>::getChannelResistance(arch::CylindricalChannel<T> const* const channel) const {

    T channelResistance = (8.0 * this->continuousPhaseViscosity * channel->getLength()) /(M_PI * pow(channel->radius, 4.0));
    return channelResistance;
}

template<typename T>
T ResistanceModelCylindricalPoiseuille<T>::getDropletLength(arch::CylindricalChannel<T> const* const channel, Droplet<T>* droplet) const {
    T len_ = droplet->getVolume() / M_PI * pow(channel->getRadius(), 2.0);
    return len_;
}

template<typename T>
T ResistanceModelCylindricalPoiseuille<T>::getDropletResistance(arch::CylindricalChannel<T> const* const channel, Droplet<T>* droplet, T volumeInsideChannel) const {
    
    T dynamicViscosity = droplet->getViscosity() / this-> continuousPhaseViscosity;
    T Ca_inf = droplet->getCa();
    T H_inf = computeH_inf_cyl(dynamicViscosity, Ca_inf, channel->getRadius());
    T droplet_radius = channel->getRadius() - H_inf; 
    T dropletLength = volumeInsideChannel / (M_PI * droplet_radius * droplet_radius);
    //T continuousPhaseResistance_deduct = computeContinuousPhaseResistance_cyl(this->continuousPhaseViscosity, dropletLength, channel->getRadius());
    T dropletResistance = computeDropletResistance_cyl(dynamicViscosity, droplet->getViscosity(), dropletLength, channel->getRadius(), H_inf);

    //T total_channel_resistance = dropletResistance - continuousPhaseResistance_deduct;

    //if(dropletResistance < 0.0){
        std::cout << "droplet length: \t" << dropletLength << std::endl;
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
    /** 
    if (dropletResistance < 0.0 ) {
        throw std::invalid_argument(
            "Invalid droplet resistance: Droplet " +
            std::to_string(droplet->getId()) +
            " has resistance = " + std::to_string(dropletResistance));
    }
    return total_channel_resistance;
    */
}

template<typename T>
T ResistanceModelCylindricalPoiseuille<T>::getRelativeDropletVelocity(arch::CylindricalChannel<T> const* const channel, Droplet<T>* droplet) const{
    T Ca_inf = droplet->getCa();
    T dynamicViscosity = droplet->getViscosity()/this->continuousPhaseViscosity;
    T H_inf = computeH_inf_cyl(dynamicViscosity, Ca_inf, channel->getRadius());
    T R = channel->getRadius();
    //T R_droplet = R - H_inf;
    T num = pow(R, 2.0) * (pow(R - H_inf, 2.0) + (2.0 * H_inf * (2.0 * R - H_inf) * dynamicViscosity));
    T den = pow(R - H_inf, 4.0) + (H_inf * (2.0 * R - H_inf) * (2.0 * pow(R, 2.0) - 2.0 * H_inf * R + pow(H_inf, 2.0)) * dynamicViscosity);
    return num/den;
}

template <typename T>
T ResistanceModelCylindricalPoiseuille<T>::computeContinuousPhaseResistance_cyl(T viscosity, T dropletlength,  T channelRadius) const {
    T continuousPhaseLength = dropletlength;
    T resistance = (8.0 * viscosity * continuousPhaseLength) / (M_PI * pow(channelRadius, 4));
    return resistance;
}  

template<typename T>
T ResistanceModelCylindricalPoiseuille<T>::computeH_inf_cyl(T lambda, T Ca, T radius) const {
        
    // Compute P_bar(lambda)
    T P_bar = (0.643 / 2.0) * (1 + pow(2.0, 2.0/3.0) +
                   (pow(2.0, 2.0/3.0) - 1) * tanh(1.28 * log10(lambda)- 2.36));

    // Q is fitting function
    T a1 = 2.21, a2 = 111.25, a3 = 33.84, a4 = 1.37;
    T b1 = 0.89, b2 = 44.86, b3 = 54.50;
    T num = a1 + a2*lambda + a3*pow(lambda, 2.0) + a4*pow(lambda, 3.0);
    T den = b1 + b2*lambda + b3*pow(lambda, 2.0) + pow(lambda, 3.0);
    T Q = num/den;
    T H_inf;
    // Compute H_inf / R
    T Ca_= pow(3.0 * Ca, 2.0/3.0);

    if (Ca < pow(10.0, -3.0)){
        T H_inf = P_bar * Ca_ * radius;
    } else if ((Ca >= pow(10.0, -3.0)) && (Ca <=1.0)){
        T H_inf = ((P_bar * Ca_ ) / (1 + (P_bar * Q * Ca_))) * radius;
    }
    return H_inf;
}

template<typename T>
T ResistanceModelCylindricalPoiseuille<T>::computeDropletResistance_cyl(T lambda, T dropletviscosity, T droplet_length, T channelradius, T uniform_H) const{
    T num = 12.0 * dropletviscosity * droplet_length;
    T den = M_PI * channelradius * uniform_H * ((3.0 * pow(channelradius, 2.0)) + (6.0 * lambda * uniform_H * channelradius) + (4.0 * lambda * pow(uniform_H, 2.0)));
    return num/den;
}




}// namespace sim