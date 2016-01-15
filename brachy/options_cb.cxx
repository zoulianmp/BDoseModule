
#include "defines.h"


#define SWAP_IMAGES		0
#define SET_ORIGIN		10
#define ADD_POINTS		30
#define ADD_STANDARD_POINTS	31
#define ADJ_POINTS		32
#define END_ADJ_POINTS		33
#define DEL_POINT		34
#define SET_GRID		40
#define SET_GRID_PLANE		41
#define TRACK_Y			50
#define NEAREST_SEED		51
#define REPORT_SRC_LEN		80
#define PRINT			60
#define SAVE			70
#define QUIT			71


static void delete_point(int num);
static void swap_images();

void
options_cb(Fl_Button *widget)
{   int		ret;
    int		i, j;
    int		*show;
    char	**list;

    init_menu("OPTIONS");
    if (state.op_flag == OP_ADJ_POINTS) {
	add_menu_item("End Adjust Points", END_ADJ_POINTS);
	ret = do_menu();
	switch (ret) {
	    case END_ADJ_POINTS :
		state.op_flag = OP_NONE;
		break;
	}
	return;
    }
    if (state.op_flag != OP_NONE) {
	add_menu_item("You must finish the current operation", 0);
	do_menu();
	return;
    }
    add_menu_item("Swap images", SWAP_IMAGES);
    add_menu_item("Track Vertical Position", TRACK_Y, state.track_y);
    if (state.pat_scale[0] != 0.0 && state.pat_scale[1] != 0.0) {
	add_menu_item("Set Origin", SET_ORIGIN);
	if (state.origin.x != 0.0) {
	    add_menu_item("Add Points", ADD_POINTS);
	    add_menu_item("Add Standard Points", ADD_STANDARD_POINTS);
	    add_menu_item("Adjust Points", ADJ_POINTS);
	    add_menu_item("Delete Points", DEL_POINT);
	    add_menu_item("Set 3D Grid", SET_GRID);
	    add_menu_item("Set 2D Grid Plane", SET_GRID_PLANE);
	    add_menu_item("Print Window", PRINT);
	    add_menu_item("Report Src Length",
			  REPORT_SRC_LEN, state.report_src_len);
	}
    }
    add_menu_item("Show Nearest Seed", NEAREST_SEED, state.show_nearest_seed);
    add_menu_item("Save Settings", SAVE);
    add_menu_item("QUIT", QUIT);
    ret = do_menu();
    switch (ret) {
	case REPORT_SRC_LEN:
	    state.report_src_len = !state.report_src_len;
	    break;
	case SWAP_IMAGES:
	    swap_images();
	    break;
	case NEAREST_SEED:
	    state.show_nearest_seed = !state.show_nearest_seed;
	    state.nearest_seed = -1;
	    break;
	case TRACK_Y:
	    state.track_y = !state.track_y;
	    break;
	case SET_ORIGIN :
	    state.op_flag = OP_ORIGIN;
	    state.op_val = 3;
	    state.write_image_flag |= 2;
	    break;
	case ADD_POINTS :
	    add_point();
	    break;
	case ADD_STANDARD_POINTS :
	    state.standard_points = TRUE;
	    state.point.count = 0;
	    add_point();
	    break;
	case ADJ_POINTS :
	    state.op_flag = OP_ADJ_POINTS;
	    break;
	case DEL_POINT :
	    list = (char **)calloc(state.point.count, sizeof(char *));
	    show = (int *)calloc(state.point.count, sizeof(int));
	    for (i = 0; i < state.point.count; i++) {
		list[i] = state.point.points[i].label;
	    }
	    pop_up_list("Points to Delete", state.point.count, list, show);
	    for (i = state.point.count-1; i >= 0; i--) {
		if (show[i]) {
		    delete_point(i);
		}
	    }
	    free(show);
	    free(list);
	    break;
	case SET_GRID :
	    set_grid_cb();
	    break;
	case SET_GRID_PLANE:
	    set_grid_plane_cb();
	    break;
	case PRINT :
	    compute_doses(TRUE);
	    state.print_flag = 1;
	    openpl("output.ps");
	    translate(50, 720);
	    scale(0.5, 0.5);
	    rotate(-90);
	    font_type("Courier");
	    font_size(24);
	    move(0, 1030);
	    label(state.pat_name);
	    clip_region(0, 0, win[0]->w(), win[0]->h());
	    win[0]->redraw();
	    break;
	case SAVE :
	    output();
	    break;
	case QUIT :
	    output();
	    exit(0);
	    break;
    }
    win[0]->redraw();
    win[1]->redraw();
}

#define ROTATE		0
#define MIRROR_X	1
#define MIRROR_Y	2
#define SET_SFD		3
#define SET_SCALE	4

void
image_button_cb(Fl_Button *widget)
{   int		ret;
    int		id = widget->id();
    int		*show;
    char	sfd_str[100];
    char	**list;
    plan_im_header	*header = &state.header;


    init_menu("IMAGE MANIPULATION");
    add_menu_item("ROTATE Image", ROTATE);
    add_menu_item("MIRROR Image in X", MIRROR_X);
    add_menu_item("MIRROR Image in Y", MIRROR_Y);
    sprintf(sfd_str, "Set SFD for Image: %.1f", win[id]->sfd);
    add_menu_item(sfd_str, SET_SFD);
    add_menu_item("Set Scale for Image", SET_SCALE);
    ret = do_menu();
    switch (ret) {
	case ROTATE :
	    rotate_cb(id);
	    state.write_image_flag |= 2;
	    break;
	case MIRROR_X :
	    mirror_cb(id, 0);
	    state.write_image_flag |= 2;
	    break;
	case MIRROR_Y :
	    mirror_cb(id, 1);
	    state.write_image_flag |= 2;
	    break;
	case SET_SFD :
	    win[id]->sfd = pop_up_easy_float("Film Dist: ", win[id]->sfd);
	    header->per_scan[id].z_position = win[id]->sfd;
	    state.write_image_flag |= 1;
	    break;
	case SET_SCALE :
	    state.op_flag = OP_SCALE;
	    state.op_val = 2;
	    state.pat_ruler =
		pop_up_easy_slider("Click on 2 Points this far apart",
				   1.0, 20.0, 1.0, 5.0);
	    state.write_image_flag |= 1;
	    break;
    }
}

static char	*std_label[10] = {
    "Art", "Alt", "Brt", "Blt", "Irt", "Ilt",
    "Blad1", "Blad2", "Rect1", "Rect2"

};

void
add_point()
{   int		ret;

    state.point.count++;
    state.point.points = (CALC_POINT *)
	Realloc(state.point.points, state.point.count*sizeof(CALC_POINT));
    state.point.dose = (float *)
	Realloc(state.point.dose, state.point.count*sizeof(float));
    state.point.points[state.point.count-1].p.x = 0.0;
    state.point.points[state.point.count-1].p.y = 0.0;
    state.point.points[state.point.count-1].p.z = 0.0;
    state.point.points[state.point.count-1].label[0] = 0;
    if (state.standard_points) {
	if (state.point.count > 10) {
	    state.op_flag = OP_NONE;
	    state.point.count--;
	    return;
	}
	strcpy(state.point.points[state.point.count-1].label,
	std_label[state.point.count-1]);
    }
    ret = pop_up_string("Next Point Name",
    			state.point.points[state.point.count-1].label,
			100);
    if (ret) {
	state.op_flag = OP_NONE;
	state.point.count--;
    }
    else {
	state.op_flag = OP_POINT;
	state.op_val = 3;
    }
}

static void
delete_point(int num)
{   int		i;

    state.point.count--;
    for (i = num; i < state.point.count; i++) {
	state.point.points[i] = state.point.points[i+1];
    }
}

void
remove_any_empty_object()
{   int			i, j;
    int			remove;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;

    /* If an object ends up empty, remove it (and its assoc. weight) */
    for (j = 0; j < obj->count; j++) {
	ob = &obj->object[j];
	remove = TRUE;
	for (i = 0; i < ob->source_count; i++) {
	    if (ob->source_list[i]) remove = FALSE;
	}
	for (i = 0; i < ob->seed_count; i++) {
	    if (ob->seed_list[i]) remove = FALSE;
	}
	if (remove) {
	    /* Delete the empty object and weight */
	    obj->count--;
	    state.weight.count--;
	    for (i = j; i < obj->count; i++) {
		obj->object[i] = obj->object[i+1];
		state.weight.weights[i] = state.weight.weights[i+1];
	    }
	    j--;
	}
    }
    printf("object_count: %d\n", obj->count);
}

static void
swap_images()
{   int			i;
    PIXELTYPE		val, *d1, *d2;
    float		fval;
    plan_im_header	*header = &state.header;

    d1 = state.data;
    d2 = state.data + state.xdim*state.ydim;
    state.write_image_flag |= 2;
    for (i = 0; i < state.xdim*state.ydim; i++) {
	val = d1[i];
	d1[i] = d2[i];
	d2[i] = val;
    }
    fval = header->pixel_to_patient_TM[3][0];
    header->pixel_to_patient_TM[3][0] = -header->pixel_to_patient_TM[3][1];
    header->pixel_to_patient_TM[3][1] = -fval;

    fval = header->pixel_to_patient_TM[0][0];
    header->pixel_to_patient_TM[0][0] = header->pixel_to_patient_TM[1][1];
    header->pixel_to_patient_TM[1][1] = fval;

    state.origin.x = header->pixel_to_patient_TM[3][0];
    state.origin.y = header->pixel_to_patient_TM[3][1];
    state.pat_scale[0] = header->pixel_to_patient_TM[0][0];
    state.pat_scale[1] = header->pixel_to_patient_TM[1][1];
    win[0]->need_refresh = TRUE;
    win[1]->need_refresh = TRUE;
    win[0]->redraw();
    win[1]->redraw();
}

