#include "ff/atom.h"
#include "ff/box.h"
#include "ff/energy.h"
#include "ff/pchg/evalence.h"
#include "ff/potent.h"
#include "math/zero.h"
#include "tool/darray.h"
#include "tool/io.h"
#include <cassert>
#include <tinker/detail/angbnd.hh>
#include <tinker/detail/angpot.hh>
#include <tinker/detail/potent.hh>

namespace tinker {
void eangleData(RcOp op)
{
   if (not usePotent(Potent::ANGLE) and not usePotent(Potent::STRBND) and
      not usePotent(Potent::OPBEND) and not usePotent(Potent::ANGTOR) and not potent::use_chgflx)
      return;

   bool rc_a = rc_flag & calc::analyz;

   if (op & RcOp::DEALLOC) {
      darray::deallocate(iang, ak, anat, afld, angtyp);

      if (rc_a)
         bufferDeallocate(rc_flag, ea, vir_ea, deax, deay, deaz);
      ea = nullptr;
      vir_ea = nullptr;
      deax = nullptr;
      deay = nullptr;
      deaz = nullptr;
   }

   if (op & RcOp::ALLOC) {
      nangle = countBondedTerm(Potent::ANGLE);
      darray::allocate(nangle, &iang, &ak, &anat, &afld, &angtyp);

      ea = eng_buf;
      vir_ea = vir_buf;
      deax = gx;
      deay = gy;
      deaz = gz;
      if (rc_a)
         bufferAllocate(rc_flag, &ea, &vir_ea, &deax, &deay, &deaz);
   }

   if (op & RcOp::INIT) {
      std::vector<int> iangvec(nangle * 4);
      for (size_t i = 0; i < iangvec.size(); ++i) {
         iangvec[i] = angbnd::iang[i] - 1;
      }
      darray::copyin(g::q0, nangle, iang, iangvec.data());
      darray::copyin(g::q0, nangle, ak, angbnd::ak);
      darray::copyin(g::q0, nangle, anat, angbnd::anat);
      darray::copyin(g::q0, nangle, afld, angbnd::afld);
      waitFor(g::q0);

      angunit = angpot::angunit;
      cang = angpot::cang;
      qang = angpot::qang;
      pang = angpot::pang;
      sang = angpot::sang;
      std::vector<eangle_t> angtypvec(nangle);
      for (int i = 0; i < nangle; ++i) {
         FstrView atyp = angpot::angtyp[i];
         if (atyp == "IN-PLANE")
            angtypvec[i] = eangle_t::in_plane;
         else if (atyp == "HARMONIC")
            angtypvec[i] = eangle_t::harmonic;
         else if (atyp == "LINEAR")
            angtypvec[i] = eangle_t::linear;
         else if (atyp == "FOURIER")
            angtypvec[i] = eangle_t::fourier;
         else {
            assert(false);
         }
      }
      darray::copyin(g::q0, nangle, angtyp, angtypvec.data());
      waitFor(g::q0);
   }
}

void eangle(int vers)
{
   bool rc_a = rc_flag & calc::analyz;
   bool do_e = vers & calc::energy;
   bool do_v = vers & calc::virial;
   bool do_g = vers & calc::grad;

   if (rc_a) {
      zeroOnHost(energy_ea, virial_ea);
      auto bsize = bufferSize();
      if (do_e)
         darray::zero(g::q0, bsize, ea);
      if (do_v)
         darray::zero(g::q0, bsize, vir_ea);
      if (do_g)
         darray::zero(g::q0, n, deax, deay, deaz);
   }

   eangle_acc(vers);

   if (rc_a) {
      if (do_e) {
         energy_ea = energyReduce(ea);
         energy_valence += energy_ea;
      }
      if (do_v) {
         virialReduce(virial_ea, vir_ea);
         for (int iv = 0; iv < 9; ++iv)
            virial_valence[iv] += virial_ea[iv];
      }
      if (do_g)
         sum_gradient(gx, gy, gz, deax, deay, deaz);
   }
}
}
