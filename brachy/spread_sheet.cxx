
#include "defines.h"

static int		spread_done;
static int		restart;

static Fl_Window	*panel;
static Fl_Button	*quit;
static Fl_Button	*print;
static Fl_Button	*set_objects;
static Fl_Button	**pt;
static Fl_Button	**ob;
static Fl_Float_Input	**act;
static Fl_Float_Input	**timei;
static Fl_Output	**dose;
static Fl_Output	***pdose;

static char		**dose_str;
static char		***pdose_str;

static void activity_cb(Fl_Float_Input *widget);
static void ob_cb(Fl_Button *widget);
static void print_spread_sheet_cb();
static void pt_cb(Fl_Button *widget);
static void quit_cb(Fl_Button *widget);
static void refresh_spread_sheet();
static void set_objects_cb(Fl_Button *widget);
static void time_cb(Fl_Float_Input *widget);

void
spread_sheet_cb(Fl_Button *widget)
{   int		i, j;
    int		pcount = state.point.count;
    int		ocount = state.objects.count;
    int		x, y, w, h;
    int		bw = 100, bh = 20;

    compute_doses(FALSE);

    w = (3 + state.objects.count)*bw;
    h = (4 + state.point.count)*bh;
    x = (Fl::w() - w)/2;
    y = (Fl::h() - h)/2;
    panel = new Fl_Window(x, y, w, h, "Spread Sheet");

    quit = new Fl_Button(0, 0, bw, bh, "CLOSE");
    quit->callback((Fl_Callback *)quit_cb);
    print = new Fl_Button(0, 3*bh, bw, bh, "PRINT");
    print->callback((Fl_Callback *)print_spread_sheet_cb);

    set_objects = new Fl_Button(2*bw, 0, bw, bh, "Set Objects");
    set_objects->callback((Fl_Callback *)set_objects_cb);

    ob = (Fl_Button **)calloc(ocount, sizeof(Fl_Button *));
    act = (Fl_Float_Input **)calloc(ocount, sizeof(Fl_Float_Input *));
    timei = (Fl_Float_Input **)calloc(ocount, sizeof(Fl_Float_Input *));
    for (i = 0; i < ocount; i++) {
	ob[i] = new
	    Fl_Button((i+3)*bw, 0, bw, bh, state.objects.object[i].label);
	ob[i]->id(i);
	ob[i]->callback((Fl_Callback *)ob_cb);

	act[i] = new Fl_Float_Input((i+3)*bw+50, bh, bw-50, bh, "Act:");
	act[i]->id(i);
	act[i]->callback((Fl_Callback *)activity_cb);
	act[i]->when(FL_WHEN_CHANGED);
	act[i]->fvalue("%.2f", state.weight.weights[i].f.brachy.activity);
    }
    for (i = 0; i < ocount; i++) {
	timei[i] = new Fl_Float_Input((i+3)*bw+50, 2*bh, bw-50, bh, "Time:");
	timei[i]->id(i);
	timei[i]->callback((Fl_Callback *)time_cb);
	timei[i]->when(FL_WHEN_CHANGED);
	timei[i]->fvalue("%.2f", state.weight.weights[i].f.brachy.time);
    }

    pt = (Fl_Button **)calloc(pcount, sizeof(Fl_Button *));
    dose = (Fl_Output **)calloc(pcount, sizeof(Fl_Output *));
    pdose = (Fl_Output ***)calloc(pcount, sizeof(Fl_Output **));
    dose_str = (char **)calloc(pcount, sizeof(char *));
    pdose_str = (char ***)calloc(pcount, sizeof(char **));
    for (i = 0; i < pcount; i++) {
	pt[i] = new
	    Fl_Button(bw, (i+3)*bh, bw, bh, state.point.points[i].label);
	pt[i]->id(i);
	pt[i]->callback((Fl_Callback *)pt_cb);
	dose[i] = new
	    Fl_Output(2*bw, (i+3)*bh, bw-40, bh, "cGy");
	dose[i]->id(i);
	dose[i]->align(FL_ALIGN_RIGHT);
	dose_str[i] = (char *)calloc(10, sizeof(char));
	sprintf(dose_str[i], "%.1f", state.point.dose[i]);
	dose[i]->value(dose_str[i]);

	pdose[i] = (Fl_Output **)calloc(ocount, sizeof(Fl_Output *));
	pdose_str[i] = (char **)calloc(ocount, sizeof(char *));
	for (j = 0; j < ocount; j++) {
	    pdose[i][j] = new Fl_Output((3+j)*bw+50, (i+3)*bh, bw-50, bh, "");
	    pdose[i][j]->id(i);
	    pdose[i][j]->align(FL_ALIGN_RIGHT);
	    pdose_str[i][j] = (char *)calloc(10, sizeof(char));
	    sprintf(pdose_str[i][j], "%.1f", partial_dose[i][j]);
	    pdose[i][j]->value(pdose_str[i][j]);
	}
    }


    panel->end();
    panel->set_modal();
    panel->show();

    spread_done = FALSE;
    restart = FALSE;
    while (!spread_done) {
	Fl::wait();
    }
    panel->hide();
    if (restart) spread_sheet_cb(widget);
    compute_doses(TRUE);
}

void
compute_doses(int grid_flag)
{   int		i, j, fd;
    char	cur_dir[100];

    if (grid_flag) please_wait("Computing Dose");
    spittime(NULL);
    getcwd(cur_dir, 100);
    chdir("/tmp");

    fd = open("implant", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_implant(fd, &state.implant);
	close(fd);
    }
    fd = open("object", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_objects(fd, &state.objects);
	close(fd);
    }
    fd = open("weight", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_weights(fd, &state.weight);
	close(fd);
    }
    if (grid_flag) {
	fd = open("grid", O_RDWR | O_CREAT | O_TRUNC, 0664);
	if (fd > 0) {
	    write_grid(fd, &state.grid, TRUE);
	    close(fd);
	}
	system("brachy_dose -i implant -o object -w weight -g grid -G grid");
	fd = open("grid", O_RDONLY);
	if (fd > 0) {
	    read_grid(fd, &state.grid, FALSE);
	    close(fd);
	}
	unlink("grid");
	state.dose_grid_computed = TRUE;
    }
    else state.dose_grid_computed = FALSE;
    partial_dose = (float **)calloc(state.point.count, sizeof(float *));
    for (i = 0; i < state.point.count; i++) {
	partial_dose[i] = (float *)calloc(state.objects.count, sizeof(float));
    }
    fd = open("point", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_points(fd, &state.point, TRUE);
	close(fd);
    }
    system("brachy_dose -i implant -o object -w weight -p point -P point -M pt");
    for (i = 0; i < state.objects.count; i++) {
	float	wt;
	char	filename[10];
	sprintf(filename, "pt.%d", i);
	fd = open(filename, O_RDONLY);
	if (fd > 0) {
	    read_points(fd, &state.point, FALSE);
	    close(fd);
	    unlink(filename);
	    wt = state.weight.weights[i].f.brachy.activity;
	    for (j = 0; j < state.point.count; j++) {
		partial_dose[j][i] = wt*state.point.dose[j];
	    }
	}
    }
    fd = open("point", O_RDONLY);
    if (fd > 0) {
	read_points(fd, &state.point, FALSE);
	close(fd);
    }
    unlink("point");
    unlink("implant");
    unlink("object");
    unlink("weight");
    chdir(cur_dir);
    spittime("done compute_doses");
    if (grid_flag) redraw_windows();
    if (grid_flag) please_wait_end();
}

static void
activity_cb(Fl_Float_Input *widget)
{
    state.weight.weights[widget->id()].f.brachy.activity = widget->fvalue();
    refresh_spread_sheet();
}

static void
time_cb(Fl_Float_Input *widget)
{   int		i;

    state.weight.weights[widget->id()].f.brachy.time = widget->fvalue();
    if (widget->id() == 0) {
	for (i = 1; i < state.objects.count; i++) {
	    timei[i]->fvalue("%.2f", widget->fvalue());
	    state.weight.weights[i].f.brachy.time = widget->fvalue();
	}
    }
    refresh_spread_sheet();
}

static void
ob_cb(Fl_Button *widget)
{   int			i, ret;
    BRACHY_OBJECT	*ob = &state.objects.object[widget->id()];
    char		str[1000][10];

    init_menu("Object contains:");
    for (i = 0; i < ob->seed_count; i++) {
	if (ob->seed_list[i]) {
	    sprintf(str[i], "Seed %d\n", i+1);
	    add_menu_item(str[i], -1);
	}
    }
    for (i = 0; i < ob->source_count; i++) {
	if (ob->source_list[i]) {
	    sprintf(str[i], "Source %d\n", i+1);
	    add_menu_item(str[i], -1);
	}
    }
    add_menu_item("Rename Object", 1);
    ret = do_menu();
    if (ret == 1) {
	ob->label[0] = 0;
	pop_up_string("New Object Name:", ob->label, 100);
	widget->hide();
	widget->show();
    }
}

void
pt_cb(Fl_Button *widget)
{
}

void
quit_cb(Fl_Button *widget)
{
    spread_done = TRUE;
}

static void
refresh_spread_sheet()
{   int		i, j;
    int		pcount = state.point.count;
    int		ocount = state.objects.count;

    compute_doses(FALSE);
    for (i = 0; i < pcount; i++) {
	sprintf(dose_str[i], "%.1f", state.point.dose[i]);
	dose[i]->value(dose_str[i]);

	for (j = 0; j < ocount; j++) {
	    sprintf(pdose_str[i][j], "%.1f", partial_dose[i][j]);
	    pdose[i][j]->value(pdose_str[i][j]);
	}
    }
}


static Fl_Button	*oquit;
static Fl_Button	**obj_but;
static Fl_Light_Button	***seed, ***source;

static int odone;

static void
oquit_cb(Fl_Button *widget)
{   int			i, j, count, fault = FALSE;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;

    /* Sanity check, every seed and source must be in 1 and only 1 object */
    for (i = 0; i < state.implant.seed_count; i++) {
	count = 0;
	for (j = 0; j < obj->count; j++) {
	    count += obj->object[j].seed_list[i];
	}
	if (count != 1) {
	    fl_message("Seed %d is in %d objects", i+1, count);
	    fault = TRUE;
	    break;
	}
    }
    for (i = 0; i < state.implant.source_count; i++) {
	count = 0;
	for (j = 0; j < obj->count; j++) {
	    count += obj->object[j].source_list[i];
	}
	if (count != 1) {
	    fl_message("Source %d is in %d objects", i+1, count);
	    fault = TRUE;
	    break;
	}
    }
    if (!fault) odone = TRUE;
}

static void
adjust_cb(Fl_Light_Button *widget)
{   int			i, j, ob_num, s_num;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;

    ob_num = widget->id()/10000;
    ob = &obj->object[ob_num];
    s_num = widget->id() - 10000*ob_num;
    if (s_num < ob->seed_count) {
	ob->seed_list[s_num] = widget->value();
	for (i = 0; i < obj->count; i++) {
	    if (i == ob_num) continue;
	    if (widget->value() == 0) break;
	    ob = &obj->object[i];
	    ob->seed_list[s_num] = !widget->value();
	    seed[i][s_num]->value(!widget->value());
	    seed[i][s_num]->redraw();
	}
    }
    else {
	s_num = s_num - ob->seed_count;
	ob->source_list[s_num] = widget->value();
	for (i = 0; i < obj->count; i++) {
	    if (i == ob_num) continue;
	    if (widget->value() == 0) break;
	    ob = &obj->object[i];
	    ob->source_list[s_num] = !widget->value();
	    source[i][s_num]->value(!widget->value());
	    source[i][s_num]->redraw();
	}
    }
}

static Fl_Window	*opanel;
static void
adjust_objects()
{   int			i, j;
    int			x, y, w, h;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;

    if (obj->object[0].seed_count > obj->object[0].source_count)
	w = 200 + 20*(obj->object[0].seed_count);
    else
	w = 200 + 20*(obj->object[0].source_count);
    h = 20*(obj->count + 2);
    if (h < 80) h = 80;
    x = (Fl::w() - w)/2;
    y = (Fl::h() - h)/2;
    opanel = new Fl_Window(x, y, w, h, "Objects");
    oquit = new Fl_Button(0, 0, 100, 20, "Close");
    oquit->callback((Fl_Callback *)oquit_cb);
    obj_but = (Fl_Button **)
	calloc(obj->count, sizeof(Fl_Button *));
    seed = (Fl_Light_Button ***)
	calloc(obj->count, sizeof(Fl_Light_Button **));
    source = (Fl_Light_Button ***)
	calloc(obj->count, sizeof(Fl_Light_Button **));
    for (i = 0; i < obj->count; i++) {
	ob = &obj->object[i];
	obj_but[i] = new Fl_Button(0, 20*(i+1), 200, 20, ob->label);
	seed[i] = (Fl_Light_Button **)
	    calloc(ob->seed_count, sizeof(Fl_Light_Button *));
	for (j = 0; j < ob->seed_count; j++) {
	    seed[i][j] = new
		Fl_Light_Button(200 + 10*j, 20*(i+1), 10, 10, "");
	    seed[i][j]->id(10000*i + j);
	    seed[i][j]->value(ob->seed_list[j]);
	    seed[i][j]->callback((Fl_Callback *)adjust_cb);
	}
	source[i] = (Fl_Light_Button **)
	    calloc(ob->source_count, sizeof(Fl_Light_Button *));
	for (j = 0; j < ob->source_count; j++) {
	    source[i][j] = new
		Fl_Light_Button(200 + 10*j, 20*(i+1)+10, 10, 10, "");
	    source[i][j]->id(10000*i + j + ob->seed_count);
	    source[i][j]->value(ob->source_list[j]);
	    source[i][j]->selection_color(FL_MAGENTA);
	    source[i][j]->callback((Fl_Callback *)adjust_cb);
	}
    }
    opanel->end();
    opanel->set_modal();
    opanel->show();

    odone = FALSE;
    while (!odone) {
	Fl::wait();
    }
    opanel->hide();
    remove_any_empty_object();
}

static void
set_objects_cb(Fl_Button *widget)
{   int		i, j, ret;
    BRACHY_OBJECTS	*obj = &state.objects;
    char		str[1000][10];

    init_menu("SET OBJECTS");
    add_menu_item("New Object", 0);
    add_menu_item("Adjust Seeds and Sources in Objects", 1);
    add_menu_item("Reset to 1 Object", 2);
    ret = do_menu();
    switch (ret) {
	case 0:
	    init_menu("Object starts at");
	    for (i = 0; i < state.implant.seed_count; i++) {
		sprintf(str[i], "Seed %d", i + 1);
		add_menu_item(str[i], i);
	    }
	    j = state.implant.seed_count;
	    for (i = 0; i < state.implant.source_count; i++) {
		sprintf(str[j], "Source %d", j + i + 1);
		add_menu_item(str[j], j + i);
	    }
	    ret = do_menu();
	    if (ret < 0) return;
	    add_object(ret);
	    break;
	case 1:
	    adjust_objects();
	    break;
	case 2:
	    obj->count = 1;
	    state.weight.count = 1;
	    for (i = 0; i < state.implant.seed_count; i++) {
		obj->object[0].seed_list[i] = 1;
	    }
	    for (i = 0; i < state.implant.source_count; i++) {
		obj->object[0].source_list[i] = 1;
	    }
	    break;
	default:
	    return;
    }
    restart = TRUE;
    quit_cb(widget);
}

static void
print_spread_sheet_cb()
{   int		i, j;
    int		ls, y;
    char	str[200];
    char	*filename = "spread.ps";
    char	*printer, *def_printer = "lpr -Pdps";

    openpl(filename);
    font_type("Courier");
    if (state.objects.count > 4) {
	font_size(8);
	ls = 10;
    }
    else {
	font_size(10);
	ls = 12;
    }
    move(50, 730);
    label(state.pat_name);

    sprintf(str, "               ");
    for (i = 0; i < state.objects.count; i++) {
	BRACHY_OBJECT *obj = &state.objects.object[i];
	sprintf(str, "%s %15s", str, obj->label);
    }
    y = 700;
    move(50, y);
    label(str);

    sprintf(str, "               ");
    for (i = 0; i < state.objects.count; i++) {
	BRACHY_OBJECT *obj = &state.objects.object[i];
	WEIGHT	*wt = &state.weight.weights[i];
	sprintf(str, "%s     Act= %5.2f", str, wt->f.brachy.activity);
    }
    y = y - ls;
    move(50, y);
    label(str);

    sprintf(str, "   Point   Dose");
    for (i = 0; i < state.objects.count; i++) {
	BRACHY_OBJECT *obj = &state.objects.object[i];
	WEIGHT	*wt = &state.weight.weights[i];
	sprintf(str, "%s     Time= %5.1f", str, wt->f.brachy.time);
    }
    y = y - ls;
    move(50, y);
    label(str);

    for (i = 0; i <state.point.count; i++) {
	BRACHY_OBJECT	*obj = &state.objects.object[i];
	float		dose;
	dose = 0.0;
	for (j = 0; j < state.objects.count; j++) {
	    WEIGHT	*wt = &state.weight.weights[j];
	    dose += partial_dose[i][j]*wt->f.brachy.time;
	}
	sprintf(str, "%9s %5.1f",
		state.point.points[i].label, dose);
	for (j = 0; j < state.objects.count; j++) {
	    sprintf(str, "%s %15.1f", str, partial_dose[i][j]);
	}
	y = y - ls;
	move(50, y);
	label(str);
    }

    y = y - ls;
    sprintf(str, "This implant contains:");
    y = y - ls;
    move(50, y);
    label(str);

    if (state.implant.seed_count) {
	SEED_SPEC *spec = &state.seed_list[state.implant.seed[0].type];
	sprintf(str, "  %d seeds of type %s (in %s)",
		state.implant.seed_count,
		spec->isotope,
		(spec->gammaUnits == MG) ? "mgRaeq" : "mCi");
	y = y - ls;
	move(50, y);
	label(str);
    }

    if (state.implant.source_count) {
	SOURCE_SPEC *spec = &state.source_list[state.implant.source[0].type];
	sprintf(str, "  %d sources of type %s (in %s)",
		state.implant.source_count,
		spec->isotope,
		(spec->gammaUnits == MG) ? "mgRaeq" : "mCi");
	y = y - ls;
	move(50, y);
	label(str);
    }

    y = y - ls;
    sprintf(str, "Objects:");
    y = y - ls;
    move(50, y);
    label(str);

    for (i = 0; i < state.objects.count; i++) {
	int		seed_count = 0, source_count = 0;
	BRACHY_OBJECT	*obj = &state.objects.object[i];
	WEIGHT		*wt = &state.weight.weights[i];
	for (j = 0; j < obj->seed_count; j++) {
	    if (obj->seed_list[j]) seed_count++;
	}
	if (seed_count) {
	    sprintf(str, "  %s contains %d seed%s with activity of %.2f",
		    obj->label, seed_count, (seed_count > 1) ? "s" : "",
		    wt->f.brachy.activity);
	    y = y - ls;
	    move(50, y);
	    label(str);
	}
	for (j = 0; j < obj->source_count; j++) {
	    if (obj->source_list[j]) source_count++;
	}
	if (source_count) {
	    sprintf(str, "  %15s contains %d source%s with activity of %.2f",
		    obj->label, source_count, (source_count > 1) ? "s" : "",
		    wt->f.brachy.activity);
	    y = y - ls;
	    move(50, y);
	    label(str);
	}
    }
    closepl();
    printer = getenv("COLOR_PS_PRINTER");
    if (printer == NULL) printer = def_printer;
    sprintf(str, "%s %s", printer, filename);
    printf("system: %s\n", str);
    system(str);
    unlink(filename);
}
