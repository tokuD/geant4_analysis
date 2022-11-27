#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include <fstream>
#include <iostream>
#include <vector>

#define rep(i, n) for(int i = 0; i < n; i++)
#define rep1(i, start, end, delta) for(int i = start; i <= end; i += delta)

void pushCSV(TString pushPath, std::vector<std::vector<double>> pushData) {
  std::ofstream ofs(pushPath);
  if(!ofs)
    std::cout << "Can't open file" << std::endl;
  for(std::vector<double> params : pushData) {
    rep(i, params.size()) {
      if(i == params.size() - 1)
        ofs << params.at(i) << std::endl;
      else
        ofs << params.at(i) << ",";
    }
  }
  ofs.close();
}

template <typename T> std::vector<std::vector<T>> fetchCSV(TString fetchPath) {
  std::string strBuf;
  std::string strCommaBuf;
  std::ifstream csvFile(fetchPath);
  std::vector<std::vector<T>> CSV;
  while(std::getline(csvFile, strBuf)) {
    std::vector<T> fitParameters;
    std::istringstream iStream(strBuf);
    while(std::getline(iStream, strCommaBuf, ',')) {
      if(typeid(T).name() == typeid(int).name())
        fitParameters.push_back(std::stoi(strCommaBuf));
      if(typeid(T).name() == typeid(double).name())
        fitParameters.push_back(std::stod(strCommaBuf));
    }
    CSV.push_back(fitParameters);
  }
  return CSV;
}

std::vector<double> getConePos(std::vector<std::vector<int>> mapping, int ch,
                               double coneRadius) {
  double x = mapping.at(ch).at(0) * coneRadius * sqrt(3);
  double y = mapping.at(ch).at(1) * coneRadius;
  return {x, y};
}

void getPos(int run) {
  int cone       = 50;
  TString runNum = Form("%03d", run);
  TFile *fin     = new TFile("../../rootfile/run" + runNum + ".root");
  TTree *tree    = (TTree *)fin->Get("tree");
  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("hitCH0", true);
  tree->SetBranchStatus("hitCH1", true);
  std::vector<int> *hitCH0 = 0;
  std::vector<int> *hitCH1 = 0;
  tree->SetBranchAddress("hitCH0", &hitCH0);
  tree->SetBranchAddress("hitCH1", &hitCH1);
  std::vector<std::vector<int>> mapping =
      fetchCSV<int>(Form("../mapping_%d.csv", cone));

  std::vector<std::vector<double>> csv0;
  std::vector<std::vector<double>> csv1;

  const int nEntries = tree->GetEntries();
  rep(entry, nEntries) {
    if(entry % 1000 == 0)
      std::cout << "Entry: " << entry << "/" << nEntries << std::endl;
    tree->GetEntry(entry);
    std::vector<double> pos0;
    std::vector<double> pos1;
    rep(hit, hitCH0->size()) {
      int ch                     = hitCH0->at(hit);
      std::vector<double> hitPos = getConePos(mapping, ch, (double)cone / 2.0);
      pos0.push_back(hitPos.at(0));
      pos0.push_back(hitPos.at(1));
    }
    rep(hit, hitCH1->size()) {
      int ch                     = hitCH1->at(hit);
      std::vector<double> hitPos = getConePos(mapping, ch, (double)cone / 2.0);
      pos1.push_back(hitPos.at(0));
      pos1.push_back(hitPos.at(1));
    }
    csv0.push_back(pos0);
    csv1.push_back(pos1);
  }

  TString pushPath0 = Form("../../csv/%d/pos0/run" + runNum + ".csv", cone);
  pushCSV(pushPath0, csv0);
  TString pushPath1 = Form("../../csv/%d/pos1/run" + runNum + ".csv", cone);
  pushCSV(pushPath1, csv1);
}