#include "TCanvas.h"
#include "TColor.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#define rep(i, n) for(int i = 0; i < n; i++)

void setHist(TH1D *hist, Color_t color) {
  hist->SetLineColorAlpha(color, .4);
  hist->SetFillColorAlpha(color, .4);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->SetLabelSize(0.04);
}

double calcAngleFromRadius(double radius /*mm*/) {
  double focalLength = 1491;
  double theta       = atan(radius / focalLength) * 1000; // mrad
  return theta;
}

void setCanvas() {
  const Int_t NRGBs     = 5;
  const Int_t NCont     = 255;
  Double_t stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
  Double_t Red[NRGBs]   = {0.00, 0.00, 0.87, 1.00, 0.51};
  Double_t Green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
  Double_t Blue[NRGBs]  = {0.51, 1.00, 0.12, 0.00, 0.00};
  TColor::CreateGradientColorTable(NRGBs, stops, Red, Green, Blue, NCont);
  gStyle->SetNumberContours(NCont);
  gStyle->SetOptStat(0);
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.15);
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

void printCanvases(std::vector<TCanvas *> canvases, TString pdfPath) {
  rep(i, canvases.size()) {
    if(i == 0)
      canvases.at(i)->Print(pdfPath + "[", "pdf");
    canvases.at(i)->Print(pdfPath, "pdf");
    if(i == canvases.size() - 1)
      canvases.at(i)->Print(pdfPath + "]", "pdf");
  }
}

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

void setGraph(TGraphErrors *graph, TString title, Color_t color) {
  graph->SetMarkerColor(color);
  graph->SetMarkerStyle(8);
  graph->SetMarkerSize(1.5);
  graph->SetTitle(title);
}

void draw(TString runNum, int cone, int pos) {
  // multiplicity
  TFile *fin  = new TFile("../../rootfile/run" + runNum + ".root");
  TTree *tree = (TTree *)fin->Get("tree");
  tree->SetBranchStatus("*", false);
  TString branch = "hitCH" + std::to_string(pos);
  tree->SetBranchStatus(branch, true);
  std::vector<int> *hitCH = 0;
  tree->SetBranchAddress(branch, &hitCH);

  TString name = "hist";
  TString title = ";Multiplicity";
  TH1D *histMulti = new TH1D(name, title, 80, 0, 80);
  setHist(histMulti, kAzure);

  const int nEntries = tree->GetEntries();
  rep(entry, nEntries){
    if(entry % 1000 == 0) std::cout << "Event: " << entry << "/" << nEntries << std::endl;
    tree->GetEntry(entry);
    std::map<int, int> multi;
    rep(hit, hitCH->size()){
      int ch = hitCH->at(hit);
      if(multi.find(ch) == multi.end()) multi[ch] = 0;
      multi.at(ch) += 1;
    }
    histMulti->Fill(multi.size());
  }

  TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 800);
  canvas->cd();
  TF1 *fitFunc = new TF1("gaus", "gaus");
  double peak = histMulti->GetBin(histMulti->GetMaximumBin());
  histMulti->Fit(fitFunc, "", "", peak - 10, peak + 10);
  histMulti->Draw();

  TString pdfPath = Form("../../images/%d/multiplicity/run" + runNum + "_pos%d.pdf", cone, pos);
  printCanvases({canvas}, pdfPath);

  TString csvPath = Form("../../csv/%d/multiplicity/run" + runNum + "_pos%d.csv", cone, pos);
  pushCSV(csvPath, {{fitFunc->GetParameter(1), fitFunc->GetParError(1)}});
}

void multiplicity(int run) {
  int cone = 50;
  setCanvas();
  TString runNum = Form("%03d", run);

  draw(runNum, cone, 0);
  draw(runNum, cone, 1);
}