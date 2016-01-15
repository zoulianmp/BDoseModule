
#include "defines.h"

static Fl_Window	*rotate_panel = NULL;
static Fl_Button	*accept_button, *cancel_button;
static Fl_Output	*rot_label;

static void rotate(int which, float angle);
static void rotate_image(int which);

static void
quit_cb(Fl_Button *widget)
{   int		id = widget->id();

    if (widget->id() != -1) {
	rotate_image(id);
	win[id]->need_refresh = TRUE;
    }
    rotate_panel->hide();
    state.op_flag = OP_NONE;
    win[id]->need_refresh = TRUE;
    win[id]->redraw();
}

void
rotate_cb(int which)
{   int		dim;

    if (rotate_panel == NULL) {
	rotate_panel = new Fl_Window(560, 0, 240, 20, "Rotate Image");
	rotate_panel->box(FL_DOWN_BOX);
	rot_label = new Fl_Output(0, 0, 100, 20, "");
	rot_label->value("Rotate Image");
	accept_button = new Fl_Button(100, 0, 70, 20, "Accept");
	accept_button->callback((Fl_Callback *)quit_cb);
	cancel_button = new Fl_Button(170, 0, 70, 20, "Cancel");
	cancel_button->callback((Fl_Callback *)quit_cb);
	cancel_button->id(-1);
	rotate_panel->end();
	rotate_panel->hide();
	return;
    }
    accept_button->id(which);
    rotate_panel->show();

    state.rx1 = 0;
    dim = state.xdim;
    state.rx1 = dim/5;
    state.rx2 = 4*dim/5;
    dim = state.ydim;
    state.ry1 = state.ry2 = dim/3;
    state.op_flag = OP_ROTATE;
    state.op_val = which;
    fl_cursor(FL_CURSOR_CROSS);
    win[which]->redraw();
}

void
rotate_set(int which, int x, int y)
{   int	dx, dy, d1, d2;

    dx = state.rx1 - x;
    dy = state.rx1 - x;
    d1 = dx*dx + dy*dy;
    dx = state.rx2 - x;
    dy = state.rx2 - x;
    d2 = dx*dx + dy*dy;
    if (d1 < d2) {
	state.rx1 = x;
	state.ry1 = y;
    }
    else {
	state.rx2 = x;
	state.ry2 = y;
    }
}

static void
rotate_image(int which)
{   int		i;
    float	dx, dy, angle;
    char	mess[50];

    dx = state.rx2 - state.rx1;
    dy = state.ry2 - state.ry1;
    angle = 180.0*atan2(dy, dx)/3.14159;
    fl_cursor(FL_CURSOR_DEFAULT);
    rotate(which, angle);
}

static void
rotate(int which, float angle)
{   int		i, j,
		ix, iy,
		index, loop;
    int		xdim, ydim;
    float	x, y,
		cx, cy,
		dx, dy,
		fx, fy;
    short	min, max, val;
    short	*in, *out;

    xdim = state.xdim;
    ydim = state.ydim;
    out = state.data + which*xdim*ydim;
    in = (short *)malloc(xdim*ydim*sizeof(short));
    memcpy(in, out, xdim*ydim*sizeof(short));
    memset(out, 0, xdim*ydim*sizeof(short));
    min = state.min;
    max = state.max;
    cx = 0.5*xdim;
    cy = 0.5*ydim;
    angle = 2.0*PI*angle/360.0;
    dx = cos(angle);
    dy = sin(angle);
    loop = 0;
    for (j = 0; j < ydim; j++) {
	x = dx*(-cx) + -dy*(j-cy);
	y = dy*(-cx) + dx*(j-cy);
	for (i = 0; i < xdim; i++, loop++) {
	    fx = x + cx;
	    fy = y + cy;
	    ix = (int)fx;
	    fx = fx - ix;
	    iy = (int)fy;
	    fy = fy - iy;
	    if (ix >= 0 && ix < xdim - 1 &&
		iy >= 0 && iy < ydim - 1) {
		index = ix + iy*xdim;
		val = (int)((1.0-fx)*(1.0-fy)*in[index] +
			    fx*(1.0-fy)*in[index+1] +
			    (1.0-fx)*fy*in[index+xdim] +
			    (fx)*fy*in[index+1+xdim]);
		if (val < min) val = min;
		if (val > max) val = max;
		out[loop] = val;
	    }
	    else out[loop] = min;
	    x += dx;
	    y += dy;
	}
    }
    free(in);
}


