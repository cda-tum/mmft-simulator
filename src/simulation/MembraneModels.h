/**
 * @file MembraneModels.h
 */

#pragma once

#include <unordered_map>

namespace sim { 

/**
 * @brief Virtual class that describes the basic functionality for mixing models.
*/
template<typename T>
class MembraneModel {
protected:

public:

    /**
     * @brief Constructor of a membrane model.
    */
    MembraneModel();

};

/**
 * @brief Class that describes the poreResistance membrane model. This membrane model derives the permeability from pore geometry.
*/
template<typename T>
class PermeabilityMembraneModel : public MembraneModel<T> {

private:

public:

    /**
     * @brief Constructor of a poreResistance membrane model.
    */
    PermeabilityMembraneModel();

};

/**
 * @brief Class that describes the poreResistance membrane model. This membrane model derives the permeability from pore geometry.
*/
template<typename T>
class PoreResistanceMembraneModel : public MembraneModel<T> {

private:

public:

    /**
     * @brief Constructor of a poreResistance membrane model.
    */
    PoreResistanceMembraneModel();

};

}   // namespace sim