
#include "defines.h"

static void
pat_name_cb(Fl_Input *widget)
{
    strcpy(state.pat_name, widget->value());
}

int
make_windows(int argc, char **argv)
{   int		i;
    char	*ptr;
    static char pat_name[100];

    Fl::visual(FL_RGB);
    sprintf(pat_name, "XBrachy: %s", state.pat_name);
    main_panel = new Fl_Window(20, 20, 1242, 940, pat_name);

    options = new Fl_Button(0, 0, 80, 20, "OPTIONS");
    options->callback((Fl_Callback *)options_cb);

    ptr = (char *)calloc(20, sizeof(char));
    sprintf(ptr, "Circle: %.1f", state.circle_radius);
    circle_button = new Fl_Light_Button(20, 20, 120, 20, ptr);
    circle_button->value(0);
    circle_button->callback((Fl_Callback *)circle_cb);
    circle_dec = new Fl_Button(0, 20, 20, 20, "<");
    circle_dec->callback((Fl_Callback *)circle_size_cb);
    circle_dec->id(0);
    circle_inc = new Fl_Button(140, 20, 20, 20, ">");
    circle_inc->callback((Fl_Callback *)circle_size_cb);
    circle_inc->id(1);

    image_button[0] = new Fl_Button(520, 20, 100, 20, "AP IMAGE");
    image_button[0]->id(AP);
    image_button[0]->callback((Fl_Callback *)image_button_cb);

    image_button[1] = new Fl_Button(620, 20, 100, 20, "LAT IMAGE");
    image_button[1]->id(LAT);
    image_button[1]->callback((Fl_Callback *)image_button_cb);

    spread_sheet = new Fl_Button(80, 0, 80, 20, "Spread");
    spread_sheet->callback((Fl_Callback *)spread_sheet_cb);

    seed_button = new Fl_Button(160, 0, 200, 20, "");
    seed_button->callback((Fl_Callback *)seed_button_cb);
    ptr = (char *)calloc(100, sizeof(char));
    sprintf(ptr, "Seed: %s",
	    state.seed_list[state.seed_num].isotope);
    seed_button->label(ptr);

    source_button = new Fl_Button(360, 0, 200, 20, "");
    source_button->callback((Fl_Callback *)source_button_cb);
    ptr = (char *)calloc(100, sizeof(char));
    sprintf(ptr, "Source: %s",
	    state.source_list[state.source_num].isotope);
    source_button->label(ptr);

    win[0] = new Fl_Image_Window(20, 40, 600, 900);
    win[0]->id(0);
    win[0]->scale = ((float)state.ydim)/win[0]->h();
    if (win[0]->scale < ((float)state.xdim)/win[0]->w())
	win[0]->scale = ((float)state.xdim)/win[0]->w();
    win[0]->end();
    win[1] = new Fl_Image_Window(622, 40, 600, 900);
    win[1]->scale = ((float)state.ydim)/win[1]->h();
    win[1]->id(1);
    if (win[1]->scale < ((float)state.xdim)/win[1]->w())
	win[1]->scale = ((float)state.xdim)/win[1]->w();
    win[1]->end();

    win[0]->sfd = state.header.per_scan[0].z_position;
    win[1]->sfd = state.header.per_scan[1].z_position;

    greybar[AP] = new Fl_Double_Slider(0, 40, 20, 900);
    greybar[AP]->id(AP);
    greybar[AP]->type(FL_VERT_FILL_SLIDER);
    greybar[AP]->selection_color(1);
    greybar[AP]->callback((Fl_Callback *)greybar_cb);

    greybar[LAT] = new Fl_Double_Slider(main_panel->w()-20, 40, 20, 900);
    greybar[LAT]->id(LAT);
    greybar[LAT]->type(FL_VERT_FILL_SLIDER);
    greybar[LAT]->selection_color(1);
    greybar[LAT]->callback((Fl_Callback *)greybar_cb);

    zoom = new Fl_Value_Scrollbar(main_panel->w()-300, 0, 300, 20, "Zoom");
    zoom->minimum(1.0/win[0]->scale); zoom->maximum(2.0);
    zoom->value(1.0/win[0]->scale); zoom->Default(1.0/win[0]->scale);
    zoom->step(0.01);
    zoom->callback((Fl_Callback *)zoom_cb);
    zoom->align(FL_ALIGN_LEFT | !FL_ALIGN_INSIDE);
    zoom_cb(zoom);

    for (i = 0; i < state.num_isodose; i++) {
	COLOR		col;
	Fl_Color	color;
	ptr = (char *)calloc(10, sizeof(char));
	sprintf(ptr, "%.0f", state.isodose[i]);
	show_iso = new Fl_Light_Button(772, 20, 50, 20, "Isos");
	show_iso->value(1);
	show_iso->callback((Fl_Callback *)show_iso_cb);
	iso_input[i] = new Fl_Button(822+i*40, 20, 40, 20, ptr);
	iso_input[i]->id(i);
	iso_input[i]->callback((Fl_Callback *)iso_val_cb);
	col = state.isodose_color[i];
	color = fl_color_cube(col.r*(FL_NUM_RED-1)/255,
			      col.g*(FL_NUM_GREEN-1)/255,
			      col.b*(FL_NUM_BLUE-1)/255);
	iso_input[i]->color(color);
    }
    rotate_cb(0);
    set_grid_cb();
    set_grid_plane_cb();

    main_panel->end();
    main_panel->show();

    if (is_file("window")) {
    	float	lo1, lo2, hi1, hi2, zval;
	FILE	*fp;

	fp = fopen("window", "r");
	fscanf(fp, "%f %f %f %f %f", &lo1, &hi1, &lo2, &hi2, &zval);
	fclose(fp);
	greybar[AP]->value(lo1);
	greybar[AP]->value2(hi1);
	greybar[LAT]->value(lo2);
	greybar[LAT]->value2(hi2);
	zoom->value(zval);
	greybar_cb(greybar[AP]);
	greybar_cb(greybar[LAT]);
	zoom_cb(zoom);
    }

    return(0);
}

void
greybar_cb(Fl_Double_Slider *widget)
{   int		id = widget->id();
    float	scale;

    scale = state.max - state.min;
    state.win_lo[id] = (scale*(1.0-widget->value2()) + state.min);
    state.win_hi[id] = (scale*(1.0-widget->value()) + state.min);
    if (state.win_hi[id] == state.win_lo[id]) state.win_hi[id] += 0.01;
    win[id]->need_refresh = TRUE;
    win[id]->redraw();
}

void
zoom_cb(Fl_Value_Scrollbar *widget)
{   int			i;
    float		mx, my;
    float		old_value;
    Fl_Image_Window	*window;

    old_value = win[0]->scale;
    for (i = 0; i < 2; i++) {
	window = win[i];
	mx = ((0.5*window->w() - window->xo)*window->scale),
	my = ((0.5*window->h() - window->yo)*window->scale);
	window->scale = ((int)(128.0/widget->value()))/128.0;
	window->xo = (int)(rint((0.5*window->w() - mx/window->scale)));
	window->yo = (int)(rint((0.5*window->h() - my/window->scale)));
	if (fabs(widget->value() - widget->Default()) < 0.01 &&
	    win[0]->scale != old_value) {
	    window->xo = 0;
	    window->yo = 0;
	}
	window->need_refresh = TRUE;
	window->redraw();
    }
}

typedef int (*SORT)(const void *, const void *);

int
comp_vals(float *a, float *b)
{
    if (*a < *b) return(1);
    if (*a > *b) return(-1);
    return(0);
}

void
iso_val_cb(Fl_Button *widget)
{   int		i, id = widget->id();
    BUBBLEFORM	bub;
    char	*str;

    bub.value = state.isodose[id];
    bub._default = bub.value;
    bub.places = 5;
    bub.fraction_places = 0;
    bub.x = Fl::event_x_root();
    bub.y = Fl::event_y_root();
    if (bubble_form(&bub) == 0) state.isodose[id] = bub.value;
    qsort(state.isodose, state.num_isodose, sizeof(float), (SORT)comp_vals);
    for (i = 0; i < state.num_isodose; i++) {
	str = (char *)(iso_input[i]->label());
	sprintf(str, "%.0f", state.isodose[i]);
	iso_input[i]->label(str);
	iso_input[i]->redraw();
    }
}

void
circle_cb(Fl_Light_Button *widget)
{
    state.circle_flag = !state.circle_flag;
    win[0]->redraw();
    win[1]->redraw();
}

void
circle_size_cb(Fl_Button *widget)
{
    float	step = 0.5;
    static char	str[20];

    if (Fl::event_state() & FL_SHIFT) step = 0.1;
    if (widget->id() == 0) {
	state.circle_radius -= step;
    }
    else {
	state.circle_radius += step;
    }
    sprintf(str, "Circle %.1f", state.circle_radius);
    circle_button->label(str);
    circle_button->redraw();
    win[0]->redraw();
    win[1]->redraw();
}

