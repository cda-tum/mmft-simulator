/**
 * @file Naive.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace mmft{

/**
 * @brief The Naive Scheme is an update scheme that sets the relaxation factor of an iterative update scheme to a 
 * given constant for all nodes.
 */
template<typename T>
class LinearDecouplingScheme : public Scheme<T> {

private:

    Eigen::VectorXd x_prev;
    Eigen::VectorXd factor;
    Eigen::VectorXd lambda;
    Eigen::VectorXd omega;

    T zeta = 10;
    Eigen::VectorXd b = Eigen::VectorXd::Zero(2);
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(2,2);
    Eigen::VectorXd ab = Eigen::VectorXd::Zero(2);

public:
    /**
     * @brief Constructor of the Naive Scheme with provided constants.
     * @param[in] modules The map of modules with boundary nodes upon which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    LinearDecouplingScheme();

    /**
     * @brief Compute the update value alpha according to alpha = 1/(1 + 10*d2xdt2).
     */
    void compute() override;

};

}   // namespace mmft