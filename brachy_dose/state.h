
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "gen.h"
#include "lib3d.h"
#include "libplan.h"
#include "libbrachy.h"
#include "libplanio.h"


typedef struct
{
    int exact;
    int have_point;
    int have_grid;
    int have_point_sum;
    int have_grid_sum;
    int have_partial_point;
    char *sum_file;
    char *point_sum_file;
    char *partial_point_file;
} STATE;

/* Global variables */
#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int		num_source_list;
EXTERN SOURCE_SPEC	*source_list;
EXTERN int		num_seed_list;
EXTERN SEED_SPEC	*seed_list;
EXTERN IMPLANT_DESC	implant;
EXTERN BRACHY_OBJECTS	objects;
EXTERN CALC_POINTS	point;
EXTERN WEIGHTS		weight;
EXTERN GRID		grid;
EXTERN STATE		state;


/* Prototypes */
void calc_norm_mat(int object, GRID *norm_mat);
void calc_norm_vec(int object, CALC_POINTS *norm_vec);
void check_socket();
int looking_for(char code);
void log_calc(char *patient, char *user);
int output(GRID *part_ptr, CALC_POINTS *part_vec_ptr);
void print_report();
void print_specs(FILE *strm);
int setup(int argc, char **argv);


