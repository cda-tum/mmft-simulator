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
class Scheme {

protected:

    std::unordered_map<int, T> alpha;       // update pressure values on the openings, i.e., Abstract-CFD interface nodes <nodeId, alpha>
    std::unordered_map<int, T> beta;        // update flow rate values on the openings, i.e., Abstract-CFD interface nodes <nodeId, alpha>
    std::unordered_map<int, int> theta;     // Amount of LBM collide and stream iterations between update steps <moduleId, theta>

    Scheme();

    Scheme(std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, std::unordered_map<int, int> theta);

    Scheme(const std::unordered_map<int, std::shared_ptr<arch::Module<T>>>& modules, T alpha, T beta, int theta);

public:

    void setAlpha(T alpha);

    void setAlpha(int nodeId, T alpha);

    void setAlpha(std::unordered_map<int, T> alpha);

    void setBeta(T beta);

    void setBeta(int nodeId, T beta);

    void setBeta(std::unordered_map<int, T> beta);

    void setTheta(int theta);

    void setTheta(int moduleId, int theta);

    void setTheta(std::unordered_map<int, int> theta);

    T getAlpha(int nodeId) const;

    const std::unordered_map<int, T>& getAlpha() const;

    T getBeta(int nodeId) const;

    const std::unordered_map<int, T>& getBeta() const;

    int getTheta(int moduleId) const;

    const std::unordered_map<int, int>& getTheta() const;

};

}   // namespace mmft