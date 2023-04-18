#include "ff/energy.h"
#include "ff/potent.h"
#include "tool/externfunc.h"
#include "tool/iofortstr.h"
#include <tinker/detail/solpot.hh>
#include <tinker/detail/solute.hh>

namespace tinker {
void esolvData(RcOp op)
{
   if (not use(Potent::SOLV))
      return;

   auto rc_a = rc_flag & calc::analyz;

   if (op & RcOp::DEALLOC) {
      solvtyp = Solv::NONE;
      darray::deallocate(rsolv);

      if (rc_a) {
         bufferDeallocate(calc::analyz, nct);
         bufferDeallocate(rc_flag, ect, vir_ect, dectx, decty, dectz);
      }
      nct = nullptr;
      ect = nullptr;
      vir_ect = nullptr;
      dectx = nullptr;
      decty = nullptr;
      dectz = nullptr;
   }

   if (op & RcOp::ALLOC) {
      darray::allocate(n, &rsolv);

      nct = nullptr;
      ect = eng_buf_elec;
      vir_ect = vir_buf_elec;
      dectx = gx_elec;
      decty = gy_elec;
      dectz = gz_elec;
      if (rc_a) {
         bufferAllocate(rc_flag, &nct);
         bufferAllocate(rc_flag, &ect, &vir_ect, &dectx, &decty, &dectz);
      }
   }

   if (op & RcOp::INIT) {
      FstrView ctypstr = ctrpot::ctrntyp;
      if (ctypstr == "SEPARATE")
         ctrntyp = Chgtrn::SEPARATE;
      else if (ctypstr == "COMBINED")
         ctrntyp = Chgtrn::COMBINED;
      else
         ctrntyp = Chgtrn::NONE;

      darray::copyin(g::q0, n, rsolv, solute::rsolv);
   }
}

TINKER_FVOID2(acc1, cu1, echgtrn, int);
TINKER_FVOID2(acc1, cu1, echgtrnAplus, int);
void esolv(int vers)
{
   auto rc_a = rc_flag & calc::analyz;
   auto do_a = vers & calc::analyz;
   auto do_e = vers & calc::energy;
   auto do_v = vers & calc::virial;
   auto do_g = vers & calc::grad;

   zeroOnHost(energy_ect);
   size_t bsize = bufferSize();
   if (rc_a) {
      if (do_a)
         darray::zero(g::q0, bsize, nct);
      if (do_e)
         darray::zero(g::q0, bsize, ect);
      if (do_v)
         darray::zero(g::q0, bsize, vir_ect);
      if (do_g)
         darray::zero(g::q0, n, dectx, decty, dectz);
   }

   if (ctrntyp == Chgtrn::SEPARATE) {
      TINKER_FCALL2(acc1, cu1, echgtrn, vers);
   } else if (ctrntyp == Chgtrn::COMBINED) {
      TINKER_FCALL2(acc1, cu1, echgtrnAplus, vers);
   }

   if (rc_a) {
      if (do_e) {
         EnergyBuffer u = ect;
         energy_prec e = energyReduce(u);
         energy_ect += e;
         energy_elec += e;
      }
      if (do_v) {
         VirialBuffer u = vir_ect;
         virial_prec v[9];
         virialReduce(v, u);
         for (int iv = 0; iv < 9; ++iv) {
            virial_ect[iv] += v[iv];
            virial_elec[iv] += v[iv];
         }
      }
      if (do_g)
         sumGradient(gx_elec, gy_elec, gz_elec, dectx, decty, dectz);
   }
}
}
