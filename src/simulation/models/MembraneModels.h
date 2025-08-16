/**
 * @file MembraneModels.h
 */

#pragma once

#include <cmath>

namespace arch {
template<typename T>
class Membrane;
}  // namespace arch

namespace sim { 
template<typename T>
class AbstractMembrane;

template<typename T>
class Mixture;

template<typename T>
class Specie;

/**
 * @brief Virtual class that describes the basic functionality for mixing models.
*/
template<typename T>
class MembraneModel {
protected:
    /**
     * @brief Constructor of a membrane model.
    */
    MembraneModel();

public:
    /**
     * @brief Get the resistance caused by the membrane based on the specific resistance model.
     *
     * @param membrane Pointer to the membrane for which the resistance should be calculated.
     * @param mixture Mixture for which the resistance of the membrane should be calculated.
     * @param specie Specie for which the resistance should be calculated.
     * @param area Area for which the resistance should be calculated in [m^3].
     * @return The resistance of this membrane to this mixture in [Pa].
     */
    virtual T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const = 0;

    /**
     * @brief Virtual default destructor for inheritance.
     */
    virtual ~MembraneModel() = default;

};

/**
 * @brief Class that describes the poreResistance membrane model. This membrane model derives the permeability from pore geometry.
*/
template<typename T>
class PermeabilityMembraneModel final : public MembraneModel<T> {

private:

    /**
     * @brief Constructor of a poreResistance membrane model.
    */
    PermeabilityMembraneModel();
    
    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that describes the poreResistance membrane model. This membrane model derives the permeability from pore geometry.
*/
template<typename T>
class PoreResistanceMembraneModel final : public MembraneModel<T> {

private:

    /**
     * @brief Constructor of a poreResistance membrane model.
    */
    PoreResistanceMembraneModel();

    friend class AbstractMembrane<T>;

};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 0
 * Based on Source: M. Ishahak, J. Hill, Q. Amin, L. Wubker, A. Hernandez, A. Mitrofanova, A. Sloan, A. Fornoni and A. Agarwal. "Modular Microphysiological System for Modeling of Biologic Barrier Function". In: Front. Bioeng. Biotechnol. 8:581163. (2020). doi: 10.3389/fbioe.2020.581163
 */
template<typename T>
class MembraneModel0 final : public MembraneModel<T> {

  private:

    MembraneModel0();

    [[nodiscard]] T getPoreResistance(arch::Membrane<T> const*  membrane, Mixture<T> const* mixture) const;
    [[nodiscard]] T getPermeabilityParameter(const arch::Membrane<T>* membrane) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 1
 * Based on Source: J. J. VanDersarl, A. M. Xu, and N. A. Melosh. “Rapid spatial and temporal controlled signal delivery over large cell culture areas”. In: Lab Chip 11 (18 2011), pp. 3057–3063. doi: 10.1039/C1LC20311H. url: https://dx.doi.org/10.1039/C1LC20311H.
 */
template<typename T>
class MembraneModel1 final : public MembraneModel<T> {
  private:
    T deviceAdjustment{};

    /**
     * @brief Instantiate the resistance model.
     *
     * @param deviceAdjustment Microfluidic chip specific value which is used as a factor for
     *                         the resistance depending on the pore density
     */
    explicit MembraneModel1(T deviceAdjustment = 1.9);

    [[nodiscard]] T getPoreExitResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPoreResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPoreDensityDependentResistance(arch::Membrane<T> const* membrane, T area, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 2
 * Based on Source: J.L. Snyder, A. Clark, D.Z. Fang, T.R. Gaborski, C.C. Striemer, P.M. Fauchet, J.L. McGrath, "An experimental and theoretical analysis of molecular separations by diffusion through ultrathin nanoporous membranes". Journal of Membrane Science Volume 369, Issues 1–2, 2011, Pages 119-129. ISSN 0376-7388,https://doi.org/10.1016/j.memsci.2010.11.056.
 */
template<typename T>
class MembraneModel2 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel2();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T area, T freeDiffusionCoefficient) const;
    [[nodiscard]] T getTransmembraneResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient, T area) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 3
 * Assumes N disk-like absorbers on the surface of a sphere
 * Based on Source: H.C. Berg. Random Walks in Biology. Princeton paperbacks. Princeton University Press, 1993. isbn: 9780691000640. url: https://books.google.at/books?id=DjdgXGLoJY8C
 */
template<typename T>
class MembraneModel3 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel3();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient, T area) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 4
 * Based on Source: C.-W. Ho, A. Ruehli, and P. Brennan. “The modified nodal approach to network analysis”. In: IEEE Transactions on Circuits and Systems 22.6 (1975), pp. 504–509. doi: 10.1109/TCS.1975.1084079.
 */
template<typename T>
class MembraneModel4 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel4();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 5
 * Adapted from Membrane Resistance Model 4 to fit the calculations in Appendix 1
 * Based on Source: C.-W. Ho, A. Ruehli, and P. Brennan. “The modified nodal approach to network analysis”. In: IEEE Transactions on Circuits and Systems 22.6 (1975), pp. 504–509. doi: 10.1109/TCS.1975.1084079.
 */
template<typename T>
class MembraneModel5 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel5();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 6
 * Based on Source: K. Ronaldson-Bouchard et al. “A multi-organ chip with matured tissue niches linked by vascular flow”. In: Nature Biomedical Engineering 6.4 (2022), pp. 351–371. doi: 10.1038/s41551-022-00882-6. url: https://doi.org/10.1038/s41551-022-00882-6 (cit. on p. 35).
 */
template<typename T>
class MembraneModel6 final : public MembraneModel<T> {
  private:
    static constexpr T diffusionFactorMembrane = 1.58e-11;  // MultiOrgan
    //static constexpr T diffusionFactorMembrane = 4.4e-11;  // TwoOrgan

    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel6();

    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 7
 * Based on Source: H.C. Berg. Random Walks in Biology. Princeton paperbacks. Princeton University Press, 1993. isbn: 9780691000640. url: https://books.google.at/books?id=DjdgXGLoJY8C
 */
template<typename T>
class MembraneModel7 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel7();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T area, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 8
 * Assumes N circular pores in a planar barrier, with the distance as membrane height
 * Based on Source: H.C. Berg. Random Walks in Biology. Princeton paperbacks. Princeton University Press, 1993. isbn: 9780691000640. url: https://books.google.at/books?id=DjdgXGLoJY8C
 */
template<typename T>
class MembraneModel8 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel8();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T area, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

/**
 * @brief Class that defines the functionality of the 1D membrane resistance model.
 * Membrane Resistance Model 9
 * Assumes N circular pores in a planar barrier, with the distance as membrane height, and the diffusion resistance equal to a disk-like absorber
 * Based on Source: H.C. Berg. Random Walks in Biology. Princeton paperbacks. Princeton University Press, 1993. isbn: 9780691000640. url: https://books.google.at/books?id=DjdgXGLoJY8C
 */
template<typename T>
class MembraneModel9 final : public MembraneModel<T> {
  private:
    /**
     * @brief Instantiate the resistance model.
     */
    MembraneModel9();

    [[nodiscard]] T getPoreDiscoveryResistance(arch::Membrane<T> const* membrane, T diffusionCoefficient) const;
    [[nodiscard]] T getPorePassageResistance(arch::Membrane<T> const* membrane, T area, T diffusionCoefficient) const;
    [[nodiscard]] T getMembraneResistance(arch::Membrane<T> const* membrane, Mixture<T> const* mixture, Specie<T> const* specie, T area) const override;

    friend class AbstractMembrane<T>;
};

}   // namespace sim
