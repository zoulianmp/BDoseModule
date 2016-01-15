
#include "defines.h"

#define BLACK	0
#define RED	1
#define GREEN	2
#define YELLOW	3
#define BLUE	4
#define MAGENTA	5
#define CYAN	6
#define WHITE	7

static int ps_color_list[] = {
    7, 0, 1, 3, 2, 4, 5, 8  
};

static int	mouse_x, mouse_y;
static int	in_win[2];
static int	win_id = -1;
static float	cur_mx[2], cur_my[2];

static void set_grid(int which, int x, int y, int mode);
static void set_grid_plane(int which, int x, int y, int mode);
static void set_origin(int which, int x, int y);
static void set_ruler_origin(int which, int x, int y);
static void set_scale(int which, int x, int y);
static void set_seed(int which, int x, int y);
static void set_strand_seed(int which, int x, int y);
static void set_source(int which, int x, int y);
static void adj_source(int which, PNT3D pt, int grab);
static void adj_seed(int which, PNT3D pt, int grab);
static void adj_point(int which, PNT3D pt, int grab);
static void
draw_isodose(Fl_Image_Window *win, GRID *grid, float iso, COLOR col);

static void 
my_color(int col)
{
    if (state.print_flag == win_id) {
	linecolor(ps_color_list[col]);
    }
    else fl_color(col);
}

static void 
my_color3(COLOR col)
{
    if (state.print_flag == win_id) {
	linecolor3(col.r, col.g, col.b);
    }
    else fl_color(fl_color_cube(col.r*(FL_NUM_RED-1)/255,
				col.g*(FL_NUM_GREEN-1)/255,
				col.b*(FL_NUM_BLUE-1)/255));
}

static void 
my_line(int x1, int y1, int x2, int y2)
{
    if (state.print_flag == win_id) {
	line(x1, win[win_id-1]->h() - y1, x2, win[win_id-1]->h() - y2);
    }
    else fl_line(x1, y1, x2, y2);
}

static void 
my_draw_image(unsigned char *data, int x, int y, int w, int h)
{
    if (state.print_flag == win_id) {
	int	i;
	short	*sdata;
	sdata = (short *)calloc(w*h, sizeof(short));
	for (i = 0; i < w*h; i++) {
	    sdata[i] = data[i];
	}
	draw_image(0, h, w, -h, w, h, sdata, 0);
	free(sdata);
    }
    else fl_draw_image(data, 0, 0, w, h, 1, 0);
}

static void 
my_draw(char *str, int x, int y)
{
    if (state.print_flag == win_id) {
	move(x, win[win_id-1]->h() - y);
	label(str);
    }
    else fl_draw(str, x, y);
}

static void 
my_font(int font, int size)
{
    if (state.print_flag == win_id) {
	font_size(size);
    }
    else fl_font(font, size);
}

static void 
my_rect(int x, int y, int w, int h, int fill_flag)
{
    if (state.print_flag == win_id) {
	box(x, y, x+w, y+h);
	if (fill_flag) fill();
    }
    else {
	if (fill_flag) fl_rectf(x, y, w, h);
	else fl_rect(x, y, w, h);
    }
}

static void
my_scale(float sx, float sy)
{
    scale(sx, sy);
}

static void
my_label(char *str)
{
    label(str);
}

static void
transform_from_screen(int which, int ix, int iy, PNT3D *pt)
{
    if (which == AP) {
	pt->x = (ix - state.origin.x)*state.pat_scale[which];
	pt->z = (iy - state.origin.z)*state.pat_scale[which];
    }
    else {
	pt->y = (-ix - state.origin.y)*state.pat_scale[which];
	pt->z = (iy - state.origin.z)*state.pat_scale[which];
    }
}

static void
transform_to_screen(int which, PNT3D pt, int *ix, int *iy)
{   float	x, y;
    float	sfd = win[which]->sfd;

    if (which == AP) {
	x = pt.x*sfd/(pt.y + sfd);
	x = x/state.pat_scale[0] + state.origin.x;
	y = pt.z*sfd/(pt.y + sfd);
	y = y/state.pat_scale[0] + state.origin.z;
	*ix = (int)(win[which]->xo + x/win[which]->scale);
	*iy = (int)(win[which]->yo + y/win[which]->scale);
    }
    else {
	x = pt.y*sfd/(pt.x + sfd);
	x = x/state.pat_scale[1] + state.origin.y;
	y = pt.z*sfd/(pt.x + sfd);
	y = y/state.pat_scale[1] + state.origin.z;
	*ix = (int)(win[which]->xo - x/win[which]->scale);
	*iy = (int)(win[which]->yo + y/win[which]->scale);
    }
}

int
Fl_Image_Window::handle(int event)
{   int			x = Fl::event_x(), y = Fl::event_y();
    int			button = Fl::event_button();
    Fl_Image_Window	*widget = this;
    static int		old_x[2], old_y[2];
    static int		old_w = -1, old_h = -1;

    if (state.data == NULL) return(0);
    if (event == FL_MOVE || event == FL_DRAG) {
	mouse_x = x;
	mouse_y = y;
	widget->mx = (int)((x - widget->xo)*widget->scale),
	widget->my = (int)((y - widget->yo)*widget->scale);
	cur_my[id()] = widget->my - state.origin.z;
	if (id() == AP) {
	    cur_mx[id()] = widget->mx - state.origin.x;
	    cur_my[1] = cur_my[0]*(state.pat_scale[0]/state.pat_scale[1]);
	    widget->pat_pt.x = (widget->mx-state.origin.x)*state.pat_scale[0];
	    widget->pat_pt.y = state.lat_y;
	    widget->pat_pt.z = (widget->my-state.origin.z)*state.pat_scale[0];
	}
	else {
	    cur_mx[id()] = widget->mx - state.origin.y;
	    cur_my[0] = cur_my[1]*(state.pat_scale[1]/state.pat_scale[0]);
	    widget->pat_pt.x = state.ap_x;
	    widget->pat_pt.y = (-widget->mx-state.origin.y)*state.pat_scale[1];
	    widget->pat_pt.z = (widget->my-state.origin.z)*state.pat_scale[1];
	}
	if (widget->mx < 0) widget->mx = 0;
	if (widget->mx >= state.xdim) widget->mx = state.xdim-1;
	if (widget->my < 0) widget->my = 0;
	if (widget->my >= state.ydim) widget->my = state.ydim-1;
    }

    if (old_w != widget->w() || old_h != widget->h()) {
	widget->need_refresh = TRUE;
	old_w = widget->w();
	old_h = widget->h();
    }
    switch (event) {
	case FL_MOVE:
	    break;
	case FL_PUSH:
	    if (state.set_ruler && button == 1) {
	    	set_ruler_origin(id(), widget->mx, widget->my);
		break;
	    }
	    if (state.op_flag == OP_ROTATE && button == 1) {
		rotate_set(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_POINT && button == 1) {
	    	set_point(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_ADJ_POINTS && button == 1) {
	    	adj_point(id(), widget->pat_pt, TRUE);
	    }
	    if (state.op_flag == OP_ORIGIN && button == 1) {
	    	set_origin(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_SCALE && button == 1) {
	    	set_scale(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_SEED && button == 1) {
	    	set_seed(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_STRAND && button == 1) {
	    	set_strand_seed(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_SOURCE && button == 1) {
	    	set_source(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_ADJ_SOURCES && button == 1) {
	    	adj_source(id(), widget->pat_pt, TRUE);
	    }
	    if (state.op_flag == OP_ADJ_SEEDS && button == 1) {
	    	adj_seed(id(), widget->pat_pt, TRUE);
	    }
	    if (state.op_flag == OP_GRID && button == 1) {
		set_grid(id(), widget->mx, widget->my, 0);
	    }
	    if (state.op_flag == OP_GRID_PLANE && button == 1) {
		set_grid_plane(id(), widget->mx, widget->my, 0);
	    }
	    if (button == 2) {
		old_x[id()] = x;
		old_y[id()] = y;
	    }
	    break;
	case FL_DRAG:
	    if (state.op_flag == OP_ADJ_POINTS && button == 1) {
	    	adj_point(id(), widget->pat_pt, FALSE);
	    }
	    if (state.op_flag == OP_ADJ_SOURCES && button == 1) {
	    	adj_source(id(), widget->pat_pt, FALSE);
	    }
	    if (state.op_flag == OP_ADJ_SEEDS && button == 1) {
	    	adj_seed(id(), widget->pat_pt, FALSE);
	    }
	    if (state.op_flag == OP_ROTATE && button == 1) {
		rotate_set(id(), widget->mx, widget->my);
	    }
	    if (state.op_flag == OP_GRID && button == 1) {
		set_grid(id(), widget->mx, widget->my, 1);
	    }
	    if (state.op_flag == OP_GRID_PLANE && button == 1) {
		set_grid_plane(id(), widget->mx, widget->my, 1);
	    }
	    if (button == 2) {
		float scale;
		scale = state.xdim/(widget->scale*widget->w());
		widget->xo += (int)((x - old_x[id()])*scale);
		scale = state.ydim/(widget->scale*widget->h());
		widget->yo += (int)((y - old_y[id()])*scale);
		if (widget->xo > widget->w()) widget->xo = widget->w();
		if (widget->yo > widget->h()) widget->yo = widget->h();
		if (widget->xo + state.xdim/widget->scale < 0)
		    widget->xo = (int)(-state.xdim/widget->scale);
		if (widget->yo + state.ydim/widget->scale < 0)
		    widget->yo = (int)(-state.ydim/widget->scale);
		old_x[id()] = x;
		old_y[0] = y;
		old_y[1] = y;
		win[0]->need_refresh = win[1]->need_refresh = TRUE;
		win[0]->yo = win[1]->yo = widget->yo;
	    }
	    break;
	case FL_ENTER:
	    in_win[id()] = TRUE;
	    break;
	case FL_LEAVE:
	    in_win[id()] = FALSE;
	    break;
	default:
	    if (Fl_Double_Window::handle(event)) return(1);
	    return(0);
    }
    win[0]->redraw();
    win[1]->redraw();
    return(1);
}

void
Fl_Image_Window::draw()
{   int			i, j;
    int			ix, iy, ix1, iy1, ix2, iy2;
    int			xdim, ydim;
    int			xo = this->xo, yo = this->yo;
    int			index, outdex;
    int			base;
    int			val;
    int			fx, fy, findex;
    float		x, y;
    float		step;
    float		orig_x = state.origin.x;
    float		orig_y = state.origin.y;
    float		orig_z = state.origin.z;
    int			istep;
    char		str[100];
    char		command[100];
    char		*printer, *def_printer = "lpr -Pdps";
    static
    unsigned char	*data[2], *win_lookup;
    Fl_Image_Window	*widget = this;

    if (state.data == NULL) return;

    if (state.print_flag) win_id = id() + 1;
    else win_id = -1;

    xdim = state.xdim;
    ydim = state.ydim;
    istep = (int)(128.0*this->scale);
    step = istep/128.0;
    if (widget->need_refresh) {
	float		scale;
	widget->need_refresh = FALSE;
	data[id()] = (unsigned char *)
	    Realloc(data[id()], widget->w()*widget->h());
	memset(data[id()], 0, widget->w()*widget->h());
	base = (int)state.win_lo[id()];
	scale = 255.9/(state.win_hi[id()] - state.win_lo[id()]);
	win_lookup = (unsigned char *)
	    malloc((state.max - state.min + 1)*sizeof(char));
	win_lookup -= state.min;
	for (i = state.min; i <= state.max; i++) {
	    val = (int)((i - base)*scale);
	    if (val < 0) val = 0;
	    if (val > 255) val = 255;
	    win_lookup[i] = val;
	}
	outdex = 0;
	fy = -istep*yo;

	state.data += widget->id()*state.xdim*state.ydim;
	for (j = 0; j < widget->h(); j++, fy += istep) {
	    iy = fy>>7;
	    if (fy < 0) {
		outdex += widget->w();
		continue;
	    }
	    if (iy >= ydim) continue;
	    fx = -istep*xo;
	    findex = (iy<<7)*xdim - istep*xo;
	    for (i = 0; i < widget->w(); i++, outdex++) {
		ix = fx>>7;
		index = findex>>7;
		if (fx < 0) val = 0;
		else if (ix >= xdim) val = 0;
		else val = win_lookup[state.data[index]];
		data[id()][outdex] = val;
		findex += istep;
		fx += istep;
	    }
	}
	state.data -= widget->id()*state.xdim*state.ydim;
	win_lookup += state.min;
	free(win_lookup);
    }
    my_draw_image(data[id()], 0, 0, widget->w(), widget->h());

    /* Draw Axes */
    my_color(YELLOW);
    if (state.pat_scale[id()] > 0.0) {
	int	ix[2], iy[2];
	PNT3D	p1, p2;

	if (id() == AP) {
	    p1.x = -10.0; p1.y = 0.0; p1.z = 0.0;
	    p2.x =  10.0; p2.y = 0.0; p2.z = 0.0;
	}
	else {
	    p1.x = 0.0; p1.y = -10.0; p1.z = 0.0;
	    p2.x = 0.0; p2.y =  10.0; p2.z = 0.0;
	}
	transform_to_screen(id(), p1, &ix[0], &iy[0]);
	transform_to_screen(id(), p2, &ix[1], &iy[1]);
	my_line(ix[0], iy[0], ix[1], iy[1]);
	p1.x = 0.0; p1.y = 0.0; p1.z = -10.0;
	p2.x = 0.0; p2.y = 0.0; p2.z = 10.0;
	transform_to_screen(id(), p1, &ix[0], &iy[0]);
	transform_to_screen(id(), p2, &ix[1], &iy[1]);
	my_line(ix[0], iy[0], ix[1], iy[1]);
	for (i = -10; i <= 10; i++) {
	    int d;
	    if (i%5 == 0) d = 10;
	    else d = 5;
	    if (id() == AP) {
		p1.x = i; p1.y = 0.0; p1.z = 0.0;
	    }
	    else {
		p1.x = 0.0; p1.y = i; p1.z = 0.0;
	    }
	    transform_to_screen(id(), p1, &ix[0], &iy[0]);
	    my_line(ix[0], iy[0]-d, ix[0], iy[0]+d);
	    p1.x = 0; p1.y = 0; p1.z = i;
	    transform_to_screen(id(), p1, &ix[0], &iy[0]);
	    my_line(ix[0]-d, iy[0], ix[0]+d, iy[0]);
	}
    }

    my_color(RED);
    if (state.op_flag == OP_ROTATE && state.op_val == id()) {
	my_line((int)((state.rx1+0.5)/step + xo),
		(int)((state.ry1+0.5)/step + yo),
		(int)((state.rx2+0.5)/step + xo),
		(int)((state.ry2+0.5)/step + yo));
    }
    if (state.op_flag == OP_SHEAR && state.op_val == id()) {
	my_line((int)((state.rx1+0.5)/step + xo),
		(int)((state.ry1+0.5)/step + yo),
		(int)((state.rx2+0.5)/step + xo),
		(int)((state.ry2+0.5)/step + yo));
    }
    if (state.op_flag == OP_GRID) {
	GRID	*grid = &state.grid;
	PNT3D	pt;

	pt.x = grid->start.x + (grid->x_count-1)*grid->inc.x;
	pt.y = grid->start.y + (grid->y_count-1)*grid->inc.y;
	pt.z = grid->start.z + (grid->z_count-1)*grid->inc.z;
	transform_to_screen(id(), grid->start, &ix1, &iy1);
	transform_to_screen(id(), pt, &ix2, &iy2);
	my_line(ix1, iy1, ix2, iy1);
	my_line(ix2, iy1, ix2, iy2);
	my_line(ix2, iy2, ix1, iy2);
	my_line(ix1, iy2, ix1, iy1);
    }
    if (state.op_flag == OP_GRID_PLANE) {
	GRID	*grid = &state.grid;
	PNT3D	pt;

	if (id() == AP) {
	    pt.x = state.ap_x;
	    pt.y = 0.0;
	    pt.z = -100.0;
	    transform_to_screen(id(), pt, &ix1, &iy1);
	    pt.z = 100.0;
	    transform_to_screen(id(), pt, &ix2, &iy2);
	}
	else {
	    pt.x = 0.0;
	    pt.y = state.lat_y;
	    pt.z = -100.0;
	    transform_to_screen(id(), pt, &ix1, &iy1);
	    pt.z = 100.0;
	    transform_to_screen(id(), pt, &ix2, &iy2);
	}
	my_line(ix1, iy2, ix1, iy1);
    }

    my_color(RED);
    if (state.ruler_flag == id()+1) {
	int	dx, dy;
	float	fx, fy, dist;
	y = state.ruler_origin.z/state.pat_scale[id()] + orig_z;
	if (id() == AP) {
	    x = state.ruler_origin.x/state.pat_scale[id()] + orig_x;
	    ix1 = (int)(widget->xo + x/widget->scale);
	    ix2 = (int)(widget->xo + (cur_mx[id()]+orig_x)/widget->scale);
	}
	else {
	    x = state.ruler_origin.y/state.pat_scale[id()] + orig_y;
	    ix1 = (int)(widget->xo - x/widget->scale);
	    ix2 = (int)(widget->xo + (cur_mx[id()]+orig_y)/widget->scale);
	}
	iy1 = (int)(widget->yo + y/widget->scale);
	iy2 = (int)(widget->yo + (cur_my[id()]+orig_z)/widget->scale);
	my_line(ix1, iy1, ix2, iy2);
	dx = ix2 - ix1;
	dy = iy2 - iy1;
	fx = dx*widget->scale*state.pat_scale[id()];
	fy = dy*widget->scale*state.pat_scale[id()];
	dist = sqrt(fx*fx + fy*fy);
	sprintf(str, "%.2f", dist);
	my_draw(str, ix2-5, iy2-5);
	if (dist > 0.0) {
	    dx = (int)(10.0*dx/dist);
	    dy = (int)(10.0*dy/dist);
	    ix2 = ix1 + dy;
	    iy2 = iy1 - dx;
	    my_line(ix1, iy1, ix2, iy2);
	    ix2 = ix1 - dy;
	    iy2 = iy1 + dx;
	    my_line(ix1, iy1, ix2, iy2);
	}
    }

    my_color(GREEN);
    my_font(FL_COURIER, 12);
    for (i = 0; i < state.point.count; i++) {
	transform_to_screen(id(), state.point.points[i].p, &ix, &iy);
	my_draw(state.point.points[i].label, ix + 5, iy);
	my_line(ix-3, iy, ix+3, iy);
	my_line(ix, iy-3, ix, iy+3);
    }
    if (state.track_y && !state.print_flag) {
	iy = (int)(widget->yo + (cur_my[id()]+orig_z)/widget->scale);
	if (!in_win[id()] && in_win[1 - id()]) {
	    int		ix1, iy1, ix2, iy2;
	    PNT3D	pt;

	    /* Xray source */
	    if (id() == AP) {
		pt.y = pt.z = 0.0;
		pt.x = -widget->sfd;
	    }
	    else {
		pt.x = pt.z = 0.0;
		pt.y = -widget->sfd;
	    }
	    transform_to_screen(id(), pt, &ix1, &iy1);

	    /* Current Pat Point (other window) */
	    if (id() == AP) {
		pt = win[LAT]->pat_pt;
	    }
	    else {
		pt = win[AP]->pat_pt;
	    }
	    transform_to_screen(id(), pt, &ix2, &iy2);

	    /* Double the line length and draw */
	    ix2 = ix2 + (ix2 - ix1);
	    iy2 = iy2 + (iy2 - iy1);
	    my_line(ix1, iy1, ix2, iy2);
	}
    }
    if (state.show_nearest_seed && in_win[id()]) {
	int min_i = -1;
	float dist, min_dist = 100000;
	for (i = 0; i < state.implant.seed_count; i++) {
	    transform_to_screen(id(), state.implant.seed[i].p, &ix, &iy);
	    dist = (ix - mouse_x)*(ix - mouse_x) +
		   (iy - mouse_y)*(iy - mouse_y);
	    if (min_dist > dist) {
		state.nearest_seed = i;
		min_dist = dist;
	    }
	}
    }
    for (i = 0; i < state.implant.seed_count; i++) {
	if (state.show_nearest_seed && i == state.nearest_seed) {
	    my_color(YELLOW);
	}
	else my_color(CYAN);
	transform_to_screen(id(), state.implant.seed[i].p, &ix, &iy);
	my_line(ix-3, iy, ix+3, iy);
	my_line(ix, iy-3, ix, iy+3);
    }
    my_color(MAGENTA);
    for (i = 0; i < state.implant.source_count; i++) {
	int	ix[2], iy[2];
	float	dx, dy, dz, len;
	PNT3D	*pt = &state.implant.source[i].p[0];

	for (j = 0; j < 2; j++) {
	  transform_to_screen(id(), pt[j], &ix[j], &iy[j]);
	  my_line(ix[j]-3, iy[j], ix[j]+3, iy[j]);
	  my_line(ix[j], iy[j]-3, ix[j], iy[j]+3);
	}
	my_line(ix[0], iy[0], ix[1], iy[1]);
	dx = pt[1].x - pt[0].x;
	dy = pt[1].y - pt[0].y;
	dz = pt[1].z - pt[0].z;
	len = sqrt(dx*dx + dy*dy + dz*dz);
	if (state.report_src_len) sprintf(str, "Src %d %.1f", i+1, len);
	else sprintf(str, "Src %d", i+1);
	my_draw(str, ((ix[0]+ix[1]) >> 1) - 16, (iy[0] + iy[1])>>1);
    }
    my_color(YELLOW);
    if (state.op_flag == OP_ORIGIN) {
	PNT3D	*pt = &state.origin;
	if (id() == AP) {
	    ix = (int)(widget->xo + pt->x/widget->scale);
	    iy = (int)(widget->yo + pt->z/widget->scale);
	}
	else {
	    ix = (int)(widget->xo - pt->y/widget->scale);
	    iy = (int)(widget->yo + pt->z/widget->scale);
	}
	my_draw("O", ix + 5, iy);
	my_line(ix-3, iy, ix+3, iy);
	my_line(ix, iy-3, ix, iy+3);
    }

    if (state.dose_grid_computed && show_iso->value()) {
	for (i = 0; i < state.num_isodose; i++) {
	    if (state.isodose[i] <= 0.0) continue;
	    draw_isodose(widget, &state.grid, state.isodose[i],
			 state.isodose_color[i]);
	}
    }

    ix = widget->mx;
    iy = widget->my;
    if (ix < 0 || ix >= xdim || iy < 0 || iy >= ydim) return;
    index = ix + iy*xdim;

    if (id() == AP) {
	PNT3D	pat_pt, grid_pt;
	float	dose = 0.0;
	pat_pt.x = (ix - state.origin.x)*state.pat_scale[0];
	pat_pt.y = state.lat_y;
	pat_pt.z = (iy - state.origin.z)*state.pat_scale[0];
	pat_pt = win[AP]->pat_pt;
	transform_point(pat_pt, state.grid.pat_to_grid_T, &grid_pt);
	dose = sample_grid(&state.grid, &grid_pt);
	sprintf(str, "%4d %4d: %4d (%.1f %.1f %.1f): %.2f",
		ix, iy, state.data[index],
		pat_pt.x, pat_pt.y, pat_pt.z, dose);
    }
    else {
	PNT3D	pat_pt, grid_pt;
	float	dose = 0.0;
	pat_pt.x = state.ap_x;
	pat_pt.y = (-ix - state.origin.y)*state.pat_scale[1];
	pat_pt.z = (iy - state.origin.z)*state.pat_scale[1];
	pat_pt = win[LAT]->pat_pt;
	transform_point(pat_pt, state.grid.pat_to_grid_T, &grid_pt);
	dose = sample_grid(&state.grid, &grid_pt);
	sprintf(str, "%4d %4d: %4d (%.1f %.1f %.1f): %.2f",
		ix, iy, state.data[index],
		pat_pt.x, pat_pt.y, pat_pt.z, dose);
    }
    my_color(BLACK);
    if (!state.print_flag) {
	my_rect(5, widget->h() - 16, 7*strlen(str), 14, TRUE);
	my_color(GREEN);
	my_draw(str, 5, widget->h() - 4);
    }
    if (state.print_flag == win_id) {
	clip_region(-10000, -10000, 10000, 10000);
	my_line(0, 0, w(), 0);
	my_line(w(), 0, w(), h());
	my_line(w(), h(), 0, h());
	my_line(0, h(), 0, 0);
	for (i = 0; i < state.num_isodose; i++) {
	    my_color3(state.isodose_color[i]);
	    my_rect(50*i, -20, 50, 20, TRUE);
	    my_color(BLACK);
	    my_rect(50*i, -20, 50, 20, FALSE);
	    move(50*i + 18, -14);
	    my_label((char *)(iso_input[i]->label()));
	}
	if (win_id == 1) sprintf(str, "Y = %.1f", state.lat_y);
	else sprintf(str, "X = %.1f", state.ap_x);
	move(300, -40);
	my_label(str);
	if (win_id == 1) {
	    win_id = 2;
	    state.print_flag = 2;
	    translate(750, 0);
	    clip_region(0, 0, win[0]->w(), win[0]->h());
	    win[1]->redraw();
	}
	else {
	    closepl();
	    printer = getenv("COLOR_PS_PRINTER");
	    if (printer == NULL) printer = def_printer;
	    sprintf(command, "%s output.ps", printer);
	    printf("system: %s\n", command);
	    system(command);
	    unlink("output.ps");
	    state.print_flag = 0;
	    win_id = -1;
	}
    }
    if (state.circle_flag && in_win[id()]) {
	int		i, j, num = 40;
	int		ix, iy;
	float		scale;
	int		x1, y1, x2, y2;

	if (id() == AP) {
	    ix = (int)(widget->xo + (cur_mx[id()]+orig_x)/widget->scale);
	}
	else {
	    ix = (int)(widget->xo + (cur_mx[id()]+orig_y)/widget->scale);
	}
	iy = (int)(widget->yo + (cur_my[id()]+orig_z)/widget->scale);
	scale = state.circle_radius/(state.pat_scale[id()]*this->scale);
	for (i = 0, j = num-1; i < num; i++) {
	    x1 = ix + (int)(scale*sin(i*2.0*PI/num));
	    y1 = iy + (int)(scale*cos(i*2.0*PI/num));
	    x2 = ix + (int)(scale*sin(j*2.0*PI/num));
	    y2 = iy + (int)(scale*cos(j*2.0*PI/num));
	    my_line(x1, y1, x2, y2);
	    if (++j == num) j = 0;
	}
    }
}

/* Used by record_point() and merge_points() */
static PNT3D	t1, t2;

void
record_point(int which, int x, int y)
{

    if (which == AP) transform_from_screen(which, x, y, &t1);
    else transform_from_screen(which, x, y, &t2);
}

void
merge_points(int which, PNT3D *final)
{   float	x1 = t1.x, y1 = t1.z;
    float	x2 = t2.y, y2 = t2.z;
    float	s1 = win[0]->sfd, s2 = win[1]->sfd;

    /* Solving the simultaneous projection equations gives */
    final->y = (s1*x1*x2 + s1*s2*x2)/(s1*s2 - x1*x2);
    final->x = (s1*x1 + x1*final->y)/s1;
    final->z = (s1*y1 + y1*final->y)/s1;
}

void
set_point(int which, int x, int y)
{   PNT3D  *pt = &state.point.points[state.point.count-1].p;

    record_point(which, x, y);
    if (which == AP) {
	state.op_val &= 0x2;
	*pt = t1;
    }
    else {
	state.op_val &= 0x1;
	*pt = t2;
    }
    if (state.op_val == 0) {
	merge_points(which, pt);
	add_point();
    }
}

static void
set_origin(int which, int x, int y)
{   int		i, dy;
    PNT3D	*pt = &state.origin;
    short	*data;
    static int	y1, y2;

    if (which == AP) {
    	pt->x = x;
	pt->z = y1 = y;
	state.op_val &= 0x2;
    }
    else {
    	pt->y = -x;
	pt->z = y2 = y;
	state.op_val &= 0x1;
    }
    if (state.op_val == 0) state.op_flag = OP_NONE;
    if (state.op_flag == OP_NONE) {
	/* Shift win[1] to align vertically with win[0] */
	data = state.data + state.xdim*state.ydim;
    	pt->z = y1;
	dy = y2 - y1;
	if (dy > 0) {
	    for (i = 0; i < state.ydim; i++) {
		if (i < state.ydim - dy)
		    memcpy(data + i*state.xdim,
			   data + (i+dy)*state.xdim,
			   state.xdim*sizeof(short));
		else
		    memset(data + i*state.xdim,
			   state.min, state.xdim*sizeof(short));
	    }
	}
	else if (dy < 0) {
	    dy = -dy;
	    for (i = state.ydim-1; i >= 0; i--) {
		if (i > dy)
		    memcpy(data + i*state.xdim,
			   data + (i-dy)*state.xdim,
			   state.xdim*sizeof(short));
		else
		    memset(data + i*state.xdim,
			   state.min, state.xdim*sizeof(short));
	    }
	}
	state.op_val = 4;
    }
    win[1]->need_refresh = TRUE;
}

static void
set_ruler_origin(int which, int x, int y)
{   int		i, dy;
    PNT3D	*pt = &state.ruler_origin;
    short	*data;

    transform_from_screen(which, x, y, pt);
    state.set_ruler = FALSE;
    state.ruler_flag = which + 1;
}

static void
set_scale(int which, int x, int y)
{   float	dx, dy, dist;
    static int	xo, yo;

    if (state.op_val == 2) {
	xo = x;
	yo = y;
    }
    else {
	dx = x - xo;
	dy = y - yo;
	dist = sqrt(dx*dx + dy*dy);
	state.pat_scale[which] = state.pat_ruler/dist;
    }
    state.op_val--;
    if (state.op_val == 0) state.op_flag = OP_NONE;
}

static void
set_seed(int which, int x, int y)
{   int			num;
    IMPLANT_DESC	*im = &state.implant;
    SEED		*seed = &im->seed[im->seed_count-1];
    static int		inum[2];

    record_point(which, x, y);
    if (which == AP) {
	inum[AP] = 1;
	seed->p = t1;
    }
    if (which == LAT) {
	inum[LAT] = 1;
	seed->p = t2;
    }
    if (inum[AP] >= 1 && inum[LAT] >= 1) {
	inum[AP] = inum[LAT] = 0;
	merge_points(which, &seed->p);
	add_seed();
    }
}

static void
set_strand_seed(int which, int x, int y)
{   int			num;
    IMPLANT_DESC	*im = &state.implant;
    SEED		*seed;
    static int		inum[2];

    if (which == AP) {
	seed = &im->seed[im->seed_count-inum[AP]-1];
	inum[AP]++;
	transform_from_screen(which, x, y, &seed->p);
    }
    if (which == LAT) {
	seed = &im->seed[im->seed_count-inum[LAT]-1];
	inum[LAT]++;
	transform_from_screen(which, x, y, &seed->p);
    }
    if (inum[AP] == state.strand_count && inum[LAT] == state.strand_count) {
	inum[AP] = inum[LAT] = 0;
	state.op_flag = OP_NONE;
    }
}

static void
set_source(int which, int x, int y)
{   int			num;
    IMPLANT_DESC	*im = &state.implant;
    SOURCE		*source = &im->source[im->source_count-1];
    static int		inum[2];

    num = inum[which];
    transform_from_screen(which, x, y, &source->p[num]);
    if (which == AP && inum[AP] < 2) {
	num = inum[AP];
	if (num == 0) {
	    source->p[1].x = source->p[0].x;
	    source->p[1].z = source->p[0].z;
	}
	inum[AP]++;
    }
    if (which == LAT && inum[LAT] < 2) {
	num = inum[LAT];
	inum[LAT]++;
    }
    if (inum[AP] == 2 && inum[LAT] == 2) {
	inum[AP] = inum[LAT] = 0;
	state.op_flag = OP_NONE;
    }
}

static void
adj_point(int which, PNT3D p1, int grab)
{   int			i;
    int			ix, iy;
    float		dx, dy, dist, min_dist;
    PNT3D		*p2;
    IMPLANT_DESC	*im = &state.implant;
    static int		min_i;

    min_dist = 1.0;
    for (i = 0; i < state.point.count; i++) {
	if (!grab) {
	    min_dist = 0.0;
	    break;
	}
	p2 = &state.point.points[i].p;
	    if (which == AP) {
		dx = p1.x - p2->x;
		dy = p1.z - p2->z;
	    }
	    else {
		dx = p1.y - p2->y;
		dy = p1.z - p2->z;
	    }
	    dist = dx*dx + dy*dy;
	    if (min_dist > dist) {
		min_i = i;
		min_dist = dist;
	    }
    }
    if (min_dist < 1.0) {
	if (which == AP) {
	    state.point.points[min_i].p.x = p1.x;
	    state.point.points[min_i].p.z = p1.z;
	}
	else {
	    state.point.points[min_i].p.y = p1.y;
	    state.point.points[min_i].p.z = p1.z;
	}
    }
}

static void
adj_source(int which, PNT3D p1, int grab)
{   int			i, j;
    int			ix, iy;
    float		dx, dy, dist, min_dist;
    PNT3D		*p2;
    IMPLANT_DESC	*im = &state.implant;
    static int		min_i, min_j;

    min_dist = 1.0;
    for (i = 0; i < im->source_count; i++) {
	if (!grab) {
	    min_dist = 0.0;
	    break;
	}
	for (j = 0; j < 2; j++) {
	    p2 = &im->source[i].p[j];
	    if (which == AP) {
		dx = p1.x - p2->x;
		dy = p1.z - p2->z;
	    }
	    else {
		dx = p1.y - p2->y;
		dy = p1.z - p2->z;
	    }
	    dist = dx*dx + dy*dy;
	    if (min_dist > dist) {
		min_i = i;
		min_j = j;
		min_dist = dist;
	    }
	}
    }
    if (min_dist < 1.0) {
	if (which == AP) {
	    im->source[min_i].p[min_j].x = p1.x;
	    im->source[min_i].p[min_j].z = p1.z;
	}
	else {
	    im->source[min_i].p[min_j].y = p1.y;
	    im->source[min_i].p[min_j].z = p1.z;
	}
    }
}

static void
adj_seed(int which, PNT3D p1, int grab)
{   int			i, j;
    int			ix, iy;
    float		dx, dy, dist, min_dist;
    PNT3D		*p2;
    IMPLANT_DESC	*im = &state.implant;
    static int		min_i, min_j;

    min_dist = 1.0;
    for (i = 0; i < im->seed_count; i++) {
	if (!grab) {
	    min_dist = 0.0;
	    break;
	}
	p2 = &im->seed[i].p;
	if (which == AP) {
	    dx = p1.x - p2->x;
	    dy = p1.z - p2->z;
	}
	else {
	    dx = p1.y - p2->y;
	    dy = p1.z - p2->z;
	}
	dist = dx*dx + dy*dy;
	if (min_dist > dist) {
	    min_i = i;
	    min_dist = dist;
	}
    }
    if (min_dist < 1.0) {
	if (which == AP) {
	    im->seed[min_i].p.x = p1.x;
	    im->seed[min_i].p.z = p1.z;
	}
	else {
	    im->seed[min_i].p.y = p1.y;
	    im->seed[min_i].p.z = p1.z;
	}
    }
}

static void
set_grid(int which, int x, int y, int mode)
{
    GRID	*grid = &state.grid;
    PNT3D	pt;
    float	fx, fy;
    float	dx1, dy1, dx2, dy2;
    float	old;
    static int	grab;

    transform_from_screen(which, x, y, &pt);
    if (which == AP) {
	fx = pt.x;
	fy = pt.z;
	if (mode == 0) {
	    dx1 = fabs(grid->start.x - fx);
	    dx2 = fabs(grid->start.x + (grid->x_count-1)*grid->inc.x - fx);
	    dy1 = fabs(grid->start.z - fy);
	    dy2 = fabs(grid->start.z + (grid->z_count-1)*grid->inc.z - fy);
	    if (dx1 < dx2 && dx1 < dy1 && dx1 < dy2) grab = 0;
	    if (dx2 < dx1 && dx2 < dy1 && dx2 < dy2) grab = 1;
	    if (dy1 < dx1 && dy1 < dx2 && dy1 < dy2) grab = 2;
	    if (dy2 < dx1 && dy2 < dx2 && dy2 < dy1) grab = 3;
	}
	if (grab == 0) {
	    old = grid->start.x + (grid->x_count - 1)*grid->inc.x;
	    grid->start.x = fx;
	    grid->inc.x = (old - grid->start.x)/(grid->x_count - 1);
	}
	if (grab == 1) grid->inc.x = (fx - grid->start.x)/(grid->x_count - 1);
	if (grab == 2) {
	    old = grid->start.z + (grid->z_count - 1)*grid->inc.z;
	    grid->start.z = fy;
	    grid->inc.z = (old - grid->start.z)/(grid->z_count - 1);
	}
	if (grab == 3) grid->inc.z = (fy - grid->start.z)/(grid->z_count - 1);
    }
    else {
	fx = pt.y;
	fy = pt.z;
	if (mode == 0) {
	    dx1 = fabs(grid->start.y - fx);
	    dx2 = fabs(grid->start.y + (grid->y_count-1)*grid->inc.y - fx);
	    dy1 = fabs(grid->start.z - fy);
	    dy2 = fabs(grid->start.z + (grid->z_count-1)*grid->inc.z - fy);
	    if (dx1 < dx2 && dx1 < dy1 && dx1 < dy2) grab = 0;
	    if (dx2 < dx1 && dx2 < dy1 && dx2 < dy2) grab = 1;
	    if (dy1 < dx1 && dy1 < dx2 && dy1 < dy2) grab = 2;
	    if (dy2 < dx1 && dy2 < dx2 && dy2 < dy1) grab = 3;
	}
	if (grab == 0) {
	    old = grid->start.y + (grid->y_count - 1)*grid->inc.y;
	    grid->start.y = fx;
	    grid->inc.y = (old - grid->start.y)/(grid->y_count - 1);
	}
	if (grab == 1) grid->inc.y = (fx - grid->start.y)/(grid->y_count - 1);
	if (grab == 2) {
	    old = grid->start.z + (grid->z_count - 1)*grid->inc.z;
	    grid->start.z = fy;
	    grid->inc.z = (old - grid->start.z)/(grid->z_count - 1);
	}
	if (grab == 3) grid->inc.z = (fy - grid->start.z)/(grid->z_count - 1);
    }
}

static void
set_grid_plane(int which, int x, int y, int mode)
{
    GRID	*grid = &state.grid;
    PNT3D	pt;
    float	fx, fy;
    float	dx1, dy1, dx2, dy2;
    float	old;
    static int	grab;

    transform_from_screen(which, x, y, &pt);
    if (which == AP) {
	state.ap_x = pt.x;
    }
    else {
	state.lat_y = pt.y;
    }
}

static void
draw_isodose(Fl_Image_Window *win, GRID *grid3, float iso, COLOR col)
{   int		i, j, index;
    int		id = win->id();
    int		x_dim, y_dim;
    int		ix, iy;
    int		hit;
    int		first = TRUE;
    float	x_step, y_step;
    float	x_start;
    float	v1, v2, v3, v4;
    float	*dose;
    float	grid3_to_grid2[4][4];
    float	det;
    PNT3D	p1, p2, p[2];
    GRID	*grid, grid2;
    char	iso_str[10];

    if (iso <= 0.0) return;
    sprintf(iso_str, "%.0f", iso);
    my_color3(col);

    grid = &grid2;
    memset(grid, 0, sizeof(GRID));
    grid->y_count = grid3->z_count;
    grid->z_count = 1;
    grid->start.y = grid3->start.z;
    grid->start.z = 0.0;
    grid->inc.y = grid3->inc.z;
    grid->inc.z = 0.0;
    memset(grid3_to_grid2, 0, sizeof(grid3_to_grid2));
    grid3_to_grid2[3][3] = 1.0;
    if (id == AP) {
	grid->x_count = grid3->x_count;
	grid->start.x = grid3->start.x;
	grid->inc.x = grid3->inc.x;
	grid3_to_grid2[0][0] = 1.0;
	grid3_to_grid2[2][1] = 1.0;
	grid3_to_grid2[1][2] = 1.0;
	grid3_to_grid2[3][2] = -state.lat_y;
    }
    else {
	grid->x_count = grid3->y_count;
	grid->start.x = grid3->start.y;
	grid->inc.x = grid3->inc.y;
	grid3_to_grid2[1][0] = -1.0;
	grid3_to_grid2[2][1] = 1.0;
	grid3_to_grid2[0][2] = 1.0;
	grid3_to_grid2[3][2] = -state.ap_x;
    }
    new_grid_slicer(grid3, grid3_to_grid2,
		    grid->inc.x, grid->inc.y, grid, TRUE);
    x_dim = grid->x_count;
    y_dim = grid->y_count;
    dose = grid->matrix;
    x_step = grid->inc.x/state.pat_scale[id]/win->scale;
    y_step = grid->inc.y/state.pat_scale[id]/win->scale;

    if (id == AP) {
	p1.x = grid->start.x;
	p1.y = 0.0;
	p1.z = grid->start.y;
    }
    else {
	p1.x = 0.0;
	p1.y = -grid->start.x;
	p1.z = grid->start.y;
    }
    transform_to_screen(id, p1, &ix, &iy);

    x_start = ix;

    p1.y = iy;
    for (j = 0; j < y_dim-2; j++) {
	index = j*x_dim;
	p1.x = ix;
	for (i = 0; i < x_dim-2; i++, index++) {
	    v1 = dose[index];
	    v2 = dose[index+1];
	    v3 = dose[index+1+x_dim];
	    v4 = dose[index+x_dim];
	    hit = 0;
	    if ((v1 <= iso && v2 > iso) || (v1 > iso && v2 <= iso)) {
		p[hit].x = p1.x + x_step*(iso-v1)/(v2-v1);
		p[hit].y = p1.y;
		hit++; 
	    }
	    if ((v2 <= iso && v3 > iso) || (v2 > iso && v3 <= iso)) {
		p[hit].x = p1.x + x_step;
		p[hit].y = p1.y + y_step*(iso-v2)/(v3-v2);
		hit++; 
	    }
	    if (hit == 2) {
		my_line((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y);
		if (first) my_draw(iso_str, (int)p[0].x, (int)p[0].y);
		first = FALSE;
		hit = 0;
	    }
	    if ((v3 <= iso && v4 > iso) || (v3 > iso && v4 <= iso)) {
		p[hit].x = p1.x + x_step - x_step*(iso-v3)/(v4-v3);
		p[hit].y = p1.y + y_step;
		hit++; 
	    }
	    if (hit == 2) {
		my_line((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y);
		if (first) my_draw(iso_str, (int)p[0].x, (int)p[0].y);
		first = FALSE;
		hit = 0;
	    }
	    if ((v4 <= iso && v1 > iso) || (v4 > iso && v1 <= iso)) {
		p[hit].x = p1.x;
		p[hit].y = p1.y + y_step - y_step*(iso-v4)/(v1-v4);
		hit++; 
	    }
	    if (hit == 2) {
		my_line((int)p[0].x, (int)p[0].y, (int)p[1].x, (int)p[1].y);
		if (first) my_draw(iso_str, (int)p[0].x, (int)p[0].y);
		first = FALSE;
		hit = 0;
	    }
	    p1.x += x_step;
	}
	p1.y += y_step;
    }
    free(grid->matrix);
}

void
show_iso_cb(Fl_Light_Button *widget)
{
    redraw_windows();
}

void
redraw_windows()
{
    win[0]->redraw();
    win[1]->redraw();
}

