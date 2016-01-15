
#include "defines.h"

static Fl_Window        *set_grid_panel = NULL;
static Fl_Button        *accept_button;
static Fl_Output        *set_label;

static void
set_grid_quit_cb(Fl_Button *widget)
{
    set_grid_panel->hide();
    state.op_flag = OP_NONE;
    state.dose_grid_computed = TRUE;
    compute_doses(TRUE);
    win[0]->redraw();
    win[1]->redraw();
}

void
set_grid_cb()
{
    GRID	*grid = & state.grid;

    if (set_grid_panel == NULL) {
	set_grid_panel = new Fl_Window(520, 0, 240, 20, "Set Grid");
	set_grid_panel->box(FL_DOWN_BOX);
	set_label = new Fl_Output(0, 0, 100, 20, "");
	set_label->value("Set Dose Grid");
	accept_button = new Fl_Button(100, 0, 70, 20, "Accept");
	accept_button->callback((Fl_Callback *)set_grid_quit_cb);
	set_grid_panel->end();
	set_grid_panel->hide();
	if (grid->x_count == 0) {
	    grid->x_count = 81;
	    grid->y_count = 81;
	    grid->z_count = 81;
	    grid->start.x = -10.0;
	    grid->start.y = -10.0;
	    grid->start.z = -10.0;
	    grid->inc.x = 0.25;
	    grid->inc.y = 0.25;
	    grid->inc.z = 0.25;
	    grid->min = grid->max = 0.0;
	    grid->matrix = (float *)
		calloc(grid->x_count*grid->y_count*grid->z_count,
		       sizeof(float));
	    init_mat(grid->grid_to_pat_T);
	    init_mat(grid->pat_to_grid_T);
	}
	return;
    }
    state.op_flag = OP_GRID;
    state.dose_grid_computed = FALSE;
    set_grid_panel->show();
}

