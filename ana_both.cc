#include <JobOptsSvc.h>
#include <GeomSvc.h>
#include "inc/UtilTrigger.h"
#include "inc/UtilTrack.h"
#include "inc/Common.h"
R__LOAD_LIBRARY(kTracker)
using namespace std;
void DrawOneTrackDist(TCanvas* c1, TH3* h3);

void ana_both(const char* fn_list_evt="auto_file/list_ana_event.txt", const char* fn_list_vtx="auto_file/list_ana_vertex.txt")
{
  int rs_id = atoi(gSystem->Getenv("ROADSET_ID"));
  cout << "Roadset " << rs_id << endl;
  auto list_road_pos_top = UtilTrigger::ReadRoadList(rs_id, +1, +1);
  auto list_road_pos_bot = UtilTrigger::ReadRoadList(rs_id, +1, -1);
  auto list_road_neg_top = UtilTrigger::ReadRoadList(rs_id, -1, +1);
  auto list_road_neg_bot = UtilTrigger::ReadRoadList(rs_id, -1, -1);

  string fn_opts = gSystem->Getenv("KTRACKER_ROOT");
  if      (rs_id == 57) fn_opts += "/opts/mc_57_2.opts";
  else if (rs_id == 67) fn_opts += "/opts/mc_67.opts";
  else {
    cout << "Cannot find a proper opts file.  Abort." << endl;
    exit(1);
  }
  JobOptsSvc::instance()->init(fn_opts.c_str());
  GeomSvc::instance()->init();
  
  TChain* tree_evt = new TChain("save");
  ifstream ifs(fn_list_evt);
  string fn_in;
  while (ifs >> fn_in) tree_evt->Add(fn_in.c_str());
  ifs.close();

  TChain* tree_vtx = new TChain("save");
  ifs.open(fn_list_vtx);
  while (ifs >> fn_in) tree_vtx->Add(fn_in.c_str());
  ifs.close();
  
  gSystem->mkdir("result/both", true);
  TFile* f_out = new TFile("result/both/result.root", "RECREATE");
  TH1* h1_cnt = new TH1D("h1_cnt", "", 10, 0.5, 10.5);
  TH2* h2_mass_xF_tbbt    = new TH2D("h2_mass_xF_tbbt"   ,       "T+B/B+T;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_xF_fpga    = new TH2D("h2_mass_xF_fpga"   ,         "FPGA1;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_xF_rec     = new TH2D("h2_mass_xF_rec"    ,         "Reco.;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_xF_rec_mat = new TH2D("h2_mass_xF_rec_mat", "Reco. Matched;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_zvtx        = new TH2D("h2_zvtx", ";z-vertex;", 100, -180, -80,  5, 0.5, 5.5);
  TH2* h2_rec_st      = new TH2D("h2_rec_st", ";Reco. status;", 40, -39.5, 0.5,  5, 0.5, 5.5);
  TH3* h3_ntrk        = new TH3D("h3_ntrk", ";N of tracks;Pos/Neg", 5, -0.5, 4.5,  3, -1.5, 1.5,  5, 0.5, 5.5);
  TH3* h3_px_vtx      = new TH3D("h3_px_vtx", ";px @ vertex;Pos/Neg", 50, -5, 5,  3, -1.5, 1.5,  5, 0.5, 5.5);  
  TH3* h3_py_vtx      = new TH3D("h3_py_vtx", ";py @ vertex;Pos/Neg", 50, -5, 5,  3, -1.5, 1.5,  5, 0.5, 5.5);  
  TH3* h3_pz_vtx      = new TH3D("h3_pz_vtx", ";pz @ vertex;Pos/Neg", 60, 0, 60,  3, -1.5, 1.5,  5, 0.5, 5.5);  
  TH3* h3_pxy_st1_pos = new TH3D("h3_pxy_st1_pos", ";px @ St.1;py @ St.1;", 60, -3, 3, 60, -3, 3, 5, 0.5, 5.5);
  TH3* h3_pxy_st1_neg = new TH3D("h3_pxy_st1_neg", ";px @ St.1;py @ St.1;", 60, -3, 3, 60, -3, 3, 5, 0.5, 5.5);
  TH3* h3_txy_st1_pos = new TH3D("h3_txy_st1_pos", ";px/pz @ St.1;py/pz @ St.1;", 60, -0.15, 0.15, 50, -0.1, 0.1, 5, 0.5, 5.5);
  TH3* h3_txy_st1_neg = new TH3D("h3_txy_st1_neg", ";px/pz @ St.1;py/pz @ St.1;", 60, -0.15, 0.15, 50, -0.1, 0.1, 5, 0.5, 5.5);
  TH3* h3_xy_st1_pos  = new TH3D("h3_xy_st1_pos" ,   ";x @ St.1;y @ St.1;", 70, -70, 70, 70, -70, 70, 5, 0.5, 5.5);
  TH3* h3_xy_st1_neg  = new TH3D("h3_xy_st1_neg" ,   ";x @ St.1;y @ St.1;", 70, -70, 70, 70, -70, 70, 5, 0.5, 5.5);
  TH3* h3_pxy_st3_pos = new TH3D("h3_pxy_st3_pos", ";px @ St.3;py @ St.3;", 60, -3, 3, 60, -3, 3, 5, 0.5, 5.5);
  TH3* h3_pxy_st3_neg = new TH3D("h3_pxy_st3_neg", ";px @ St.3;py @ St.3;", 60, -3, 3, 60, -3, 3, 5, 0.5, 5.5);
  TH3* h3_txy_st3_pos = new TH3D("h3_txy_st3_pos", ";px/pz @ St.1;py/pz @ St.1;", 60, -0.15, 0.15, 50, -0.1, 0.1, 5, 0.5, 5.5);
  TH3* h3_txy_st3_neg = new TH3D("h3_txy_st3_neg", ";px/pz @ St.1;py/pz @ St.1;", 60, -0.15, 0.15, 50, -0.1, 0.1, 5, 0.5, 5.5);
  TH3* h3_xy_st3_pos  = new TH3D("h3_xy_st3_pos" ,   ";x @ St.3;y @ St.3;", 85, -170, 170, 85, -170, 170, 5, 0.5, 5.5);
  TH3* h3_xy_st3_neg  = new TH3D("h3_xy_st3_neg" ,   ";x @ St.3;y @ St.3;", 85, -170, 170, 85, -170, 170, 5, 0.5, 5.5);
  //TH3* h3_nhit_cham   = new TH3D("h3_nhit_cham"  , ";N of hits;D1,D2,D3,P4;", 50, -0.5, 49.5, 4, 0.5, 4.5, 5, 0.5, 5.5);
  
  //tree_evt->Print();
  unsigned int n_tree_evt = tree_evt->GetNtrees();
  unsigned int n_ent_evt  = tree_evt->GetEntries();
  cout << "Event :  " << n_tree_evt << " trees, " << n_ent_evt << " entries" << endl;  
  unsigned int n_tree_vtx = tree_vtx->GetNtrees();
  unsigned int n_ent_vtx  = tree_vtx->GetEntries();
  cout << "Vertex:  " << n_tree_vtx << " trees, " << n_ent_vtx << " entries" << endl;

  SRawMCEvent* raw = 0;
  SRecEvent  * rec = 0;
  tree_evt->SetBranchAddress("rawEvent", &raw);
  tree_vtx->SetBranchAddress("recEvent", &rec);
  unsigned int n_evt_missed = 0;
  unsigned int i_ent_vtx = 0;
  for (unsigned int i_ent_evt = 0; i_ent_evt < n_ent_evt; i_ent_evt++) {
    if      ( (i_ent_evt+1) % (n_ent_evt/ 10) == 0) cout << "o" << flush;
    else if ( (i_ent_evt+1) % (n_ent_evt/100) == 0) cout << "." << flush;
    tree_evt->GetEntry(i_ent_evt);
    int   run_id = raw->getRunID();
    int event_id = raw->getEventID();
    bool found = false;
    while (true) {
      tree_vtx->GetEntry(i_ent_vtx);
      int rr = rec->getRunID();
      int ee = rec->getEventID();
      if (rr == run_id && ee == event_id) {
        found = true;
        break;
      } else if (rr > run_id || (rr == run_id && ee > event_id)) {
        n_evt_missed++;
        break;
      } else if (i_ent_vtx == n_ent_vtx - 1) {
        n_evt_missed++;
        break;
      }
      i_ent_vtx++;
    }
    if (! found) {
      if (i_ent_vtx == n_ent_vtx - 1) break; // No more candidates in tree_vtx.
      continue;
    }
    h1_cnt->Fill(1);

    double mass_t = raw->mass;
    double xF_t   = raw->xF;
    double zvtx_t = raw->vtx.Z();
    double weight = raw->weight;
    int    rec_st = rec->getRecStatus();

    //if (mass_t < 6 || mass_t > 7) continue;
    //if (fabs(raw->v_station1[0].X()) > 50) continue;
    //if (fabs(raw->v_station1[1].X()) > 50) continue;
    //if (fabs(raw->v_station1[0].Y()) < 10) continue;
    //if (fabs(raw->v_station1[1].Y()) < 10) continue;
    //if (fabs(raw->v_station1[0].Y()) > 40) continue;
    //if (fabs(raw->v_station1[1].Y()) > 40) continue;
    //if (fabs(raw->v_station3[0].Y()) > 140) continue;
    //if (fabs(raw->v_station3[1].Y()) > 140) continue;
    
    ///
    /// Check the T+B/B+T condition using hodoscope hits.
    ///
    auto list_h1b = raw->getHitsIndexInDetector(31);
    auto list_h1t = raw->getHitsIndexInDetector(32);
    auto list_h2b = raw->getHitsIndexInDetector(37);
    auto list_h2t = raw->getHitsIndexInDetector(38);
    auto list_h3b = raw->getHitsIndexInDetector(39);
    auto list_h3t = raw->getHitsIndexInDetector(40);
    auto list_h4b = raw->getHitsIndexInDetector(45);
    auto list_h4t = raw->getHitsIndexInDetector(46);
    if (list_h1b.size() == 0 || list_h1t.size() == 0 ||
        list_h2b.size() == 0 || list_h2t.size() == 0 ||
        list_h3b.size() == 0 || list_h3t.size() == 0 ||
        list_h4b.size() == 0 || list_h4t.size() == 0   ) {
      continue; // Require the T+B/B+T condition.
    }
    h1_cnt->Fill(2);
    h2_mass_xF_tbbt->Fill(mass_t, xF_t, weight);
    h2_zvtx        ->Fill(zvtx_t, 1, weight);
    h2_rec_st      ->Fill(rec_st, 1, weight);

    int n_trk_pos = 0;
    int n_trk_neg = 0;
    for (int i_trk = 0; i_trk < rec->getNTracks(); i_trk++) {
      SRecTrack trk = rec->getTrack(i_trk);
      if (trk.getCharge() > 0) n_trk_pos++;
      else                     n_trk_neg++;
    }    
    h3_ntrk->Fill(n_trk_pos, +1, 1, weight);
    h3_ntrk->Fill(n_trk_neg, -1, 1, weight);

    TVector3* p_vtx_pos = &raw->p_vertex[0];
    TVector3* p_vtx_neg = &raw->p_vertex[1];
    h3_px_vtx->Fill(p_vtx_pos->X(), +1, 1, weight);
    h3_py_vtx->Fill(p_vtx_pos->Y(), +1, 1, weight);
    h3_pz_vtx->Fill(p_vtx_pos->Z(), +1, 1, weight);
    h3_px_vtx->Fill(p_vtx_neg->X(), -1, 1, weight);
    h3_py_vtx->Fill(p_vtx_neg->Y(), -1, 1, weight);
    h3_pz_vtx->Fill(p_vtx_neg->Z(), -1, 1, weight);
    
    TVector3* p_st1_pos = &raw->p_station1[0];
    TVector3* p_st1_neg = &raw->p_station1[1];
    TVector3* v_st1_pos = &raw->v_station1[0];
    TVector3* v_st1_neg = &raw->v_station1[1];
    TVector3* p_st3_pos = &raw->p_station3[0];
    TVector3* p_st3_neg = &raw->p_station3[1];
    TVector3* v_st3_pos = &raw->v_station3[0];
    TVector3* v_st3_neg = &raw->v_station3[1];
    h3_pxy_st1_pos->Fill(p_st1_pos->X(), p_st1_pos->Y(), 1, weight);
    h3_pxy_st1_neg->Fill(p_st1_neg->X(), p_st1_neg->Y(), 1, weight);
    h3_txy_st1_pos->Fill(p_st1_pos->X()/p_st1_pos->Z(), p_st1_pos->Y()/p_st1_pos->Z(), 1, weight);
    h3_txy_st1_neg->Fill(p_st1_neg->X()/p_st1_neg->Z(), p_st1_neg->Y()/p_st1_neg->Z(), 1, weight);
    h3_xy_st1_pos ->Fill(v_st1_pos->X(), v_st1_pos->Y(), 1, weight);
    h3_xy_st1_neg ->Fill(v_st1_neg->X(), v_st1_neg->Y(), 1, weight);
    h3_pxy_st3_pos->Fill(p_st3_pos->X(), p_st3_pos->Y(), 1, weight);
    h3_pxy_st3_neg->Fill(p_st3_neg->X(), p_st3_neg->Y(), 1, weight);
    h3_txy_st3_pos->Fill(p_st3_pos->X()/p_st3_pos->Z(), p_st3_pos->Y()/p_st3_pos->Z(), 1, weight);
    h3_txy_st3_neg->Fill(p_st3_neg->X()/p_st3_neg->Z(), p_st3_neg->Y()/p_st3_neg->Z(), 1, weight);
    h3_xy_st3_pos ->Fill(v_st3_pos->X(), v_st3_pos->Y(), 1, weight);
    h3_xy_st3_neg ->Fill(v_st3_neg->X(), v_st3_neg->Y(), 1, weight);

    //int n_d1 = raw->getNHitsInD1();
    //int n_d2 = raw->getNHitsInD2();
    //int n_d3 = raw->getNHitsInD3();
    //int n_p4 = raw->getNHitsInP1() + raw->getNHitsInP2();
    //h3_nhit_cham->Fill(n_d1, 1, 1, weight);
    //h3_nhit_cham->Fill(n_d2, 2, 1, weight);
    //h3_nhit_cham->Fill(n_d3, 3, 1, weight);
    //h3_nhit_cham->Fill(n_p4, 4, 1, weight);
    
    ///
    /// Check FPGA1 trigger condition using hodoscope hits.
    ///
    vector<int> list_road_top = MakeRoadList(+1, list_h1t, list_h2t, list_h3t, list_h4t, raw);
    vector<int> list_road_bot = MakeRoadList(-1, list_h1b, list_h2b, list_h3b, list_h4b, raw);

    auto fired_pos_top = UtilTrigger::FindEnabledRoads(list_road_top, list_road_pos_top);
    auto fired_pos_bot = UtilTrigger::FindEnabledRoads(list_road_bot, list_road_pos_bot);
    auto fired_neg_top = UtilTrigger::FindEnabledRoads(list_road_top, list_road_neg_top);
    auto fired_neg_bot = UtilTrigger::FindEnabledRoads(list_road_bot, list_road_neg_bot);
    bool fired_fpga1 = (fired_pos_top.size() > 0 && fired_neg_bot.size() > 0) || (fired_pos_bot.size() > 0 && fired_neg_top.size() > 0);
    if (fired_fpga1) {
      h1_cnt->Fill(3);
      h2_mass_xF_fpga->Fill(mass_t, xF_t, weight);
      h2_zvtx        ->Fill(zvtx_t, 2, weight);
      h2_rec_st      ->Fill(rec_st, 2, weight);
      h3_ntrk        ->Fill(n_trk_pos, +1, 2, weight);
      h3_ntrk        ->Fill(n_trk_neg, -1, 2, weight);
      h3_px_vtx      ->Fill(p_vtx_pos->X(), +1, 2, weight);
      h3_py_vtx      ->Fill(p_vtx_pos->Y(), +1, 2, weight);
      h3_pz_vtx      ->Fill(p_vtx_pos->Z(), +1, 2, weight);
      h3_px_vtx      ->Fill(p_vtx_neg->X(), -1, 2, weight);
      h3_py_vtx      ->Fill(p_vtx_neg->Y(), -1, 2, weight);
      h3_pz_vtx      ->Fill(p_vtx_neg->Z(), -1, 2, weight);
      h3_pxy_st1_pos->Fill(p_st1_pos->X(), p_st1_pos->Y(), 2, weight);
      h3_pxy_st1_neg->Fill(p_st1_neg->X(), p_st1_neg->Y(), 2, weight);
      h3_txy_st1_pos->Fill(p_st1_pos->X()/p_st1_pos->Z(), p_st1_pos->Y()/p_st1_pos->Z(), 2, weight);
      h3_txy_st1_neg->Fill(p_st1_neg->X()/p_st1_neg->Z(), p_st1_neg->Y()/p_st1_neg->Z(), 2, weight);
      h3_xy_st1_pos ->Fill(v_st1_pos->X(), v_st1_pos->Y(), 2, weight);
      h3_xy_st1_neg ->Fill(v_st1_neg->X(), v_st1_neg->Y(), 2, weight);
      h3_pxy_st3_pos->Fill(p_st3_pos->X(), p_st3_pos->Y(), 2, weight);
      h3_pxy_st3_neg->Fill(p_st3_neg->X(), p_st3_neg->Y(), 2, weight);
      h3_txy_st3_pos->Fill(p_st3_pos->X()/p_st3_pos->Z(), p_st3_pos->Y()/p_st3_pos->Z(), 2, weight);
      h3_txy_st3_neg->Fill(p_st3_neg->X()/p_st3_neg->Z(), p_st3_neg->Y()/p_st3_neg->Z(), 2, weight);
      h3_xy_st3_pos ->Fill(v_st3_pos->X(), v_st3_pos->Y(), 2, weight);
      h3_xy_st3_neg ->Fill(v_st3_neg->X(), v_st3_neg->Y(), 2, weight);
      //h3_nhit_cham->Fill(n_d1, 1, 2, weight);
      //h3_nhit_cham->Fill(n_d2, 2, 2, weight);
      //h3_nhit_cham->Fill(n_d3, 3, 2, weight);
      //h3_nhit_cham->Fill(n_p4, 4, 2, weight);
      //if (rec->getNDimuons() == 0) {
      //  cout << "XX " << i_ent_evt << " " << run_id << " " << event_id << " : "
      //       << v_st1_pos->X() << " " << v_st1_pos->Y() << " : "
      //       << v_st3_pos->X() << " " << v_st3_pos->Y() << " :: "
      //       << v_st1_neg->X() << " " << v_st1_neg->Y() << " : "
      //       << v_st3_neg->X() << " " << v_st3_neg->Y()
      //       << endl;
      //  double dxdz3 = p_st3_pos->X() / p_st3_pos->Z();
      //  double x03   = v_st3_pos->X() - dxdz3 * v_st3_pos->Z();
      //  double dydz3 = p_st3_pos->Y() / p_st3_pos->Z();
      //  double y03   = v_st3_pos->Y() - dydz3 * v_st3_pos->Z();
      //  cout << "  pos X-Z: " << dxdz3 << " * z + " << x03 << endl
      //       << "  pos Y-Z: " << dydz3 << " * z + " << y03 << endl;
      //  dxdz3 = p_st3_neg->X() / p_st3_neg->Z();
      //  x03   = v_st3_neg->X() - dxdz3 * v_st3_neg->Z();
      //  dydz3 = p_st3_neg->Y() / p_st3_neg->Z();
      //  y03   = v_st3_neg->Y() - dydz3 * v_st3_neg->Z();
      //  cout << "  neg X-Z: " << dxdz3 << " * z + " << x03 << endl
      //       << "  neg Y-Z: " << dydz3 << " * z + " << y03 << endl;
      //  for (int ii = 0; ii < rec->getNTracks(); ii++) {
      //    SRecTrack trk = rec->getTrack(ii);
      //    TVector3 v1 = trk.getPositionVecSt1();
      //    //TVector3 p1 = trk.getMomentumVecSt1();
      //    TVector3 v3 = trk.getPositionVecSt3();
      //    //TVector3 p3 = trk.getMomentumVecSt3();
      //    cout << "  " << trk.getCharge() << " " << v1.X() << " " << v1.Y() << " : " << v3.X() << " " << v3.Y() << endl;
      //  }
      //}
    }
    
    ///
    /// Check FPGA1 trigger condition using hodoscope hits.
    ///
    int i_dim_best = -1;
    double mass_diff_best = 1e10;
    for (int i_dim = 0; i_dim < rec->getNDimuons(); i_dim++) {
      h1_cnt->Fill(4);
      SRecDimuon dim = rec->getDimuon(i_dim);
      double mass_diff = fabs(dim.mass - mass_t);
      if (mass_diff < mass_diff_best) {
        mass_diff_best = mass_diff;
        i_dim_best = i_dim;
      }
    }
    if (i_dim_best >= 0) {
      SRecDimuon dim = rec->getDimuon(i_dim_best);
      //double mass_r  = dim.mass;
      //double xF_r    = dim.xF;
      h2_mass_xF_rec ->Fill(mass_t, xF_t, weight);
      h2_zvtx        ->Fill(zvtx_t, 3, weight);
      h2_rec_st      ->Fill(rec_st, 3, weight);
      h3_ntrk        ->Fill(n_trk_pos, +1, 3, weight);
      h3_ntrk        ->Fill(n_trk_neg, -1, 3, weight);
      h3_px_vtx      ->Fill(p_vtx_pos->X(), +1, 3, weight);
      h3_py_vtx      ->Fill(p_vtx_pos->Y(), +1, 3, weight);
      h3_pz_vtx      ->Fill(p_vtx_pos->Z(), +1, 3, weight);
      h3_px_vtx      ->Fill(p_vtx_neg->X(), -1, 3, weight);
      h3_py_vtx      ->Fill(p_vtx_neg->Y(), -1, 3, weight);
      h3_pz_vtx      ->Fill(p_vtx_neg->Z(), -1, 3, weight);
      h3_pxy_st1_pos->Fill(p_st1_pos->X(), p_st1_pos->Y(), 3, weight);
      h3_pxy_st1_neg->Fill(p_st1_neg->X(), p_st1_neg->Y(), 3, weight);
      h3_txy_st1_pos->Fill(p_st1_pos->X()/p_st1_pos->Z(), p_st1_pos->Y()/p_st1_pos->Z(), 3, weight);
      h3_txy_st1_neg->Fill(p_st1_neg->X()/p_st1_neg->Z(), p_st1_neg->Y()/p_st1_neg->Z(), 3, weight);
      h3_xy_st1_pos ->Fill(v_st1_pos->X(), v_st1_pos->Y(), 3, weight);
      h3_xy_st1_neg ->Fill(v_st1_neg->X(), v_st1_neg->Y(), 3, weight);
      h3_pxy_st3_pos->Fill(p_st3_pos->X(), p_st3_pos->Y(), 3, weight);
      h3_pxy_st3_neg->Fill(p_st3_neg->X(), p_st3_neg->Y(), 3, weight);
      h3_txy_st3_pos->Fill(p_st3_pos->X()/p_st3_pos->Z(), p_st3_pos->Y()/p_st3_pos->Z(), 3, weight);
      h3_txy_st3_neg->Fill(p_st3_neg->X()/p_st3_neg->Z(), p_st3_neg->Y()/p_st3_neg->Z(), 3, weight);
      h3_xy_st3_pos ->Fill(v_st3_pos->X(), v_st3_pos->Y(), 3, weight);
      h3_xy_st3_neg ->Fill(v_st3_neg->X(), v_st3_neg->Y(), 3, weight);
      //h3_nhit_cham->Fill(n_d1, 1, 3, weight);
      //h3_nhit_cham->Fill(n_d2, 2, 3, weight);
      //h3_nhit_cham->Fill(n_d3, 3, 3, weight);
      //h3_nhit_cham->Fill(n_p4, 4, 3, weight);
      
      SRecTrack trk_pos = rec->getTrack(dim.trackID_pos);
      SRecTrack trk_neg = rec->getTrack(dim.trackID_neg);
      vector<int> list_road_trk_pos = UtilTrack::FindMatchedRoads(&trk_pos);
      vector<int> list_road_trk_neg = UtilTrack::FindMatchedRoads(&trk_neg);
      //cout << "list_road_trk_pos: ";
      //for (auto it = list_road_trk_pos.begin(); it != list_road_trk_pos.end(); it++) cout << " " << *it;
      //cout << endl;
      //cout << "list_road_trk_neg: ";
      //for (auto it = list_road_trk_neg.begin(); it != list_road_trk_neg.end(); it++) cout << " " << *it;
      //cout << endl;
      auto matched_pos_top = UtilTrigger::FindEnabledRoads(list_road_trk_pos, list_road_pos_top);
      auto matched_pos_bot = UtilTrigger::FindEnabledRoads(list_road_trk_pos, list_road_pos_bot);
      auto matched_neg_top = UtilTrigger::FindEnabledRoads(list_road_trk_neg, list_road_neg_top);
      auto matched_neg_bot = UtilTrigger::FindEnabledRoads(list_road_trk_neg, list_road_neg_bot);
      //cout << "N " << matched_pos_top.size() << " " << matched_neg_bot.size() << " | " << matched_pos_bot.size() << " " << matched_neg_top.size() << endl;
      if ( (matched_pos_top.size() > 0 && matched_neg_bot.size() > 0) ||
           (matched_pos_bot.size() > 0 && matched_neg_top.size() > 0)   ) {
        h1_cnt->Fill(5);
        h2_mass_xF_rec_mat->Fill(mass_t, xF_t, weight);
        h2_zvtx       ->Fill(zvtx_t, 4, weight);
        h2_rec_st     ->Fill(rec_st, 4, weight);
        h3_ntrk       ->Fill(n_trk_pos, +1, 4, weight);
        h3_ntrk       ->Fill(n_trk_neg, -1, 4, weight);
        h3_px_vtx     ->Fill(p_vtx_pos->X(), +1, 4, weight);
        h3_py_vtx     ->Fill(p_vtx_pos->Y(), +1, 4, weight);
        h3_pz_vtx     ->Fill(p_vtx_pos->Z(), +1, 4, weight);
        h3_px_vtx     ->Fill(p_vtx_neg->X(), -1, 4, weight);
        h3_py_vtx     ->Fill(p_vtx_neg->Y(), -1, 4, weight);
        h3_pz_vtx     ->Fill(p_vtx_neg->Z(), -1, 4, weight);
        h3_pxy_st1_pos->Fill(p_st1_pos->X(), p_st1_pos->Y(), 4, weight);
        h3_pxy_st1_neg->Fill(p_st1_neg->X(), p_st1_neg->Y(), 4, weight);
        h3_txy_st1_pos->Fill(p_st1_pos->X()/p_st1_pos->Z(), p_st1_pos->Y()/p_st1_pos->Z(), 4, weight);
        h3_txy_st1_neg->Fill(p_st1_neg->X()/p_st1_neg->Z(), p_st1_neg->Y()/p_st1_neg->Z(), 4, weight);
        h3_xy_st1_pos ->Fill(v_st1_pos->X(), v_st1_pos->Y(), 4, weight);
        h3_xy_st1_neg ->Fill(v_st1_neg->X(), v_st1_neg->Y(), 4, weight);
        h3_pxy_st3_pos->Fill(p_st3_pos->X(), p_st3_pos->Y(), 4, weight);
        h3_pxy_st3_neg->Fill(p_st3_neg->X(), p_st3_neg->Y(), 4, weight);
        h3_txy_st3_pos->Fill(p_st3_pos->X()/p_st3_pos->Z(), p_st3_pos->Y()/p_st3_pos->Z(), 4, weight);
        h3_txy_st3_neg->Fill(p_st3_neg->X()/p_st3_neg->Z(), p_st3_neg->Y()/p_st3_neg->Z(), 4, weight);
        h3_xy_st3_pos ->Fill(v_st3_pos->X(), v_st3_pos->Y(), 4, weight);
        h3_xy_st3_neg ->Fill(v_st3_neg->X(), v_st3_neg->Y(), 4, weight);
        //h3_nhit_cham->Fill(n_d1, 1, 4, weight);
        //h3_nhit_cham->Fill(n_d2, 2, 4, weight);
        //h3_nhit_cham->Fill(n_d3, 3, 4, weight);
        //h3_nhit_cham->Fill(n_p4, 4, 4, weight);
      }
    //} else {
    //  if (fired_fpga1) {
    //    bool ok_bot = false;
    //    bool ok_top = false;
    //    ostringstream oss;
    //    oss << "H1B: ";
    //    for (auto it = list_h1b.begin(); it != list_h1b.end(); it++) {
    //      int eid = raw->getHit(*it).elementID;
    //      if (5 <= eid && eid <= 19) ok_bot = true;
    //      oss << " " << eid;
    //    }
    //    oss << "\nH1T: ";
    //    for (auto it = list_h1t.begin(); it != list_h1t.end(); it++) {
    //      int eid = raw->getHit(*it).elementID;
    //      if (5 <= eid && eid <= 19) ok_top = true;
    //      oss << " " << eid;
    //    }
    //    oss << endl;
    //    if (!ok_bot || !ok_top) cout << oss.str();
    //  }
    }

    i_ent_vtx++;
  }
  cout << endl;
  if (n_evt_missed > 0) {
    cout << "!!WARNING!!  n_evt_missed = " << n_evt_missed << endl;
  }
  
  ofstream ofs("result/both/result.txt");
  ofs << "N of trees   = " << n_tree_evt << "\n"
      << "N of all events            = " << n_ent_evt << "\n"
      << "N of matched events        = " << h1_cnt->GetBinContent(1) << "\n"
      << "N of T+B/B+T events        = " << h1_cnt->GetBinContent(2) << "\n"
      << "N of FPGA1 events          = " << h1_cnt->GetBinContent(3) << "\n"
      << "N of reco. events          = " << h1_cnt->GetBinContent(4) << "\n"
      << "N of reco. matched events  = " << h1_cnt->GetBinContent(5) << "\n"
      << "N of missed events         = " << n_evt_missed << "\n";
  ofs.close();

  gErrorIgnoreLevel = 1111;
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();

  int opt_stat = gStyle->GetOptStat();
  gStyle->SetOptStat(0);
  c1->SetLogz(true);
  h2_mass_xF_tbbt   ->Draw("colz");  c1->SaveAs("result/both/h2_mass_xF_01_tbbt.pdf");
  h2_mass_xF_fpga   ->Draw("colz");  c1->SaveAs("result/both/h2_mass_xF_02_fpga.pdf");
  h2_mass_xF_rec    ->Draw("colz");  c1->SaveAs("result/both/h2_mass_xF_03_rec.pdf");
  h2_mass_xF_rec_mat->Draw("colz");  c1->SaveAs("result/both/h2_mass_xF_04_rec_mat.pdf");
  c1->SetLogz(false);

  TH1* h1_zvtx[5];
  for (int type = 1; type <= 4; type++) {
    h1_zvtx[type] = h2_zvtx->ProjectionX(Form("h1_zvtx_%i", type), type, type);
    h1_zvtx[type]->SetLineColor(type);
    h1_zvtx[type]->Draw( type==1 ? "HIST" : "HISTsame" );    
  }
  c1->SaveAs("result/both/h1_zvtx.pdf");

  TH1* h1_rec_st[5];
  for (int type = 1; type <= 4; type++) {
    h1_rec_st[type] = h2_rec_st->ProjectionX(Form("h1_rec_st_%i", type), type, type);
    h1_rec_st[type]->SetLineWidth(5-type);
    h1_rec_st[type]->SetLineColor(type);
    h1_rec_st[type]->Draw( type==1 ? "HIST" : "HISTsame" );    
  }
  c1->SaveAs("result/both/h1_rec_st.pdf");

  TH1* h1_ntrk[5]; // [type]
  h3_ntrk->GetYaxis()->SetRange(3, 3); // pos
  for (int type = 1; type <= 4; type++) {
    h3_ntrk->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_ntrk->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_ntrk_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_ntrk_pos.pdf");
  h3_ntrk->GetYaxis()->SetRange(1, 1); // neg
  for (int type = 1; type <= 4; type++) {
    h3_ntrk->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_ntrk->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_ntrk_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_ntrk_neg.pdf");

  TH1* h1_px_vtx[5];
  h3_px_vtx->GetYaxis()->SetRange(3, 3); // pos
  for (int type = 1; type <= 4; type++) {
    h3_px_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_px_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_px_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_px_vtx_pos.pdf");
  h3_px_vtx->GetYaxis()->SetRange(1, 1); // neg
  for (int type = 1; type <= 4; type++) {
    h3_px_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_px_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_px_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_px_vtx_neg.pdf");

  TH1* h1_py_vtx[5];
  h3_py_vtx->GetYaxis()->SetRange(3, 3); // pos
  for (int type = 1; type <= 4; type++) {
    h3_py_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_py_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_py_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_py_vtx_pos.pdf");
  h3_py_vtx->GetYaxis()->SetRange(1, 1); // neg
  for (int type = 1; type <= 4; type++) {
    h3_py_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_py_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_py_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_py_vtx_neg.pdf");

  TH1* h1_pz_vtx[5];
  h3_pz_vtx->GetYaxis()->SetRange(3, 3); // pos
  for (int type = 1; type <= 4; type++) {
    h3_pz_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_pz_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_pz_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_pz_vtx_pos.pdf");
  h3_pz_vtx->GetYaxis()->SetRange(1, 1); // neg
  for (int type = 1; type <= 4; type++) {
    h3_pz_vtx->GetZaxis()->SetRange(type, type);
    h1_ntrk[type] = h3_pz_vtx->Project3D("x");
    h1_ntrk[type]->SetName( Form("h1_pz_vtx_%i", type) );
    h1_ntrk[type]->SetLineColor(type);
    h1_ntrk[type]->SetTitle("");    
    h1_ntrk[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  }
  c1->SaveAs("result/both/h1_pz_vtx_neg.pdf");

  ///
  /// 2D acceptance plots
  ///
  TH2* h2_acc_fpga = (TH2*)h2_mass_xF_fpga->Clone("h2_acc_fpga");
  h2_acc_fpga->Divide(h2_mass_xF_tbbt);
  h2_acc_fpga->SetTitle("Trigger acceptance");
  h2_acc_fpga->Draw("colz");
  h2_acc_fpga->GetZaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/both/h2_acc_02_fpga.pdf");

  TH2* h2_acc_rec = (TH2*)h2_mass_xF_rec->Clone("h2_acc_rec");
  h2_acc_rec->Divide(h2_mass_xF_tbbt);
  h2_acc_rec->SetTitle("Trigger acceptance");
  h2_acc_rec->Draw("colz");
  h2_acc_rec->GetZaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/both/h2_acc_03_rec.pdf");

  TH2* h2_acc_rec_mat = (TH2*)h2_mass_xF_rec_mat->Clone("h2_acc_rec_mat");
  h2_acc_rec_mat->Divide(h2_mass_xF_tbbt);
  h2_acc_rec_mat->SetTitle("Trigger acceptance");
  h2_acc_rec_mat->Draw("colz");
  h2_acc_rec_mat->GetZaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/both/h2_acc_04_rec_mat.pdf");

  ///
  /// 1D acceptance plots
  ///
  const double mass_lo =  4.0;
  const double mass_hi = 10.0;
  const double   xF_lo =  0.0;
  const double   xF_hi =  1.0;
  int bin_mass_lo = h2_mass_xF_tbbt->GetXaxis()->FindBin(mass_lo);
  int bin_mass_hi = h2_mass_xF_tbbt->GetXaxis()->FindBin(mass_hi);
  int bin_xF_lo   = h2_mass_xF_tbbt->GetYaxis()->FindBin(  xF_lo);
  int bin_xF_hi   = h2_mass_xF_tbbt->GetYaxis()->FindBin(  xF_hi);
  
  TH1* h1_mass_tbbt        = h2_mass_xF_tbbt   ->ProjectionX("h1_mass_tbbt"       , bin_xF_lo, bin_xF_hi);
  TH1* h1_acc_fpga_mass    = h2_mass_xF_fpga   ->ProjectionX("h1_acc_fpga_mass"   , bin_xF_lo, bin_xF_hi);
  TH1* h1_acc_rec_mass     = h2_mass_xF_rec    ->ProjectionX("h1_acc_rec_mass"    , bin_xF_lo, bin_xF_hi);
  TH1* h1_acc_rec_mat_mass = h2_mass_xF_rec_mat->ProjectionX("h1_acc_rec_mat_mass", bin_xF_lo, bin_xF_hi);
  h1_acc_fpga_mass   ->Divide(h1_mass_tbbt);
  h1_acc_rec_mass    ->Divide(h1_mass_tbbt);
  h1_acc_rec_mat_mass->Divide(h1_mass_tbbt);
  delete h1_mass_tbbt;
  h1_acc_fpga_mass   ->SetLineColor(kBlack);  
  h1_acc_rec_mass    ->SetLineColor(kRed);
  h1_acc_rec_mat_mass->SetLineColor(kBlue);
  h1_acc_fpga_mass->SetTitle( Form("%.1f < xF < %.1f;Mass;Trigger acceptance", xF_lo, xF_hi) );
  h1_acc_fpga_mass->Draw("HIST");
  h1_acc_fpga_mass->GetYaxis()->SetRangeUser(0, 1);
  h1_acc_rec_mass    ->Draw("HISTsame");
  h1_acc_rec_mat_mass->Draw("HISTsame");
  c1->SaveAs("result/both/h1_acc_mass.pdf");

  TH1* h1_xF_tbbt        = h2_mass_xF_tbbt   ->ProjectionY("h1_xF_tbbt"       , bin_mass_lo, bin_mass_hi);
  TH1* h1_acc_fpga_xF    = h2_mass_xF_fpga   ->ProjectionY("h1_acc_fpga_xF"   , bin_mass_lo, bin_mass_hi);
  TH1* h1_acc_rec_xF     = h2_mass_xF_rec    ->ProjectionY("h1_acc_rec_xF"    , bin_mass_lo, bin_mass_hi);
  TH1* h1_acc_rec_mat_xF = h2_mass_xF_rec_mat->ProjectionY("h1_acc_rec_mat_xF", bin_mass_lo, bin_mass_hi);
  h1_acc_fpga_xF   ->Divide(h1_xF_tbbt);
  h1_acc_rec_xF    ->Divide(h1_xF_tbbt);
  h1_acc_rec_mat_xF->Divide(h1_xF_tbbt);
  delete h1_xF_tbbt;
  h1_acc_fpga_xF   ->SetLineColor(kBlack);  
  h1_acc_rec_xF    ->SetLineColor(kRed);
  h1_acc_rec_mat_xF->SetLineColor(kBlue);
  h1_acc_fpga_xF->SetTitle( Form("%.1f < Mass < %.1f;xF;Trigger acceptance", mass_lo, mass_hi) );
  h1_acc_fpga_xF->Draw("HIST");
  h1_acc_fpga_xF->GetYaxis()->SetRangeUser(0, 1);  
  h1_acc_rec_xF    ->Draw("HISTsame");
  h1_acc_rec_mat_xF->Draw("HISTsame");
  c1->SaveAs("result/both/h1_acc_xF.pdf");

  ///
  /// Track dist.
  ///
  DrawOneTrackDist(c1, h3_pxy_st1_pos);
  DrawOneTrackDist(c1, h3_pxy_st1_neg);
  DrawOneTrackDist(c1, h3_txy_st1_pos);
  DrawOneTrackDist(c1, h3_txy_st1_neg);
  DrawOneTrackDist(c1, h3_xy_st1_pos);
  DrawOneTrackDist(c1, h3_xy_st1_neg);
  DrawOneTrackDist(c1, h3_pxy_st3_pos);
  DrawOneTrackDist(c1, h3_pxy_st3_neg);
  DrawOneTrackDist(c1, h3_txy_st3_pos);
  DrawOneTrackDist(c1, h3_txy_st3_neg);
  DrawOneTrackDist(c1, h3_xy_st3_pos);
  DrawOneTrackDist(c1, h3_xy_st3_neg);

  ///
  /// Nhit dist.
  ///
  //for (int st = 1; st <= 4; st++) {
  //  h3_nhit_cham->GetYaxis()->SetRange(st, st);
  //  TH1* h1[5];
  //  for (int type = 1; type <= 4; type++) {
  //    h3_nhit_cham->GetZaxis()->SetRange(type, type);
  //    h1[type] = h3_nhit_cham->Project3D("x");
  //    h1[type]->SetName( Form("h1_type%i", type) );
  //    h1[type]->SetTitle("");
  //    h1[type]->SetLineColor(type);
  //    h1[type]->Draw( type==1 ? "HIST" : "HISTsame" );
  //  }
  //  c1->SaveAs( Form("result/both/h1_nhit_st%i.pdf", st) );
  //  for (int type = 1; type <= 4; type++) delete h1[type];
  //}
  
  f_out->Write();
  f_out->Close();

  exit(0);
}

void DrawOneTrackDist(TCanvas* c1, TH3* h3)
{
  string name = h3->GetName();
  name = name.substr(3, string::npos); // Remove 'h3_'
  
  h3->GetZaxis()->SetRange(1, 1);
  TH1* h2_01 = h3->Project3D("yx");
  h2_01->SetName("h2_01");
  h2_01->SetTitle("");
  h2_01->Draw("colz");
  c1->SaveAs( ("result/both/h2_"+name+"_01_tbbt.pdf").c_str() );

  h3->GetZaxis()->SetRange(2, 2);
  TH1* h2_02 = h3->Project3D("yx");
  h2_02->SetName("h2_02");
  h2_02->SetTitle("");
  h2_02->Draw("colz");
  c1->SaveAs( ("result/both/h2_"+name+"_02_fpga.pdf").c_str() );

  h3->GetZaxis()->SetRange(3, 3);
  TH1* h2_03 = h3->Project3D("yx");
  h2_03->SetName("h2_03");
  h2_03->SetTitle("");
  h2_03->Draw("colz");
  c1->SaveAs( ("result/both/h2_"+name+"_03_rec.pdf").c_str() );

  //h3->GetZaxis()->SetRange(4, 4);
  //TH1* h2_04 = h3->Project3D("yx");
  //h2_04->SetName("h2_04");
  //h2_04->SetTitle("");
  //h2_04->Draw("colz");
  //c1->SaveAs( ("result/both/h2_"+name+"_04_rec_mat.pdf").c_str() );

  ((TH2*)h2_02)->Rebin2D(2, 2);
  ((TH2*)h2_03)->Rebin2D(2, 2);
  h2_03->Divide(h2_02);
  h2_03->GetZaxis()->SetRangeUser(0, 1);
  h2_03->Draw("colz");
  c1->SaveAs( ("result/both/h2_"+name+"_03to02.pdf").c_str() );
  
  //delete h2_01;
  delete h2_02;
  delete h2_03;
  //delete h2_04;
}

