
#include <stdio.h>
#include "plan_file_io.h"
#include "gen.h"
#include "libplanio.h"
#include "libbrachy.h"
#include "state.h"

int
output(GRID *part_ptr, CALC_POINTS *vec_ptr)
{   int		i;
    int		fdes;
    char	filename[100];

    if (state.have_grid_sum) {
	fdes = open(state.sum_file, O_RDWR | O_TRUNC | O_CREAT, 0664);
	if (fdes < 0) {
	    fprintf(stderr, "Could not open %s for output\n", state.sum_file);
	    return(1);
	}
	if (write_grid(fdes, &grid, FALSE)) {
	    fprintf(stderr, "Could not write %s\n", state.sum_file);
	    return(1);
	}
	close(fdes);
    }

    if (state.have_point_sum) {
	fdes = open(state.point_sum_file, O_RDWR | O_TRUNC | O_CREAT, 0664);
	if(fdes < 0) {
	    fprintf(stderr, "Could not open %s for output\n",
		    state.point_sum_file);
	    return(1);
	}
	if (write_points(fdes, &point, FALSE)) {
	    fprintf(stderr, "Could not write %s\n", state.point_sum_file);
	    return(1);
	}
	close(fdes);
    }

    if (state.have_partial_point) {
	for (i = 0; i < objects.count; i++) {
	    sprintf(filename, "%s.%d", state.partial_point_file, i);
	    fdes = open(filename, O_RDWR | O_TRUNC | O_CREAT, 0664);
	    if(fdes < 0) {
		return(1);
	    }
	    if (write_points(fdes, &vec_ptr[i], FALSE)) {
		return(1);
	    }
	    close(fdes);
	}
    }

    return(0);
}
	    

