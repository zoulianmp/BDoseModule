
#include "defines.h"

static Fl_Window        *set_grid_panel = NULL;
static Fl_Button        *accept_button;
static Fl_Output        *set_label;

static void
set_grid_plane_quit_cb(Fl_Button *widget)
{
    set_grid_panel->hide();
    state.op_flag = OP_NONE;
    state.dose_grid_computed = TRUE;
    compute_doses(TRUE);
    win[0]->redraw();
    win[1]->redraw();
}

void
set_grid_plane_cb()
{
    GRID	*grid = & state.grid;

    if (set_grid_panel == NULL) {
	set_grid_panel = new Fl_Window(520, 0, 240, 20, "Set Grid Plane");
	set_grid_panel->box(FL_DOWN_BOX);
	set_label = new Fl_Output(0, 0, 100, 20, "");
	set_label->value("Set 2D Grid Plane");
	accept_button = new Fl_Button(100, 0, 70, 20, "Accept");
	accept_button->callback((Fl_Callback *)set_grid_plane_quit_cb);
	set_grid_panel->end();
	set_grid_panel->hide();
	return;
    }
    state.op_flag = OP_GRID_PLANE;
    set_grid_panel->show();
}

