#include "adeConcBoundaries2D.h"

namespace olb {

template<typename T>
AdeConcBC<T>::AdeConcBC(const arch::Opening<T>& opening, std::vector<T> concentrationField) :
  AnalyticalF2D<T,T>(2),
  width(opening.width),
  xc(opening.node->getPosition()[0]),
  yc(opening.node->getPosition()[0]),
  x0(xc - opening.tangent[0]*0.5*width),
  y0(yc - opening.tangent[1]*0.5*width),
  array(concentrationField) { }

template<typename T>
bool AdeConcBC<T>::operator()(T output[], const T input[]) {
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
AdeConc1D<T>::AdeConc1D(FunctorPtr<SuperF2D<T>>&& f,
          SuperGeometry<T, 2>&      geometry,
          const Hyperplane2D<T>&    hyperplane,
          FunctorPtr<SuperIndicatorF2D<T>>&&  integrationIndicator,
          FunctorPtr<IndicatorF1D<T>>&&       subplaneIndicator)
: SuperPlaneIntegralF2D<T>(
    std::forward<decltype(f)>(f), 
    geometry, 
    hyperplane, 
    std::forward<decltype(integrationIndicator)>(integrationIndicator), 
    std::forward<decltype(subplaneIndicator)>(subplaneIndicator)) { }

template<typename T>
std::vector<T> AdeConc1D<T>::getCfdConc() {
  this->getSuperStructure().communicate();

  this->_reductionF.update();

  std::vector<T> concentrationField;

  std::vector<std::tuple<T, T>> data;

  for (int pos : this->_rankLocalSubplane) {
    T outTmp[3];  // 3 to prevent stack issues
    Vector<T,2> s = this->_reductionF.getPhysR(pos);
    T dist = std::sqrt(s[0]*s[0] + s[1]*s[1]);
    this->_reductionF(outTmp, pos);

    data.push_back(std::tuple<T,T>(dist, outTmp[0]));
  }

  // Sort data by distance
  std::sort(data.begin(), data.end(), [](auto& a, auto& b) {
    return std::get<0>(a) < std::get<0>(b);
  });

  for (auto& d : data) {
    concentrationField.push_back(std::get<1>(d));
  }

  return concentrationField;
}

template<typename T>
void AdeConc1D<T>::print() {

  OstreamManager clout("AdeConc1D");
  clout << "length of concentration field =" << this->_rankLocalSubplane.size() << std::endl;

}

} // namespace olb
