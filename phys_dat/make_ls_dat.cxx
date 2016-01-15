

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "plan_file_io.h"
#include "gen.h"
#include "libplanio.h"
#include "libbrachy.h"

/*
 ---------------------------------------------------------------------------
 
   NAME
 	make_ls_dat - create file giving physical parameters of line sources
 
   SYNOPSIS
	make_ls_dat
 
   DESCRIPTION
	This is easier than writing an editing program. 
 
   DIAGNOSTICS
 
 
   FILES
 
 
   SEE ALSO
 
 
   BUGS
 
 
   AUTHOR
 	George W. Sherouse
 	7 November 1984
 
	(c) Copyright 1984, 1985, 1986, 1987 - George W. Sherouse
	All rights reserved.

 ---------------------------------------------------------------------------
*/


int
main(int argc, char **argv)
{
    char target[200];
    static SOURCE_SPEC sspec[] = {
     {"Cs-137 cervix tube",	/* isotope name */
      8.261,			/* gamma - (rad cm**2)/(mg h) per Saylor,
				 * NOTE: rads - where did this really come
				 * from? */
      MG,			/* units of activity  */
      1.0,			/* f-factor */
      30.0 * 365.0 * 24.0,	/* half-life in hours */
      2.0,			/* physical source length  */
      1.4,			/* active source length  */
      0.05,			/* wall thickness (cm)  */
      0.320,			/* wall attenuation coefficient  */
      0.305,			/* capsule diameter */
      1.0091,			/* tissue attenuation fit coefficients  */
      -9.015e-3,
      -3.459e-4,
      -2.817e-5,
      0.0,			/* linear tissue attenuation coefficient -
				 * will be calculated here  */
      FALSE			/* last source  */
      },

     {"Buchler Cs-137",		/* isotope name */
      3.28,			/* gamma - (R cm**2)/(mCi h) */
      MC,			/* units of activity  */
      0.957,			/* f-factor */
      30.0 * 365.0 * 24.0,	/* half-life in hours */
      0.75,			/* physical source length  */
      0.35,			/* active source length  */
      0.125,			/* wall thickness (cm)  */
      0.320,			/* wall attenuation coefficient  */
      0.45,			/* capsule diameter */
      1.0091,			/* tissue attenuation fit coefficients  */
      -9.015e-3,
      -3.459e-4,
      -2.817e-5,
      0.0,			/* linear tissue attenuation coefficient -
				 * will be calculated here  */
      FALSE			/* last source  */
      },

     {"Big Buchler Cs-137",	/* isotope name */
      3.28,			/* gamma - (R cm**2)/(mCi h) */
      MC,			/* units of activity  */
      .957,			/* f-factor */
      30.0 * 365.0 * 24.0,	/* half-life in hours */
      2.0,			/* physical source length  */
      1.54,			/* active source length  */
      0.125,			/* wall thickness (cm)  */
      0.320,			/* wall attenuation coefficient  */
      0.45,			/* capsule diameter */
      1.0091,			/* tissue attenuation fit coefficients  */
      -9.015e-3,
      -3.459e-4,
      -2.817e-5,
      0.0,			/* linear tissue attenuation coefficient -
				 * will be calculated here  */
      TRUE			/* last source  */
      }
    };
    SOURCE_SPEC *sptr;


    int       fdes,
              loop,
              radius_loop,
              angle_loop,
              steps;

    float     constant,
              half_len,
              cos_crit_angle,
              thick,
              fit_a,
              fit_b,
              fit_c,
              fit_d,
              TA_at_10,
              cos_theta,
              sin_theta,
              cos_theta_1,
              cos_theta_2,
              S1,
              S2,
              b,
              px,
              theta,
              radius,
	      delta,
	      delta_step;


    sprintf(target, "%s/%s", get_phys_dat_dir(), "ls_dat");
    fdes = open(target, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fdes < 0) {
	perror("make_ls_dat");
	fprintf(stderr, "Could not create ls_dat\n");
	exit(1);
    }

    loop = 0;
    printf("\nmake_ls_dat\n");
    do {
/*
Calculate an effective tissue attenutaion coefficient based on
two points calculated from polynomial fit.
*/
	sptr = &sspec[loop];
	sptr->mu = (LOG (
			   (POLY (7.0, sptr->TA_fit[0],
				  sptr->TA_fit[1],
				  sptr->TA_fit[2],
				  sptr->TA_fit[3]) /
			    POLY (10.0, sptr->TA_fit[0],
				  sptr->TA_fit[1],
				  sptr->TA_fit[2],
				  sptr->TA_fit[3])))) / 3.0;

	printf("mu for %s is %f\n", sptr->isotope, sptr->mu);
/*
  We precalculate here a polar map of dose rates to be used later in
  brachy_dose for really fast calculations.  The following code is more
  or less lifted from brachy_dose
*/

	constant = sptr->gamma*sptr->R_to_r/sptr->act_length;
printf("gamma, R_to_r, act_length, %f %f %f\n",
sptr->gamma, sptr->R_to_r, sptr->act_length);
printf("gamma*R_to_r/act_length: %f\n", constant);
printf("wall atten: %f %f : %f\n",
sptr->wall, sptr->wall_mu, exp(-sptr->wall*sptr->wall_mu));

	half_len = sptr->act_length * 0.5;
	cos_crit_angle = COS(ATAN(sptr->diameter/sptr->phys_length));
	thick = sptr->wall * sptr->wall_mu;
	fit_a = sptr->TA_fit[0];
	fit_b = sptr->TA_fit[1];
	fit_c = sptr->TA_fit[2];
	fit_d = sptr->TA_fit[3];
	TA_at_10 = POLY (10.0, fit_a, fit_b, fit_c, fit_d);
printf("TA_at_10: %f\n", TA_at_10);

	for (radius_loop = 0; radius_loop < 20; radius_loop++) {
	    float radius, TA;
	    radius = (float)radius_loop;
	    if (radius < 10.0) TA = POLY(radius, fit_a, fit_b, fit_c, fit_d);
	    else TA = TA_at_10*exp(-(radius-10.0)*sptr->mu);
	    printf("  TA[%.0f]: %f\n", radius, TA);
	}

	steps = POLAR_RADII - 1;
	
	sptr->polar_radii[0] = 0.0;
	sptr->polar_radii[POLAR_RADII - 1] = MAXIMUM_RADIUS;
	delta = MAXIMUM_RADIUS;
	delta_step = pow((0.1 / MAXIMUM_RADIUS), (1.0 / (steps - 1)));
	for (radius_loop = steps - 1; radius_loop > 0; radius_loop--) {
	    sptr->polar_radii[radius_loop] = (delta *= delta_step);
	}
	
	for (angle_loop = 0; angle_loop < POLAR_ANGLES; angle_loop++) {
	    sptr->polar_angles[angle_loop] =
		theta = angle_loop * PI / 2.0 / (POLAR_ANGLES - 1);
	    cos_theta = COS (theta);
	    if (cos_theta > cos_crit_angle)
		cos_theta = cos_crit_angle;
	    theta = ACOS (cos_theta);
	    sin_theta = SIN (theta);

	    for (radius_loop = 0; radius_loop < POLAR_RADII; radius_loop++) {
		radius = sptr->polar_radii[radius_loop];
		if (radius < 0.0001) radius = 0.0001;
		b = radius * sin_theta;
		if (b < 0.0001)
		    b = 0.0001;
		px = radius * cos_theta;
		if (px < 0.0001)
		    px = 0.0001;
		cos_theta_1 = b / DIST (b, px + half_len);
		if (cos_theta_1 < COS_88)
		    cos_theta_1 = COS_88;
		cos_theta_2 = b / DIST (b, px - half_len);
		if (cos_theta_2 < COS_88)
		    cos_theta_2 = COS_88;
/*
printf("\n--> b, px, half_len, cos_theta_1, cos_theta_2 = %f %f %f %f %f\n",
       b, px, half_len, cos_theta_1, cos_theta_2);
printf("theta_1, theta_2 = %f %f\n",
	ACOS(cos_theta_1) * 180.0 / 3.1415926,
	ACOS(cos_theta_2) * 180.0 / 3.1415926);
*/

/*
  Calculate the Sievert integrals for the two ends.
*/
		S1 = Sievert(cos_theta_1, thick, b, fit_a, fit_b, fit_c, fit_d, sptr->mu, TA_at_10, FALSE);
		S2 = Sievert(cos_theta_2, thick, b, fit_a, fit_b, fit_c, fit_d, sptr->mu, TA_at_10, FALSE);
/*
 Make sure the limits of integration are in the right order.  If the
 point lies along the source the integral is from theta1 to theta2,
 or ( 0 to theta1) + (0 to theta2).  If the point is beyond the ends
 the source, the integral is theta1 to theta2, or ( 0 to theta1) -
 ( 0 to theta2).
*/
		if (px <= half_len) S2 *= -1.0;
		sptr->polar_table[angle_loop][radius_loop] = (constant/b)*(S1 - S2);

if (angle_loop == POLAR_ANGLES-1) {
float WA, TA;
WA = exp(-sptr->wall*sptr->wall_mu);
if (radius < 10.0)
TA = POLY(radius, sptr->TA_fit[0], sptr->TA_fit[1], sptr->TA_fit[2], sptr->TA_fit[3]);
else TA = TA_at_10*exp(-(radius-10.0)*sptr->mu);
printf ("r = %.2f Sievert = %.3f WA,TA = %.3f %.3f dose = %.3f (%.3f)\n",
radius, S1-S2, WA, TA,
sptr->polar_table[angle_loop][radius_loop],
sptr->gamma*sptr->R_to_r*WA*TA/(radius*radius));
}
	    }
	}
	write (fdes, sspec + loop, sizeof (SOURCE_SPEC));
    }
    while (!sspec[loop++].last_entry);

    close (fdes);
    return(0);
}

