#pragma once

#include <Network.h>

namespace nodal {

    /**
     * @brief Conducts the Modifed Nodal Analysis (e.g., http://qucs.sourceforge.net/tech/node14.html) and computes the pressure levels for each node.
     * Hence, the passed nodes contain the final pressure levels when the function is finished.
     *
     * @param[in,out] nodes List of nodes, which contain the result (pressure values) of the modified nodal analysis.
     * @param[in] channels List of channels.
     * @param[in,out] pressurePumps List of pressure pumps.
     * @param[in] flowRatePumps List of flowrate pumps.
     */
    template<typename T>
    bool conductNodalAnalysis( const arch::Network<T>* network);

}   // namespace nodal