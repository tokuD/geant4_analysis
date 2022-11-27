#include "TCanvas.h"
#include "TColor.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TStyle.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#define rep(i, n) for(int i = 0; i < n; i++)

double calcAngleFromRadius(double radius /*mm*/) {
  double focalLength = 1491;
  double theta       = atan(radius / focalLength) * 1000; // mrad
  return theta;
}

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
  gStyle->SetPadBottomMargin(0.12);
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

void drawRadiusAndAngle(int run, int pos) {
  int cone = 50;
  setCanvas();
  TString runNum = Form("%03d", run);
  TString posNum = Form("%d", pos);
  TString csvPath =
      Form("../../csv/%d/pos" + posNum + "/run" + runNum + ".csv", cone);
  std::vector<std::vector<double>> csvData = fetchCSV<double>(csvPath);

  TH1D *histRadiusRow =
      new TH1D("histRadius", "Row;Ring radius (mm)", 200, 380, 480);
  TH1D *histRadiusAverage =
      new TH1D("histRadius", "Average;Ring radius (mm)", 100, 420, 440);
  TH1D *histAngleRow =
      new TH1D("histAngle", "Row;Cherenkov angle (mrad)", 160, 240, 320);
  TH1D *histAngleAverage =
      new TH1D("histAngle", "Average;Cherenkov angle (mrad)", 100, 270, 290);
  
  setHist(histRadiusRow, kAzure);
  setHist(histRadiusAverage, kAzure);
  setHist(histAngleRow, kAzure);
  setHist(histAngleAverage, kAzure);

  for(auto row : csvData) {
    double radiusAverage = 0;
    double angleAverage  = 0;
    double hit = 0;
    for(int i = 0; i < row.size() - 1; i += 2) {
      double x      = row.at(i);
      double y      = row.at(i + 1);
      double radius = sqrt(pow(x, 2) + pow(y, 2));
      double theta  = calcAngleFromRadius(radius);
      histRadiusRow->Fill(radius);
      histAngleRow->Fill(theta);
      radiusAverage += radius;
      angleAverage += theta;
      hit += 1;
    }
    radiusAverage /= hit;
    angleAverage /= hit;
    histRadiusAverage->Fill(radiusAverage);
    histAngleAverage->Fill(angleAverage);
  }

  TCanvas *canvasRadiusRow =
      new TCanvas("canvasRadiusRow", "canvasRadiusRow", 800, 800);
  TCanvas *canvasRadiusAverage =
      new TCanvas("canvasRadiusAverage", "canvasRadiusAverage", 800, 800);
  TCanvas *canvasAngleRow =
      new TCanvas("canvasAngleRow", "canvasAngleRow", 800, 800);
  TCanvas *canvasAngleAverage =
      new TCanvas("canvasAngleAverage", "canvasAngleAverage", 800, 800);

  canvasRadiusRow->cd();
  histRadiusRow->Draw();

  canvasRadiusAverage->cd();
  histRadiusAverage->Draw();

  canvasAngleRow->cd();
  histAngleRow->Draw();

  canvasAngleAverage->cd();
  histAngleAverage->Draw();

  TString pdfPathRaduis =
      Form("../../images/%d/radius/run" + runNum + "_pos%d.pdf", cone, pos);
  printCanvases({canvasRadiusRow, canvasRadiusAverage}, pdfPathRaduis);
  TString pdfPathAngle =
      Form("../../images/%d/angle/run" + runNum + "_pos%d.pdf", cone, pos);
  printCanvases({canvasAngleRow, canvasAngleAverage}, pdfPathAngle);
}