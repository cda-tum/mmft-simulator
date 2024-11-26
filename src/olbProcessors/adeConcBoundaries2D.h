#pragma once

#include <olb2D.h>
#include <olb2D.hh>

namespace arch {

// Forward declared dependencies
template<typename T>
class Opening;

template <typename T>
class Node;

}

namespace olb {

template<typename T>
class AdeConcBC : public AnalyticalF2D<T,T> {

private:
  T width;                ///< Physical width of the opening.
  T xc;                   ///< The physical x coordinate, xc, of the opening node (center).
  T yc;                   ///< The physical y coordinate, yc, of the opening node (center).
  T x0;                   ///< The physical x coordinate, x0, at which the opening "starts" (bottom for right facing opening).
  T y0;                   ///< The physical y coordinate, y0, at which the opening "starts" (bottom for right facing opening).
  std::vector<T> array;   ///< Vector that contains the array of concentration values along the opening.

public:

  AdeConcBC(const arch::Opening<T>& opening, std::vector<T> concentrationField);

  bool operator() (T output[], const T input[]) override;

};

template<typename T>
class AdeConc1D : public SuperPlaneIntegralF2D<T> {

private:
  T width;                ///< Physical width of the opening.
  T xc;                   ///< The physical x coordinate, xc, of the opening node (center).
  T yc;                   ///< The physical y coordinate, yc, of the opening node (center).
  T x0;                   ///< The physical x coordinate, x0, at which the opening "starts" (bottom for right facing opening).
  T y0;                   ///< The physical y coordinate, y0, at which the opening "starts" (bottom for right facing opening).

public:

  AdeConc1D(FunctorPtr<SuperF2D<T>>&& f,
            SuperGeometry<T, 2>&      geometry,
            const Hyperplane2D<T>&    hyperplane,
            FunctorPtr<SuperIndicatorF2D<T>>&&  integrationIndicator,
            FunctorPtr<IndicatorF1D<T>>&&       subplaneIndicator);

  bool operator() (T output[], const int input[]) override { return true; }

  std::vector<T> getCfdConc();

  void print();

};

}
