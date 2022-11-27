#include <vector>


std::vector<double> getConePos(std::vector<std::vector<int>> mapping, int ch, double coneRadius){
  double x = mapping.at(ch).at(0) * coneRadius * sqrt(3);
  double y = mapping.at(ch).at(1) * coneRadius;
  return {x,y};
}