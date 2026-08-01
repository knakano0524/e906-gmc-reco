#include <JobOptsSvc.h>
#include <GeomSvc.h>
#include "inc/Common.h"
#include "inc/UtilCut.h"
R__LOAD_LIBRARY(kTracker)
using namespace std;
void CalcAndDrawEff(TH2* h2, const string label);

void ana_clean_messy(const char* fn_list="auto_file/list_ana_clean_messy.txt")
{
  const int tgt = 1; // LH2
  int rs_id = atoi(gSystem->Getenv("ROADSET_ID"));
  cout << "Roadset " << rs_id << endl;
  //auto list_road_pos_top = UtilTrigger::ReadRoadList(rs_id, +1, +1);
  //auto list_road_pos_bot = UtilTrigger::ReadRoadList(rs_id, +1, -1);
  //auto list_road_neg_top = UtilTrigger::ReadRoadList(rs_id, -1, +1);
  //auto list_road_neg_bot = UtilTrigger::ReadRoadList(rs_id, -1, -1);

  string fn_opts = gSystem->Getenv("KTRACKER_ROOT");
  if      (rs_id == 57) fn_opts += "/opts/mc_57_2.opts";
  else if (rs_id == 67) fn_opts += "/opts/mc_67.opts";
  else {
    cout << "Cannot find a proper opts file.  Abort." << endl;
    exit(1);
  }
  JobOptsSvc::instance()->init(fn_opts.c_str());
  GeomSvc::instance()->init();
  
  TChain* tree_cl = new TChain("save");
  TChain* tree_me = new TChain("save");
  ifstream ifs(fn_list);
  string fn_in_cl, fn_in_me;
  while (ifs >> fn_in_cl >> fn_in_me) {
    tree_cl->Add(fn_in_cl.c_str());
    tree_me->Add(fn_in_me.c_str());
  }
  ifs.close();
  
  gSystem->mkdir("result/cl_me", true);
  TFile* f_out = new TFile("result/cl_me/result.root", "RECREATE");
  TH1* h1_cnt = new TH1D("h1_cnt", "", 10, 0.5, 10.5);
  TH2* h2_d1 = new TH2D("h2_d1", ";D1;N of clean dimuons", 40, 0, 400, 4, 0.5, 4.5);
  TH2* h2_d2 = new TH2D("h2_d2", ";D2;N of clean dimuons", 20, 0, 200, 4, 0.5, 4.5);
  
  //tree_evt->Print();
  unsigned int n_tree_cl = tree_cl->GetNtrees();
  unsigned int n_ent_cl  = tree_cl->GetEntries();
  unsigned int n_ent_me  = tree_me->GetEntries();
  cout << "n_tree_cl = " << n_tree_cl << ", n_ent_cl = " << n_ent_cl << ", n_ent_me = " << n_ent_me << endl;
  if (n_ent_cl != n_ent_me) {
    cout << "!!ERROR!!  n_ent_cl != n_ent_me.  Abort." << endl;
    exit(1);
  }

  SRecEvent  * rec_cl = 0;
  SRawMCEvent* raw_me = 0;
  SRecEvent  * rec_me = 0;
  tree_cl->SetBranchAddress("recEvent", &rec_cl);
  tree_me->SetBranchAddress("rawEvent", &raw_me);
  tree_me->SetBranchAddress("recEvent", &rec_me);
  for (unsigned int i_ent = 0; i_ent < n_ent_cl; i_ent++) {
    if      ( (i_ent+1) % (n_ent_cl/100*10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent_cl/100   ) == 0) cout << "." << flush;
    tree_cl->GetEntry(i_ent);
    tree_me->GetEntry(i_ent);
    int   run_id = rec_cl->getRunID();
    int event_id = rec_cl->getEventID();
    if (run_id != rec_me->getRunID() || event_id != rec_me->getEventID()) {
      cout << "!!ERROR!!  run_id or event_id mismatch.  Abort." << endl;
      exit(1);
    }
    h1_cnt->Fill(1);

    ///
    /// Check the T+B/B+T condition using hodoscope hits.
    ///
    auto list_h1b = raw_me->getHitsIndexInDetector(31);
    auto list_h1t = raw_me->getHitsIndexInDetector(32);
    auto list_h2b = raw_me->getHitsIndexInDetector(37);
    auto list_h2t = raw_me->getHitsIndexInDetector(38);
    auto list_h3b = raw_me->getHitsIndexInDetector(39);
    auto list_h3t = raw_me->getHitsIndexInDetector(40);
    auto list_h4b = raw_me->getHitsIndexInDetector(45);
    auto list_h4t = raw_me->getHitsIndexInDetector(46);
    if (list_h1b.size() == 0 || list_h1t.size() == 0 ||
        list_h2b.size() == 0 || list_h2t.size() == 0 ||
        list_h3b.size() == 0 || list_h3t.size() == 0 ||
        list_h4b.size() == 0 || list_h4t.size() == 0   ) {
      continue; // Require the T+B/B+T condition.
    }
    h1_cnt->Fill(2);

    double mass_t = raw_me->mass;
    double xF_t   = raw_me->xF;
    double pT_t   = raw_me->pT;
    double weight = raw_me->weight;
    //double weight = raw_me->weight * GetPtReWeight(tgt, pT_t, xF_t, mass_t);
    int n_d1 = raw_me->getNHitsInD1();
    int n_d2 = raw_me->getNHitsInD2();

    ///
    /// Find reco. dimuons in Clean and Measy.
    ///
    int n_dim_cl = rec_cl->getNDimuons();
    if (n_dim_cl > 0) h1_cnt->Fill(3);    
    bool found_dim_cl = false;
    for (int i_dim = 0; i_dim < n_dim_cl; i_dim++) {
      SRecDimuon dim    = rec_cl->getDimuon(i_dim);
      SRecTrack trk_pos = rec_cl->getTrack(dim.trackID_pos);
      SRecTrack trk_neg = rec_cl->getTrack(dim.trackID_neg);
      //if (dim.mass > 4.2) {
      if (UtilCut::Doc2111v42(rs_id, raw_me, &dim, &trk_pos, &trk_neg)) {
        found_dim_cl = true;
        break;
      }
    }

    int n_dim_me = rec_me->getNDimuons();
    if (n_dim_me > 0) h1_cnt->Fill(4);
    bool found_dim_me = false;
    for (int i_dim = 0; i_dim < n_dim_me; i_dim++) {
      SRecDimuon dim    = rec_me->getDimuon(i_dim);
      SRecTrack trk_pos = rec_me->getTrack(dim.trackID_pos);
      SRecTrack trk_neg = rec_me->getTrack(dim.trackID_neg);
      //if (dim.mass > 4.2) {
      if (UtilCut::Doc2111v42(rs_id, raw_me, &dim, &trk_pos, &trk_neg)) {
        found_dim_me = true;
        break;
      }
    }
    
    if (found_dim_cl) h1_cnt->Fill(5);
    if (found_dim_me) h1_cnt->Fill(6);
    int bin_y = 1 + (found_dim_cl ? 2 : 0) + (found_dim_me ? 1 : 0);
    h2_d1->Fill(n_d1, bin_y, weight);
    h2_d2->Fill(n_d2, bin_y, weight);
  }
  cout << endl;
  
  ofstream ofs("result/cl_me/result.txt");
  ofs << "N of trees = " << n_tree_cl << "\n"
      << "N of all events     = " << n_ent_cl << "\n"
      << "N of matched events = " << h1_cnt->GetBinContent(1) << "\n"
      << "N of T+B/B+T events = " << h1_cnt->GetBinContent(2) << "\n"
      << "N of events with clean dimuons      = " << h1_cnt->GetBinContent(3) << "\n"
      << "N of events with messy dimuons      = " << h1_cnt->GetBinContent(4) << "\n"
      << "N of events with good clean dimuons = " << h1_cnt->GetBinContent(5) << "\n"
      << "N of events with good messy dimuons = " << h1_cnt->GetBinContent(6) << "\n";
  ofs.close();

  gErrorIgnoreLevel = 1111;

  CalcAndDrawEff(h2_d1, "D1");
  CalcAndDrawEff(h2_d2, "D2");
  
  f_out->Write();
  f_out->Close();

  exit(0);
}

void CalcAndDrawEff(TH2* h2, const string label)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  TH1* h1_c0m1 = h2->ProjectionX("h1_c0m1", 2, 2);
  TH1* h1_c1m0 = h2->ProjectionX("h1_c1m0", 3, 3);
  TH1* h1_c1m1 = h2->ProjectionX("h1_c1m1", 4, 4);
  h1_c0m1->SetFillColor(kRed);
  h1_c1m0->SetFillColor(kYellow);
  h1_c1m1->SetFillColor(kGreen);
  THStack hs("hs", (";"+label+";N of dimuons").c_str());
  hs.Add(h1_c1m1, "HIST");
  hs.Add(h1_c0m1, "HIST");
  hs.Add(h1_c1m0, "HIST");
  hs.Draw();
  TLegend leg(0.70, 0.75, 0.99, 0.90);
  leg.AddEntry(h1_c1m1, "  clean &   messy", "f");
  leg.AddEntry(h1_c0m1, "! clean &   messy", "f");
  leg.AddEntry(h1_c1m0, "  clean & ! messy", "f");
  leg.Draw();
  c1->SetLogy(true);
  c1->SaveAs(Form("result/cl_me/h1_%s.pdf", label.c_str()));
  c1->SetLogy(false);

  TH1* h1_eff = (TH1*)h1_c1m1->Clone(Form("h1_eff_%s", label.c_str()));
  h1_eff->Reset();
  for (int ix = 1; ix <= h1_eff->GetNbinsX(); ix++) {
    double n01 = h1_c0m1->GetBinContent(ix);
    double e01 = h1_c0m1->GetBinError  (ix);
    double n10 = h1_c1m0->GetBinContent(ix);
    double e10 = h1_c1m0->GetBinError  (ix);
    double n11 = h1_c1m1->GetBinContent(ix);
    double e11 = h1_c1m1->GetBinError  (ix);
    if (n10 + n11 == 0) continue;
    double eff = (n11 + n01) / (n11 + n10);
    double err = sqrt( pow((n10-n01)*e11, 2) + pow((n11+n01)*e10, 2) + pow((n11+n10)*e01, 2) ) / pow(n11+n10, 2);
    h1_eff->SetBinContent(ix, eff);
    h1_eff->SetBinError  (ix, err);
  }
  gStyle->SetOptStat(0);
  h1_eff->SetMarkerStyle(7);
  h1_eff->Draw("E1");
  h1_eff->SetTitle( Form(";%s;Efficiency", label.c_str()) );
  h1_eff->GetYaxis()->SetRangeUser(-0.1, 1.2);
  c1->SaveAs( Form("result/cl_me/h1_eff_%s.pdf", label.c_str()) );
  h1_eff->Write();
  delete h1_c0m1;
  delete h1_c1m0;
  delete h1_c1m1;
  delete c1;
}
