
void draw_multi()
{
  TFile* f_r2 = new TFile("result-run2/event/result.root");
  TFile* f_r3 = new TFile("result-run3/event/result.root");

  TH2* h2_acc_r2      = (TH2*)f_r2->Get("h2_acc");
  TH1* h1_acc_mass_r2 = (TH1*)f_r2->Get("h1_acc_mass");
  TH1* h1_acc_xF_r2   = (TH1*)f_r2->Get("h1_acc_xF");
  
  TH2* h2_acc_r3      = (TH2*)f_r3->Get("h2_acc");
  TH1* h1_acc_mass_r3 = (TH1*)f_r3->Get("h1_acc_mass");
  TH1* h1_acc_xF_r3   = (TH1*)f_r3->Get("h1_acc_xF");

  gSystem->mkdir("result", true);
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  gStyle->SetOptStat(0);

  TLatex tex;
  tex.SetNDC(true);

  ///
  /// Draw two histgrams together
  ///
  h1_acc_mass_r2->SetLineColor(kBlue);
  h1_acc_mass_r3->SetLineColor(kRed);
  h1_acc_mass_r2->Draw("HIST");
  h1_acc_mass_r3->Draw("HISTsame");
  tex.SetTextColor(kBlue);  tex.DrawLatex(0.15, 0.80, "Run 2");
  tex.SetTextColor(kRed );  tex.DrawLatex(0.15, 0.75, "Run 3");
  c1->SaveAs("result/h1_acc_mass_run23.pdf");
  
  h1_acc_xF_r2->SetLineColor(kBlue);
  h1_acc_xF_r3->SetLineColor(kRed);
  h1_acc_xF_r2->Draw("HIST");
  h1_acc_xF_r3->Draw("HISTsame");
  tex.SetTextColor(kBlue);  tex.DrawLatex(0.15, 0.80, "Run 2");
  tex.SetTextColor(kRed );  tex.DrawLatex(0.15, 0.75, "Run 3");
  c1->SaveAs("result/h1_acc_xF_run23.pdf");

  ///
  /// Draw ratios
  ///
  h2_acc_r2     ->Divide(h2_acc_r3);
  h1_acc_mass_r2->Divide(h1_acc_mass_r3);
  h1_acc_xF_r2  ->Divide(h1_acc_xF_r3);

  h2_acc_r2->GetZaxis()->SetRangeUser(0.5, 1.5);
  h2_acc_r2->Draw("colz");
  h2_acc_r2->SetTitle("Run-2 / Run-3 ratio of trigger acceptance");
  c1->SaveAs("result/h2_acc_ratio.pdf");

  h1_acc_mass_r2->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_acc_mass_r2->Draw("HIST");
  h1_acc_mass_r2->GetYaxis()->SetTitle("Run-2/Run-3 ratio of trigger acceptance");
  c1->SaveAs("result/h1_acc_mass_ratio.pdf");

  h1_acc_xF_r2->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_acc_xF_r2->Draw("HIST");
  h1_acc_xF_r2->GetYaxis()->SetTitle("Run-2/Run-3 ratio of trigger acceptance");
  c1->SaveAs("result/h1_acc_xF_ratio.pdf");

  exit(0);
}
