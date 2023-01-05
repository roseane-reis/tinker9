Induced Dipole
==============

Energy
------

:math:`\mu` is the induced dipole in the external field *E*.
The induced field due to the induced dipole is :math:`E^u=-T\mu`, and the
induced dipole is proportional to the total field :math:`E^t`:

.. math::

   \mu = \alpha E^t = \alpha(E+E^u),

where :math:`\alpha` is the polarizability.
Defining :math:`\tilde{T}=\alpha^{-1}+T`, the induced dipole is the solution
to the linear equation

.. math::
   :label: u-dipole

   \tilde{T}\mu = E.

The polarization energy is given by

.. math::
   :label: polar1

   U &= -\mu E +\int_0^\mu d\mu\ \tilde{T}\mu \\
     &= -\mu E +\frac{1}{2}\mu\tilde{T}\mu.

On the right-hand side of :eq:`polar1`:

   - the 1st term is the contribution from the external field;
   - the 2nd term is the mutual and self polarization energy.

Finally, the polarization energy is

.. math::
   :label: polar2

   U = -\frac{1}{2}\mu E.

Gradient
--------

With limited numerical precision, the solution to linear equation :eq:`u-dipole`
cannot be fully precise:

.. math::
   :label: u-dipole2

   \tilde{T}\mu = \epsilon + E.

The gradient of the induced dipole can be written in

.. math::

   \mu' = \tilde{T}^{-1}(\epsilon' + E' - \tilde{T}'\mu),

and the polarization gradient is

.. math::

   U' &= -\frac{1}{2} (E\mu' + \mu E') \\
      &= -\frac{1}{2} [(-\epsilon+\mu\tilde{T})\tilde{T}^{-1}(\epsilon' +E' -\tilde{T}'\mu) +\mu E'] \\
      &\approx -\frac{1}{2} (\mu E' -\mu\tilde{T}'\mu +\mu E'),

only if the convergence of :eq:`u-dipole2` is tight that :math:`\epsilon` and :math:`\epsilon'`
terms will drop.

Conjugate Gradient
------------------

Tinker uses the following Conjugate Gradient algorithm (C.G.) with a sparse
matrix preconditioner (denoted as *M*) :cite:`cg-precond`
to obtain the induced dipoles.
Related Tinker variables and routines are tabulated.

.. figure:: ../fig/cg.*
   :width: 300 px
   :align: center

====================  =============================
C.G. Terms            Tinker variables and routines
====================  =============================
:math:`\gamma`        a
:math:`\beta`         b
:math:`r`             rsd
:math:`M r`           zrsd
:math:`p`             conj
:math:`\tilde{T} p`   vec
:math:`-T`            ufield()
:math:`M`             uscale()
====================  =============================

Polarization Model: AMOEBA (Thole Damping 2)
--------------------------------------------

AMOEBA force field adopts two polarization schemes, *d* and *p*, for the
external field due to the permanent multipoles, and a third scheme *u* for
mutual induced dipole interactions. Both *d* and *u* schemes are group-based.
The *p* scheme is atomic connectivity-based. Tinker uses C.G. iterations to
solve the following linear equations

.. math::

   (1/\alpha+T^u)\mu_d &= E_d \\
   (1/\alpha+T^u)\mu_p &= E_p,

and defines the polarization energy as

.. math::
   :label: amoeba-polar1

   U = -\frac{1}{2}\mu_d E_p.

From an optimizational perspective, :eq:`amoeba-polar1` is the minimum of the
target function

.. math::

   f_1(\mu_d,\mu_p)=\frac{1}{2}\left(\frac{1}{2}\mu_d\tilde{T}\mu_p
      +\frac{1}{2}\mu_p\tilde{T}\mu_d -E_d\mu_p-E_p\mu_d\right),

whereas the way C.G. coded in Tinker is to solve the minimum of
another target function

.. math::

   f_2(\mu_d,\mu_p)=\frac{1}{2}\left(\frac{1}{2}\mu_d\tilde{T}\mu_d
      +\frac{1}{2}\mu_p\tilde{T}\mu_p -E_d\mu_d-E_p\mu_p\right).

The difference in two target functions is usually negligible unless other
loose convergence methods are used to compute the induced dipoles.

In the Thole damping model, a charge distribution
:math:`\rho` is used as a replacement for the point dipole model.
AMOEBA adopts the second functional form

.. math::

   \rho = \frac{3a}{4\pi}\exp(-au^3)

from paper :cite:`thole-damping`, where *u* is the polarizability-scaled
distance. The electrostatic field and potential at distance *r* can be obtained
from Gauss's law,

.. math::

   E(r) = -\phi'(r) = \frac{1}{r^2} \int_0^u du\ 4\pi u^2 \rho = \frac{1-\exp(-au^3)}{r^2},

.. math::

   \phi(r) = \int_r^\infty dr\ E(r) = \frac{\lambda_1}{r} = \frac{1}{r}\left[
      1-\frac{(au^3)^\frac{1}{3}}{3}\Gamma(-\frac{1}{3},au^3)\right],

where :math:`\lambda_1` serves as the :math:`B_0` term in EWALD
quadrupole interactions.
:math:`\lambda_n` terms are also related via derivatives

.. math::

   \phi'' &= \frac{1}{r^3}\left[2-(2+3au^3)\exp(-au^3)\right],          \\
   \phi''' &= \frac{3}{r^4}\left[-2+(2+2au^3+3a^2u^6)\exp(-au^3)\right], \\
   \phi'''' &= \frac{3}{r^5}\left[8-(8+8au^3+9a^3u^9)\exp(-au^3)\right],

.. math::

   \phi'_i        &= \phi'\frac{r_i}{r}, \\
   \phi''_{ij}    &= \left(\phi''-\frac{\phi'}{r}\right)\frac{r_i r_j}{r^2}
                     +\frac{\phi'}{r}\delta_{ij}, \\
   \phi'''_{ijk}  &= \left(\phi'''-\frac{3\phi''}{r}+\frac{3\phi'}{r^2}\right)\frac{r_i r_j r_k}{r^3}
                     +\left(\frac{\phi''}{r}-\frac{\phi'}{r^2}\right)\frac{\sum r_k \delta_{ij}}{r}, \\
   \phi''''_{ijkl}&= \left(\phi''''-\frac{6\phi'''}{r}+\frac{15\phi''}{r^2}-\frac{15\phi'}{r^3}\right)
                     \frac{r_i r_j r_k r_l}{r^4} \\
                  &+\left(\frac{\phi'''}{r}-\frac{3\phi''}{r^2}+\frac{3\phi'}{r^3}\right)\frac{\sum r_k r_l\delta_{ij}}{r^2}
                     +\left(\frac{\phi''}{r^2}-\frac{\phi'}{r^3}\right)\sum\delta_{kl}\delta_{ij}.

Thus,

.. math::

   -\lambda_3/r^3 &= \phi'/r \Rightarrow        \\
                  &\lambda_3 = 1 - \exp(-au^3), \\
   3\lambda_5/r^5 &= (\phi''-\phi'/r)/r^2 \Rightarrow   \\
                  &\lambda_5 = 1 - (1+au^3)\exp(-au^3), \\
   -15\lambda_7/r^7 &= (\phi'''-3\phi''/r+3\phi'/r^2)/r^3 \Rightarrow                   \\
                    &\lambda_7 = 1 - \left(1+au^3+\frac{3}{5}a^2 u^6\right)\exp(-au^3), \\
   105\lambda_9/r^9 &= (\phi''''-6\phi'''/r+15\phi''/r^2-15\phi'/r^3)/r^4 \Rightarrow   \\
                    &\lambda_9 = 1 - \left(1+au^3+\frac{18}{35}a^2 u^6+\frac{9}{35}a^3 u^9\right)\exp(-au^3).
