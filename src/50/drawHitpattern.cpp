#include "TCanvas.h"
#include "TColor.h"
#include "TH2D.h"
#include "TString.h"
#include "TStyle.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define rep(i, n) for(int i = 0; i < n; i++)

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
  gStyle->SetPaintTextFormat(".2f");
}

void setHist(TH2D *hist) {
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->SetLabelSize(0.04);
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

void drawHitpattern(int run) {
  int cone = 50;
  setCanvas();
  TString runNum  = Form("%03d", run);
  TString csvPath0 = Form("../../csv/%d/pos0/run" + runNum + ".csv", cone);
  TString csvPath1 = Form("../../csv/%d/pos1/run" + runNum + ".csv", cone);
  std::vector<std::vector<double>> csvData0 = fetchCSV<double>(csvPath0);
  std::vector<std::vector<double>> csvData1 = fetchCSV<double>(csvPath1);

  TH2D *histHitPattern0 =
      new TH2D("hist0", ";x [mm]; y[mm]", 100, -550, 550, 100, -550, 550);
  setHist(histHitPattern0);
  TH2D *histHitPattern1 =
      new TH2D("hist1", ";x [mm]; y[mm]", 100, -550, 550, 100, -550, 550);
  setHist(histHitPattern1);

  int entries0 = 0;
  for(auto row : csvData0) {
    entries0 += row.size() / 2;
  }
  int entries1 = 0;
  for(auto row : csvData1) {
    entries1 += row.size() / 2;
  }

  for(auto row : csvData0) {
    for(int i = 0; i < row.size() - 1; i += 2) {
      double x = row.at(i);
      double y = row.at(i + 1);
      histHitPattern0->Fill(x, y, 1.0 / entries0 * 100);
    }
  }

  for(auto row : csvData1) {
    for(int i = 0; i < row.size() - 1; i += 2) {
      double x = row.at(i);
      double y = row.at(i + 1);
      histHitPattern1->Fill(x, y, 1.0 / entries1 * 100);
    }
  }

  TCanvas *canvasHitPattern0 = new TCanvas("canvas0", "canvas", 800, 800);
  canvasHitPattern0->cd();
  histHitPattern0->Draw("colz text");

  TCanvas *canvasHitPattern1 = new TCanvas("canvas1", "canvas", 800, 800);
  canvasHitPattern1->cd();
  histHitPattern1->Draw("colz text");

  TString pdfPath0 = Form("../../images/%d/hitpattern/run" + runNum + "_pos0.pdf", cone);
  printCanvases({canvasHitPattern0}, pdfPath0);

  TString pdfPath1 = Form("../../images/%d/hitpattern/run" + runNum + "_pos1.pdf", cone);
  printCanvases({canvasHitPattern1}, pdfPath1);
}