using namespace std;

void DrawOneVar(const char* var, vector<string> list_name, vector<TFile*> list_file, const int i_ds0=-1)
{
  
  int n_ds = list_name.size();
  vector<TH1*> list_h1;
  THStack hs("hs", Form(";%s;Efficiency", var));
  TLegend leg(0.90, 0.90-n_ds*0.05, 0.99, 0.90);
  
  for (int i_ds = 0; i_ds < n_ds; i_ds++) {
    TH1* h1 = (TH1*)list_file[i_ds]->Get( Form("h1_eff_%s", var) );
    if (! h1) {
      cout << "!!ERROR!!  Cannot find h1 for " << list_name[i_ds] << ".  Abort." << endl;
      exit(1);
    }
    list_h1.push_back(h1);
    h1->SetLineColor(i_ds+1);
    h1->SetMarkerColor(i_ds+1);
    h1->SetMarkerStyle(7);
    hs.Add(h1, "E1");
    leg.AddEntry(h1, list_name[i_ds].c_str(), "pl");
  }
  
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();

  hs.Draw("nostack");
  leg.Draw();
  c1->SaveAs( Form("result/comp-eff/h1_eff_%s.pdf", var) );

  TCanvas* c2 = new TCanvas("c2", "");
  c2->SetGrid();
  
  if (i_ds0 >= 0) {
    const char* name0 = list_name[i_ds0].c_str();
    TLegend leg2(0.90, 0.90-n_ds*0.05, 0.99, 0.90);
    leg2.SetHeader( Form("To %s", name0) );
    bool is_1st = true;
    for (int i_ds = 0; i_ds < n_ds; i_ds++) {
      if (i_ds == i_ds0) continue;
      const char* name = list_name[i_ds].c_str();
      TH1* h1_ratio = (TH1*)list_h1[i_ds]->Clone( Form("h1_ratio_%i", i_ds) );
      h1_ratio->Divide(list_h1[i_ds0]);
      TH1* h1_ratio2 = (TH1*)h1_ratio->Clone( Form("h1_ratio2_%i", i_ds) );

      c1->cd();
      if (is_1st) {
        h1_ratio->SetTitle( Form(";%s;Efficiency Ratio", var) );
        h1_ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
        h1_ratio->Draw("E1");
        is_1st = false;
      } else {
        h1_ratio->Draw("E1same");
      }
      leg2.AddEntry(h1_ratio, name, "pl");

      c2->cd();
      h1_ratio2->SetTitle( Form("%s / %s;%s;Efficiency Ratio", name, name0, var) );
      h1_ratio2->GetYaxis()->SetRangeUser(0.5, 1.5);
      h1_ratio2->Fit("pol1", "EMQ", "E1");
      TF1* f1 = h1_ratio2->GetFunction("pol1");
      TLatex tex;
      tex.SetNDC(true);
      ostringstream oss;
      oss << "#chi^{2}/NDF = " << f1->GetChisquare() << " / " << f1->GetNDF();
      tex.DrawLatex(0.12, 0.85, oss.str().c_str());
      oss.str("");
      oss << "p0 = " << f1->GetParameter(0) << " #pm " << f1->GetParError(0);
      tex.DrawLatex(0.12, 0.80, oss.str().c_str());
      oss.str("");
      oss << "p1 = (" << 1e5 * f1->GetParameter(1) << " #pm " << 1e5 * f1->GetParError(1) << ") #times 10^{-5}";
      tex.DrawLatex(0.12, 0.75, oss.str().c_str());
      c2->SaveAs( Form("result/comp-eff/h1_ratio_%s_%s.pdf", var, name) );      
    }
    c1->cd();
    leg2.Draw();
    c1->SaveAs( Form("result/comp-eff/h1_ratio_%s.pdf", var) );
  }

  delete c1;
  delete c2;
}

void comp_eff_datasets()
{
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(true);
  gSystem->mkdir("result/comp-eff", true);

  vector<string> list_name = { "rs57", "rs59", "rs62", "rs67", "rs70" };
  vector<TFile*> list_file = {
    new TFile("0828/lh2/rs57/cl_me/result.root"), 
    new TFile("0828/lh2/rs59/cl_me/result.root"), 
    new TFile("0828/lh2/rs62/cl_me/result.root"), 
    new TFile("0828/lh2/rs67/cl_me/result.root"), 
    new TFile("0828/lh2/rs70/cl_me/result.root"), 
  };

  const int i_ds0 = 3; // Used as the denominator of the ratio plots.
  DrawOneVar("D1"   , list_name, list_file, i_ds0);
  DrawOneVar("D1-10", list_name, list_file, i_ds0);
  DrawOneVar("D2"   , list_name, list_file, i_ds0);
  
  exit(0);
}

