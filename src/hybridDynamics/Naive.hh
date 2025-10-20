#include "Naive.h"

namespace mmft {

template<typename T>
NaiveScheme<T>::NaiveScheme(const std::shared_ptr<arch::CfdModule<T>> module, T alpha, T beta, int theta) :
    Scheme<T>(module, alpha, beta, theta) { }

template<typename T>
NaiveScheme<T>::NaiveScheme(const std::shared_ptr<arch::CfdModule<T>> module, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta) :
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
NaiveScheme<T>::NaiveScheme(std::unordered_map<int, T> alpha_, std::unordered_map<int, T> beta_, int theta_) :
    Scheme<T>(alpha_, beta_, theta_) { }

}   // namespace mmft