#ifndef _COMMON__H_
#define _COMMON__H_
#include <SRecEvent.h>
#include "UtilTrigger.h"

std::vector<int> MakeRoadList(
  const int top_bottom,
  const std::list<int> list_h1,
  const std::list<int> list_h2, 
  const std::list<int> list_h3,
  const std::list<int> list_h4,
  SRawEvent* raw)
{
  std::vector<int> list_road;
  for (auto it1 = list_h1.begin(); it1 != list_h1.end(); it1++) {
    int e1 = raw->getHit(*it1).elementID;
    for (auto it2 = list_h2.begin(); it2 != list_h2.end(); it2++) {
      int e2 = raw->getHit(*it2).elementID;
      for (auto it3 = list_h3.begin(); it3 != list_h3.end(); it3++) {
        int e3 = raw->getHit(*it3).elementID;
        for (auto it4 = list_h4.begin(); it4 != list_h4.end(); it4++) {
          int e4 = raw->getHit(*it4).elementID;
          list_road.push_back( UtilTrigger::Hodo2Road(e1, e2, e3, e4, top_bottom) );
        }
      }
    }
  }
  return list_road;
}

#endif // _COMMON__H_
