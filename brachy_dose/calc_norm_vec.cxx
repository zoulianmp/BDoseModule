
#include "state.h"

void
calc_norm_vec(int object_num, CALC_POINTS *norm_vec)
{
    int		source_loop,
		seed_loop,
		loop;
    PNT3D	center,
		axis,
		pt;
    float	r_axis;

    memset(norm_vec->dose, 0, norm_vec->count*sizeof(float));

/*
  Compute contributions from line sources
*/
    for (source_loop = 0; source_loop < implant.source_count; source_loop++) {
	if (!objects.object[object_num].source_list[source_loop]) continue;

/*
  Find center coordinates of sources
*/
	center.x = (implant.source[source_loop].p[0].x +
		    implant.source[source_loop].p[1].x) / 2.0;
	center.y = (implant.source[source_loop].p[0].y +
		    implant.source[source_loop].p[1].y) / 2.0;
	center.z = (implant.source[source_loop].p[0].z +
		    implant.source[source_loop].p[1].z) / 2.0;

/*
  Form vector from center to end 0 and calculate its magnitude
*/
	axis.x = implant.source[source_loop].p[0].x - center.x;
	axis.y = implant.source[source_loop].p[0].y - center.y;
	axis.z = implant.source[source_loop].p[0].z - center.z;
	r_axis = SQRT(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

/*
  pt is vector from center to calculation point and r_pt is its
  magnitude.  pt is effectively the coordinates of the calculation
  point wrt the source center in the grid coordinate system.
*/
	for (loop = 0; loop < norm_vec->count; loop++) {
	    pt.x = norm_vec->points[loop].p.x - center.x;
	    pt.y = norm_vec->points[loop].p.y - center.y;
	    pt.z = norm_vec->points[loop].p.z - center.z;
	    norm_vec->dose[loop] +=
		source_pdose(source_list + implant.source[source_loop].type,
			     state.exact,
			     axis.x, axis.y, axis.z, r_axis,
			     pt.x, pt.y, pt.z);
	}
    }

/*
  Compute contributions from seeds...
*/
    for (seed_loop = 0; seed_loop < implant.seed_count; seed_loop++) {
	if (!objects.object[object_num].seed_list[seed_loop]) continue;

	for (loop = 0; loop < norm_vec->count; loop++) {
	    pt.x = norm_vec->points[loop].p.x - implant.seed[seed_loop].p.x;
	    pt.y = norm_vec->points[loop].p.y - implant.seed[seed_loop].p.y;
	    pt.z = norm_vec->points[loop].p.z - implant.seed[seed_loop].p.z;

	    norm_vec->dose[loop] +=
		seed_pdose(seed_list + implant.seed[seed_loop].type,
			   state.exact,
			   pt.x, pt.y, pt.z);
	}
    }
}

