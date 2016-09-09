
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "libbrachy.h"

#include <io.h>
#include <fcntl.h>

char target[100];


int
main(int argc, char **argv)
{
    static SEED_SPEC sspec[] =
    {{
	"I-125",			/* name */
	1.0,				/* gamma */
	MC,				/* units - MC or MG */
	1.0,				/* R to rad */
	60.2 * 24.0,			/* half-life (hours) */
	14,				/* number of entries in tissue
					 *  attenuation table  */
	/* attenuation factors */
	1.10, 1.11, 1.10, 1.04, 0.95, 0.86, 0.77,
	0.59, 0.44, 0.33, 0.24, 0.18, 0.10, 0.057,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	/* corresponding distances (cm) */
	0.057, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0,
	4.0, 5.0, 6.0, 7.0, 8.0, 10.0, 12.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0,				/* mu */
	FALSE				/* boolean - last entry in file? */
    },

    {
	"Ir-192 (mCi)",			/* name */
	4.62,				/* gamma */
	MC,				/* units - MC or MG */
	0.96,				/* R to rad */
	74.2 * 24.0,			/* half-life (hours) */
	13,				/* number of entries in tissue
					 *  attenuation table  */
	/* attenuation factors */
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	/* corresponding distances (cm) */
	0.5, 1.0, 1.5, 2.0, 2.5,
	3.0, 4.0, 5.0, 6.0, 7.0,
	8.0, 9.0, 10.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0,				/* mu */
	FALSE				/* boolean - last entry in file? */
    },

    {
	"Ir-192 (mg Ra-eq)",			/* name */
	4.62 * 1.79,			/* gamma - 1.79 mCi per mg Ra-eq */
	MG,				/* units - MC or MG */
	0.96,				/* R to rad */
	74.2 * 24.0,			/* half-life (hours) */
	13,				/* number of entries in tissue
					 *  attenuation table  */
	/* attenuation factors */
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	/* corresponding distances (cm) */
	0.5, 1.0, 1.5, 2.0, 2.5,
	3.0, 4.0, 5.0, 6.0, 7.0,
	8.0, 9.0, 10.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0,				/* mu */
	TRUE				/* boolean - last entry in file? */
    }};

    int n,
	loop,
	fdes,
	record,
	index;
    float sumxy,
	  sumx2,
	  x,
	  y,
	  sumx,
	  sumy,
	  dist,
          dist_square,
          atten,
          fx;

/*
  calculate mu
*/

	for (record = 0; record < sizeof(sspec) /
		sizeof(SEED_SPEC); record++)
	{
	    if (record == 1 || record == 2)	/* the iridiums */
	    {
		for (loop = 0; loop < sspec[record].TA_count; loop++)
		{
		    sspec[record].tissue_attenuation[loop] =
				POLY(sspec[record].TA_distance[loop],
				1.0128, 0.00519, -0.001178, -0.00002008);

		    printf("TA(%f) = %f\n", sspec[record].TA_distance[loop],
				sspec[record].tissue_attenuation[loop]);
		}
	    }

	    sumxy = 0.0;
	    sumx2 = 0.0;
	    sumx = 0.0;
	    sumy = 0.0;
	    n = 0;
	    for (loop = sspec[record].TA_count - 1;
		 loop > sspec[record].TA_count - 7 && loop > 0; loop--)
	    {
		x = sspec[record].TA_distance[loop];
		y = log((double) sspec[record].tissue_attenuation[loop]);
		sumxy += x * y;
		sumx2 += x * x;
		sumx += x;
		sumy += y;
		n++;
	    }
	    sspec[record].mu = (sumxy - sumx * sumy / n) /
			(sumx2 - sumx * sumx / n);

	    /*
	    printf("mu for %s is %f\n", sspec[record].isotope,
		   sspec[record].mu);
	    */

/*
  The following code (which is liberally borrowed from seed_pdose)
  calculates the dose_table on a millimeter vector.
  */
printf("    dist,   gamma,   R_to_r,  atten,   dist_sq:   dose\n");
	    for (loop = 0; loop < SEED_RADII; loop++)
	    {
		dist = ((float) loop) / 100.0;
		dist_square = dist * dist;
		if (dist_square < .00001)
		    dist_square = .00001;

		if (sspec[record].TA_count < 2 ||
		    dist > sspec[record].TA_distance[sspec[record].TA_count - 1])
		    atten = (float) exp((double) (sspec[record].mu * dist));
		else
		    atten = v_interp(0, sspec[record].TA_count,
				     sspec[record].TA_distance,
				     dist,
				     sspec[record].tissue_attenuation,
				     &index, &fx);

		sspec[record].seed_dose_table[loop] = sspec[record].gamma *
		    sspec[record].R_to_r * atten / dist_square;
if (loop % 100 == 0)
printf("dose[%.2f]: %f %f %f %f: %f\n",
dist, sspec[record].gamma, sspec[record].R_to_r, atten, dist_square,
sspec[record].seed_dose_table[loop]);
	    }
	}

	

	sprintf(target, "%s/%s", get_phys_dat_dir(), "seed_dat");

	if ((fdes = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0664)) < 0) {
		fprintf(stderr, "make_seed_dat: could not create file %s", target);
		exit(1);
	}

	int ss = sizeof(SEED_SPEC);

	for (record = 0; record < sizeof(sspec) /
		sizeof(SEED_SPEC); record++)
	{
		if (write(fdes, &sspec[record], sizeof(sspec[record])) !=
			sizeof(sspec[record]))
		{
			fprintf(stderr, "make_seed_dat: write fail\n");
			exit(1);
		}
	}

	close(fdes);
	return(0);
}
