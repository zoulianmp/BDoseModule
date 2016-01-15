
#include <stdio.h>
#include "gen.h"
#include "libbrachy.h"

float
Sievert(float cos_theta, 
	float thick, 
	float b, 
	float fit_a, 
	float fit_b, 
	float fit_c, 
	float fit_d, 
	float TA_mu, 
	float TA_at_10,
	int debug)
/*
 * This routine calculates the Sievert integral for a ray passing
 * through a given filter thickness and described by cos_theta and b.
 * Theta is the angle between the normal to the source axis and the
 * ray from the calculation point to the end of the source.  B is
 * the perpendicular distance from the source axis to the calculation
 * point.
 *
 * The calculation of the Sievert integral also includes a correction
 * for tissue attenuation.  This is done using a polynomial expansion
 * for the Sievert integral into which a polynomial model of the tissue
 * effects has been folded.  See "Computers in Radiotherapy Planning"
 * by R. G. Wood (p. 56) for a more complete description.
 *
 * The polynomial approximation of the tissue effects as presented
 * by Meisberger, et al is only good from 1 cm to 10 cm.  In this
 * routine, we assume that the approximation is good for 0 - 10 cm.
 * At distances beyond 10 cm, an exponential fit to the last few
 * cm in the polynomial fit is used.  See make_source_dat for details.
 *
 */
{
	static	float	last_cos = 2.0,
			I[TERMS + 4];
	float	temp,
		sec_theta,
		tan_theta,
		theta,
		mu_d,
		S,
		S1,
		S2,
		S3,
		cos_theta_10;
	int	loop,
		loopm1,
		loopm2;

if (debug) {
printf("Sievert(%f %f %f %f %f %f %f %f %f\n",
cos_theta, thick, b, fit_a, fit_b, fit_c, fit_d, TA_mu, TA_at_10);
}

/*
 * Calculate a table of coefficients for the expansion of the integral.
 * Try to save a little calcuating by verifying that cos_theta is different
 * from the last time around.
 *
 */
	if (cos_theta != last_cos) {
	    theta = acos(cos_theta);
	    sec_theta = 1.0/cos_theta;
	    tan_theta = tan(theta);
	    I[0] = theta;
	    I[1] = LOG(sec_theta + tan_theta);
	    I[2] = tan_theta;

	    loopm1 = 2;
	    loopm2 = 1;
	    temp = tan_theta * sec_theta;
	    for (loopm2 = 0, loopm1 = 1, loop = 3;
		 loop < (TERMS + 4);
		 loop++, loopm1++, loopm2++) {
		I[loop] = (1.0/loopm1)*(temp + loopm2*I[loopm2]);
		temp *= sec_theta;
	    }

	    last_cos = cos_theta;
	}
/*
 * Check to see whether we will use the polynomial fit to the tissue
 * attenuation curve, or the exponential fit to the polynomial fit.
 *
 */
	if (b / cos_theta <= 10.0) {
if (debug) printf("Sievert: b / cos_theta: %f <= 10.0\n", b / cos_theta);
	    temp = mu_d = -thick;
	    S = POLY(b, fit_a * I[0], fit_b * I[1], fit_c * I[2], fit_d * I[3]);
	    for (loop = 1; loop < TERMS; temp *= (mu_d / (float)(++loop))) {
		S += (temp * POLY(b, fit_a * I[loop], fit_b * I[loop + 1],
				    fit_c * I[loop + 2], fit_d * I[loop + 3]));
	    }
	}
	else {
if (debug) printf("Sievert: b / cos_theta: %f > 10.0\n", b / cos_theta);
/*
 Need to use the exponential fit for part of the range of theta.
 Proceed as follows:
 IF b <= 10.0
 1. Calculate integral from 0 to theta using exponential (S1)
 2. Find angle for which b * sec(theta) is 10.0  (theta_10)
 3. Calculate integral from 0 to theta_10 using exponential (S2)
 4. Calculate integral from 0 to theta_10 using polynomial (S3)
 5. S = S3 + (S1 - S2)

 IF b > 10.0  S = S1
*/

	    temp = mu_d = -thick - b*TA_mu;
	    S1 = I[0];
	    for (loop = 1; loop < TERMS; temp *= mu_d/(++loop)) {
		S1 += temp*I[loop];
	    }

	    if (b > 10.0) {
		S =  S1*TA_at_10*exp(10.0*TA_mu);
		return(S);
	    }

	    cos_theta_10 = b / 10.0;

/*
  Need to break up the integral.
  Update the I vector
*/
	    theta = acos(cos_theta_10);
	    sec_theta = 1.0/cos_theta_10;
	    tan_theta = tan(theta);
	    I[0] = theta;
	    I[1] = LOG(sec_theta + tan_theta);
	    I[2] = tan_theta;

	    loopm1 = 2;
	    loopm2 = 1;
	    temp = tan_theta * sec_theta;
	    for (loopm2 = 0, loopm1 = 1, loop = 3;
		 loop < TERMS + 4;
		 loop++, loopm1++, loopm2++) {
		I[loop] = (1.0/loopm1)*(temp + loopm2*I[loopm2]);
		temp *= sec_theta;
	    }

	    last_cos = cos_theta_10;

/*
  integral 0 to theta_10 using exponential
*/
	    temp = mu_d = -thick - b*TA_mu;
	    S2 = I[0];
	    for (loop = 1; loop < TERMS; temp *= (mu_d/(++loop))) {
		S2 += temp*I[loop];
	    }
	    S = S1 - S2;
if (debug) printf("S1 = %e, S2 = %e\n", S1, S2);
	    S *= TA_at_10*exp(10.0*TA_mu);

/*
  integral 0 to theta_10 using polynomial
*/
	    temp = mu_d = -thick;
	    S3 = POLY(b, fit_a*I[0], fit_b*I[1], fit_c*I[2], fit_d*I[3]);
	    for (loop = 1; loop < TERMS; temp *= mu_d/(++loop)) {
		S3 += (temp*POLY(b, fit_a*I[loop], fit_b*I[loop + 1],
				    fit_c*I[loop + 2], fit_d*I[loop + 3]));
	    }

if (debug) printf("S3 = %e\n", S3);
	    S += S3;
	}

	return(S);
}

