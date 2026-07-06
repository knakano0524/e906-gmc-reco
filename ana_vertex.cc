R__LOAD_LIBRARY(kTracker)

void ana_vertex(const char* fn_list="auto_file/list_ana_vertex.txt")
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

  TH1* h1_cnt  = new TH1D("h1_cnt", "", 10, 0.5, 10.5);
  TH1* h1_ndim = new TH1D("h1_ndim", ";N of dimuons/event;", 10, -0.5, 9.5);
  TH1* h1_mass_t = new TH1D("h1_mass_t",  ";True mass;", 100, 0, 10);
  TH1* h1_mass_r = new TH1D("h1_mass_r", ";Reco. mass;", 100, 0, 10);
  TH1* h1_mass_d = new TH1D("h1_mass_d", ";M_{reco} - M_{true};", 100, -2, 2);
  TH1* h1_xF_t = new TH1D("h1_xF_t",  ";True xF;", 150, -0.5, 1);
  TH1* h1_xF_r = new TH1D("h1_xF_r", ";Reco. xF;", 150, -0.5, 1);
  TH1* h1_xF_d = new TH1D("h1_xF_d", ";xF_{reco} - xF_{true};", 100, -0.5, 0.5);
  TH1* h1_z_t = new TH1D("h1_z_t",  ";True z-vertex;", 100, -230, -30);
  TH1* h1_z_r = new TH1D("h1_z_r", ";Reco. z-vertex;", 100, -230, -30);
  TH1* h1_z_d = new TH1D("h1_z_d", ";z_{reco} - z_{true};", 100, -100, 100);
  
  SRawMCEvent* raw = 0;
  SRecEvent  * rec = 0;
  tree->SetBranchAddress("rawEvent", &raw);
  tree->SetBranchAddress("recEvent", &rec);
  for (unsigned int i_ent = 0; i_ent < n_ent; i_ent++) {
    if      ( (i_ent+1) % (n_ent/ 10) == 0) cout << "o" << flush;
    else if ( (i_ent+1) % (n_ent/100) == 0) cout << "." << flush;
    tree->GetEntry(i_ent);
    h1_cnt->Fill(1);
    
    double mass_t = raw->mass;
    double xF_t   = raw->xF;
    double zvtx_t = raw->vtx.Z();
    //double weight = raw->weight;
    h1_mass_t->Fill(mass_t);
    h1_xF_t  ->Fill(xF_t);
    h1_z_t   ->Fill(zvtx_t);

    int n_dim = rec->getNDimuons();
    h1_ndim->Fill(n_dim);
    for (int i_dim = 0; i_dim < n_dim; i_dim++) {
      h1_cnt->Fill(2);      
      SRecDimuon dim = rec->getDimuon(i_dim);
      double mass_r  = dim.mass;
      double xF_r    = dim.xF;
      double zvtx_r  = dim.vtx.Z();
      h1_mass_r->Fill(mass_r);
      h1_mass_d->Fill(mass_r - mass_t);
      h1_xF_r  ->Fill(xF_r);
      h1_xF_d  ->Fill(xF_r - xF_t);
      h1_z_r   ->Fill(zvtx_r);
      h1_z_d   ->Fill(zvtx_r - zvtx_t);
    }
  }
  
  gSystem->mkdir("result/vertex", true);
  ofstream ofs("result/vertex/result.txt");
  ofs << "N of trees   = " << n_tree << "\n"
      << "N of events  = " << h1_cnt->GetBinContent(1) << "\n"
      << "N of dimuons = " << h1_cnt->GetBinContent(2) << "\n";
  ofs.close();

  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();

  h1_ndim  ->Draw();  c1->SaveAs("result/vertex/h1_ndim.pdf");
  h1_mass_t->Draw();  c1->SaveAs("result/vertex/h1_mass_t.pdf");
  h1_mass_r->Draw();  c1->SaveAs("result/vertex/h1_mass_r.pdf");
  h1_mass_d->Draw();  c1->SaveAs("result/vertex/h1_mass_d.pdf");
  h1_xF_t  ->Draw();  c1->SaveAs("result/vertex/h1_xF_t.pdf");
  h1_xF_r  ->Draw();  c1->SaveAs("result/vertex/h1_xF_r.pdf");
  h1_xF_d  ->Draw();  c1->SaveAs("result/vertex/h1_xF_d.pdf");
  h1_z_t   ->Draw();  c1->SaveAs("result/vertex/h1_z_t.pdf");
  h1_z_r   ->Draw();  c1->SaveAs("result/vertex/h1_z_r.pdf");
  h1_z_d   ->Draw();  c1->SaveAs("result/vertex/h1_z_d.pdf");

  exit(0);
}
