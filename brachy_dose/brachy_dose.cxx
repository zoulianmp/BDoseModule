
#define MAIN

#include "state.h"

int
main(int argc, char **argv)
{
    int		loop, object_loop, matrix_size, vector_size;
    float	*wts;
    GRID	*part_grid;
    CALC_POINTS	*part_point;

    spittime(NULL);
    setup(argc, argv);

/*
  Allocate space for partial dose matrices.
*/
    if (state.have_grid) {
	matrix_size = (grid.x_count*grid.y_count*grid.z_count*sizeof(float));
	part_grid = (GRID *) calloc(objects.count, sizeof(GRID));
	if (part_grid == NULL) {
	    fprintf(stderr, "brachy_dose: cannot malloc\n");
	    exit(1);
	}
	    
	for (object_loop = 0; object_loop < objects.count; object_loop++) {
	    /* copy grid description into partial grid buffers */
	    part_grid[object_loop] = grid; 
	    part_grid[object_loop].matrix = (float *)malloc(matrix_size);
	    if (part_grid[object_loop].matrix == NULL) {
		fprintf(stderr, "brachy_dose: cannot malloc\n");
		exit(1);
	    }
	}
    }

    if (state.have_point) {
	vector_size = (point.count*sizeof(float));
	part_point = (CALC_POINTS *)
		     calloc(objects.count, sizeof(CALC_POINTS));
	    
	for (object_loop = 0; object_loop < objects.count; object_loop++) {
	    part_point[object_loop] = point; 
	    part_point[object_loop].dose = (float *)malloc(vector_size);
	    if (part_point[object_loop].dose == NULL) {
		fprintf(stderr, "brachy_dose: cannot malloc\n");
		exit(1);
	    }
	}
    }

    if (state.have_grid_sum || state.have_point_sum) {
	wts = (float *)calloc(objects.count, sizeof(float));
	if (wts == NULL) {
	    fprintf(stderr, "brachy_dose: cannot malloc\n");
	    exit(1);
	}
     }
	    
    for (object_loop = 0; object_loop < objects.count; object_loop++) {
	if (state.have_grid)
	    calc_norm_mat(object_loop, &part_grid[object_loop]);
	if (state.have_point)
	    calc_norm_vec(object_loop, &part_point[object_loop]);

/*
  If we are going to sum later, store weights away now
*/
	if (state.have_grid_sum || state.have_point_sum) {
	    wts[object_loop] =
		weight.weights[object_loop].f.brachy.time*
		weight.weights[object_loop].f.brachy.activity;
	}
    }

    if (state.have_grid) {
	for (object_loop = 0; object_loop < objects.count; object_loop++) {
	    grid_min_max(&part_grid[object_loop]);
	}
    }
    
/*
  Do weighted sum if requested
*/
    if (state.have_grid_sum) {
	if (state.have_grid) {
	    sum_mat(objects.count, wts, part_grid, &grid);
	    grid_min_max(&grid);
	}
    }
    if (state.have_point_sum) {
	if (state.have_point) {
	    sum_vec(objects.count, wts, part_point, &point);
	}
    }

    spittime("done");
    output(part_grid, part_point);
    return(0);
}

