
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gen.h"
#include "libplan.h"
#include "libbrachy.h"

/*
 This routine calculates the dose to a point from a given linear source.
 It requires the source type - an index into the source specifications
 file - and the coordinates of the source end (*_axis) and the calculation
 point (*_pt) relative to the source center.

 If the source type is not the same as the previous type, the new
 source specs are read and some constants calculated.  One of those
 constants is the critical angle - that angle beyond which a ray from
 the point of calculation to some part of the source *does not* pass
 through the side wall of the capsule.  Beyond the critical angle, the
 critical angle is assumed.  While this distorts the on-axis distribution,
 it greatly simplifies the calculation.  The actual volume affected is
 relatively small.

 A second restriction on angles is the result of problems with floating
 point overflow in the terms of the polynomial for Sievert angles > 88
 degrees.  If the angle exceeds that limit, it is set to 88 degrees.

 There are two coordinate systems in use here - don't confuse them.
 The critical angle and the angles labeled *_s are the theta part of
 a polar coordinate system with its origin at the center of the source.
 The other angles are between the normal to the source and the ray from
 the calculation point to the end of the source.  That is how the Sievert
 integral is defined.

 It may help to know that I visualize the source as lying along the
 x axis.
*/

float
source_pdose(
    SOURCE_SPEC *source_spec,
    int exact,
    float x_axis,
    float y_axis,
    float z_axis,
    float r_axis, 
    float x_pt, 
    float y_pt, 
    float z_pt)
{
    static float	gamma,
			constant,
			half_len,
			cos_crit_angle,
			thick,
			fit_a,
			fit_b,
			fit_c,
			fit_d,
			TA_at_10;	/* tissue attenuation at
						the twilight zone */
    float	r_pt,		/* polar rho coordinate of calc point */
		dose,
		cos_theta_s,
		sin_theta_s,
		theta_s,
		cos_theta_1,
		cos_theta_2,
		b,	/* perp. distance from source axis to point */
		px,	/* "along" distance of intersect point of perp.  */
		S1,
		S2,
		temp_1,
		temp_2,
		fx;
	int	rad_index,
		ang_index;

    gamma = source_spec->gamma;
    constant = gamma * source_spec->R_to_r/source_spec->act_length;
    half_len = source_spec->act_length * 0.5;
    cos_crit_angle = COS(ATAN(source_spec->diameter /
			source_spec->phys_length));
    thick = source_spec->wall * source_spec->wall_mu;
    fit_a = source_spec->TA_fit[0];
    fit_b = source_spec->TA_fit[1];
    fit_c = source_spec->TA_fit[2];
    fit_d = source_spec->TA_fit[3];
    TA_at_10 = POLY(10.0, fit_a, fit_b, fit_c, fit_d);

/*
 The basic idea is:
 1. Calculate the polar coordinates of the calculation point wrt the source
	center.
 2. If the radius is nearly zero, avoid a blow-up by making it non-zero.
 3. If the angle is beyond the critical angle, set it back to the critical
	angle.
 4. Get the sine of the angle.
 5. Switch coordinate systems as follows:
    a. find the perpendicular distance from the point to the source axis
    b. find the point of intersection of that normal with the source axis
    c. find the angle between the normal and the ray from the calc point
	end # 1 of the source.
    d. if that angle is too big, set it back to 88 degrees.
    e. do the same for end #2.
*/

	r_pt = SQRT(x_pt * x_pt + y_pt * y_pt + z_pt * z_pt);
	if (r_pt < 0.0001)
	    r_pt = 0.0001;
	cos_theta_s = ABS((x_axis * x_pt) + (y_axis * y_pt) +
			(z_axis * z_pt))/(r_pt * r_axis);
	if (exact || (r_pt > source_spec->polar_radii[POLAR_RADII - 1])) {
	    if (cos_theta_s > cos_crit_angle)
		cos_theta_s = cos_crit_angle;
	    theta_s = ACOS(cos_theta_s);
	    sin_theta_s = SIN(theta_s);

	    b = r_pt * sin_theta_s;
	    px = r_pt * cos_theta_s;
	    cos_theta_1 = b / DIST(b, px + half_len);
	    if (cos_theta_1 < COS_88)
		cos_theta_1 = COS_88;
	    cos_theta_2 = b / DIST(b, px - half_len);
	    if (cos_theta_2 < COS_88)
		cos_theta_2 = COS_88;

/*
 Calculate the Sievert integrals for the two ends.

*/
	    S1 = Sievert(cos_theta_1, thick, b, fit_a, fit_b, fit_c, fit_d,
			 source_spec->mu, TA_at_10,
			 FALSE);
	    S2 = Sievert(cos_theta_2, thick, b, fit_a, fit_b, fit_c, fit_d,
			 source_spec->mu, TA_at_10,
			 FALSE);
/*
 Make sure the limits of integration are in the right order.  If the
 point lies along the source the integral is from theta1 to theta2,
 or ( 0 to theta1) + (0 to theta2).  If the point is beyond the ends
 the source, the integral is theta1 to theta2, or ( 0 to theta1) -
 ( 0 to theta2).
*/
	    if (px <= half_len) S2 *= -1.0;
	    dose = constant/b*(S1 - S2);
	}
	else {
	    theta_s = ACOS(cos_theta_s);
	    ang_index = (int) (theta_s/PI*2.0 * (POLAR_ANGLES - 1));
	    if (ang_index > (POLAR_ANGLES - 2))
		ang_index = POLAR_ANGLES - 2;
	    temp_1 = v_interp(0, POLAR_RADII, source_spec->polar_radii,
			      r_pt, source_spec->polar_table[ang_index],
			      &rad_index, &fx);
	    temp_2 = source_spec->polar_table[ang_index+1][rad_index] +
		fx*(source_spec->polar_table[ang_index+1][rad_index + 1] -
		      source_spec->polar_table[ang_index+1][rad_index]);
	    dose = interp (0, source_spec->polar_angles[ang_index],
			   source_spec->polar_angles[ang_index+1],
			   theta_s, temp_1, temp_2, &fx);
	}
    return(dose);
}

