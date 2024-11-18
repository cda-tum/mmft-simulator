#include "DynamicDamping.h"

namespace mmft {

template<typename T>
DynamicDampingScheme<T>::DynamicDampingScheme(int theta_, int size_) :
    Scheme<T>() 
{ 
    /*
    this->theta.try_emplace(0,theta_);
    for (int i = 0; i < size_; i++) {
        this->alpha.try_emplace(i, 0.0);
    }
    */
}

template<typename T>
void DynamicDampingScheme<T>::compute() {

    if (this->x_star.size() == this->x.size() && this->x.size() == x_prev.size()) {
        // Central difference scheme for second order derivative
        Eigen::VectorXd d2x_dt2 = this->x_star.array() - 2*this->x.array() + x_prev.array();
        factor = (1.0/(1.0 + 10*(d2x_dt2.array().abs())));
        x_prev = this->x;
        this->x = (1 - factor.array()) * this->x.array() + factor.array() * this->x_star.array();
    } else {
        x_prev = this->x;
        this->x = this->x_star;
    }

}

}   // namespace mmft