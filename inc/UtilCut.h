#ifndef _UTIL_CUT__H_
#define _UTIL_CUT__H_
#include <SRecEvent.h>

namespace UtilCut {
  
bool Doc2111v42Track(const int rs_id, SRecTrack* trk)
{
  double y_off = rs_id < 60  ?  0.4  :  1.6;
  return
       trk->getChisqTarget() < 15
    && trk->getMomentumVecSt1().Z() >  9 
    && trk->getMomentumVecSt1().Z() < 75
    && trk->getNHits() > 13
    && pow(trk->getTargetPos().X(), 2) + pow(trk->getTargetPos().Y() - y_off, 2) <  320
    && pow(trk->getDumpPos  ().X(), 2) + pow(trk->getDumpPos  ().Y() - y_off, 2) < 1100
    && pow(trk->getDumpPos  ().X(), 2) + pow(trk->getDumpPos  ().Y() - y_off, 2) >   16
    && trk->getChisqTarget() < 1.5 * trk->getChisqUpstream()
    && trk->getChisqTarget() < 1.5 * trk->getChisqDump()
    && trk->getVertexPos().Z() <   -5
    && trk->getVertexPos().Z() > -320
    && trk->getChisq() / (trk->getNHits() - 5) < 12
    && trk->getPositionVecSt1().Y() / trk->getPositionVecSt3().Y() < 1
    && fabs( fabs(trk->getMomentumVecSt1().X() - trk->getMomentumVecSt3().X()) - 0.416 ) < 0.008
    && fabs( trk->getMomentumVecSt1().Y() - trk->getMomentumVecSt3().Y() ) < 0.008
    && fabs( trk->getMomentumVecSt1().Z() - trk->getMomentumVecSt3().Z() ) < 0.08 
    && trk->getPositionVecSt1().Y() * trk->getPositionVecSt3().Y() > 0
    && fabs( trk->getMomentumVecSt1().Y() ) > 0.02
    ;
}

bool Doc2111v42Dimuon(const int rs_id, SRecDimuon* dim, SRecTrack* trk1, SRecTrack* trk2)
{
  double y_off = rs_id < 60  ?  0.4  :  1.6;
  TLorentzVector mom = dim->p_pos + dim->p_neg;
  double trk_sep = trk1->getZVertex() - trk2->getZVertex();
  return
       fabs( dim->vtx.X() ) < 0.25
    && fabs( dim->vtx.Y() - y_off ) < 0.22
    && pow(dim->vtx.X(), 2) + pow(dim->vtx.Y() - y_off, 2) < 0.06
    && dim->vtx.Z() > -280
    && dim->vtx.Z() <   -5
    && fabs( mom.X() ) < 1.8
    && fabs( mom.Y() ) < 2.0
    && mom.X()*mom.X() + mom.Y()*mom.Y() < 5
    && mom.Z() >  38
    && mom.Z() < 116
    && fabs(trk_sep) < 270
    && dim->chisq_kf < 18
    && fabs( trk1->getChisqTarget() + trk2->getChisqTarget() - dim->chisq_kf) < 2
    && trk1->getMomentumVecSt3().Y() * trk2->getMomentumVecSt3().Y() < 0
    && trk1->getNHits() + trk2->getNHits() > 29
    && trk1->getNHitsInStation(1) + trk2->getNHitsInStation(1) > 8
    && fabs( trk1->getPositionVecSt1().X() + trk2->getPositionVecSt1().X() ) < 42
    ;
}

bool Doc2111v42Phys(SRecDimuon* dim)
{
  return
       dim->mass > 4.2
    && dim->mass < 8.8
    && dim->xF < 0.95
    && dim->xF > -0.1
    && dim->x2 > 0.05
    && dim->x2 < 0.55
    && fabs(dim->costh) < 0.5
    ;
}

bool Doc2111v42Occ(SRawEvent* raw)
{
  int D1 = raw->getNHitsInD1();
  int D2 = raw->getNHitsInD2();
  int D3 = raw->getNHitsInD3();
  return
       D1 < 400 // Need consider D0 also
    && D2 < 400
    && D3 < 400
    && D1 + D2 + D3 < 1000
//    && (RFp00-34)*G2SEM/(QIEsum-369000*588*34) > 0
//    && (RFp00-34)*G2SEM/(QIEsum-369000*588*34) < 80000
    ;
}

bool Doc2111v42(const int rs_id, SRawEvent* raw, SRecDimuon* dim, SRecTrack* trk1, SRecTrack* trk2)
{
  return
       Doc2111v42Track (rs_id, trk1)
    && Doc2111v42Track (rs_id, trk2)
    && Doc2111v42Dimuon(rs_id, dim, trk1, trk2)
    && Doc2111v42Phys(dim)
    && Doc2111v42Occ(raw)
    ;
}

} // UtilCut

#endif // _UTIL_CUT__H_
