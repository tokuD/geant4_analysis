#include "TColor.h"
#include "TH1D.h"

void setHist(TH1D *hist, Color_t color){
  hist->SetLineColorAlpha(color, .4);
  hist->SetFillColorAlpha(color, .4);
  hist->GetXaxis()->SetTitleSize(0.05);
  hist->GetYaxis()->SetTitleSize(0.05);
  hist->SetLabelSize(0.04);
}