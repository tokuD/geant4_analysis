#include "TCanvas.h"
#include "TColor.h"
#include "TF1.h"
#include "TGraphErrors.h"
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
  TString csvPath = Form("../../csv/%d/pos%d/run" + runNum + ".csv", cone, pos);
  std::vector<std::vector<double>> csvData = fetchCSV<double>(csvPath);
  // hit数毎に分けて分解能を出す
  std::map<int, TH1D *> histsAngle;

  for(auto row : csvData) {
    double angleAverage = 0;
    int hit             = 0;
    for(int i = 0; i < row.size() - 1; i += 2) {
      double x      = row.at(i);
      double y      = row.at(i + 1);
      double radius = sqrt(pow(x, 2) + pow(y, 2));
      double theta  = calcAngleFromRadius(radius);
      angleAverage += theta;
      hit += 1;
    }
    angleAverage /= (double)hit;
    if(histsAngle.find(hit) == histsAngle.end()) {
      TString name    = Form("hit:%d", hit);
      TString title   = Form("hit: %d; Cherenkov angle (mrad)", hit);
      histsAngle[hit] = new TH1D(name, title, 60, 270, 300);
    }
    histsAngle.at(hit)->Fill(angleAverage);
  }

  std::vector<double> graphX;
  std::vector<double> graphY;
  std::vector<double> graphYError;
  std::vector<TCanvas *> canvasesAngle;
  for(auto itr = histsAngle.begin(); itr != histsAngle.end(); itr++) {
    int hit    = itr->first;
    TH1D *hist = itr->second;
    setHist(hist, kAzure);
    TF1 *fitFunc = new TF1("gaus", "gaus");
    double peak  = hist->GetMean();
    hist->Fit(fitFunc, "", "", peak - 3, peak + 3);
    double mean          = fitFunc->GetParameter(1);
    double meanError     = fitFunc->GetParameter(1);
    double sigma         = fitFunc->GetParameter(2);
    double sigmaError    = fitFunc->GetParError(2);
    TString name         = Form("hit%d", hit);
    TString title        = Form("hit: %d", hit);
    TCanvas *canvasAngle = new TCanvas(name, title, 800, 800);
    canvasAngle->cd();
    hist->Draw();
    canvasesAngle.push_back(canvasAngle);
    if(hist->GetEntries() >= 300) {
      graphX.push_back(hit);
      graphY.push_back(sigma);
      graphYError.push_back(sigmaError);
    }
  }

  TGraphErrors *graph = new TGraphErrors(graphX.size(), &graphX.at(0),
                                         &graphY.at(0), 0, &graphYError.at(0));
  setGraph(graph, ";Number of hit;Angular resolution (mrad)", kAzure);
  TF1 *fitFunc = new TF1("fitFunc", "[0] / sqrt(x)");
  graph->Fit(fitFunc);
  double reso      = fitFunc->GetParameter(0);
  double resoError = fitFunc->GetParError(0);
  TString pushPath = Form(
      "../../csv/%d/angularResolution/run" + runNum + "_pos%d.csv", cone, pos);
  pushCSV(pushPath, {{reso, resoError}});

  TString name    = "Angular resolution";
  TString title   = "Angular resolution";
  TCanvas *canvas = new TCanvas(name, title, 800, 800);
  canvas->cd();
  graph->Draw("AP");

  canvasesAngle.insert(canvasesAngle.begin(), canvas);
  TString pdfPath =
      Form("../../images/%d/angularResolution/run" + runNum + "_pos%d.pdf",
           cone, pos);
  printCanvases(canvasesAngle, pdfPath);
}

void angularResolution(int run) {
  int cone = 50;
  setCanvas();
  TString runNum = Form("%03d", run);

  draw(runNum, cone, 0);
  draw(runNum, cone, 1);
}