#include <AdeConcBoundary2D.h>

namespace olb {

template<typename T>
AdeConcBoundary2D<T>::AdeConcBoundary2D(const arch::Opening<T>& opening, std::vector<T> concentrationField) :
  width(opening.width),
  xc(opening.node->getPosition()[0]),
  yc(opening.node->getPosition()[0]),
  x0(xc - opening.tangent[0]*0.5*width),
  y0(yc - opening.tangent[1]*0.5*width),
  array(concentrationField) { }

template<typename T>
bool AdeConcBoundary2D<T>::operator()(T output[], const T input[]) {
  T s   = std::sqrt((input[0]-x0)*(input[0]-x0) + (input[1]-y0)*(input[1]-y0));
  T d   = std::sqrt((input[0]-xc)*(input[0]-xc) + (input[1]-yc)*(input[1]-yc));
  T res = width / array.size();
  if (s < width) {
    output[0] = array[std::floor(s/res)];
  }
  if (0.5*width - d < 0.0) {
    output[0] = T();
  }
  return true;
}

template<typename T>
AdeConc1D<T>::AdeConc1D(const arch::Opening<T>& opening, std::vector<T> concentrationField) 
  : SuperPlaneIntegralF2D<T>()

template<typename T>
bool AdeConc1D<T>::operator()(T output[], const T input[]) {

}

} // namespace olb