#pragma once
#include "tool/rcman.h"

namespace tinker {
/// \ingroup esolv
/// \brief method to be used for the Born radius computation
enum class Borntyp
{
   NONE,
   STILL, 
   ONION,
   GONION,
   HCT,
   OBC,
   GRYCUK,
   ACE
};

/// \ingroup esolv
void esolvData(RcOp);
/// \ingroup esolv
void esolv(int vers);
}

//====================================================================//
//                                                                    //
//                          Global Variables                          //
//                                                                    //
//====================================================================//

namespace tinker {
/// \ingroup esolv
/// \brief atomic radius of each atom for continuum solvation
TINKER_EXTERN real* rsolv;

TINKER_EXTERN real doffset;
TINKER_EXTERN real onipr;
TINKER_EXTERN real stillp1;
TINKER_EXTERN real stillp2;
TINKER_EXTERN real stillp3;
TINKER_EXTERN real stillp4;
TINKER_EXTERN real stillp5;
TINKER_EXTERN real* rsolv;
TINKER_EXTERN real* rdescr;
TINKER_EXTERN real* rborn;
TINKER_EXTERN real* drb;
TINKER_EXTERN real* drbp;
TINKER_EXTERN real* gpol;
TINKER_EXTERN real* shct;
TINKER_EXTERN real* aobc;
TINKER_EXTERN real* bobc;
TINKER_EXTERN real* gobc;
TINKER_EXTERN real* vsolv;
}
