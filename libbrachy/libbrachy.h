
/* brachy.h */

#include <math.h>

#define TERMS 5
#define POLY(x, a, b, c, d) ((((d) * (x) + (c)) * (x) + (b)) * (x) + (a))
#define DIST(a, b) ((float) sqrt((double) ((a) * (a) + (b) * (b))))
#define SIN(x) (((x) < 0.0001) ? (x) : (float) sin((double) (x)))
#define COS(x) (((x) > (1.5706963)) ? (0.0001) : (float) cos((double) (x)))
#define LOG(x) ((float) log((double) (x)))
#define COS_88 (0.0348995)
#define SQRT(x) ((float) sqrt((double) (x)))
#define ACOS(x) ((x < -1.0) ? 3.141592 : (x > 1.0) ? 0.0 : acos((double)(x)))
#define ATAN(x) ((float) atan((double) (x)))
#define ABS(x) ((float) fabs((double) (x)))
#ifdef MIN
#undef MIN
#endif
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#ifndef _PLUNC_HAVE_BRACHY_H
#define _PLUNC_HAVE_BRACHY_H

#define MG 1
#define MC 2



#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef PI
#define PI (3.141592654)
#endif




/*
  defines for lookup tables
*/
#define POLAR_RADII (50)
#define POLAR_ANGLES (46)
#define MAXIMUM_RADIUS (30.0)
#define SEED_RADII (3001)	/* up to and including 30.0 cm */

typedef struct
{
    char isotope[50];
    float gamma;	/*  R per (MG or MC) per hour at 1 cm  */
    int gammaUnits;	/*  MG or MC  */
    float R_to_r;	/*  rad/Roentgen factor  */
    float half_life;	/*  in hours  */
    int TA_count;	/*  number of entries in tissue atten. table  */
    float tissue_attenuation[20];
    float TA_distance[20];
    float mu;		/*  linear attenuation coefficient - used past
				end of TA table  */
    int last_entry;	/*  boolean  */

                        /*  The next table is for a precalculated
			    table of values on a radius vector.  The
			    corresponding radii are on .1 mm increments
			    such that the table index is the radius in
			    .1 millimeters.  The dose engine should use
			    nearest-neighbor look-up.  This should
			    result in some plan-time cycle savings
			    */
    float seed_dose_table[SEED_RADII];
} SEED_SPEC;


typedef struct 
{
	float x; //!< x-coordinate of the 3D-point in cm.
	float y; //!< y-coordinate of the 3D-point in cm.
	float z; //!< z-coordinate of the 3D-point in cm.
} PNT3D;

typedef struct
{
    int type;
    PNT3D p;		/*  entered coordinates in cm  */
} SEED;

typedef struct
{
    char label[100];
    int seed_count;	/* number of seeds in implant */
    int source_count;	/* number of sources in implant */
    int *seed_list;	/* is seed in object or no? */
    int *source_list;	/* is source in object or no? */
} BRACHY_OBJECT;

typedef struct
{
    int count;
    BRACHY_OBJECT *object;
} BRACHY_OBJECTS;


/* Prototypes */


int read_seeds(int *num_seeds, SEED_SPEC **seed_list);
float seed_pdose(SEED_SPEC *seed_spec, int exact, float x, float y, float z);



float
v_interp(
	int         mode,
	int         vec_size,
	float       xvec[],
	float       x,
	float       yvec[],
	int         *index,
	float       *fx);
	
	

char *
get_phys_dat_dir();
	
	
	
	
	

#endif

