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

}   // namespace sim