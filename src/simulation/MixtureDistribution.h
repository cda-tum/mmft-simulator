/**
 * @file Mixture.h
 */

#pragma once

#include <unordered_map>
#include <vector>

#include "../architecture/Channel.h"
#include "../architecture/ChannelPosition.h"

#include "Fluid.h"
#include "Specie.h"
#include "Mixture.h"

namespace sim {

template<typename T>
struct DistributionGrid {
    std::vector<T> gridPoints;
    std::vector<T> concentrations;

    /**
     * @brief Constructs a mixture position
    */
    DistributionGrid();
};

template<typename T>
class MixtureDistribution {
private:
    // int const id;
    T diffusionFactor;
    arch::RectangularChannel<T>* channel;
    T channelWidth;
    T channelLength;
    std::unordered_map<int, Specie<T>*> species;
    std::unordered_map<int, T> specieConcentrations;
    Mixture<int>* mixtureId;

public:
    /**
     * @brief Construct a new mixture out of a list of fluids and their concentration values.
     * 
     * @param id Id of the mixture.
     * @param fluidConcentrations Map of fluid id and fluid concentration pairs.
     * @param viscosity Viscosity of the mixture in Pas.
     * @param density Density of the mixture in kg/m^3.
     * @param largestMolecularSize Largest molecular size in that mixture in molecular size in m^3.
     */
    MixtureDistribution(arch::RectangularChannel<T>* channel, std::unordered_map<int, Specie<T>*> species);

    /**
     * @brief Get the id of this mixture
     * 
     * @return Unique identifier of the mixture.
     */
    Mixture<int>* getId() const;

    /**
     * @brief Get the id of this mixture
     * 
     * @return width of that specific channel.
     */
    arch::RectangularChannel<T>* getWidth() const;

        /**
     * @brief Get the id of this mixture
     * 
     * @return length of that channel.
     */
    arch::RectangularChannel<T>* getLength() const;

    struct DistributionGrid<T> getDistributionGridAtChannelEnd() const; //TODO

    double getConcentrationChange(double resistance, double timeStep, double mixtureLength, double concentrationDifference, double concentrationTank, double concentrationChannel, double currTime) const;


};

}   /// namespace sim