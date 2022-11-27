#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "TString.h"

template <typename T>
std::vector<std::vector<T>> fetchCSV(TString fetchPath){
  std::string strBuf;
  std::string strCommaBuf;
  std::ifstream csvFile(fetchPath);
  std::vector<std::vector<T>> CSV;
  while(std::getline(csvFile, strBuf)){
    std::vector<T> fitParameters;
    std::istringstream iStream(strBuf);
    while(std::getline(iStream, strCommaBuf, ',')){
      if(typeid(T).name() == typeid(int).name()) fitParameters.push_back(std::stoi(strCommaBuf));
      if(typeid(T).name() == typeid(double).name()) fitParameters.push_back(std::stod(strCommaBuf));
    }
    CSV.push_back(fitParameters);
  }
  return CSV;
}

