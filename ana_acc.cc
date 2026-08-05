#include <JobOptsSvc.h>
#include <GeomSvc.h>
#include "inc/UtilTrigger.h"
#include "inc/UtilTrack.h"
#include "inc/Common.h"
#include "inc/UtilCut.h"
R__LOAD_LIBRARY(kTracker)
using namespace std;
void Ana4pi(const char* fn_list);
void AnaAcc(const char* fn_list, const int rs_id);
void DrawOneKin(TH1* h1_4pi, TH1* h1_acc, TH1* h1_acc2, const char* var);

void ana_acc(const char* fn_list_4pi="auto_file/list_ana_4pi.txt", const char* fn_list_acc="auto_file/list_ana_vertex.txt")
{
  gSystem->mkdir("result/acc", true);

  int rs_id = atoi(gSystem->Getenv("ROADSET_ID"));
  cout << "Roadset " << rs_id << endl;
  string fn_opts = gSystem->Getenv("KTRACKER_ROOT");
  if      (rs_id == 57) fn_opts += "/opts/mc_57_2.opts";
  else if (rs_id == 67) fn_opts += "/opts/mc_67.opts";
  else {
    cout << "Cannot find a proper opts file.  Abort." << endl;
    exit(1);
  }
  JobOptsSvc::instance()->init(fn_opts.c_str());
  GeomSvc::instance()->init();

  /// Can skip Ana4pi() and/or AnaAcc() if their outputs are unchanged
  Ana4pi(fn_list_4pi);
  AnaAcc(fn_list_acc, rs_id);

  const double n_evt_gen = 10e6;
  
  TFile* f_4pi = new TFile("result/acc/4pi.root");
  TH1* h1_x1_4pi  = (TH1*)f_4pi->Get("h1_x1_4pi");
  TH1* h1_x2_4pi  = (TH1*)f_4pi->Get("h1_x2_4pi");
  TH1* h1_pT_4pi  = (TH1*)f_4pi->Get("h1_pT_4pi");
  h1_x1_4pi ->Scale(1/n_evt_gen);
  h1_x2_4pi ->Scale(1/n_evt_gen);
  h1_pT_4pi ->Scale(1/n_evt_gen);
  
  TFile* f_acc = new TFile("result/acc/acc.root");
  TH1* h1_x1_acc  = (TH1*)f_acc->Get("h1_x1_acc");
  TH1* h1_x2_acc  = (TH1*)f_acc->Get("h1_x2_acc");
  TH1* h1_pT_acc  = (TH1*)f_acc->Get("h1_pT_acc");
  TH1* h1_x1_acc2 = (TH1*)f_acc->Get("h1_x1_acc2");
  TH1* h1_x2_acc2 = (TH1*)f_acc->Get("h1_x2_acc2");
  TH1* h1_pT_acc2 = (TH1*)f_acc->Get("h1_pT_acc2");
  h1_x1_acc ->Scale(1/n_evt_gen);
  h1_x2_acc ->Scale(1/n_evt_gen);
  h1_pT_acc ->Scale(1/n_evt_gen);
  h1_x1_acc2->Scale(1/n_evt_gen);
  h1_x2_acc2->Scale(1/n_evt_gen);
  h1_pT_acc2->Scale(1/n_evt_gen);

  TFile* f_out = new TFile("result/acc/result.root", "RECREATE");
  gErrorIgnoreLevel = 1111;
  gStyle->SetOptStat(0);
  DrawOneKin(h1_x1_4pi, h1_x1_acc, h1_x1_acc2, "x1");
  DrawOneKin(h1_x2_4pi, h1_x2_acc, h1_x2_acc2, "x2");
  DrawOneKin(h1_pT_4pi, h1_pT_acc, h1_pT_acc2, "pT");
  f_out->Close();
  
  exit(0);
}

////////////////////////////////////////////////////////////////
/// Functions
///
void Ana4pi(const char* fn_list)
{
  cout << "ana_4pi()" << endl;
  TChain* tree = new TChain("save");
  ifstream ifs(fn_list);
  string fn_in;
  while (ifs >> fn_in) tree->Add(fn_in.c_str());
  ifs.close();
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "  " << n_tree << " trees, " << n_ent << " entries" << endl;

  TFile* f_out = new TFile("result/acc/4pi.root", "RECREATE");
  TH1* h1_x1_4pi  = new TH1D("h1_x1_4pi" , "", 40, 0.0, 1.0);
  TH1* h1_x2_4pi  = new TH1D("h1_x2_4pi" , "", 20, 0.0, 0.5);
  TH1* h1_pT_4pi  = new TH1D("h1_pT_4pi" , "", 20, 0.0, 2.0);

  SRawMCEvent* raw = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/100*10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100   ) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    if (UtilCut::Doc2111v42TrueDimuon(raw)) {
      double weight = raw->weight;
      h1_x1_4pi->Fill(raw->x1, weight);
      h1_x2_4pi->Fill(raw->x2, weight);
      h1_pT_4pi->Fill(raw->pT, weight);
    }
  }
  cout << endl;

  f_out->Write();
  f_out->Close();
}

void AnaAcc(const char* fn_list, const int rs_id)
{
  cout << "AnaAcc()" << endl;
  auto list_road_pos_top = UtilTrigger::ReadRoadList(rs_id, +1, +1);
  auto list_road_pos_bot = UtilTrigger::ReadRoadList(rs_id, +1, -1);
  auto list_road_neg_top = UtilTrigger::ReadRoadList(rs_id, -1, +1);
  auto list_road_neg_bot = UtilTrigger::ReadRoadList(rs_id, -1, -1);

  TChain* tree = new TChain("save");
  ifstream ifs(fn_list);
  string fn_in;
  while (ifs >> fn_in) tree->Add(fn_in.c_str());
  ifs.close();
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "  " << n_tree << " trees, " << n_ent << " entries" << endl;
  
  TFile* f_out = new TFile("result/acc/acc.root", "RECREATE");
  TH1* h1_x1_acc  = new TH1D("h1_x1_acc" , "", 40, 0.0, 1.0);
  TH1* h1_x2_acc  = new TH1D("h1_x2_acc" , "", 20, 0.0, 0.5);
  TH1* h1_pT_acc  = new TH1D("h1_pT_acc" , "", 20, 0.0, 2.0);
  TH1* h1_x1_acc2 = new TH1D("h1_x1_acc2", "", 40, 0.0, 1.0);
  TH1* h1_x2_acc2 = new TH1D("h1_x2_acc2", "", 20, 0.0, 0.5);
  TH1* h1_pT_acc2 = new TH1D("h1_pT_acc2", "", 20, 0.0, 2.0);

  SRawMCEvent* raw = 0;
  SRecEvent  * rec = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  tree->SetBranchAddress("recEvent", &rec);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/100*10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100   ) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    double weight = raw->weight;
    double mass_t = raw->mass;
    //double x2_t   = raw->x2;
    //double xF_t   = raw->xF;
    //double zvtx_t = raw->vtx.Z();

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
      h1_x1_acc->Fill(dim.x1, weight);
      h1_x2_acc->Fill(dim.x2, weight);
      h1_pT_acc->Fill(dim.pT, weight);
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
      h1_x1_acc2->Fill(dim.x1, weight);
      h1_x2_acc2->Fill(dim.x2, weight);
      h1_pT_acc2->Fill(dim.pT, weight);
    }
  }
  cout << endl;

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
