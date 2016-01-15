
#define EBUG 1

#include <stdio.h>
#include <math.h>
#include "gen.h"
#include "libplan.h"
#include "libbrachy.h"

/*
 This routine calculates the dose to a point resulting from its proximity
 to a seed.  A tissue attenuation correction is applied using a polynomial
 expression from Meisberger, et al.  The polynomial is only good from
 1 - 10 cm.  We assume that it is also good from 0 - 1 cm.  Beyond 10
 cm we use an exponential fit to the last few cm of the polynomial fit.
*/

float
seed_pdose(SEED_SPEC *seed_spec, int exact, float x, float y, float z)
{
    float dist_square;
    static float gamma;
    float atten;
    float fx;
    float dist;
    int index;
    float dose;
    int seed_table_index;

    gamma = seed_spec->gamma;
    dist_square = x * x + y * y + z * z;
    if (dist_square < .00001)
	dist_square = .00001;
    dist = sqrt(dist_square);
    seed_table_index = (int) (dist*100.0);
    if (exact || (seed_table_index >= SEED_RADII)) {
	if (seed_spec->TA_count < 2 ||
	    dist > seed_spec->TA_distance[seed_spec->TA_count - 1])
	    atten = exp(seed_spec->mu*dist);
	else
	    atten = v_interp(0, seed_spec->TA_count,
			     seed_spec->TA_distance,
			     dist,
			     seed_spec->tissue_attenuation,
			     &index, &fx);
	dose = gamma * seed_spec->R_to_r * atten / dist_square;
    }
    else {
	dose = seed_spec->seed_dose_table[seed_table_index];
    }
    return(dose);
}

