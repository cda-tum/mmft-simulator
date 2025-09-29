/**
 * @file ConcentrationSemantics.h
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
class Edge;

template<typename T>
class Module;

template<typename T>
class Network;

}

namespace sim {

// Forward declared dependencies
template<typename T>
class Event;

template<typename T>
class MixingModel;

template<typename T>
class DiffusionMixingModel;

template<typename T>
class InstantaneousMixingModel;

template<typename T>
class Mixture;

template<typename T>
class MixtureInjection;

template<typename T>
class Specie;

/**
 * @brief Class that defines the semantics of concentration for concentration-based simulators
 */
template<typename T>
class ConcentrationSemantics {

private:

    std::shared_ptr<arch::Network<T>>& networkRef;
    std::unique_ptr<MixingModel<T>> mixingModel = nullptr;                                          ///< The mixing model used for a mixing simulation.
    std::unordered_map<size_t, std::shared_ptr<Specie<T>>> species;                                 ///< Species specified for the simulation.
    std::unordered_map<size_t, std::shared_ptr<Mixture<T>>> mixtures;                               ///< Mixtures present in the simulation.
    std::unordered_map<size_t, std::shared_ptr<MixtureInjection<T>>> mixtureInjections;             ///< Injections of fluids that should take place during the simulation.
    std::unordered_map<size_t, std::shared_ptr<MixtureInjection<T>>> permanentMixtureInjections;    ///< Permanent injections of fluids that should take place during the simulation. Used to simulate a fluid change or include an exposure of the system to a specific mixture/concentration.
    std::unordered_map<size_t, std::set<size_t>> injectionMap;                                      ///< Map of injections to mixtures stored as <mixtureId, <injectionId1, injectionId2, ...>>.

protected:

    ConcentrationSemantics(std::shared_ptr<arch::Network<T>>& networkRef) : networkRef(networkRef) { }
    
    virtual void assertInitialized() const;

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<size_t, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] species Unordered map of specie ids and pointer to that specie.
     * @param[in] specieConcentrations unordered map of specie id and corresponding concentration.
     * @return Pointer to created mixture.
     */
    Mixture<T>* addMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<size_t, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, T> specieConcentrations);

    /**
     * @brief Create mixture.
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);
    
    /**
     * @brief Create mixture.
     * @param[in] specieIds
     * @param[in] specieConcentrations
     * @return Pointer to created mixture.
     */
    Mixture<T>* addDiffusiveMixture(std::unordered_map<size_t, Specie<T>*> species, std::unordered_map<size_t, std::tuple<std::function<T(T)>, std::vector<T>, T>> specieDistributions);

    /**
     * @brief Create and add a mixture to the simulation.
     * @param[in] concentration Concentration of the specie in the mixture in g/m^3.
     * @return Pointer to created mixture.
     */
    [[maybe_unused]] std::shared_ptr<Mixture<T>> createMixture(std::unordered_map<size_t, T> specieConcentrations);

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<MixtureInjection<T>>>& getMixtureInjections() const { return mixtureInjections; }

    /**
     * @brief Get injection
     * @return Reference to the unordered map of MixtureInjections
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<MixtureInjection<T>>>& getPermanentMixtureInjections() const { return permanentMixtureInjections; }

    /**
     * @brief Get the mixing model that is used in the simulation.
     * @return The mixing model of the simulation.
     */
    MixingModel<T>* getMixingModel() const { return mixingModel.get(); }

    /**
     * @brief Get mixtures.
     * @return Reference to the unordered map of mixtures
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Mixture<T>>>& getMixtures() const { return mixtures; }

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture
     * @return Pointer to mixture with the correspondig id
     */
    [[nodiscard]] inline const std::unordered_map<size_t, std::shared_ptr<Specie<T>>>& getSpecies() const { return species; }

    /**
     * @brief Remove specie from the simulator. If a mixture contains the specie, it is removed from the mixture as well.
     * A mixture consisting of a single specie is removed from the simulation.
     * @param[in] specieId Id of the specie that should be removed.
     * @throws std::logic_error if the specie is not present in the simulation.
     */
    void removeSpecie(int specieId);

    /**
     * @brief Remove mixture from the simulation.
     * @param[in] mixtureId Id of the mixture that should be removed.
     * @throws std::logic_error if the mixture is not present in the simulation.
     * @note If the mixture is present in the simulation, it is removed from the simulation.
     */
    void removeMixture(int mixtureId);

    /**
     * @brief Remove mixture injection from the simulation.
     * @param[in] injectionId Id of the mixture injection that should be removed.
     * @throws std::logic_error if the injection is not present in the simulation.
     */
    void removeMixtureInjection(int mixtureInjectionId);

public:

    /**
     * @brief Sets an instantaneous mixing model for the simulation.
     */
    void setInstantaneousMixingModel();

    /**
     * @brief Check if the mixing model is an instantaneous mixing model.
     */
    [[nodiscard]] inline bool hasInstantaneousMixingModel() const { return mixingModel->isInstantaneous(); }

    /**
     * @brief Sets a diffusive mixing model for the simulation.
     */
    void setDiffusiveMixingModel();

    /**
     * @brief Check if the mixing model is a diffusive mixing model.
     */
    [[nodiscard]] inline bool hasDiffusiveMixingModel() const { return mixingModel->isDiffusive(); }

    /**
     * @brief Create and add a specie to the simulation.
     * @param[in] diffusivity Diffusion coefficient of the specie in the carrier medium in m^2/s.
     * @param[in] satConc Saturation concentration of the specie in the carrier medium in g/m^3.
     * @return Pointer to created specie.
     */
    [[maybe_unused]] std::shared_ptr<Specie<T>> addSpecie(T diffusivity, T satConc);

    /**
     * @brief Get specie.
     * @param specieId Id of the specie.
     * @return Pointer to specie with the correspondig id.
     */
    [[nodiscard]] inline std::shared_ptr<Specie<T>> getSpecie(int specieId) const { return species.at(specieId); }

    /**
     * @brief Remove specie from the simulator. If a mixture contains the specie, it is removed from the mixture as well.
     * A mixture consisting of a single specie is removed from the simulation.
     * @param[in] specie The specie to be removed.
     * @throws std::logic_error if the specie is not present in the simulation.
     */
    void removeSpecie(const std::shared_ptr<Specie<T>>& specie);

    /**
     * @brief Create and add a mixture to the simulation, respective to the mixing model. The mixture is composed of a single specie.
     * @param[in] specie pointer to the specie that composes the mixture.
     * @param[in] concentration Concentration of the specie in the mixture in g/m^3.
     * @return Pointer to created mixture.
     */
    [[maybe_unused]] std::shared_ptr<Mixture<T>> addMixture(const std::shared_ptr<Specie<T>>& specie, T concentration);

    /**
     * @brief Create and add a mixture to the simulation, respective to the mixing model. The mixture is composed of a set of species.
     * @param[in] species vector of pointers to the species that composes the mixture.
     * @param[in] concentration vector of concentrations of the species in the mixture in g/m^3.
     * @return Pointer to created mixture.
     * @note The concentrations should be in the same order as the species and have the same length.
     * @throws std::logic_error if the species and concentrations vectors are not of the same length.
     * @throws std::logic_error if the species vector is empty.
     */
    [[maybe_unused]] std::shared_ptr<Mixture<T>> addMixture(const std::vector<std::shared_ptr<Specie<T>>>& species, const std::vector<T>& concentrations);

    /**
     * @brief Get mixture.
     * @param mixtureId Id of the mixture.
     * @return Pointer to mixture with the correspondig id.
     * @throws std::out_of_range if the mixture with the given id does not exist.
     * @note The mixture must be present in the simulation.
     */
    [[nodiscard]] std::shared_ptr<Mixture<T>> getMixture(int mixtureId) const { return mixtures.at(mixtureId); }

    /**
     * @brief Return a read-only map of mixtures currently stored in the simulation
     * @return Unordered map of mixture ids and const pointers to the mixtures
     */
    [[nodiscard]] const std::unordered_map<size_t, const Mixture<T>*> readMixtures() const;

    /**
     * @brief Remove mixture from the simulation.
     * @param[in] mixture Pointer to the mixture that should be removed.
     * @throws std::logic_error if the mixture is not present in the simulation.
     * @note If the mixture is present in the simulation, it is removed from the simulation.
     */
    void removeMixture(const std::shared_ptr<Mixture<T>>& mixture);

    /**
     * @brief Create mixture injection. The injection is always performed at the beginning (position 0.0) of the edge.
     * @param[in] mixtureId Id of the mixture that should be injected.
     * @param[in] edgeId Id of the edge, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @param[in] isPermanent If true, the injection is permanent and will be performed at every time step. If false, the injection is only performed once at the specified time.
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<MixtureInjection<T>> addMixtureInjection(int mixtureId, int edgeId, T injectionTime, bool isPermanent = false);

    /**
     * @brief Create mixture injection. The injection is always performed at the beginning (position 0.0) of the edge.
     * @param[in] mixture Pointer to the mixture that should be injected.
     * @param[in] edge Pointer to the edge, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @param[in] isPermanent If true, the injection is permanent and will be performed at every time step. If false, the injection is only performed once at the specified time.
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<MixtureInjection<T>> addMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime, bool isPermanent = false);

    /**
     * @brief Create mixture permanent injection. The injection is always performed at the beginning (position 0.0) of the edge.
     * @param[in] mixtureId Id of the mixture that should be injected.
     * @param[in] edgeId Id of the edge, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<MixtureInjection<T>> addPermanentMixtureInjection(int mixtureId, int edgeId, T injectionTime);

    /**
     * @brief Create mixture permanent injection. The injection is always performed at the beginning (position 0.0) of the edge.
     * @param[in] mixture Pointer to the mixture that should be injected.
     * @param[in] edge Pointer to the edge, where specie should be injected.
     * @param[in] injectionTime Time at which the injection should be injected in s.
     * @return Pointer to created injection.
     */
    [[maybe_unused]] std::shared_ptr<MixtureInjection<T>> addPermanentMixtureInjection(const std::shared_ptr<Mixture<T>>& mixture, const std::shared_ptr<arch::Edge<T>>& edge, T injectionTime);


    /**
     * @brief Get injection
     * @param injectionId The id of the injection
     * @return Pointer to injection with the corresponding id.
     * @throws std::out_of_range if the injection with the given id does not exist.
     * @note The injection must be present in the simulation.
     */
    [[nodiscard]] std::shared_ptr<MixtureInjection<T>> getMixtureInjection(int injectionId) const;

    /**
     * @brief Remove mixture injection from the simulation.
     * @param[in] injection Pointer to the injection that should be removed.
     * @throws std::logic_error if the injection is not present in the simulation.
     */
    void removeMixtureInjection(const std::shared_ptr<MixtureInjection<T>>& injection);

    friend class DiffusionMixingModel<T>;
    friend class InstantaneousMixingModel<T>;

};

}   // namespace sim