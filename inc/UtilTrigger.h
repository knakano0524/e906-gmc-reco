#ifndef _UTIL_TRIGGER__H_
#define _UTIL_TRIGGER__H_

namespace UtilTrigger {

/// Convert a set of hodo IDs to a roadset ID.
/**
 * Note that the max hodo IDs are 23 (H1), 16 (H2), 16 (H3) and 16 (H4).
 */
int Hodo2Road(const int h1, const int h2, const int h3, const int h4, const int tb)
{
  if (h1 * h2 * h3 * h4 * tb == 0) return 0; // if any is 0.
  int road = (h1-1)*16*16*16 + (h2-1)*16*16 + (h3-1)*16 + h4;
  return (tb > 0 ? +1 : -1) * road;
}

/// Convert a roadset ID to a set of hodo IDs.
void Road2Hodo(const int road, int& h1, int& h2, int& h3, int& h4, int& tb)
{
  if (road == 0) {
    h1 = h2 = h3 = h4 = tb = 0;
    return;
  }
  int rr = abs(road) - 1;
  h1 = 1 + (rr/16/16/16);
  h2 = 1 + (rr/16/16   ) %16;
  h3 = 1 + (rr/16      ) %16;
  h4 = 1 + (rr         ) %16;
  tb = road>0 ? +1 : -1;
}

/// Flip the given road ID in the left-right direction.
int FlipRoadLeftRight(const int road)
{
  int h1, h2, h3, h4, tb;
  Road2Hodo(road, h1, h2, h3, h4, tb);
  h1 = 24 - h1; // 1 <-> 23, 2 <-> 22,,,
  h2 = 17 - h2; // 1 <-> 16, 2 <-> 15,,,
  h3 = 17 - h3;
  h4 = 17 - h4;
  return Hodo2Road(h1, h2, h3, h4, tb);
}

/// Flip the given road ID in the top-bottom direction.
int FlipRoadTopBottom(const int road)
{
  int h1, h2, h3, h4, tb;
  Road2Hodo(road, h1, h2, h3, h4, tb);
  return Hodo2Road(h1, h2, h3, h4, -tb);
}

std::vector<int> ReadRoadList(const std::string fname)
{
  std::vector<int> list_roads;
  ifstream ifs(fname);
  string line;
  while (getline(ifs, line)) {
    istringstream iss(line);
    int road_id;
    iss >> road_id;
    list_roads.push_back(road_id);
  }
  ifs.close();
  return list_roads;
}

/**
 * See DocDB 11568 about the handling of roadsets 67 and 70.
 */
std::vector<int> ReadRoadList(const int rs_id, const int plus_minus, const int top_bottom)
{
  std::string fname = "/exp/seaquest/app/software/osg/software/AL9/seaquest/trigger/firmware/roads/L1/";
  fname += std::to_string(rs_id) + "/roads";
  if (rs_id == 67 || rs_id == 70) fname += (plus_minus > 0 ? "_minus" : "_plus");
  else                            fname += (plus_minus > 0 ? "_plus" : "_minus");
  fname += (top_bottom > 0 ? "_top" : "_bottom");
  fname += ".txt";
  
  return ReadRoadList(fname);
}
  
std::vector<int> FindEnabledRoads(const std::vector<int>& list_road_id, const std::vector<int>& roadset)
{
  std::vector<int> list_enabled_roads;
  for (auto it = list_road_id.begin(); it != list_road_id.end(); it++) {
    if (std::find(roadset.begin(), roadset.end(), *it) != roadset.end()) list_enabled_roads.push_back(*it);
  }
  return list_enabled_roads;
}

bool IsEnabledRoad(const int road_id, const std::vector<int>& roadset)
{
  std::vector<int> list_road = { road_id };
  std::vector<int> list_road_enabled = FindEnabledRoads(list_road, roadset);
  return list_road_enabled.size() > 0;
}
  
}; // namespace UtilTrigger

#endif // _UTIL_TRIGGER__H_
