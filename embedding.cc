R__LOAD_LIBRARY(kTracker)
using namespace std;
void FindMcFiles(const string dir_data, vector<string>& list_data);
void SelectNim3Events(TTree* tree, const unsigned int n_evt_mc, int& n_loop, int& i_evt, vector<unsigned int>& list_evt_idx);
const string dir_nim3_data = "/pnfs/e906/persistent/users/kenichi/NIM3_data";
  
/**
 * ROOT macro to embed NIM3 events into MC events.
 */
void embedding(const string ds, const int tgt, const string raw_name, const string dir_gmc_base)
{
  time_t time0 = time(0);
  ostringstream oss;
  oss << dir_nim3_data << "/NIM3_ds" << ds << "_tgt" << tgt << ".root";
  string fn_nim3 = oss.str();
  cout << "fn_nim3 = " << fn_nim3 << endl;
  TFile* file_nim3 = new TFile(fn_nim3.c_str());
  TTree* tree_nim3 = (TTree*)file_nim3->Get("save");
  Long64_t n_evt_nim3 = tree_nim3->GetEntries();
  cout << fn_nim3 << "\n"
       << "  n_evt_nim3 = " << n_evt_nim3 << "\n";
  gRandom->SetSeed(1);

  SRawEvent* raw_mc   = 0;
  SRawEvent* raw_nim3 = 0;
  float     intensity = 0;

  TH1* h1_inte_all = new TH1D("h1_inte_all", "", 100, 0, 100e3);
  TH1* h1_inte_emb = new TH1D("h1_inte_emb", "", 100, 0, 100e3);
  tree_nim3->SetBranchAddress("intensity", &intensity);
  for (unsigned int i_evt = 0; i_evt < n_evt_nim3; i_evt++) {
    tree_nim3->GetEntry(i_evt);
    h1_inte_all->Fill(intensity);
  }
  
  ///
  /// Make and output directory and a list of input files
  ///
  string dir_out = dir_gmc_base + "/messy/" + raw_name;
  gSystem->mkdir(dir_out.c_str(), true);
  
  string dir_mc = dir_gmc_base + "/clean/" + raw_name;
  vector<string> list_mc;
  FindMcFiles(dir_mc, list_mc);
  unsigned int n_file_mc = list_mc.size();
  cout << "n_file_mc = " << n_file_mc << endl;

  ///
  /// Loop over the input files
  ///
  unsigned int n_evt_mc_tot = 0;
  int n_loop_nim3 = 0; 
  int i_evt_nim3 = -1;
  for (auto it = list_mc.begin(); it != list_mc.end(); it++) {
    string fn_mc = *it;
    cout << "MC file: " << fn_mc << "\n"
         << "  n_loop_nim3 = " << n_loop_nim3 << "\n"
         << "  i_evt_nim3  = " << i_evt_nim3 << endl;
    TFile* file_mc = new TFile( (dir_mc + "/" + fn_mc).c_str());
    TTree* tree_mc = (TTree*)file_mc->Get("save");
    unsigned int n_evt_mc = tree_mc->GetEntries();
    tree_mc->SetBranchAddress("rawEvent", &raw_mc);

    TFile* file_out = new TFile( (dir_out + "/" + fn_mc).c_str(), "RECREATE");
    TTree* tree_out = new TTree("save", "Created by embedding.cc");
    tree_out->Branch("rawEvent", &raw_mc, 256000, 99);
    tree_out->Branch("intensity", &intensity, "intensity/F");

    vector<unsigned int> list_evt_idx_nim3;
    SelectNim3Events(tree_nim3, n_evt_mc, n_loop_nim3, i_evt_nim3, list_evt_idx_nim3);
    tree_nim3->ResetBranchAddresses();
    tree_nim3->SetBranchAddress("rawEvent", &raw_nim3);
    tree_nim3->SetBranchAddress("intensity", &intensity);

    for (unsigned int i_evt_mc = 0; i_evt_mc < n_evt_mc; i_evt_mc++) {
      tree_mc->GetEntry(i_evt_mc);
      tree_nim3->GetEntry(list_evt_idx_nim3.at(i_evt_mc));
      raw_mc->mergeEvent(*raw_nim3);
      tree_out->Fill();
      h1_inte_emb->Fill(intensity);
    }
    file_out->Write();
    file_out->Close();
    n_evt_mc_tot += n_evt_mc;
  }
  time_t time1 = time(0);
  
  ///
  /// End
  ///
  /// Output files (on pnfs) become unreadable if not deleted in advance.
  gSystem->Unlink((dir_out + "/stat.txt").c_str());
  gSystem->Unlink((dir_out + "/h1_inte_all.pdf").c_str());
  gSystem->Unlink((dir_out + "/h1_inte_emb.pdf").c_str());
  
  ofstream ofs( (dir_out + "/stat.txt").c_str() );
  ofs << "n_file_mc    = " << n_file_mc << "\n"
      << "n_evt_mc_tot = " << n_evt_mc_tot << "\n"
      << "fn_nim3      = " << fn_nim3 << "\n"
      << "n_evt_nim3   = " << n_evt_nim3 << "\n"
      << "n_loop_nim3  = " << n_loop_nim3 << "\n"
      << "i_evt_nim3   = " << i_evt_nim3 << "\n"
      << "time         = " << (time1-time0)/60.0 << " min\n";
  ofs.close();

  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  h1_inte_all->Draw();
  c1->SaveAs( (dir_out + "/h1_inte_all.pdf").c_str() );
  h1_inte_emb->Draw();
  c1->SaveAs( (dir_out + "/h1_inte_emb.pdf").c_str() );
  
  exit(0);
}

////////////////////////////////////////////////////////////////
/// Sub-functions

/// Function to search the given directory for MC files.
void FindMcFiles(const string dir_data, vector<string>& list_data)
{
  void* dirp = gSystem->OpenDirectory(dir_data.c_str());
  if (dirp == 0) {
    cout << "Cannot open the data directory: '" << dir_data << "'.  Abort." << endl;
    exit(1);
  }
  const char* name_char;
  unsigned int n_file = 0;
  while ((name_char = gSystem->GetDirEntry(dirp))) {
    string name = name_char;
    int length = name.length();
    if (name.substr(0, 5) != "user_" || name.substr(length-5, 5) != ".root") continue;
    list_data.push_back(name);
    n_file++;
  }
  gSystem->FreeDirectory(dirp);
  sort(list_data.begin(), list_data.end());
}

/**
 * Function to randomly select NIM3 events to be embedded.
 * The number of selected NIM3 events is `n_evt_mc`.
 * The selection probability is proportional to the intensity of events.
 * `i_evt` is the starting index of NIM3 events.
 * When the selection finished scanning all NIM3 events, it restarts at i_evt = 0.
 * `n_loop` keeps track of the number of i_evt loops.
 */
void SelectNim3Events(TTree* tree, const unsigned int n_evt_mc, int& n_loop, int& i_evt, vector<unsigned int>& list_evt_idx)
{
  list_evt_idx.clear();
  list_evt_idx.reserve(n_evt_mc);
  
  float intensity = 0;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("intensity", &intensity);
  Long64_t n_evt_nim3 = tree->GetEntries();
  
  for (unsigned int i_evt_mc = 0; i_evt_mc < n_evt_mc; i_evt_mc++) {
      const double inte_max = 80e3;
      while (true) {
        i_evt++;
        if (i_evt >= n_evt_nim3) {
          i_evt = 0;
          n_loop++;
        }
        tree->GetEntry(i_evt);
        if (gRandom->Rndm() < intensity / inte_max) break;
      }
      list_evt_idx.push_back(i_evt);
  }
  sort(list_evt_idx.begin(), list_evt_idx.end());
}
