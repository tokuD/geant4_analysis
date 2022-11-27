#include <vector>
#include "TCanvas.h"
#include "TString.h"

#define rep(i,n) for(int i=0; i<n; i++)

void printCanvases(std::vector<TCanvas*> canvases, TString pdfPath){
  rep(i, canvases.size()){
    if(i == 0) canvases.at(i)->Print(pdfPath + "[", "pdf");
    canvases.at(i)->Print(pdfPath, "pdf");
    if(i == canvases.size() - 1) canvases.at(i)->Print(pdfPath + "]", "pdf");
  }
}