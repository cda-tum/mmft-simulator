#include "Scheme.h"

namespace mmft {

template<typename T>
Scheme<T>::Scheme() { }

template<typename T>
Scheme<T>::Scheme(std::unordered_map<int, T> alpha_, std::unordered_map<int, int> theta_) :
    alpha(alpha_), theta(theta_) { }

template<typename T>
void Scheme<T>::setAlpha(T alpha_) {
    for (auto a : alpha) {
        a = alpha_;
    }
}

template<typename T>
void Scheme<T>::setAlpha(int nodeId_, T alpha_) {
    alpha.at(nodeId_) = alpha_;
}

template<typename T>
void Scheme<T>::setAlpha(std::unordered_map<int, T> alpha_) {
    alpha = alpha_;
}

template<typename T>
void Scheme<T>::setTheta(int theta_) {
    for (auto t : theta) {
        t = theta_;
    }
}

template<typename T>
void Scheme<T>::setTheta(int moduleId_, int theta_) {
    theta.at(moduleId_) = theta_;
}

template<typename T>
void Scheme<T>::setTheta(std::unordered_map<int, int> theta_) {
    theta = theta_;
}

template<typename T>
T Scheme<T>::getAlpha(int nodeId_) const {
    return alpha.at(nodeId_);
}

template<typename T>
const std::unordered_map<int, T>& Scheme<T>::getAlpha() const {
    return alpha;
}

template<typename T>
int Scheme<T>::getTheta(int moduleId_) const {
    return theta.at(moduleId_);
}

template<typename T>
const std::unordered_map<int, int>& Scheme<T>::getTheta() const {
    return theta;
}

}   // namespace mmft