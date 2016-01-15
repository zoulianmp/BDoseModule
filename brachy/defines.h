
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "plan_file_io.h"
#include "plan_strings.h"
#include "gen.h"
#include "plan_im.h"
#include "libbrachy.h"
#include "lib3d.h"
#include "libplanio.h"
#include "libplan.h"
#include "libplot.h"
#include "pop_up.h"
#include "bubdefs.h"
#include "libplanX.h"

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif


/* Offsets into the lumisys file */
#define XY_DIM_OFFSET           806
#define XY_DIM_LEN              4
#define NUM_IMAGES_OFFSET       804
#define NUM_IMAGES_LEN          2
#define PIXEL_START_OFFSET      2048

#define AP	0
#define LAT	1

#define OP_NONE			0
#define OP_ROTATE		1
#define OP_SHEAR		2
#define OP_POINT		3
#define OP_ADJ_POINTS		4
#define OP_ORIGIN		5
#define OP_RULER		6
#define OP_SCALE		7
#define OP_SOURCE		8
#define OP_ADJ_SOURCES		9
#define OP_SEED			10
#define OP_STRAND		11
#define OP_ADJ_SEEDS		12
#define OP_GRID			13
#define OP_GRID_PLANE		14	

typedef struct {
    int	r, g, b;
} COLOR;

typedef struct {
    int			xdim, ydim, zdim;
    int			min, max;
    float		win_lo[2], win_hi[2];
    int			op_flag, op_val;
    int			track_y;
    int			show_nearest_seed;
    int			nearest_seed;
    int			rx1, rx2, ry1, ry2;
    char		pat_name[100];
    short		*data;
    plan_im_header	header;
    int			write_image_flag;
    float		pat_ruler;
    float		pat_scale[2]; /* cm per pixels */
    PNT3D		origin;
    PNT3D		ruler_origin;
    int			ruler_flag;
    int			set_ruler;
    CALC_POINTS		point;
    GRID		grid;
    WEIGHTS		weight;
    IMPLANT_DESC	implant;
    BRACHY_OBJECTS	objects;
    int			num_seed_list;
    int			seed_num;
    SEED_SPEC		*seed_list;
    int			num_source_list;
    int			source_num;
    SOURCE_SPEC		*source_list;
    int			dose_grid_computed;
    int			num_isodose;
    float		isodose[10];
    COLOR		isodose_color[10];
    int			print_flag;
    float		ap_x, lat_y;
    int			strand_count, strand_i[2];
    int			circle_flag;
    float		circle_radius;
    int			standard_points;
    int			report_src_len;
} STATE;

class
Fl_Image_Window : public Fl_Double_Window
{
    int handle(int);
    void draw();
public:
    float	scale;
    float	sfd;
    int		xo, yo;
    int		mx, my;
    PNT3D	pat_pt;
    int		need_refresh;
    Fl_Image_Window(int x, int y, int w, int h)
	: Fl_Double_Window(x, y, w, h, "") {
	    scale = 1.0;
	    sfd = 140.0;
	    xo = yo = 0;
	    mx = my = 0;
	    need_refresh = TRUE;
	}
};


/* Globals */
EXTERN STATE			state;
EXTERN Fl_Window		*main_panel;
EXTERN Fl_Image_Window		*win[2];
EXTERN Fl_Button		*options;
EXTERN Fl_Button		*image_button[2];
EXTERN Fl_Button		*spread_sheet;
EXTERN Fl_Button		*seed_button;
EXTERN Fl_Button		*source_button;
EXTERN Fl_Button		*iso_input[10];
EXTERN Fl_Button		*circle_inc, *circle_dec;
EXTERN Fl_Light_Button		*show_iso;
EXTERN Fl_Light_Button		*circle_button;
EXTERN Fl_Double_Slider		*greybar[2];
EXTERN Fl_Value_Scrollbar	*zoom;

EXTERN float			**partial_dose;

/* Prototypes */
void add_object(int num);
void add_point();
void add_seed();
void add_seed_strand();
void compute_doses(int grid_flag);
void greybar_cb(Fl_Double_Slider *widget);
void image_button_cb(Fl_Button *widget);
int init(int argc, char **argv);
short interp_data(short *data, int i, int j, int xdim, float *warp_data);
void iso_val_cb(Fl_Button *widget);
void make_blank_image();
int make_windows(int argc, char **argv);
void mirror_cb(int which, int axis);
void options_cb(Fl_Button *widget);
void output();
void please_wait(char *message);
void please_wait_end();
int read_image(char *in_file);
int read_lumisys(int num, char *in_file, char *warp_file);
void redraw_windows();
void remove_any_empty_object();
void rotate_cb(int which);
void rotate_set(int which, int x, int y);
void circle_cb(Fl_Light_Button *widget);
void circle_size_cb(Fl_Button *widget);
void set_grid_cb();
void set_grid_plane_cb();
void set_point(int which, int x, int y);
void seed_button_cb(Fl_Button *widget);
void show_iso_cb(Fl_Light_Button *widget);
void source_button_cb(Fl_Button *widget);
void spread_sheet_cb(Fl_Button *widget);
void zoom_cb(Fl_Value_Scrollbar *widget);
void usage();

