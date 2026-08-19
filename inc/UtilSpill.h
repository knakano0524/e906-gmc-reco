#ifndef _UTIL_SPILL__H_
#define _UTIL_SPILL__H_
#include <SRawEvent.h>

namespace UtilSpill {

struct SpillInfo {
  float g2sem;
  float qiesum;
  float qie_ped;
  SpillInfo() : g2sem(0), qiesum(0), qie_ped(0) {;}
  virtual ~SpillInfo() {;}
};
typedef std::unordered_map<unsigned int, SpillInfo> SpillInfoList_t;

double GetIntensity(const int rs_id, const int spill_id, const double RFp00)
{
  static SpillInfoList_t spill_info_list;
  if (spill_info_list.size() == 0) {
    string fn_in = "/seaquest/users/kenichi/intensity_table/data_" + to_string(rs_id) + ".root";
    cout << "UtilSpill::GetIntensity(): " << fn_in << endl;
    TFile* file = new TFile(fn_in.c_str());
    TTree* tree = (TTree*)file->Get("tree");
    unsigned int sp_id;
    SpillInfo si;
    tree->SetBranchAddress("spill_id", &sp_id);
    tree->SetBranchAddress("g2sem"   , &si.g2sem);
    tree->SetBranchAddress("qiesum"  , &si.qiesum);
    tree->SetBranchAddress("qie_ped" , &si.qie_ped);
    for (int i_ent = 0; i_ent < tree->GetEntries(); i_ent++) {
      tree->GetEntry(i_ent);
      spill_info_list[sp_id] = si;
    }
    file->Close();
    if (spill_info_list.size() == 0) {
      cout << "!!ERROR!!  Cannot read any entry.  Abort." << endl;
      exit(1);
    }
  }

  auto it = spill_info_list.find(spill_id);
  if (it == spill_info_list.end()) {
    cout << "!!ERROR!!  Cannot find spill " << spill_id
         << " in spill_info_list.  Abort." << endl;
    exit(1);
  }
  SpillInfo* si = &it->second;
  return (RFp00 - si->qie_ped) * si->g2sem / si->qiesum;
}
  
}; // End of "namespace UtilSpill"

#endif // _UTIL_SPILL__H_
