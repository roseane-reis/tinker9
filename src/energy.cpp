#include "energy.h"
#include "error.h"
#include "md.h"
#include "nblist.h"
#include "potent.h"


TINKER_NAMESPACE_BEGIN
void energy_data(rc_op op)
{
   if ((rc_flag & calc::vmask) == 0)
      return;

   rc_man egv42{egv_data, op};

   // bonded terms

   rc_man ebond42{ebond_data, op};
   rc_man eangle42{eangle_data, op};
   rc_man estrbnd42{estrbnd_data, op};
   rc_man eurey42{eurey_data, op};
   rc_man eopbend42{eopbend_data, op};
   rc_man etors42{etors_data, op};
   rc_man epitors42{epitors_data, op};
   rc_man etortor42{etortor_data, op};

   // misc. terms

   rc_man egeom42{egeom_data, op};

   // non-bonded terms

   rc_man evdw42{evdw_data, op};

   // Must call elec_data() before any electrostatics routine.

   rc_man elec42{elec_data, op};

   rc_man echarge42{echarge_data, op};

   rc_man empole42{empole_data, op};
   rc_man epolar42{epolar_data, op};
   // Must follow empole_data() and epolar_data().
   rc_man emplar42{emplar_data, op};
}


const TimeScaleConfig& default_tsconfig()
{
   static TimeScaleConfig tsconfig{
      {"ebond", 0},   {"eangle", 0},  {"estrbnd", 0},
      {"eurey", 0},   {"eopbend", 0}, {"etors", 0},
      {"epitors", 0}, {"etortor", 0}, {"egeom", 0},

      {"evdw", 0},

      {"echarge", 0},

      {"emplar", 0},  {"empole", 0},  {"epolar", 0},
   };
   return tsconfig;
}


void energy(int vers, unsigned tsflag, const TimeScaleConfig& tsconfig)
{
   auto tscfg = [&](std::string eng) {
      auto local_flag = tsflag;
      const auto& local_cfg = tsconfig;
      try {
         return local_flag & (1 << local_cfg.at(eng));
      } catch (const std::out_of_range&) {
         TINKER_THROW(format("Time scale of the {} term is unknown.\n", eng));
      }
   };


   vers = vers & calc::vmask;
   zero_egv(vers);


   // bonded terms


   if (use_potent(bond_term))
      if (tscfg("ebond"))
         ebond(vers);
   if (use_potent(angle_term))
      if (tscfg("eangle"))
         eangle(vers);
   if (use_potent(strbnd_term))
      if (tscfg("estrbnd"))
         estrbnd(vers);
   if (use_potent(urey_term))
      if (tscfg("eurey"))
         eurey(vers);
   if (use_potent(opbend_term))
      if (tscfg("eopbend"))
         eopbend(vers);
   if (use_potent(torsion_term))
      if (tscfg("etors"))
         etors(vers);
   if (use_potent(pitors_term))
      if (tscfg("epitors"))
         epitors(vers);
   if (use_potent(tortor_term))
      if (tscfg("etortor"))
         etortor(vers);


   // misc. terms


   if (use_potent(geom_term))
      if (tscfg("egeom"))
         egeom(vers);


   // non-bonded terms


   if (use_potent(vdw_term))
      if (tscfg("evdw"))
         evdw(vers);


   if (use_potent(charge_term))
      if (tscfg("echarge"))
         echarge(vers);


   bool calc_mpole = use_potent(mpole_term); // quadrupole
   bool calc_polar = use_potent(polar_term); // AMOEBA polarization (Thole)
   bool calc_mplar = calc_mpole && calc_polar;
   calc_mplar = calc_mplar && !(vers & calc::analyz);
   calc_mplar = calc_mplar && (mlist_version() == NBL_SPATIAL);
   if (calc_mplar) {
      if (tscfg("emplar")) {
         elec_init(vers);
         emplar(vers);
         torque(vers);
      }
   } else {
      // update logical flags by time-scale configurations
      calc_mpole = calc_mpole && tscfg("empole");
      calc_polar = calc_polar && tscfg("epolar");
      if (calc_mpole || calc_polar) {
         elec_init(vers);
         if (calc_mpole)
            empole(vers);
         if (calc_polar)
            epolar(vers);
         torque(vers);
      }
   }


   sum_energy(vers);
}


void energy(int vers)
{
   energy(vers, 1, default_tsconfig());
}
TINKER_NAMESPACE_END
