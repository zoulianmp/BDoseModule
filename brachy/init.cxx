// HISTORY
// 28-jun-2004 zsx/gst: use new usage() from usage.h

#include "defines.h"

char *usageStr = "\
TBD\n";

char *purposeStr = "\
TBD\n";

#include "usage.h"

static float default_iso[] = {
    200.0, 100.0, 80.0, 60.0, 50.0, 40.0, 30.0, 20.0, 10.0, 5.0
};

static COLOR iso_col[] = {
    { 255, 255, 255 },
    { 255,   0, 255 },
    { 255, 255,   0 },
    {   0, 255, 255 },
    {   0,   0, 255 },
    { 128, 128, 128 },
    { 128,   0, 128 },
    { 128, 128,   0 },
    {   0, 128, 128 },
    {   0,   0, 128 }
};

int
init(int argc, char **argv)
{   int		i, j, ret;
    int		fd;
    char	warp_file[100];
    FILE	*fp;



    exitIfQueryFlag(argc, argv);

    memset(&state, 0, sizeof(STATE));
    state.circle_radius = 1.0;

    while ((ret = getopt(argc, argv, "")) != EOF) {
	switch (ret) {
	}
    }

    get_plunc_env();
    printf("COLOR_PS_PRINTER: %s\n", getenv("COLOR_PS_PRINTER"));

    if (is_file("ap_im") && is_file("lat_im") && !is_file("plan_im")) {
	system("plan_im_cat -i ap_im -i lat_im -o plan_im");
    }
    if (read_image("plan_im")) {
	sprintf(warp_file, "%s/site/warp", get_plunc_root_dir());
	fprintf(stderr, "No plan_im, trying .LUM files\n");
	char **dirs = get_dirs(".", "LUM");
	for (i = 0; dirs != NULL && dirs[i] != NULL; i++) {
	    printf("reading %s\n", dirs[i]);
	    if (read_lumisys(state.zdim++, dirs[i], warp_file)) {
		fprintf(stderr, "Can't read %s\n", dirs[i]);
		exit(1);
	    }
	}
    }
    if (state.xdim == 0) {
	fprintf(stderr, "No image found, starting with blank image\n");
	make_blank_image();
    }

    fp = fopen("patient_name", "r");
    if (fp != NULL) {
	fscanf(fp, "%100c", state.pat_name);
	fclose(fp);
    }
    else {
    	pop_up_string("Pat Name, ID: ", state.pat_name, 100);
    }

    fd = open("point", O_RDONLY);
    if (fd > 0) {
	read_points(fd, &state.point, TRUE);
	close(fd);
    }

    fd = open("implant", O_RDONLY);
    if (fd > 0) {
	read_implant(fd, &state.implant);
	if (state.implant.seed_count > 0)
	    state.seed_num = state.implant.seed[0].type;
	if (state.implant.source_count > 0)
	    state.source_num = state.implant.source[0].type;
	close(fd);
    }

    fd = open("object", O_RDONLY);
    if (fd > 0) {
	read_objects(fd, &state.objects);
	close(fd);
    }
    fd = open("weight", O_RDONLY);
    if (fd > 0) {
	read_weights(fd, &state.weight);
	close(fd);
    }

    fd = open("grid", O_RDONLY);
    if (fd > 0) {
	read_grid(fd, &state.grid, FALSE);
	close(fd);
    }

    if (read_seeds(&state.num_seed_list, &state.seed_list)) {
	fprintf(stderr, "Can't read seed list\n");
	exit(1);
    }

    if (read_sources(&state.num_source_list, &state.source_list)) {
	fprintf(stderr, "Can't read source list\n");
	exit(1);
    }

    /*
    if (state.origin.x != 0.0) state.track_y = TRUE;
    */

    state.num_isodose = 10;
    for (i = 0; i < state.num_isodose; i++) {
	state.isodose[i] = default_iso[i];
	state.isodose_color[i] = iso_col[i];
    }
    return(0);
}

void
make_blank_image()
{   int		i, j;
    plan_im_header	*header = &state.header;

    state.xdim = header->x_dim = 1000;
    state.ydim = header->y_dim = 1400;
    state.zdim = header->slice_count = 2;
    state.pat_scale[0] = header->pixel_to_patient_TM[0][0] = 0.02;
    state.pat_scale[1] = header->pixel_to_patient_TM[1][1] = 0.02;
    header->pixel_to_patient_TM[3][0] = 0.0;
    header->pixel_to_patient_TM[3][1] = 0.0;
    header->pixel_to_patient_TM[3][2] = 0.0;
    state.origin.x = 0.5*state.xdim;
    state.origin.z = 0.5*state.ydim;
    state.origin.y = -0.5*state.xdim;
    state.min = header->min = 0;
    state.max = header->max = 4095;
    state.win_lo[0] = state.min;
    state.win_hi[0] = state.max;
    state.win_lo[1] = state.min;
    state.win_hi[1] = state.max;
    state.data = (short *)
	calloc(state.xdim*state.ydim*state.zdim, sizeof(short));
    for (i = 0; i < state.xdim*state.ydim*state.zdim; i++) {
	state.data[i] = 1000;
    }
}

void
add_object(int num)
{   int			i, j;
    BRACHY_OBJECTS	*objs = &state.objects;;
    BRACHY_OBJECT	*obj;
    WEIGHT		 *wt;

    objs->count++;
    objs->object = (BRACHY_OBJECT *)
	Realloc(objs->object, objs->count*sizeof(BRACHY_OBJECT));
    obj = &objs->object[objs->count-1];
    memset(obj, 0, sizeof(BRACHY_OBJECT));
    pop_up_string("Object Name:", obj->label, 100);
    obj->seed_count = state.implant.seed_count;
    obj->seed_list = (int *)calloc(obj->seed_count, sizeof(int));
    obj->source_count = state.implant.source_count;
    obj->source_list = (int *)calloc(obj->source_count, sizeof(int));
    for (i = 0; i < obj->seed_count; i++) {
	obj->seed_list[i] = (i >= num);
    }
    for (i = 0; i < obj->source_count; i++) {
	obj->source_list[i] = (i >= num-obj->seed_count);
    }
    for (j = 0; j < objs->count-1; j++) {
	obj = &objs->object[j];
	/*
	obj->seed_count = state.implant.seed_count;
	obj->seed_list = (int *)
	    Realloc(obj->seed_list, obj->seed_count*sizeof(int));
	*/
	for (i = num; i < obj->seed_count; i++) {
	    obj->seed_list[i] = 0;
	}
	/*
	obj->source_count = state.implant.source_count;
	if (obj->source_count == 0) continue;
	obj->source_list = (int *)
	    Realloc(obj->source_list, obj->source_count*sizeof(int));
	*/
	for (i = 0; i < obj->source_count; i++) {
	    obj->source_list[i] = 0;
	}
    }

    state.weight.count = objs->count;
    state.weight.weights = (WEIGHT *)
	calloc(state.weight.count, sizeof(WEIGHT));
    for (i = 0; i < state.weight.count; i++) {
	wt = &state.weight.weights[i];
	wt->factors = 2;
	wt->f.brachy.activity = 10.0;
	wt->f.brachy.time = 1.0;
	wt->f.brachy.decay = 0.0;
    }
}
