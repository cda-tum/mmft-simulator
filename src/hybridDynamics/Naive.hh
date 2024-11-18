#include "Naive.h"

namespace mmft {

template<typename T>
NaiveScheme<T>::NaiveScheme(const std::unordered_map<int, std::shared_ptr<arch::Module<T>>>& modules, T alpha, T beta, int theta) :
    Scheme<T>(modules, alpha, beta, theta) { }

template<typename T>
NaiveScheme<T>::NaiveScheme(const std::shared_ptr<arch::Module<T>> module, T alpha, T beta, int theta) :
    Scheme<T>(module, alpha, beta, theta) { }

template<typename T>
NaiveScheme<T>::NaiveScheme(const std::shared_ptr<arch::Module<T>> module, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta) :
    Scheme<T>(module, alpha, beta, theta) { }

template<typename T>
NaiveScheme<T>::NaiveScheme(std::vector<int> nodeIds, std::vector<int> moduleIds, T alpha, T beta, int theta) :
    Scheme<T>() 
{
    for (auto nodeId : nodeIds) {
        this->alpha.try_emplace(nodeId, alpha);
    }
    for (auto nodeId : nodeIds) {
        this->beta.try_emplace(nodeId, beta);
    }
    for (auto moduleId : moduleIds) {
        this->alpha.try_emplace(moduleId, theta);
    }
}

template<typename T>
NaiveScheme<T>::NaiveScheme(std::unordered_map<int, T> alpha_, std::unordered_map<int, T> beta_, std::unordered_map<int, int> theta_) :
    Scheme<T>(alpha_, beta_, theta_) { }

template<typename T>
void NaiveScheme<T>::compute() { 
    if (this->x.size() == this->x_star.size()) {
        this->x = (1 - this->alpha.array()) * this->x.array() + this->alpha.array() * this->x_star.array();
    } else {
        // The first iteration the vector lengths might differ
        this->x = this->x_star;
    }
}

}   // namespace mmft