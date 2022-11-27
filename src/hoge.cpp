#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TColor.h"
#include "TStyle.h"
#include "TString.h"
#include "TH1D.h"
#include "TCanvas.h"

#define rep(i,n) for(int i = 0; i < n; i++)

void setCanvas(){
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t Red[NRGBs] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t Green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t Blue[NRGBs] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, Red, Green, Blue, NCont);
  gStyle->SetNumberContours(NCont);
  gStyle->SetOptStat(0);
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.15);
}

void printCanvases(std::vector<TCanvas*> canvases, TString pdfPath){
  rep(i, canvases.size()){
    if(i == 0) canvases.at(i)->Print(pdfPath + "[", "pdf");
    canvases.at(i)->Print(pdfPath, "pdf");
    if(i == canvases.size() - 1) canvases.at(i)->Print(pdfPath + "]", "pdf");
  }
}

void setHist(TH1D *hist, Color_t color){
  hist->SetLineColorAlpha(color, .4);
  hist->SetFillColorAlpha(color, .4);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->SetLabelSize(0.02);
}
void hoge(int run){
  TString runNum = Form("%03d", run);
  setCanvas();

  TFile *fin = new TFile("../rootfile/run" + runNum + ".root");
  TTree *tree = (TTree*)fin->Get("tree");
  std::vector<double> *posx0u = 0;
  std::vector<double> *posy0u = 0;
  tree->SetBranchAddress("posx0u", &posx0u);
  tree->SetBranchAddress("posy0u", &posy0u);

  TH1D *histX = new TH1D("histX", ";x [mm]", 1000, -500, 500);
  setHist(histX, kAzure);
  TH1D *histY = new TH1D("histY", ";y [mm]", 1000, -500, 500);
  setHist(histY, kAzure);

  const int nEntries = tree->GetEntries();
  rep(entry, nEntries){
    if(entry % 1000 == 0) std::cout << "Entry: " << entry << "/" << nEntries << std::endl;
    tree->GetEntry(entry);
    const int nHits = posx0u->size();
    rep(hit, nHits){
      double x = posx0u->at(hit);
      double y = posy0u->at(hit);
      histX->Fill(x);
      histY->Fill(y);
    }
  }

  TCanvas *canvas1 = new TCanvas("canvas1", "canvas1", 800, 800);
  canvas1->cd();
  histX->Draw();
  TCanvas *canvas2 = new TCanvas("canvas2", "canvas2", 800, 800);
  canvas2->cd();
  histY->Draw();

  TString pdfPath = "./" + runNum + ".pdf";
  printCanvases({canvas1, canvas2}, pdfPath);

  system("say -v Alex 'complete no cone.cpp'");
}