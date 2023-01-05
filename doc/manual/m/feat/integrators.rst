Integrators and Ensembles
=========================

.. _label-monte-carlo-barostat:

Monte Carlo Barostat
--------------------

.. _label-berendsen-barostat:

Berendsen Barostat
------------------

.. _label-verlet:

Verlet Integrator
-----------------

.. _label-respa:

RESPA Integrator
----------------

.. _label-nose-hoover:

Extended Nosé-Hoover Chain
--------------------------

Authors of paper :cite:`mtk-nhc` (MTK) discussed several methods for NVT and
NPT ensembles.

======  ===============  ======
Number  Sections in MTK  Method
======  ===============  ======
1a      2.1 4.3          NVT
2a      2.2 4.4          NPT (isotropic cell fluctuations)
3a      2.3 4.5          NPT (full cell fluctuations)
4a      5.2              XO-RESPA
4b      5.2              XI-RESPA
1b      5.3              RESPA 1a
2b      5.4              RESPA 2a
3b      5.4              RESPA 3a
======  ===============  ======

The isothermal-isobaric integrator implemented in Fortran Tinker and here is
NPT-XO (#2a-4a).

.. tip::

   Nosé-Hoover Chain can be enabled by keywords

   .. code-block:: text

      integrator nose-hoover

   or

   .. code-block:: text

      thermostat nose-hoover
      barostat   nose-hoover

   with the NPT option in the *dynamic* program.

.. _label-lpiston:

Langevin Piston
---------------

The Langevin piston method for constant pressure :cite:`lpiston` is
integrated in the Leapfrog framework.

.. tip::

   Langevin Piston can be enabled by keywords

   .. code-block:: text

      integrator lpiston

   or

   .. code-block:: text

      thermostat lpiston
      barostat   lpiston

   with the NPT option in the *dynamic* program.
