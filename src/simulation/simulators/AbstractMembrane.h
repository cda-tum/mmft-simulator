/**
 * @file AbstractMembrane.h
 */

#pragma once

#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace arch {

// Forward declared dependencies
template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class MembraneModel;

/**
 * @brief Class that conducts an abstract continuous simulation
 */
template<typename T>
class AbstractMembrane final : public AbstractMixing<T> {
private:
    std::unique_ptr<MembraneModel<T>> membraneModel = nullptr;                                          ///< The membrane model used for an OoC simulation.

    void assertInitialized() const override;

public:

    /**
     * @brief Constructor of the abstract membrane simulator object
     * @param[in] network Pointer to the network object, in which the simulation takes place
     */
    AbstractMembrane(std::shared_ptr<arch::Network<T>> network);

    /** TODO: HybridOocSimulation
     * Check if a Permeability membrane model is relevant
     */
    /** 
     * @brief Set the permeability membrane model to be used in the simulation.
     */
    // void setPermeabilityMembraneModel() {
    //     this->membraneModel = std::unique_ptr<PermeabilityMembraneModel<T>>(new PermeabilityMembraneModel<T>());
    // }

    /** TODO: HybridOocSimulation
     * Check if a PoreResistance membrane model is relevant
     */
    /**
     * @brief Set the pore resistance membrane model to be used in the simulation.
     */
    // void setPoreResistanceMembraneModel() {
    //     this->membraneModel = std::unique_ptr<PoreResistanceMembraneModel<T>>(new PoreResistanceMembraneModel<T>());
    // }

    /**
     * @brief Set the membrane model 0 to be used in the simulation.
     */
    void setMembraneModel0() {
        this->membraneModel = std::unique_ptr<MembraneModel0<T>>(new MembraneModel0<T>());
    }

    /**
     * @brief Set the membrane model 1 to be used in the simulation.
     */
    void setMembraneModel1() {
        this->membraneModel = std::unique_ptr<MembraneModel1<T>>(new MembraneModel1<T>());
    }

    /**
     * @brief Set the membrane model 2 to be used in the simulation.
     */
    void setMembraneModel2() {
        this->membraneModel = std::unique_ptr<MembraneModel2<T>>(new MembraneModel2<T>());
    }

    /**
     * @brief Set the membrane model 3 to be used in the simulation.
     */
    void setMembraneModel3() {
        this->membraneModel = std::unique_ptr<MembraneModel3<T>>(new MembraneModel3<T>());
    }

    /**
     * @brief Set the membrane model 4 to be used in the simulation.
     */
    void setMembraneModel4() {
        this->membraneModel = std::unique_ptr<MembraneModel4<T>>(new MembraneModel4<T>());
    }

    /**
     * @brief Set the membrane model 5 to be used in the simulation.
     */
    void setMembraneModel5() {
        this->membraneModel = std::unique_ptr<MembraneModel5<T>>(new MembraneModel5<T>());
    }

    /**
     * @brief Set the membrane model 6 to be used in the simulation.
     */
    void setMembraneModel6() {
        this->membraneModel = std::unique_ptr<MembraneModel6<T>>(new MembraneModel6<T>());
    }

    /**
     * @brief Set the membrane model 7 to be used in the simulation.
     */
    void setMembraneModel7() {
        this->membraneModel = std::unique_ptr<MembraneModel7<T>>(new MembraneModel7<T>());
    }

    /**
     * @brief Set the membrane model 8 to be used in the simulation.
     */
    void setMembraneModel8() {
        this->membraneModel = std::unique_ptr<MembraneModel8<T>>(new MembraneModel8<T>());
    }

    /**
     * @brief Set the membrane model 9 to be used in the simulation.
     */
    void setMembraneModel9() {
        this->membraneModel = std::unique_ptr<MembraneModel9<T>>(new MembraneModel9<T>());
    }

    [[nodiscard]] T getMembraneResistance(const std::shared_ptr<arch::Membrane<T>>& membrane, const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<Specie<T>>& specie, T area) const {
        if (membraneModel == nullptr) {
            throw std::logic_error("Membrane model is not set.");
        }
        return membraneModel->getMembraneResistance(membrane, mixture.get(), specie.get(), area);
    }
    
    /**
     * @brief Abstract simulation with membranes/tanks
     * Simulation loop:
     * - conduct nodal analysis
     * - compute events
     * - sort events
     * - search for next event (break if no event is left)
     * - perform next event
     */
    void simulate() override;

};

}   // namespace sim
