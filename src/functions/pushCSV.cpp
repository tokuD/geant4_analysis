#include <fstream>
#include <iostream>
#include <vector>
#include "TString.h"

#define rep(i,n) for(int i=0; i<n; i++)
#define rep1(i, start, end, delta) for(int i=start; i<=end; i+=delta)

void pushCSV(TString pushPath, std::vector<std::vector<double>> pushData){
  std::ofstream ofs(pushPath);
  if(!ofs) std::cout << "Can't open file" << std::endl;
  for(std::vector<double> params : pushData){
    rep(i, params.size()){
      if(i == params.size() - 1) ofs << params.at(i) << std::endl;
      else ofs << params.at(i) << ",";
    }
  }
  ofs.close();
}