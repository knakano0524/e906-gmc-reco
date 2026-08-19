
TH1* ReadHist(const char* var, const char* fname)
{
  double bin_edge[999];
  double list_acc[999];
  double list_err[999];
  ifstream ifs(fname);
  string line;
  getline(ifs, line); // Skip the header line
  double bin_lo, bin_hi, bin_ce;
  string acc, err;
  int n_bin = 0;
  while (ifs >> bin_lo >> bin_hi >> bin_ce >> acc >> err) {
    double acc2 = acc != "NaN"  ?  stod(acc)  :  -1;
    double err2 = err != "NaN"  ?  stod(err)  :  -1;
    bin_edge[n_bin  ] = bin_lo;
    bin_edge[n_bin+1] = bin_hi;
    list_acc[n_bin  ] = acc2;
    list_err[n_bin  ] = err2;
    n_bin++;
  }
  ifs.close();
  TH1* h1 = new TH1D(Form("h1_acc_%s_m027", var), "", n_bin, bin_edge);
  for (int ii = 0; ii < n_bin; ii++) {
    if (list_acc[ii] < 0) continue;
    h1->SetBinContent(ii+1, list_acc[ii]);
    h1->SetBinError  (ii+1, list_err[ii]);
  }
  return h1;
}

void DrawOne(const char* var, TH1* h1_new, TH1* h1_m027)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  h1_new ->SetLineColor(kRed);
  h1_m027->SetLineColor(kBlack);
  h1_new->Draw("E1");
  h1_m027->Draw("E1same");
  h1_new->SetTitle( Form("Red: New, Black: M027_S001;%s;Acceptance", var) );
  c1->SaveAs( Form("result/comp/h1_acc_%s.pdf", var) );

  TH1* h1_ratio = (TH1*)h1_m027->Clone("h1_ratio");
  h1_ratio->Divide(h1_new);
  h1_ratio->Draw("E1");
  h1_ratio->Fit("pol0");
  h1_ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  h1_ratio->SetTitle( Form(";%s;M027_S001 / New", var) );
  c1->SaveAs( Form("result/comp/h1_ratio_%s.pdf", var) );

  delete c1;
}

void comp_acc_M027_S001()
{
  TH1* h1_acc_x1_m027 = ReadHist("x1", "data/acceptance_factors_LH2_xB.txt");
  TH1* h1_acc_x2_m027 = ReadHist("x2", "data/acceptance_factors_LH2_xT.txt");
  TH1* h1_acc_pT_m027 = ReadHist("pT", "data/acceptance_factors_LH2_pT.txt");

  TFile* f_new = new TFile("result/acc/result.root");
  TH1* h1_acc_x1_new = (TH1*)f_new->Get("h1_acc_x1");
  TH1* h1_acc_x2_new = (TH1*)f_new->Get("h1_acc_x2");
  TH1* h1_acc_pT_new = (TH1*)f_new->Get("h1_acc_pT");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(true);
  gSystem->mkdir("comp", true);

  DrawOne("x1", h1_acc_x1_new, h1_acc_x1_m027);
  DrawOne("x2", h1_acc_x2_new, h1_acc_x2_m027);
  DrawOne("pT", h1_acc_pT_new, h1_acc_pT_m027);
  
  exit(0);
}

