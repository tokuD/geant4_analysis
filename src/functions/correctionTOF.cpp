#include <vector>
#include <math.h>

double correctionTOF(double TOF, double TOT, std::vector<double> params){
  double p0 = params.at(0);
  double p1 = params.at(1);
  double p2 = params.at(2);
  if(TOT >= 80) return TOF;
  return TOF - (p0 * pow(TOT,2) + p1 * TOT + p2);
}