
#include "defines.h"

static void adj_seeds();
static void del_seed(int num);
static void add_source();
static void adj_sources();
static void del_source(int num);

void
seed_button_cb(Fl_Button *widget)
{   int		i, ret;
    int		show[1000];
    char	*list[1000];

    if (state.op_flag == OP_SEED) {
	init_menu("SEED MENU");
	add_menu_item("End Adding Seeds", 102);
    }
    else if (state.op_flag == OP_STRAND) {
	init_menu("You must finish the current strand of seeds");
	do_menu();
	return;
    }
    else {
	init_menu("SELECT A SEED TYPE");
	for (i = 0; i < state.num_seed_list; i++) {
	    add_menu_item(state.seed_list[i].isotope, i);
	}
	add_menu_item("SELECT AN OPERATION", -1, -2);
	add_menu_item("Add Seeds", 100);
	add_menu_item("Add Seed Strand", 103);
    }
    if (state.implant.seed_count > 0) {
	add_menu_item("Delete a Seed", 101);
	if (state.op_flag == OP_ADJ_SEEDS)
	    add_menu_item("End Adjust Seeds", 105);
	else add_menu_item("Adjust Seeds", 104);
    }
    ret = do_menu();
    if (ret < 0) return;
    switch (ret) {
	case 100:
	    add_seed();
	    break;
	case 101:
	    for (i = 0; i < state.implant.seed_count; i++) {
		list[i] = (char *)calloc(20, sizeof(char));
		sprintf(list[i], "Seeds %d", i+1);
	    }
	    pop_up_list("Seeds to Delete",
			state.implant.seed_count,
			(char **)list, (int *)show);
	    for (i = state.implant.seed_count-1; i >= 0; i--) {
		if (show[i]) {
		    del_seed(i);
		}
		free(list[i]);
	    }
	    break;
	case 102:
	    state.op_flag = OP_NONE;;
	    del_seed(state.implant.seed_count-1);
	    break;
	case 103:
	    add_seed_strand();
	    break;
	case 104:
	    adj_seeds();
	    break;
	case 105:
	    state.op_flag = OP_NONE;
	    break;
	default:
	    state.seed_num = ret;
	    sprintf((char *)widget->label(), "Seed: %s",
		   state.seed_list[state.seed_num].isotope);
	    if (state.implant.seed_count == 0) break;
	    for (i = 0; i < state.implant.seed_count; i++) {
		list[i] = (char *)calloc(20, sizeof(char));
		sprintf(list[i], "Seed %d", i+1);
		show[i] = 1;
	    }
	    pop_up_list("Apply to Existing Seeds",
			state.implant.seed_count,
			(char **)list, (int *)show);
	    for (i = 0; i < state.implant.seed_count; i++) {
		if (show[i]) {
		    state.implant.seed[i].type = state.seed_num;
		}
		free(list[i]);
	    }
	    break;
    }
    widget->redraw();
}

void
source_button_cb(Fl_Button *widget)
{   int		i, ret;
    int		show[1000];
    char	*list[1000];

    init_menu("SELECT A SOURCE TYPE");
    for (i = 0; i < state.num_source_list; i++) {
	add_menu_item(state.source_list[i].isotope, i);
    }
    add_menu_item("SELECT AN OPERATION", -1, -2);
    add_menu_item("Add a Source", 100);
    if (state.implant.source_count > 0) {
	add_menu_item("Delete a Source", 101);
	if (state.op_flag == OP_ADJ_SOURCES)
	    add_menu_item("End Adjust Sources", 103);
	else add_menu_item("Adjust Sources", 102);
    }
    ret = do_menu();
    if (ret < 0) return;
    switch (ret) {
	case 100:
	    add_source();
	    break;
	case 101:
	    for (i = 0; i < state.implant.source_count; i++) {
		list[i] = (char *)calloc(20, sizeof(char));
		sprintf(list[i], "Source %d", i+1);
		show[i] = 0;
	    }
	    pop_up_list("Sources to Delete",
			state.implant.source_count,
			(char **)list, (int *)show);
	    for (i = state.implant.source_count-1; i >= 0; i--) {
		if (show[i]) {
		    del_source(i);
		}
		free(list[i]);
	    }
	    break;
	case 102:
	    adj_sources();
	    break;
	case 103:
	    state.op_flag = OP_NONE;
	    break;
	default:
	    state.source_num = ret;
	    sprintf((char *)widget->label(), "Source: %s",
		    state.source_list[state.source_num].isotope);
	    if (state.implant.source_count == 0) break;
	    for (i = 0; i < state.implant.source_count; i++) {
		list[i] = (char *)calloc(20, sizeof(char));
		sprintf(list[i], "Source %d", i+1);
		show[i] = 1;
	    }
	    pop_up_list("Apply to Existing Sources",
			state.implant.source_count,
			(char **)list, (int *)show);
	    for (i = 0; i < state.implant.source_count; i++) {
		if (show[i]) {
		    state.implant.source[i].type = state.source_num;
		}
		free(list[i]);
	    }
	    compute_doses(TRUE);
	    break;
    }
    widget->redraw();
}

void
add_seed()
{   int			i;
    IMPLANT_DESC	*im = &state.implant;
    BRACHY_OBJECT	*obj;

    if (state.objects.count == 0) {
	add_object(im->seed_count + im->source_count);
    }

    state.op_flag = OP_SEED;
    im->seed_count++;
    im->seed = (SEED *)
	Realloc(im->seed, im->seed_count*sizeof(SEED));
    memset(&im->seed[im->seed_count-1], 0, sizeof(SEED));
    im->seed[im->seed_count-1].type = state.seed_num;

    for (i = 0; i < state.objects.count; i++) {
	obj = &state.objects.object[i];
	obj->seed_count++;
	obj->seed_list = (int *)
	    Realloc(obj->seed_list, obj->seed_count*sizeof(int));
    }
    obj->seed_list[obj->seed_count-1] = 1;
    state.dose_grid_computed = FALSE;
}

void
add_seed_strand()
{   int			i;
    inter_struct	inter;
    IMPLANT_DESC	*im = &state.implant;
    BRACHY_OBJECT	*obj;
    static int		strand_count = 10;

    inter.name = "Strand Count";
    inter.id = 0;
    inter.val = strand_count;
    inter.cb = NULL;
    pop_up_int(1, &inter, NULL);
    strand_count = inter.val;
    printf("using a strand count of %d\n", strand_count);
    if (state.objects.count == 0) {
	add_object(im->seed_count + im->source_count);
    }

    im->seed_count += strand_count;
    im->seed = (SEED *)
	Realloc(im->seed, im->seed_count*sizeof(SEED));
    memset(&im->seed[im->seed_count-strand_count],
	   0, strand_count*sizeof(SEED));
    for (i = 0; i < strand_count; i++) {
	im->seed[im->seed_count-i-1].type = state.seed_num;
    }

    for (i = 0; i < state.objects.count; i++) {
	obj = &state.objects.object[i];
	obj->seed_count += strand_count;
	obj->seed_list = (int *)
	    Realloc(obj->seed_list, obj->seed_count*sizeof(int));
    }
    for (i = 0; i < strand_count; i++) {
	obj->seed_list[obj->seed_count-i-1] = 1;
    }
    state.op_flag = OP_STRAND;
    state.dose_grid_computed = FALSE;
    state.strand_count = strand_count;
}

static void
add_source()
{   int			i;
    IMPLANT_DESC	*im = &state.implant;
    BRACHY_OBJECT	*obj;

    if (state.objects.count == 0) {
	add_object(im->seed_count + im->source_count);
    }

    state.op_flag = OP_SOURCE;
    im->source_count++;
    im->source = (SOURCE *)
	Realloc(im->source, im->source_count*sizeof(SOURCE));
    memset(&im->source[im->source_count-1], 0, sizeof(SOURCE));
    im->source[im->source_count-1].type = state.source_num;
    for (i = 0; i < state.objects.count; i++) {
	obj = &state.objects.object[i];
	obj->source_count++;
	obj->source_list = (int *)
	    Realloc(obj->source_list, obj->source_count*sizeof(int));
    }
    obj->source_list[obj->source_count-1] = 1;
    state.dose_grid_computed = FALSE;
}

static void
adj_sources()
{
    state.op_flag = OP_ADJ_SOURCES;
    state.dose_grid_computed = FALSE;

}

static void
adj_seeds()
{
    state.op_flag = OP_ADJ_SEEDS;
    state.dose_grid_computed = FALSE;

}

static void
del_seed(int num)
{   int			i, j;
    int			remove;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;
    
    state.implant.seed_count--;
    /* Remove the seed from the implant list */
    for (i = num; i < state.implant.seed_count; i++) {
	state.implant.seed[i] = state.implant.seed[i+1];
    }
    /* Remove the seed from the object lists (shrinking each list) */
    for (j = 0; j < obj->count; j++) {
	ob = &obj->object[j];
	ob->seed_count = state.implant.seed_count;
	for (i = num; i < state.implant.seed_count; i++) {
	    ob->seed_list[i] = ob->seed_list[i+1];
	}
    }
    remove_any_empty_object();
    state.dose_grid_computed = FALSE;
}

static void
del_source(int num)
{   int			i, j;
    int			remove;
    BRACHY_OBJECTS	*obj = &state.objects;
    BRACHY_OBJECT	*ob;
    
    state.implant.source_count--;
    /* Remove the source from the implant list */
    for (i = num; i < state.implant.source_count; i++) {
	state.implant.source[i] = state.implant.source[i+1];
    }
    /* Remove the source from the object lists (shrinking each list) */
    for (j = 0; j < obj->count; j++) {
	ob = &obj->object[j];
	ob->source_count = state.implant.source_count;
	for (i = num; i < state.implant.source_count; i++) {
	    ob->source_list[i] = ob->source_list[i+1];
	}
    }
    remove_any_empty_object();
    state.dose_grid_computed = FALSE;
}

