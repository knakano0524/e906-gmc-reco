#include <JobOptsSvc.h>
#include <GeomSvc.h>
#include "inc/UtilTrigger.h"
#include "inc/UtilTrack.h"
#include "inc/Common.h"
#include "inc/UtilCut.h"
R__LOAD_LIBRARY(kTracker)
using namespace std;
void Ana4pi(const int tgt_id);
void AnaAcc(const string bg_mode, const int tgt_id, const int rs_id);
void DrawOneKin(TH1* h1_4pi, TH1* h1_acc, TH1* h1_acc2, const char* var);

struct AccData {
  double weight;
  double mass;
  double xF;
  double pT;
  AccData(): weight(0), mass(0), xF(0), pT(0) {;}
  virtual ~AccData() {;}
};

TFile* f_tree;

void ana_acc()
{
  gSystem->mkdir("result/acc", true);

  int tgt_id = atoi(gSystem->Getenv("TGT_ID"));  
  int rs_id  = atoi(gSystem->Getenv("ROADSET_ID"));
  cout << "tgt_id = " << tgt_id << "\n"
       << "rs_id  = " << rs_id << endl;
  
  string opt_name = gSystem->Getenv("OPT_NAME");  
  string fn_opts  = gSystem->Getenv("KTRACKER_ROOT");
  fn_opts += "/opts/mc_" + opt_name + ".opts";
  JobOptsSvc::instance()->init(fn_opts.c_str());
  GeomSvc::instance()->init();

  f_tree = new TFile("result/acc/tree.root", "RECREATE");
  
  /// Can skip Ana4pi() and/or AnaAcc() if their outputs are unchanged
  Ana4pi(tgt_id);
  AnaAcc("clean", tgt_id, rs_id);

  /// Read the 4pi and acc outputs to compute the acceptance.
  const double n_evt_gen = 10e6;
  
  TFile* f_4pi = new TFile("result/acc/4pi.root");
  TH1* h1_mass_4pi = (TH1*)f_4pi->Get("h1_mass_4pi");
  TH1* h1_xF_4pi   = (TH1*)f_4pi->Get("h1_xF_4pi");
  TH1* h1_x1_4pi   = (TH1*)f_4pi->Get("h1_x1_4pi");
  TH1* h1_x2_4pi   = (TH1*)f_4pi->Get("h1_x2_4pi");
  TH1* h1_pT_4pi   = (TH1*)f_4pi->Get("h1_pT_4pi");
  h1_mass_4pi->Scale(1/n_evt_gen);
  h1_xF_4pi  ->Scale(1/n_evt_gen);
  h1_x1_4pi  ->Scale(1/n_evt_gen);
  h1_x2_4pi  ->Scale(1/n_evt_gen);
  h1_pT_4pi  ->Scale(1/n_evt_gen);
  
  TFile* f_acc = new TFile("result/acc/acc.root");
  TH1* h1_mass_acc  = (TH1*)f_acc->Get("h1_mass_acc");
  TH1* h1_xF_acc    = (TH1*)f_acc->Get("h1_xF_acc");
  TH1* h1_x1_acc    = (TH1*)f_acc->Get("h1_x1_acc");
  TH1* h1_x2_acc    = (TH1*)f_acc->Get("h1_x2_acc");
  TH1* h1_pT_acc    = (TH1*)f_acc->Get("h1_pT_acc");
  TH1* h1_mass_acc2 = (TH1*)f_acc->Get("h1_mass_acc2");
  TH1* h1_xF_acc2   = (TH1*)f_acc->Get("h1_xF_acc2");
  TH1* h1_x1_acc2   = (TH1*)f_acc->Get("h1_x1_acc2");
  TH1* h1_x2_acc2   = (TH1*)f_acc->Get("h1_x2_acc2");
  TH1* h1_pT_acc2   = (TH1*)f_acc->Get("h1_pT_acc2");
  h1_mass_acc ->Scale(1/n_evt_gen);
  h1_xF_acc   ->Scale(1/n_evt_gen);
  h1_x1_acc   ->Scale(1/n_evt_gen);
  h1_x2_acc   ->Scale(1/n_evt_gen);
  h1_pT_acc   ->Scale(1/n_evt_gen);
  h1_mass_acc2->Scale(1/n_evt_gen);
  h1_xF_acc2  ->Scale(1/n_evt_gen);
  h1_x1_acc2  ->Scale(1/n_evt_gen);
  h1_x2_acc2  ->Scale(1/n_evt_gen);
  h1_pT_acc2  ->Scale(1/n_evt_gen);

  TFile* f_out = new TFile("result/acc/result.root", "RECREATE");
  gErrorIgnoreLevel = 1111;
  gStyle->SetOptStat(0);
  DrawOneKin(h1_mass_4pi, h1_mass_acc, h1_mass_acc2, "mass");
  DrawOneKin(h1_xF_4pi  , h1_xF_acc  , h1_xF_acc2  , "xF");
  DrawOneKin(h1_x1_4pi  , h1_x1_acc  , h1_x1_acc2  , "x1");
  DrawOneKin(h1_x2_4pi  , h1_x2_acc  , h1_x2_acc2  , "x2");
  DrawOneKin(h1_pT_4pi  , h1_pT_acc  , h1_pT_acc2  , "pT");
  f_out->Close();
  
  exit(0);
}

////////////////////////////////////////////////////////////////
/// Functions
///
void Ana4pi(const int tgt_id)
{
  cout << "ana_4pi()" << endl;
  TChain* tree = new TChain("save");
  string dir_data = "data/${RAW_NAME_BASE}_4pi";
  vector<string> list_in = FindFiles(dir_data, "user_*.root");
  for (auto it = list_in.begin(); it != list_in.end(); it++) tree->Add(it->c_str());
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "  " << n_tree << " trees, " << n_ent << " entries" << endl;

  TFile* f_out = new TFile("result/acc/4pi.root", "RECREATE");
  TH1* h1_mass_4pi = new TH1D("h1_mass_4pi", "", 50,    0, 10);
  TH1* h1_xF_4pi   = new TH1D("h1_xF_4pi"  , "", 30, -0.2, 1.0);
  TH1* h1_x1_4pi   = new TH1D("h1_x1_4pi"  , "", 40,  0.0, 1.0);
  TH1* h1_x2_4pi   = new TH1D("h1_x2_4pi"  , "", 20,  0.0, 0.5);
  TH1* h1_pT_4pi   = new TH1D("h1_pT_4pi"  , "", 20,  0.0, 2.0);

  f_tree->cd();
  TTree* tr_out = new TTree("tree_4pi", "");
  AccData acc_data;
  tr_out->Branch("event", &acc_data);
  
  SRawMCEvent* raw = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/100*10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100   ) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    if (UtilCut::Doc2111v42TrueDimuon(raw)) {
      double mass   = raw->mass;
      double xF     = raw->xF;
      double x1     = raw->x1;
      double x2     = raw->x2;
      double pT     = raw->pT;
      double weight = raw->weight * GetPtReWeight(tgt_id, pT, xF, mass);
      h1_mass_4pi->Fill(mass, weight);
      h1_xF_4pi  ->Fill(xF  , weight);
      h1_x1_4pi  ->Fill(x1  , weight);
      h1_x2_4pi  ->Fill(x2  , weight);
      h1_pT_4pi  ->Fill(pT  , weight);
      
      acc_data.weight = weight;
      acc_data.mass   = mass;
      acc_data.xF     = xF;
      acc_data.pT     = pT;
      tr_out->Fill();
    }
  }
  cout << endl;

  f_tree->cd();
  f_tree->Write();
  delete tr_out;

  f_out->cd();
  f_out->Write();
  f_out->Close();
}

void AnaAcc(const string bg_mode, const int tgt_id, const int rs_id)
{
  cout << "AnaAcc()" << endl;
  auto list_road_pos_top = UtilTrigger::ReadRoadList(rs_id, +1, +1);
  auto list_road_pos_bot = UtilTrigger::ReadRoadList(rs_id, +1, -1);
  auto list_road_neg_top = UtilTrigger::ReadRoadList(rs_id, -1, +1);
  auto list_road_neg_bot = UtilTrigger::ReadRoadList(rs_id, -1, -1);

  TChain* tree = new TChain("save");
  string dir_data = (string)"vertex/${RAW_NAME_BASE}_acc/" + bg_mode + "/vertex";
  vector<string> list_in = FindFiles(dir_data, "vertex_*.root");
  for (auto it = list_in.begin(); it != list_in.end(); it++) tree->Add(it->c_str());
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "  " << n_tree << " trees, " << n_ent << " entries" << endl;
  
  TFile* f_out = new TFile("result/acc/acc.root", "RECREATE");
  TH1* h1_mass_acc  = new TH1D("h1_mass_acc" , "", 50,    0, 10);
  TH1* h1_xF_acc    = new TH1D("h1_xF_acc"   , "", 30, -0.2, 1.0);
  TH1* h1_x1_acc    = new TH1D("h1_x1_acc"   , "", 40,  0.0, 1.0);
  TH1* h1_x2_acc    = new TH1D("h1_x2_acc"   , "", 20,  0.0, 0.5);
  TH1* h1_pT_acc    = new TH1D("h1_pT_acc"   , "", 20,  0.0, 2.0);
  TH1* h1_mass_acc2 = new TH1D("h1_mass_acc2", "", 50,    0, 10);
  TH1* h1_xF_acc2   = new TH1D("h1_xF_acc2"  , "", 30, -0.2, 1.0);
  TH1* h1_x1_acc2   = new TH1D("h1_x1_acc2"  , "", 40,  0.0, 1.0);
  TH1* h1_x2_acc2   = new TH1D("h1_x2_acc2"  , "", 20,  0.0, 0.5);
  TH1* h1_pT_acc2   = new TH1D("h1_pT_acc2"  , "", 20,  0.0, 2.0);

  f_tree->cd();
  TTree* tr_out = new TTree("tree_acc", "");
  AccData acc_data;
  tr_out->Branch("event", &acc_data);
  
  SRawMCEvent* raw = 0;
  SRecEvent  * rec = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  tree->SetBranchAddress("recEvent", &rec);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/100*10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100   ) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    double mass_t = raw->mass;
    double xF_t   = raw->xF;
    double pT_t   = raw->pT;
    //double x1_t   = raw->x1;
    //double x2_t   = raw->x2;
    //double zvtx_t = raw->vtx.Z();
    double weight = raw->weight * GetPtReWeight(tgt_id, pT_t, xF_t, mass_t);

    bool list_cut_ok[99];
    int i_dim_best = -1;
    double mass_diff_best = 1e10;
    for (int i_dim = 0; i_dim < rec->getNDimuons(); i_dim++) {
      SRecDimuon dim = rec->getDimuon(i_dim);
      SRecTrack trk_pos = rec->getTrack(dim.trackID_pos);
      SRecTrack trk_neg = rec->getTrack(dim.trackID_neg);
      bool cut_ok = list_cut_ok[i_dim] = UtilCut::Doc2111v42(rs_id, raw, &dim, &trk_pos, &trk_neg);
      double mass_diff = fabs(dim.mass - mass_t);
      if (cut_ok && mass_diff < mass_diff_best) {
        mass_diff_best = mass_diff;
        i_dim_best = i_dim;
      }
    }
    if (i_dim_best >= 0) {
      SRecDimuon dim = rec->getDimuon(i_dim_best);
      h1_mass_acc->Fill(dim.mass, weight);
      h1_xF_acc  ->Fill(dim.xF  , weight);
      h1_x1_acc  ->Fill(dim.x1  , weight);
      h1_x2_acc  ->Fill(dim.x2  , weight);
      h1_pT_acc  ->Fill(dim.pT  , weight);

      acc_data.weight = weight;
      acc_data.mass   = dim.mass;
      acc_data.xF     = dim.xF;
      acc_data.pT     = dim.pT;
      tr_out->Fill();
    }
    
    i_dim_best = -1;
    mass_diff_best = 1e10;
    for (int i_dim = 0; i_dim < rec->getNDimuons(); i_dim++) {
      if (! list_cut_ok[i_dim]) continue;
      SRecDimuon dim = rec->getDimuon(i_dim);
      SRecTrack trk_pos = rec->getTrack(dim.trackID_pos);
      SRecTrack trk_neg = rec->getTrack(dim.trackID_neg);
      vector<int> list_road_trk_pos = UtilTrack::FindMatchedRoads(&trk_pos, 1.0);
      vector<int> list_road_trk_neg = UtilTrack::FindMatchedRoads(&trk_neg, 1.0);
      auto matched_pt = UtilTrigger::FindEnabledRoads(list_road_trk_pos, list_road_pos_top);
      auto matched_pb = UtilTrigger::FindEnabledRoads(list_road_trk_pos, list_road_pos_bot);
      auto matched_nt = UtilTrigger::FindEnabledRoads(list_road_trk_neg, list_road_neg_top);
      auto matched_nb = UtilTrigger::FindEnabledRoads(list_road_trk_neg, list_road_neg_bot);
      bool fpga1 = (matched_pt.size() > 0 && matched_nb.size() > 0) ||
                   (matched_pb.size() > 0 && matched_nt.size() > 0);
      if (! fpga1) continue;
      
      double mass_diff = fabs(dim.mass - mass_t);
      if (mass_diff < mass_diff_best) {
        mass_diff_best = mass_diff;
        i_dim_best = i_dim;
      }
    }
    if (i_dim_best >= 0) {
      SRecDimuon dim = rec->getDimuon(i_dim_best);
      h1_mass_acc2->Fill(dim.mass, weight);
      h1_xF_acc2  ->Fill(dim.xF  , weight);
      h1_x1_acc2  ->Fill(dim.x1  , weight);
      h1_x2_acc2  ->Fill(dim.x2  , weight);
      h1_pT_acc2  ->Fill(dim.pT  , weight);
    }
  }
  cout << endl;

  f_tree->cd();
  f_tree->Write();
  delete tr_out;

  f_out->cd();
  f_out->Write();
  f_out->Close();
}


void DrawOneKin(TH1* h1_4pi, TH1* h1_acc, TH1* h1_acc2, const char* var)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  h1_4pi->Draw("E1");
  h1_4pi->SetTitle( Form("4pi;%s;sigWeighted yield", var) );
  c1->SaveAs( Form("result/acc/h1_%s_4pi.pdf", var) );
  
  h1_acc2->SetLineColor(kRed);
  h1_acc ->Draw("E1");
  h1_acc2->Draw("E1same");
  h1_acc->SetTitle( Form("Acc;%s;sigWeighted yield", var) );
  c1->SaveAs( Form("result/acc/h1_%s_acc.pdf", var) );
  
  TH1* h1_a  = (TH1*)h1_acc ->Clone( Form("h1_acc_%s" , var) );
  TH1* h1_a2 = (TH1*)h1_acc2->Clone( Form("h1_acc2_%s", var) );
  h1_a ->Divide(h1_4pi);
  h1_a2->Divide(h1_4pi);
  h1_a ->Draw("E1");
  h1_a2->Draw("E1same");
  h1_a->SetTitle( Form(";%s;Acceptance", var) );
  c1->SaveAs( Form("result/acc/h1_%s_acceptance.pdf", var) );
  h1_a ->Write();
  h1_a2->Write();
  //delete h1_a;
  //delete h1_a2;
  delete c1;
}
