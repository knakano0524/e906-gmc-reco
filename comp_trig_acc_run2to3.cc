
void DrawOne(const char* var, TH1* h1_run2, TH1* h1_run3)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  h1_run2 ->SetLineColor(kRed);
  h1_run3->SetLineColor(kBlack);
  h1_run2->Draw("E1");
  h1_run3->Draw("E1same");
  h1_run2->SetTitle( Form("Red: Run 2, Black: Run 3;%s;Trigger acceptance", var) );
  c1->SaveAs( Form("0805-run2/run2to3/h1_trig_acc_%s.pdf", var) );

  TH1* h1_ratio = (TH1*)h1_run2->Clone("h1_ratio");
  h1_ratio->Divide(h1_run3);
  h1_ratio->Draw("E1");
  h1_ratio->Fit("pol0");
  h1_ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_ratio->SetTitle( Form(";%s;Run 2 / Run 3", var) );
  c1->SaveAs( Form("0805-run2/run2to3/h1_trig_acc_ratio_%s.pdf", var) );

  delete c1;
}

void comp_trig_acc_run2to3()
{
  TFile* f_run2 = new TFile("0805-run2/both/result.root");
  TH1* h1_acc_ma_run2 = (TH1*)f_run2->Get("h1_acc_fpga_mass");
  TH1* h1_acc_xF_run2 = (TH1*)f_run2->Get("h1_acc_fpga_xF");

  TFile* f_run3 = new TFile("0805-350/both/result.root");
  TH1* h1_acc_ma_run3 = (TH1*)f_run3->Get("h1_acc_fpga_mass");
  TH1* h1_acc_xF_run3 = (TH1*)f_run3->Get("h1_acc_fpga_xF");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(true);
  gSystem->mkdir("0805-run2/run2to3", true);

  DrawOne("mass", h1_acc_ma_run2, h1_acc_ma_run3);
  DrawOne("xF"  , h1_acc_xF_run2, h1_acc_xF_run3);
  
  exit(0);
}

