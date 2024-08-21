/**
 * @file Scheme.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace arch {

template<typename T>
class Module;

}

namespace mmft{

/**
 * @brief Update scheme that defines the update rules between Abstract and CFD for Hybrid simulation.
 */
template<typename T>
class NaiveScheme : public Scheme<T> {

public:

    NaiveScheme(const std::unordered_map<int, std::shared_ptr<arch::Module<T>>>& modules, T alpha, T beta, int theta);

    NaiveScheme(std::vector<int> nodeIds, std::vector<int> moduleIds, T alpha, T beta, int theta);

    NaiveScheme(std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, std::unordered_map<int, int> theta);

};

}   // namespace mmft