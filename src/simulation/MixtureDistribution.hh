#include "MixtureDistribution.h"

namespace sim {

template<typename T>
DistributionGrid<T>::DistributionGrid() { }

template<typename T>
MixtureDistribution<T>::MixtureDistribution(int id, std::unordered_map<int, Specie<T>*> species) : 
                    id(id), species(species) { }



template<typename T>
Mixture<int>* MixtureDistribution::getId() const {
    return id;
}

template<typename T>
arch::RectangularChannel<T>* MixtureDistribution<T>::getWidth() {
    return channelWidth;
}

template<typename T>
arch::RectangularChannel<T>* MixtureDistribution<T>::getWidth() {
    return channelLength;
}

struct DistributionGrid {
    std::vector<T> gridPoints;
    std::vector<T> concentrations;

    /**
     * @brief Constructs a mixture position
    */
    DistributionGrid();
};

double MixtureDistribution::getConcentrationChange(double resistance, double timeStep, double mixtureLength, double concentrationDifference, double concentrationTank, double concentrationChannel, double currTime) const {  //TODO remove timeStep
#define f(time, concentration, permeability) (permeability * concentration)
    auto* membraneChannel = this->getChannel();
    double permeability = 1 / resistance;
    //TODO
    //double permeability = 1.5e-6;  // From extended Figure 4 in Ronaldson-Bouchard 2022 [m/s]
    //double permeability = 3.8e-6;  // m/s Diffusion Example
    //END
    // Ficks law
    // double flux = permeability * concentrationDifference;  // [mol/s]
    // return flux * timeStep;

    //for (i = 0; i < n; i++) { // das ist unser zeitschritt loop
    double k1 = timeStep * (f(currTime, concentrationDifference, permeability));
    double k2 = timeStep * (f((currTime + timeStep / 2), (concentrationDifference + k1 / 2), permeability));
    double k3 = timeStep * (f((currTime + timeStep / 2), (concentrationDifference + k2 / 2), permeability));
    double k4 = timeStep * (f((currTime + timeStep), (concentrationDifference + k3), permeability));
    double concentrationChange = (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    return concentrationChange;
    //}
}


}   // namespace sim