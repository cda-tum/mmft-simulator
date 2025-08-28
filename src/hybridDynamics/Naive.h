/**
 * @file Naive.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class CfdModule;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class HybridContinuous;

}

namespace mmft{

/**
 * @brief The Naive Scheme is an update scheme that sets the relaxation factor of an iterative update scheme to a 
 * given constant for all nodes.
 */
template<typename T>
class NaiveScheme final : public Scheme<T> {

private:

    /**
     * @brief Constructor of the Naive Scheme with provided constants.
     * @param[in] modules The map of modules with boundary nodes upon which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    NaiveScheme(const std::shared_ptr<arch::CfdModule<T>> module, T alpha, T beta, int theta);

    /**
     * @brief Constructor of the Naive Scheme with provided constants.
     * @param[in] modules The map of modules with boundary nodes upon which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    NaiveScheme(const std::shared_ptr<arch::CfdModule<T>> module, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Constructor of the Naive Scheme with provided constants.
     * @param[in] nodeIds The Ids of the set of nodes on which this scheme acts
     * @param[in] moduleIds The Ids of the set of modules on which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    NaiveScheme(std::vector<int> nodeIds, std::vector<int> moduleIds, T alpha, T beta, int theta);

    /**
     * @brief Constructor of the Naive Scheme with provided constants. The ids of the nodes and modules
     * are passed in the maps.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    NaiveScheme(std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Returns whether this scheme is naive or not
     * @returns true
     * @note This function overrides Scheme::isNaive() which defaults to false
     */
    bool isNaive() const override { return true; }

    // Friend class definition, because the Scheme constructors are private
    friend class sim::HybridContinuous<T>;

};

}   // namespace mmft