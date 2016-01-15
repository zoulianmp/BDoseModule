
#include "defines.h"

static void rotate(float angle);

void
mirror_cb(int which, int axis)
{   int		i, j, index1, index2;
    float	tmp;
    short	*data, val;

    data = state.data + which*state.xdim*state.ydim;
    if (axis == 0) {
	for (j = 0; j < state.ydim; j++) {
	    index1 = j*state.xdim;
	    index2 = index1 + state.xdim - 1;
	    for (i = 0; i < state.xdim>>1; i++, index1++, index2--) {
		val = data[index1];
		data[index1] = data[index2];
		data[index2] = val;
	    }
	}
	/* Mirroring pts, etc. is still a work in progress */
	for (i = 0; i < state.point.count; i++) {
	    if (which == AP) {
		state.point.points[i].p.x *= -1.0;
	    }
	    else {
		state.point.points[i].p.y *= -1.0;
	    }
	}
	for (i = 0; i < state.implant.source_count; i++) {
	    if (which == AP) {
		state.implant.source[i].p[0].x *= -1.0;
		state.implant.source[i].p[1].x *= -1.0;
	    }
	    else {
		state.implant.source[i].p[0].y *= -1.0;
		state.implant.source[i].p[1].y *= -1.0;
	    }
	}

    }
    else {
	for (j = 0; j < state.ydim>>1; j++) {
	    index1 = j*state.xdim;
	    index2 = (state.ydim - 1 - j)*state.xdim;
	    for (i = 0; i < state.xdim; i++, index1++, index2++) {
		val = data[index1];
		data[index1] = data[index2];
		data[index2] = val;
	    }
	}
    }
    win[which]->need_refresh = TRUE;
    win[which]->redraw();
}

