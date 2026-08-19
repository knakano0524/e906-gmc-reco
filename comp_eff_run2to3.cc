
void DrawOne(const char* var, TH1* h1_run2, TH1* h1_run3)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  h1_run2->SetMarkerColor(kRed);
  h1_run2->SetLineColor  (kRed);
  h1_run3->SetMarkerColor(kBlack);
  h1_run3->SetLineColor  (kBlack);
  h1_run2->Draw("E1");
  h1_run3->Draw("E1same");
  h1_run2->SetTitle( Form("Red: Run 2, Black: Run 3;%s;Acceptance", var) );
  c1->SaveAs( Form("0812-eff/comp/h1_eff_%s.pdf", var) );

  TH1* h1_ratio = (TH1*)h1_run2->Clone("h1_ratio");
  h1_ratio->Divide(h1_run3);
  h1_ratio->Draw("E1");
  //h1_ratio->Fit("pol0");
  h1_ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_ratio->SetTitle( Form(";%s;Run 2 / Run 3", var) );
  c1->SaveAs( Form("0812-eff/comp/h1_ratio_eff_%s.pdf", var) );

  delete c1;
}

void comp_eff_run2to3()
{
  TFile* f_run2 = new TFile("0812-eff/run2/result.root");
  TH1* h1_eff_D1_run2 = (TH1*)f_run2->Get("h1_eff_D1");
  TH1* h1_eff_D2_run2 = (TH1*)f_run2->Get("h1_eff_D2");

  TFile* f_run3 = new TFile("0812-eff/run3/result.root");
  TH1* h1_eff_D1_run3 = (TH1*)f_run3->Get("h1_eff_D1");
  TH1* h1_eff_D2_run3 = (TH1*)f_run3->Get("h1_eff_D2");

  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(true);
  gSystem->mkdir("0812-eff/comp", true);

  DrawOne("D1", h1_eff_D1_run2, h1_eff_D1_run3);
  DrawOne("D2", h1_eff_D2_run2, h1_eff_D2_run3);
  
  exit(0);
}

