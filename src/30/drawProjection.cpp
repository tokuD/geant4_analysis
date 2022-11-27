#include "TCanvas.h"
#include "TColor.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TStyle.h"
#include <fstream>
#include <iostream>
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

void drawProjection(int run, int pos) {
  int cone = 30;
  setCanvas();
  TString runNum = Form("%03d", run);
  TString posNum = Form("%d", pos);
  TString csvPath =
      Form("../../csv/%d/pos" + posNum + "/run" + runNum + ".csv", cone);
  std::vector<std::vector<double>> csvData = fetchCSV<double>(csvPath);

  TH1D *projectionX = new TH1D("projectionX", ";x (mm)", 1100, -550, 550);
  TH1D *projectionY = new TH1D("projectionX", ";y (mm)", 1100, -550, 550);
  setHist(projectionX, kAzure);
  setHist(projectionY, kAzure);

  for(auto row : csvData) {
    for(int i = 0; i < row.size() - 1; i += 2) {
      double x = row.at(i);
      double y = row.at(i + 1);
      projectionX->Fill(x);
      projectionY->Fill(y);
    }
  }

  TCanvas *canvasProjectionX = new TCanvas("canvasProjectionX", "canvasProjectionX", 800, 800);
  TCanvas *canvasProjectionY = new TCanvas("canvasProjectionY", "canvasProjectionY", 800, 800);
  canvasProjectionX->cd();
  projectionX->Draw();
  canvasProjectionY->cd();
  projectionY->Draw();

  TString pdfPath =
      Form("../../images/%d/projection/run" + runNum + "_pos%d.pdf", cone, pos);
  printCanvases({canvasProjectionX, canvasProjectionY}, pdfPath);
}