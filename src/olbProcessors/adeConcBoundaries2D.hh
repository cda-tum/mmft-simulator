#include "adeConcBoundaries2D.h"

namespace olb {

template<typename T>
AdeConcBC<T>::AdeConcBC(const std::shared_ptr<arch::Module<T>> module, const arch::Opening<T>& opening, std::vector<T> concentrationField) :
  AnalyticalF2D<T,T>(2),
  width(opening.width),
  xc(opening.node->getPosition()[0] - module->getPosition()[0]),
  yc(opening.node->getPosition()[1] - module->getPosition()[1]),
  x0(xc - opening.tangent[0]*0.5*width),
  y0(yc - opening.tangent[1]*0.5*width),
  array(concentrationField)
  { 
    std::cout << "[AdeConcBC] AdeConcBC()" << std::endl; 
  }

template<typename T>
bool AdeConcBC<T>::operator()(T output[], const T input[]) {
  std::cout << "[AdeConcBC] operator()" << std::endl;
  T s   = std::sqrt((input[0]-x0)*(input[0]-x0) + (input[1]-y0)*(input[1]-y0));
  T d   = std::sqrt((input[0]-xc)*(input[0]-xc) + (input[1]-yc)*(input[1]-yc));
  T res = width / array.size();
  if (s < width) {
    output[0] = array[std::floor(s/res)];
    std::cout << "Setting " << array[std::floor(s/res)] << " at " << std::floor(s/res) << std::endl;
  }
  if (0.5*width - d < 0.0) {
    output[0] = T();
  }
  return true;
}

template<typename T>
AdeConc1D<T>::AdeConc1D(FunctorPtr<SuperF2D<T>>&& f,
          SuperGeometry<T, 2>&      geometry,
          const Vector<T,2>& origin, 
          const Vector<T,2>& u,
          std::vector<int> materials)
: SuperPlaneIntegralF2D<T>(
    std::forward<decltype(f)>(f), 
    geometry, 
    Hyperplane2D<T>().originAt(origin).parallelTo(u), 
    geometry.getMaterialIndicator(std::forward<decltype(materials)>(materials))) { std::cout << "[AdeConc1D] AdeConc1D()" << std::endl;}

template<typename T>
std::vector<T> AdeConc1D<T>::getCfdConc() {
  std::cout << "[AdeConc1D] getCfdConc()" << std::endl;
  this->getSuperStructure().communicate();

  this->_reductionF.update();

  std::vector<T> concentrationField;

  std::vector<std::tuple<T, T>> data;

  for (int pos : this->_rankLocalSubplane) {
    T outTmp[3];  // 3 to prevent stack issues
    const Vector<T,2> physR = this->_reductionF.getPhysR(pos);
    const Vector<T,2> physRelativeToOrigin = physR - this->_origin;
    const T physOnHyperplane = physRelativeToOrigin * this->_u;
    this->_reductionF(outTmp, pos);

    data.push_back(std::tuple<T,T>(physOnHyperplane, outTmp[0]));
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
