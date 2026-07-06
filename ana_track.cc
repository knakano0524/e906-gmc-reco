R__LOAD_LIBRARY(kTracker)

TCanvas* c1;

void DrawPosNegHist(TH2* h2)
{
  TH1* h1_neg = h2->ProjectionX("h1_neg", 1, 1);
  TH1* h1_pos = h2->ProjectionX("h1_pos", 3, 3);
  h1_neg->SetLineWidth(3);
  h1_pos->SetLineWidth(2);
  h1_neg->SetLineColor(kBlue);
  h1_pos->SetLineColor(kRed);
  THStack hs("hs", Form(";%s;", h2->GetXaxis()->GetTitle()));
  hs.Add(h1_neg);
  hs.Add(h1_pos);
  hs.Draw("nostack");

  string name = h2->GetName();
  if (name.substr(0, 3) == "h2_") name = "h1_" + name.substr(3, string::npos);
  c1->SaveAs(Form("result/track/%s.pdf", name.c_str()));
  delete h1_neg;
  delete h1_pos;
}

void ana_track(const char* fn_list="auto_file/list_ana_track.txt")
{
  TChain* tree = new TChain("save");
  
  ifstream ifs(fn_list);
  string fn_in;
  while (ifs >> fn_in) tree->Add(fn_in.c_str());
  ifs.close();

  //tree->Print();
  unsigned int n_tree = tree->GetNtrees();
  unsigned int n_ent  = tree->GetEntries();
  cout << "N of trees = " << n_tree << endl
       << "N of entries = " << n_ent << endl;

  TH1* h1_cnt = new TH1D("h1_cnt", "", 10, 0.5, 10.5);
  TH1* h1_ntrk = new TH1D("h1_ntrk", ";N of tracks/event;", 10, -0.5, 9.5);

  TH2* h2_nhit = new TH2D("h2_nhit", ";N of hits/track;", 10, 10.5, 20.5,  3, -1.5, 1.5);
  TH2* h2_chi2 = new TH2D("h2_chi2", ";chi2;", 100, 0, 50,  3, -1.5, 1.5);

  TH2* h2_x1_t  = new TH2D("h2_x1_t" , ";x_{true} at St.1;", 100, -100, 100,  3, -1.5, 1.5);
  TH2* h2_y1_t  = new TH2D("h2_y1_t" , ";y_{true} at St.1;", 100, -100, 100,  3, -1.5, 1.5);
  TH2* h2_z1_t  = new TH2D("h2_z1_t" , ";z_{true} at St.1;", 100,  550, 650,  3, -1.5, 1.5);
  TH2* h2_px1_t = new TH2D("h2_px1_t", ";px_{true} at St.1;", 100, -5, 5,  3, -1.5, 1.5);
  TH2* h2_py1_t = new TH2D("h2_py1_t", ";py_{true} at St.1;", 100, -5, 5,  3, -1.5, 1.5);
  TH2* h2_pz1_t = new TH2D("h2_pz1_t", ";pz_{true} at St.1;", 100, 10, 110,  3, -1.5, 1.5);
  
  TH2* h2_x1_r  = new TH2D("h2_x1_r" , ";x_{reco} at St.1;", 100, -100, 100,  3, -1.5, 1.5);
  TH2* h2_y1_r  = new TH2D("h2_y1_r" , ";y_{reco} at St.1;", 100, -100, 100,  3, -1.5, 1.5);
  TH2* h2_z1_r  = new TH2D("h2_z1_r" , ";z_{reco} at St.1;", 100,  550, 650,  3, -1.5, 1.5);
  TH2* h2_px1_r = new TH2D("h2_px1_r", ";px_{reco} at St.1;", 100, -5, 5,  3, -1.5, 1.5);
  TH2* h2_py1_r = new TH2D("h2_py1_r", ";py_{reco} at St.1;", 100, -5, 5,  3, -1.5, 1.5);
  TH2* h2_pz1_r = new TH2D("h2_pz1_r", ";pz_{reco} at St.1;", 100, 10, 110,  3, -1.5, 1.5);
  
  TH2* h2_x1_d  = new TH2D("h2_x1_d" , ";x_{reco} - x_{true} at St.1;", 100, -2.5, 2.5,  3, -1.5, 1.5);
  TH2* h2_y1_d  = new TH2D("h2_y1_d" , ";y_{reco} - y_{true} at St.1;", 100, -2.5, 2.5,  3, -1.5, 1.5);
  TH2* h2_z1_d  = new TH2D("h2_z1_d" , ";z_{reco} - z_{true} at St.1;", 100,  -10,  10,  3, -1.5, 1.5);
  TH2* h2_px1_d = new TH2D("h2_px1_d", ";px_{reco} - px_{true} at St.1;", 100, -0.25, 0.25,  3, -1.5, 1.5);
  TH2* h2_py1_d = new TH2D("h2_py1_d", ";py_{reco} - py_{true} at St.1;", 100, -0.25, 0.25,  3, -1.5, 1.5);
  TH2* h2_pz1_d = new TH2D("h2_pz1_d", ";pz_{reco} - pz_{true} at St.1;", 100, -5.00, 5.00,  3, -1.5, 1.5);
  
  SRawMCEvent* raw = 0;
  SRecEvent  * rec = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  tree->SetBranchAddress("recEvent", &rec);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/ 10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    h1_cnt->Fill(1);
    
    //int nhit_cham = raw->getNChamberHitsAll();
    //int nhit_hodo = raw->getNHodoHitsAll();
    //int nhit_prop = raw->getNPropHitsAll();
    //cout << "  cham " << raw->getNHitsInD0() << "," << raw->getNHitsInD1() << "," << raw->getNHitsInD2() << "," << raw->getNHitsInD3p() << "," << raw->getNHitsInD3m()
    //     << "  hodo " << raw->getNHitsInH1() << "," << raw->getNHitsInH2() << "," << raw->getNHitsInH3() << "," << raw->getNHitsInH4()
    //     << "  prop " << raw->getNHitsInP1() << "," << raw->getNHitsInP2() << "\n";

    TVector3* p_pos_st1 = &raw->p_station1[0];
    TVector3* p_neg_st1 = &raw->p_station1[1];
    TVector3* v_pos_st1 = &raw->v_station1[0];
    TVector3* v_neg_st1 = &raw->v_station1[1];
    
    int n_trk = rec->getNTracks();
    h1_ntrk->Fill(n_trk);
    for (int i_trk = 0; i_trk < n_trk; i_trk++) {
      h1_cnt->Fill(2);

      SRecTrack trk = rec->getTrack(i_trk);
      int    charge = trk.getCharge();
      int    nhit   = trk.getNHits();
      double chi2   = trk.getChisq();
      //cout << "  track " << i_trk << ": " << charge << " " << chi2 << " " << nhit << endl;
      h2_nhit->Fill(nhit, charge);
      h2_chi2->Fill(chi2, charge);

      TVector3 p_st1_r = trk.getMomentumVecSt1();
      TVector3 v_st1_r = trk.getPositionVecSt1();
      TVector3* p_st1_t = (charge > 0 ? p_pos_st1 : p_neg_st1);
      TVector3* v_st1_t = (charge > 0 ? v_pos_st1 : v_neg_st1);
      h2_x1_t ->Fill(v_st1_t->X(), charge);
      h2_y1_t ->Fill(v_st1_t->Y(), charge);
      h2_z1_t ->Fill(v_st1_t->Z(), charge);
      h2_px1_t->Fill(p_st1_t->X(), charge);
      h2_py1_t->Fill(p_st1_t->Y(), charge);
      h2_pz1_t->Fill(p_st1_t->Z(), charge);
      h2_x1_r ->Fill(v_st1_r.X(), charge);
      h2_y1_r ->Fill(v_st1_r.Y(), charge);
      h2_z1_r ->Fill(v_st1_r.Z(), charge);
      h2_px1_r->Fill(p_st1_r.X(), charge);
      h2_py1_r->Fill(p_st1_r.Y(), charge);
      h2_pz1_r->Fill(p_st1_r.Z(), charge);
      h2_x1_d ->Fill(v_st1_r.X() - v_st1_t->X(), charge);
      h2_y1_d ->Fill(v_st1_r.Y() - v_st1_t->Y(), charge);
      h2_z1_d ->Fill(v_st1_r.Z() - v_st1_t->Z(), charge);
      h2_px1_d->Fill(p_st1_r.X() - p_st1_t->X(), charge);
      h2_py1_d->Fill(p_st1_r.Y() - p_st1_t->Y(), charge);
      h2_pz1_d->Fill(p_st1_r.Z() - p_st1_t->Z(), charge);
    }
  }
  cout << endl;
  
  gSystem->mkdir("result/track", true);
  ofstream ofs("result/track/result.txt");
  ofs << "N of trees  = " << n_tree << "\n"
      << "N of events = " << h1_cnt->GetBinContent(1) << "\n"
      << "N of tracks = " << h1_cnt->GetBinContent(2) << "\n";
  ofs.close();
  
  c1 = new TCanvas("c1", "");
  c1->SetGrid();

  h1_ntrk->Draw();  c1->SaveAs("result/track/h1_ntrk.pdf");

  DrawPosNegHist(h2_nhit);
  DrawPosNegHist(h2_chi2);
  DrawPosNegHist(h2_x1_t );
  DrawPosNegHist(h2_y1_t );
  DrawPosNegHist(h2_z1_t );
  DrawPosNegHist(h2_px1_t);
  DrawPosNegHist(h2_py1_t);
  DrawPosNegHist(h2_pz1_t);
  DrawPosNegHist(h2_x1_r );
  DrawPosNegHist(h2_y1_r );
  DrawPosNegHist(h2_z1_r );
  DrawPosNegHist(h2_px1_r);
  DrawPosNegHist(h2_py1_r);
  DrawPosNegHist(h2_pz1_r);
  DrawPosNegHist(h2_x1_d );
  DrawPosNegHist(h2_y1_d );
  DrawPosNegHist(h2_z1_d );
  DrawPosNegHist(h2_px1_d);
  DrawPosNegHist(h2_py1_d);
  DrawPosNegHist(h2_pz1_d);
  
  exit(0);
}
