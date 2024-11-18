#include "LinearDecoupling.h"

namespace mmft {

template<typename T>
LinearDecouplingScheme<T>::LinearDecouplingScheme() :
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
void LinearDecouplingScheme<T>::compute() {

    if (this->x_star.size() == this->x.size() && this->x.size() == x_prev.size()) {
        // Central difference scheme for second order derivative
        Eigen::VectorXd d2x_dt2 = this->x_star.array() - 2*this->x.array() + x_prev.array();

        lambda = d2x_dt2.array()/this->x.array();

        int n = 0;
        T l_sum = 0.0;
        T w_sum = 0.0;
        T inv_l_sum = 0.0;
        T inv_w_sum = 0.0;

        for (int i = 0; i < lambda.size(); i++) {
            if (lambda[i] > 1e-12) {
                T w = sqrt(lambda[i]);
                l_sum += lambda[i];
                w_sum += w;
                inv_l_sum += 1.0/lambda[i];
                inv_w_sum += 1.0/w;
                n++;
            }
        }


        A(0,0) = 0.25*inv_l_sum;
        A(1,0) = 0.25*n;
        A(0,1) = 0.25*n;
        A(1,1) = 0.25*l_sum;

        b(0) = 0.5*inv_w_sum*zeta;
        b(1) = 0.5*w_sum*zeta;

        ab = A.colPivHouseholderQr().solve(b);

        T factor_a = (1.0)/(1.0 + ab[0]);
        T factor_b = (ab[1] - 1.0)/(1.0 + ab[0]);
        T factor_c = 1.0 + (-1.0)/(1.0 + ab[0]);

        std::cout << "alpha: " << ab[0] << "\tbeta: " << ab[1] << std::endl;
        std::cout << "a: " << factor_a << "\tb: " << factor_b << "\tc: " << factor_c << std::endl;

        factor = (1.0/(1.0 + 10*(d2x_dt2.array().abs())));

        x_prev = this->x;
        
        this->x = factor_a*this->x_star.array() + factor_b*lambda.array()*this->x_star.array() + factor_c*this->x.array();
        //this->x = (1 - factor.array()) * this->x.array() + factor.array() * this->x_star.array();
    } else {
        x_prev = this->x;
        this->x = this->x_star;
    }

}

}   // namespace mmft