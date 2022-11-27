#include <math.h>

double calcAngleFromRadius(double radius /*mm*/) {
  double focalLength = 1491;
  double theta       = atan(radius / focalLength) * 1000; //mrad
  return theta;
}