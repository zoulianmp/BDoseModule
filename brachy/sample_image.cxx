
#include "defines.h"

void
sample_image(short *data_in,
	     short *data_out,
	     int in_xdim,
	     int in_ydim,
	     int x_start,
	     int x_end,
	     int y_start,
	     int y_end,
	     int slice,
	     int out_xdim,
	     int out_ydim)
{   int		i, j;
    int		ix, iy;
    int		ox, oy;
    int		xdim, ydim;
    int		index, outdex;
    int		val;
    float	x_inc, y_inc;
    float	x, y, dx, dy;

    xdim = x_end - x_start + 1;
    ydim = y_end - y_start + 1;
    x_inc = (xdim - 1.0)/(out_xdim - 1.0);
    y_inc = (ydim - 1.0)/(out_ydim - 1.0);
    outdex = slice*out_xdim*out_ydim;
    y = 0.0;
    for (j = 0; j < out_ydim; j++) {
	iy = (int)y;
	oy = iy + y_start;
	dy = y - iy;
	x = 0.0;
	for (i = 0; i < out_xdim; i++, outdex++) {
	    ix = (int)x;
	    dx = x - ix;
	    ox = ix + x_start;
	    if (ox < 0 || ox >= in_xdim-1 || oy < 0 || oy >= in_ydim-1) {
		data_out[outdex] = state.min;
		x += x_inc;
		continue;
	    }
	    index = ox + oy*in_xdim;
	    val = (int)((1.0-dx)*(1.0-dy)*data_in[index] +
			(    dx)*(1.0-dy)*data_in[index + 1] +
			(1.0-dx)*(    dy)*data_in[index + in_xdim] +
			(    dx)*(    dy)*data_in[index + 1 + in_xdim]);
	    data_out[outdex] = val;
	    x += x_inc;
	}
	y += y_inc;
    }
}


short
interp_data(short *data, int i, int j, int xdim, float *warp_data)
{   int		ix, index;
    float	x, dx;
    float	val;

    x = warp_data[i];
    ix = (int)x;
    if (ix >= xdim-1) ix = xdim - 2;
    dx = x - ix;
    index = ix + j*xdim;
    val = (1.0 - dx)*data[index] + dx*data[index+1];
    return((short)val);
}


