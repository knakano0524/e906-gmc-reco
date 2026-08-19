
TGraphAsymmErrors* ReadM027(const char* fname)
{
  TGraphAsymmErrors* gr = new TGraphAsymmErrors();
  ifstream ifs(fname);
  string line;
  getline(ifs, line); // Skip the header line
  double bin_lo, bin_hi, bin_ce;
  double y_cl, y_me, eff, err_lo, err_hi;
  int n_bin = 0;
  while (ifs >> bin_lo >> bin_hi >> bin_ce
         >> y_cl >> y_me >> eff >> err_lo >> err_hi) {
    int n_pt = gr->GetN();
    gr->SetPoint(n_pt, bin_ce, eff);
    gr->SetPointError(n_pt, bin_ce-bin_lo, bin_hi-bin_ce, err_lo, err_hi);
  }
  ifs.close();
  return gr;
}

void comp_eff_M027_S001()
{
  TGraphAsymmErrors* gr_m027 = ReadM027("data/effi_D1_reweight_new_mod.txt");

  TFile* f_new = new TFile("result/cl_me/result.root");
  TH1* h1_new  = (TH1*)f_new->Get("h1_eff_D1");
  TH1* h1_new2 = (TH1*)f_new->Get("h1_eff_D1-10");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(true);
  gSystem->mkdir("result/comp", true);

  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();

  gr_m027->SetLineColor(kBlack);
  h1_new ->SetMarkerColor(kRed);
  h1_new ->SetLineColor  (kRed);
  h1_new2->SetMarkerColor(kBlue);
  h1_new2->SetLineColor  (kBlue);
  gr_m027->Draw("AP");
  h1_new ->Draw("E1same");
  h1_new2->Draw("E1same");
  gr_m027->SetTitle("Red: New, Black: M027_S001;D1;Efficiency");
  TLegend leg(0.70, 0.75, 0.99, 0.90);
  leg.AddEntry(h1_new , "New MC: D1"   , "lp");
  leg.AddEntry(h1_new2, "New MC: D1-10", "lp");
  leg.AddEntry(gr_m027, "M027_S001"    , "lp");
  leg.Draw();
  c1->SaveAs("result/comp/h1_eff.pdf");

  if (gr_m027->GetN() != h1_new->GetNbinsX()) {
    cout << "Binning mismatch!!" << endl;
    exit(0);
  }
  TH1* h1_ratio  = (TH1*)h1_new ->Clone("h1_ratio");
  TH1* h1_ratio2 = (TH1*)h1_new2->Clone("h1_ratio2");
  for (int ii = 0; ii < gr_m027->GetN(); ii++) {
    double eff_m027    = gr_m027->GetPointY(ii);
    double err_hi_m027 = gr_m027->GetErrorYhigh(ii);
    double err_lo_m027 = gr_m027->GetErrorYlow (ii);
    double err_m027    = (err_hi_m027 + err_lo_m027) / 2;
    
    double eff_new     = h1_new->GetBinContent(ii+1);
    double err_new     = h1_new->GetBinError  (ii+1);
    double ratio = eff_new / eff_m027;
    double err_ra = ratio * sqrt( pow(err_m027/eff_m027, 2) + pow(err_new/eff_new, 2) );
    h1_ratio->SetBinContent(ii+1, ratio);
    h1_ratio->SetBinError  (ii+1, err_ra);

    double eff_new2     = h1_new2->GetBinContent(ii+1);
    double err_new2     = h1_new2->GetBinError  (ii+1);
    double ratio2 = eff_new2 / eff_m027;
    double err_ra2 = ratio2 * sqrt( pow(err_m027/eff_m027, 2) + pow(err_new2/eff_new2, 2) );
    h1_ratio2->SetBinContent(ii+1, ratio2);
    h1_ratio2->SetBinError  (ii+1, err_ra2);
  }
  h1_ratio ->Draw("E1");
  h1_ratio2->Draw("E1same");
  //h1_ratio->Fit("pol0");
  h1_ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_ratio->SetTitle(";D1;M027_S001 / New");
  c1->SaveAs("result/comp/h1_ratio.pdf");
  delete c1;
  
  exit(0);
}
