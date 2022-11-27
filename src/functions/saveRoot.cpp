#include "TString.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"

void saveRoot(TString path, std::vector<TCanvas*> canvases){
  TFile *fout = new TFile(path, "RECREATE");
  for(auto canvas: canvases){
    canvas->Write();
  }
  fout->Close();
}