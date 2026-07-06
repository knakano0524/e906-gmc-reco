#include "inc/UtilTrigger.h"
#include "inc/Common.h"
R__LOAD_LIBRARY(kTracker)
using namespace std;

void ana_event(const char* fn_list="auto_file/list_ana_event.txt")
{
  int rs_id = atoi(gSystem->Getenv("ROADSET_ID"));
  cout << "Roadset " << rs_id << endl;
  auto list_road_pos_top = UtilTrigger::ReadRoadList(rs_id, +1, +1);
  auto list_road_pos_bot = UtilTrigger::ReadRoadList(rs_id, +1, -1);
  auto list_road_neg_top = UtilTrigger::ReadRoadList(rs_id, -1, +1);
  auto list_road_neg_bot = UtilTrigger::ReadRoadList(rs_id, -1, -1);
  cout << "  N of roads = "
       << list_road_pos_top.size() << " pos top, "
       << list_road_pos_bot.size() << " pos bot, "
       << list_road_neg_top.size() << " neg top, "
       << list_road_neg_bot.size() << " neg bot\n";
  
  TChain* tree = new TChain("save");
  
  ifstream ifs(fn_list);
  string fn_in;
  while (ifs >> fn_in) tree->Add(fn_in.c_str());
  ifs.close();

  gSystem->mkdir("result/event", true);
  TFile* f_out = new TFile("result/event/result.root", "RECREATE");
  TH1* h1_cnt = new TH1D("h1_cnt", "", 10, 0.5, 10.5);
  TH2* h2_mass_xF_all  = new TH2D("h2_mass_xF_all" ,     "All;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_xF_tbbt = new TH2D("h2_mass_xF_tbbt", "T+B/B+T;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_xF_fpga = new TH2D("h2_mass_xF_fpga",   "FPGA1;Mass;xF", 50, 0, 10, 30, -0.2, 1.0);
  TH2* h2_mass_x2_all  = new TH2D("h2_mass_x2_all" ,     "All;Mass;x2", 50, 0, 10, 35, 0, 0.7);
  TH2* h2_mass_x2_tbbt = new TH2D("h2_mass_x2_tbbt", "T+B/B+T;Mass;x2", 50, 0, 10, 35, 0, 0.7);
  TH2* h2_mass_x2_fpga = new TH2D("h2_mass_x2_fpga",   "FPGA1;Mass;x2", 50, 0, 10, 35, 0, 0.7);
  TH3* h3_nhit_cham    = new TH3D("h3_nhit_cham", ";N of hits;D1,D2,D3,P4;In-time?", 50, -0.5, 49.5,  4, 0.5, 4.5, 2, -0.5, 1.5);
  
  //tree->Print();
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "N of trees = " << n_tree << endl
       << "N of entries = " << n_ent << endl;

  SRawMCEvent* raw = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/ 10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    h1_cnt->Fill(1);

    double mass   = raw->mass;
    double xF     = raw->xF;
    double x2     = raw->x2;
    double weight = raw->weight;
    h2_mass_xF_all->Fill(mass, xF, weight);
    h2_mass_x2_all->Fill(mass, x2, weight);
        
    //int nhit_cham = raw->getNChamberHitsAll();
    //int nhit_hodo = raw->getNHodoHitsAll();
    //int nhit_prop = raw->getNPropHitsAll();
    //cout << i_ent << ":";
    //cout << "  cham " << raw->getNHitsInD0() << "," << raw->getNHitsInD1() << "," << raw->getNHitsInD2() << "," << raw->getNHitsInD3p() << "," << raw->getNHitsInD3m()
    //     << "  hodo " << raw->getNHitsInH1() << "," << raw->getNHitsInH2() << "," << raw->getNHitsInH3() << "," << raw->getNHitsInH4()
    //     << "  prop " << raw->getNHitsInP1() << "," << raw->getNHitsInP2() << "\n";
    
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
      continue;
    }
    h1_cnt->Fill(2);
    h2_mass_xF_tbbt->Fill(mass, xF, weight);
    h2_mass_x2_tbbt->Fill(mass, x2, weight);

    vector<int> list_road_top = MakeRoadList(+1, list_h1t, list_h2t, list_h3t, list_h4t, raw);
    vector<int> list_road_bot = MakeRoadList(-1, list_h1b, list_h2b, list_h3b, list_h4b, raw);

    auto fired_pos_top = UtilTrigger::FindEnabledRoads(list_road_top, list_road_pos_top);
    auto fired_pos_bot = UtilTrigger::FindEnabledRoads(list_road_bot, list_road_pos_bot);
    auto fired_neg_top = UtilTrigger::FindEnabledRoads(list_road_top, list_road_neg_top);
    auto fired_neg_bot = UtilTrigger::FindEnabledRoads(list_road_bot, list_road_neg_bot);
    if ( (fired_pos_top.size() > 0 && fired_neg_bot.size() > 0) ||
         (fired_pos_bot.size() > 0 && fired_neg_top.size() > 0)   ) {
      h1_cnt->Fill(3);
      h2_mass_xF_fpga->Fill(mass, xF, weight);
      h2_mass_x2_fpga->Fill(mass, x2, weight);

      int nhit[5][2]; // [D1,D2,D3,P4][inTime]
      memset(nhit, 0, sizeof(nhit));
      for (int i_hit = 0; i_hit < raw->getNHitsAll(); i_hit++) {
        Hit hit = raw->getHit(i_hit);
        int in_time = (hit.isInTime() ? 1 : 0);
        short det_id = hit.detectorID;
        if      ( 6 < det_id && det_id <= 12) nhit[1][in_time]++;
        else if (12 < det_id && det_id <= 18) nhit[2][in_time]++;
        else if (18 < det_id && det_id <= 30) nhit[3][in_time]++;
        else if (46 < det_id && det_id <= 54) nhit[4][in_time]++;
      }
      for (int st = 1; st <= 4; st++) {
        for (int it = 0; it < 2; it++) {
          h3_nhit_cham->Fill(nhit[st][it], st, it);
        }
      }
    }
  }
  cout << endl;
    
  ofstream ofs("result/event/result.txt");
  ofs << "N of trees   = " << n_tree << "\n"
      << "N of all events     = " << h1_cnt->GetBinContent(1) << "\n"
      << "N of T+B/B+T events = " << h1_cnt->GetBinContent(2) << "\n"
      << "N of FPGA1  events  = " << h1_cnt->GetBinContent(3) << "\n";
  ofs.close();

  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();

  int opt_stat = gStyle->GetOptStat();
  gStyle->SetOptStat(0);
  c1->SetLogz(true);
  h2_mass_xF_all ->Draw("colz");  c1->SaveAs("result/event/h2_mass_xF_all.pdf");
  h2_mass_xF_tbbt->Draw("colz");  c1->SaveAs("result/event/h2_mass_xF_tbbt.pdf");
  h2_mass_xF_fpga->Draw("colz");  c1->SaveAs("result/event/h2_mass_xF_fpga.pdf");
  h2_mass_x2_all ->Draw("colz");  c1->SaveAs("result/event/h2_mass_x2_all.pdf");
  h2_mass_x2_tbbt->Draw("colz");  c1->SaveAs("result/event/h2_mass_x2_tbbt.pdf");
  h2_mass_x2_fpga->Draw("colz");  c1->SaveAs("result/event/h2_mass_x2_fpga.pdf");
  c1->SetLogz(false);

  TH2* h2_acc = (TH2*)h2_mass_xF_fpga->Clone("h2_acc");
  h2_acc->Divide(h2_mass_xF_tbbt);
  h2_acc->SetTitle("Trigger acceptance");
  h2_acc->Draw("colz");
  h2_acc->GetZaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/event/h2_acc.pdf");

  TH2* h2_acc_x2 = (TH2*)h2_mass_x2_fpga->Clone("h2_acc_x2");
  h2_acc_x2->Divide(h2_mass_x2_tbbt);
  h2_acc_x2->SetTitle("Trigger acceptance");
  h2_acc_x2->Draw("colz");
  h2_acc_x2->GetZaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/event/h2_acc_x2.pdf");
  
  const double mass_lo =  4.0;
  const double mass_hi = 10.0;
  const double   xF_lo =  0.0;
  const double   xF_hi =  1.0;
  int bin_mass_lo = h2_mass_xF_tbbt->GetXaxis()->FindBin(mass_lo);
  int bin_mass_hi = h2_mass_xF_tbbt->GetXaxis()->FindBin(mass_hi);
  int bin_xF_lo   = h2_mass_xF_tbbt->GetYaxis()->FindBin(  xF_lo);
  int bin_xF_hi   = h2_mass_xF_tbbt->GetYaxis()->FindBin(  xF_hi);
  
  TH1* h1_mass_tbbt = h2_mass_xF_tbbt->ProjectionX("h1_mass_tbbt", bin_xF_lo, bin_xF_hi);
  TH1* h1_acc_mass  = h2_mass_xF_fpga->ProjectionX("h1_acc_mass" , bin_xF_lo, bin_xF_hi);
  h1_acc_mass->Divide(h1_mass_tbbt);
  delete h1_mass_tbbt;
  h1_acc_mass->SetTitle( Form("%.1f < xF < %.1f;Mass;Trigger acceptance", xF_lo, xF_hi) );
  h1_acc_mass->Draw("HIST");
  h1_acc_mass->GetYaxis()->SetRangeUser(0, 1);
  c1->SaveAs("result/event/h1_acc_mass.pdf");

  TH1* h1_xF_tbbt = h2_mass_xF_tbbt->ProjectionY("h1_xF_tbbt", bin_mass_lo, bin_mass_hi);
  TH1* h1_acc_xF  = h2_mass_xF_fpga->ProjectionY("h1_acc_xF" , bin_mass_lo, bin_mass_hi);
  h1_acc_xF->Divide(h1_xF_tbbt);
  delete h1_xF_tbbt;
  h1_acc_xF->SetTitle( Form("%.1f < Mass < %.1f;xF;Trigger acceptance", mass_lo, mass_hi) );
  h1_acc_xF->Draw("HIST");
  h1_acc_xF->GetYaxis()->SetRangeUser(0, 1);  
  c1->SaveAs("result/event/h1_acc_xF.pdf");

  TH1* h1_x2_tbbt = h2_mass_x2_tbbt->ProjectionY("h1_x2_tbbt", bin_mass_lo, bin_mass_hi);
  TH1* h1_acc_x2  = h2_mass_x2_fpga->ProjectionY("h1_acc_x2" , bin_mass_lo, bin_mass_hi);
  h1_acc_x2->Divide(h1_x2_tbbt);
  delete h1_x2_tbbt;
  h1_acc_x2->SetTitle( Form("%.1f < Mass < %.1f;x2;Trigger acceptance", mass_lo, mass_hi) );
  h1_acc_x2->Draw("HIST");
  h1_acc_x2->GetYaxis()->SetRangeUser(0, 1);  
  c1->SaveAs("result/event/h1_acc_x2.pdf");

  ///
  /// N of chamber hits/station
  ///
  for (int st = 1; st <= 4; st++) {
    h3_nhit_cham->GetYaxis()->SetRange(st, st);
    TH1* h1[2];
    for (int it = 0; it < 2; it++) {
      h3_nhit_cham->GetZaxis()->SetRange(it+1, it+1);
      h1[it] = h3_nhit_cham->Project3D("x");
      h1[it]->SetName( Form("h1_nhit_%i", it) );
    }
    h1[1]->SetTitle( Form("Station %i;N of hits/station;", st) );
    h1[1]->SetLineColor(kRed);
    h1[1]->Draw("HIST");
    h1[0]->Draw("HISTsame");
    c1->SaveAs( Form("result/event/h1_nhit_st%i.pdf", st));
    for (int it = 0; it < 2; it++) delete h1[it];
  }
  
  f_out->Write();
  f_out->Close();
  
  exit(0);
}
