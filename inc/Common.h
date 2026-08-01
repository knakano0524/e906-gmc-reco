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

/// cf. https://seaquest-docdb.fnal.gov/cgi-bin/sso/ShowDocument?docid=9397
/// DY only
double GetPtReWeight(const int tgt, const double pT, const double xF, const double mass)
{
  const double P = 120.0;
  const double M = 0.938;
  const double S = 2*M*M + 2*M*sqrt(P*P+M*M);
  const double p1_old = 2.8; //DY
  //Double_t p1_old = 3.0;//jpsi & psip
	
  double tau = mass*mass/S;
  double pTsqMax = S/4 * (1-tau)*(1-tau)*(1-xF*xF);
  if (pTsqMax < 0.09) return 1.0;

  const double P10[8] = { 0,  2.411 , 0,  2.394 , 0,  2.518 ,  2.434 ,  2.403  }; // DY
  const double P11[8] = { 0, -0.7658, 0, -0.7576, 0, -0.9891, -0.5689, -0.5147 }; // DY
  Double_t p1_new = P10[tgt] + P11[tgt] * fabs(xF);

  double f_old = pow(1+pT*pT/p1_old/p1_old, 6) * p1_old*p1_old * (1 - pow(1 + pTsqMax/p1_old/p1_old, -5));
  double f_new = pow(1+pT*pT/p1_new/p1_new, 6) * p1_new*p1_new * (1 - pow(1 + pTsqMax/p1_new/p1_new, -5));
  return f_old / f_new;
}

#endif // _COMMON__H_
