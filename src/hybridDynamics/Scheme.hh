#include "Scheme.h"

namespace mmft {

template<typename T>
Scheme<T>::Scheme() { }

template<typename T>
Scheme<T>::Scheme(std::unordered_map<int, T> alpha_, std::unordered_map<int, T> beta_, int theta_) :
    alpha(alpha_), beta(beta_), theta(theta_) { }

template<typename T>
Scheme<T>::Scheme(const std::shared_ptr<arch::CfdModule<T>> module_, T alpha_, T beta_, int theta_) 
{
    for (auto& [nodeId, node] : module_->getNodes()) {
        alpha.try_emplace(nodeId, alpha_);
        beta.try_emplace(nodeId, beta_);
    }
    this->theta = theta_;
}

template<typename T>
Scheme<T>::Scheme(const std::shared_ptr<arch::CfdModule<T>> module_, std::unordered_map<int, T> alpha_, std::unordered_map<int, T> beta_, int theta_) :
    alpha(alpha_), beta(beta_)
{
    this->theta = theta_;
}

template<typename T>
void Scheme<T>::setAlpha(T alpha_) {
    for (auto& [nodeId, a] : alpha) {
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
void Scheme<T>::setBeta(T beta_) {
    for (auto& [nodeId, b] : beta) {
        b = beta_;
    }
}

template<typename T>
void Scheme<T>::setBeta(int nodeId_, T beta_) {
    beta.at(nodeId_) = beta_;
}

template<typename T>
void Scheme<T>::setBeta(std::unordered_map<int, T> beta_) {
    beta = beta_;
}

template<typename T>
void Scheme<T>::setTheta(int theta_) {
    theta = theta_;
}

template<typename T>
void Scheme<T>::setParameters(T alpha_, T beta_, int theta_) {
    setAlpha(alpha_);
    setBeta(beta_);
    setTheta(theta_);
}

template<typename T>
void Scheme<T>::setParameters(std::unordered_map<int, T> alpha_, std::unordered_map<int, T> beta_, int theta_) {
    setAlpha(alpha_);
    setBeta(beta_);
    setTheta(theta_);
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
T Scheme<T>::getBeta(int nodeId_) const {
    return beta.at(nodeId_);
}

template<typename T>
const std::unordered_map<int, T>& Scheme<T>::getBeta() const {
    return beta;
}

template<typename T>
int Scheme<T>::getTheta() const {
    return theta;
}

}   // namespace mmft
