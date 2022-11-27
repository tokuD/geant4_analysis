#include "TColor.h"
#include "TH2D.h"

void setHist(TH2D *hist){
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->SetLabelSize(0.04);
}