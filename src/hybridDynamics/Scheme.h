/**
 * @file Scheme.h
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

namespace mmft{

/**
 * @brief "Virtual" definition of a general update scheme that functions as the base definition for other 
 * update schemes. An update scheme defines the update rules between Abstract and CFD for Hybrid simulation.
 * The "virtuality" stems from the protected specifier for all constructors.
 */
template<typename T>
class Scheme {
private:

    std::unordered_map<int, T> alpha;       // relaxation value for pressure value updates on the Abstract-CFD interface nodes <nodeId, alpha>
    std::unordered_map<int, T> beta;        // relaxation value for pressure value flow rate on the Abstract-CFD interface nodes <nodeId, beta>
    int theta;                              // Amount of LBM collide and stream iterations between update steps

protected:

    /**
     * @brief Default constructor of a Scheme.
     */
    Scheme();

    /**
     * @brief Constructor of a Scheme with the required maps. The ids of the nodes and modules are passed in the maps.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    Scheme(std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Constructor of a Scheme with provided constants. The constants are set for all provided modules (and, hence, nodes).
     * @param[in] modules The map of modules with boundary nodes upon which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    Scheme(const std::shared_ptr<arch::CfdModule<T>> module, T alpha, T beta, int theta);

    /**
     * @brief Constructor of a Scheme with provided constants. The constants are set for all provided modules (and, hence, nodes).
     * @param[in] modules The map of modules with boundary nodes upon which this scheme acts.
     * @param[in] alpha The relaxation value for the pressure value update.
     * @param[in] beta The relaxation value of the flow rate value update.
     * @param[in] theta The amount of LBM stream and collide cycles between updates for a module.
     */
    Scheme(const std::shared_ptr<arch::CfdModule<T>> module, std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

public:

    /**
     * @brief Sets the relaxation value for pressure updates for all nodes to the provided value.
     * @param[in] alpha The relaxation value for pressure updates.
     */
    void setAlpha(T alpha);

    /**
     * @brief Sets the relaxation value for pressure updates for node with nodeId to the provided value.
     * @param[in] nodeId The id of the node that is to be updated.
     * @param[in] alpha The relaxation value for pressure updates.
     */
    void setAlpha(int nodeId, T alpha);

    /**
     * @brief Sets the relaxation value for pressure updates for all nodes to the provided value.
     * @param[in] alpha The map of relaxation values and node Ids. <nodeId, alpha>
     */
    void setAlpha(std::unordered_map<int, T> alpha);

    /**
     * @brief Sets the relaxation value for flow rate updates for all nodes to the provided value.
     * @param[in] beta The relaxation value for flow rate updates.
     */
    void setBeta(T beta);

    /**
     * @brief Sets the relaxation value for flow rate updates for node with nodeId to the provided value.
     * @param[in] nodeId The id of the node that is to be updated.
     * @param[in] beta The relaxation value for flow rate updates.
     */
    void setBeta(int nodeId, T beta);

    /**
     * @brief Sets the relaxation value for flow rate updates for all nodes to the provided value.
     * @param[in] beta The map of relaxation values and node Ids. <nodeId, beta>
     */
    void setBeta(std::unordered_map<int, T> beta);

    /**
     * @brief Sets the number of LBM iterations between update steps for all modules to the provided value.
     * @param[in] theta The number of LBM iterations between update steps.
     */
    void setTheta(int theta);

    /** 
     * @brief Sets the update parameters for all nodes for this scheme.
     * @param[in] alpha The alpha update value.
     * @param[in] beta The beta update value.
     * @param[in] theta The number of LBM iterations between update steps.
     */
    void setParameters(T alpha, T beta, int theta);

    /** 
     * @brief Sets the update parameters for all nodes for this scheme, with the given node-value mapping.
     * @param[in] alpha The <nodeId, alpha update value> mapping.
     * @param[in] beta The <nodeId, beta update value> mapping.
     * @param[in] theta The number of LBM iterations between update steps.
     */
    void setParameters(std::unordered_map<int, T> alpha, std::unordered_map<int, T> beta, int theta);

    /**
     * @brief Returns the relaxation value for pressure updates for node with nodeId.
     * @param[in] nodeId The node for which alpha is returned.
     * @returns alpha.
     */
    T getAlpha(int nodeId) const;

    /**
     * @brief Returns the relaxation value for pressure updates for all nodes.
     * @returns Map of alpha values. <nodeId, alpha>
     */
    const std::unordered_map<int, T>& getAlpha() const;

    /**
     * @brief Returns the relaxation value for flow rate updates for node with nodeId.
     * @param[in] nodeId The node for which beta is returned.
     * @returns beta.
     */
    T getBeta(int nodeId) const;

    /**
     * @brief Returns the relaxation value for flow rate updates for all nodes.
     * @returns Map of beta values. <nodeId, beta>
     */
    const std::unordered_map<int, T>& getBeta() const;

    /**
     * @brief Returns the number of LBM iterations between update steps for module with moduleId.
     * @param[in] moduleId The module for which theta is returned.
     * @returns theta.
     */
    int getTheta() const;

    /**
     * @brief Returns whether this scheme is naive or not
     * @returns false
     * @note This function is overriden by NaiveScheme::isNaive() which defaults to true
     */
    virtual bool isNaive() const { return false; }

    virtual ~Scheme() = default;

};

}   // namespace mmft