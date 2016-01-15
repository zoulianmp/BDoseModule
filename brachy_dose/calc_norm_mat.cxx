
#include "state.h"

void
calc_norm_mat(int object_num, GRID *norm_mat)
{
    int		num,
		source_loop,
		seed_loop,
		type,
		x_loop,
		y_loop,
		z_loop;
    PNT3D	center,
		gcenter,
		axis,
		pt,
		end0,
		end1;
    float	r_axis;


/*
  Zero out the dose_matrix area.
*/

    num = norm_mat->x_count*norm_mat->y_count*norm_mat->z_count;
    memset(norm_mat->matrix, 0, num*sizeof (float));


/*
  Compute contributions from line sources
*/
    for (source_loop = 0; source_loop < implant.source_count; source_loop++) {
	if (!objects.object[object_num].source_list[source_loop])
	    continue;
	
	type = implant.source[source_loop].type;

/*
  Transform source into coordinate system of grid.
*/
	transform_point (implant.source[source_loop].p[0],
			 norm_mat->pat_to_grid_T, &end0);
	transform_point (implant.source[source_loop].p[1],
			 norm_mat->pat_to_grid_T, &end1);
/*
  Find center coordinates of sources
*/
	center.x = (end0.x + end1.x) / 2.0;
	center.y = (end0.y + end1.y) / 2.0;
	center.z = (end0.z + end1.z) / 2.0;

/*
  Form vector from center to end 0 and calculate its magnitude
*/
	axis.x = end0.x - center.x;
	axis.y = end0.y - center.y;
	axis.z = end0.z - center.z;
	r_axis = SQRT(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

/*
  pt is vector from center to calculation point and r_pt is its
  magnitude.  pt is effectively the coordinates of the calculation
  point wrt the source center in the grid coordinate system.
*/
	pt.z = norm_mat->start.z - center.z;
	for (z_loop = 0; z_loop < norm_mat->z_count;
	     z_loop++, pt.z += norm_mat->inc.z) {
	    pt.y = norm_mat->start.y - center.y;
	    for (y_loop = 0; y_loop < norm_mat->y_count;
		 y_loop++, pt.y += norm_mat->inc.y) {
		pt.x = norm_mat->start.x - center.x;
		for (x_loop = 0; x_loop < norm_mat->x_count;
		     x_loop++, pt.x += norm_mat->inc.x) {
                    GRID_VALUE(norm_mat, x_loop, y_loop, z_loop) +=
			source_pdose(source_list + type,
				     state.exact,
				     axis.x, axis.y, axis.z, r_axis,
				     pt.x, pt.y, pt.z);
		}
	    }
	}
    }

/*
  Compute contributions from seeds...
*/
    for (seed_loop = 0; seed_loop < implant.seed_count; seed_loop++) {
	if (!objects.object[object_num].seed_list[seed_loop])
	    continue;

	type = implant.seed[seed_loop].type;
	transform_point (implant.seed[seed_loop].p,
			 norm_mat->pat_to_grid_T, &gcenter);

	pt.z = norm_mat->start.z - gcenter.z;
	for (z_loop = 0; z_loop < norm_mat->z_count;
	     z_loop++, pt.z += norm_mat->inc.z) {
	    pt.y = norm_mat->start.y - gcenter.y;
	    for (y_loop = 0; y_loop < norm_mat->y_count;
		 y_loop++, pt.y += norm_mat->inc.y) {
		pt.x = norm_mat->start.x - gcenter.x;
		for (x_loop = 0; x_loop < norm_mat->x_count;
		     x_loop++, pt.x += norm_mat->inc.x) {
                    GRID_VALUE(norm_mat, x_loop, y_loop, z_loop) +=
			seed_pdose(seed_list + type,
				   state.exact,
				   pt.x, pt.y, pt.z);
		}
	    }
	}
    }
}


