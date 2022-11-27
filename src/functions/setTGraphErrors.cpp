#include "TGraphErrors.h"
#include "TString.h"
#include "TColor.h"

void setGraph(TGraphErrors *graph, TString title, Color_t color){
  graph->SetMarkerColor(color);
  graph->SetMarkerStyle(8);
  graph->SetMarkerSize(1.5);
  graph->SetTitle(title);
}