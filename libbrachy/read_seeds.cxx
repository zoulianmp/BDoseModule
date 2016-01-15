
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "plan_file_io.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "gen.h"
#include "libplanio.h"
#include "libbrachy.h"

int
read_seeds(int *num_seed_list, SEED_SPEC **seed_list)
{
    int		num, loop, ret, fdes;
    struct stat status;
    char	filename[100];
    SEED_SPEC	*seeds;

    sprintf(filename, "%s/seed_dat", get_phys_dat_dir());
    if ((fdes = open(filename, O_RDONLY, 0400)) < 0) {
	fprintf(stderr, "Error opening  %s\n", filename);
	return(1);
    }

    if(fstat(fdes, &status) == -1) {
	fprintf(stderr, "seed_get_specs: stat fail");
	return(1);
    }

    num = status.st_size / sizeof(SEED_SPEC);
    if (num < 1) {
	fprintf(stderr, "seed_get_specs: No seeds on file\n");
	return(1);
    }

    seeds = (SEED_SPEC *)malloc(num*sizeof(SEED_SPEC));
    if (seeds == NULL) {
	fprintf(stderr, "seed_get_specs: malloc failed\n");
	return(1);
    }

    for (loop = 0; loop < num; loop++) {
	ret = read(fdes, (char *)&seeds[loop], sizeof(SEED_SPEC));
	if (ret != sizeof(SEED_SPEC)) {
	    fprintf(stderr, "seed_get_specs: error reading seed_dat\n");
	    return(1);
	}
    }
    close(fdes);

    *num_seed_list = num;
    *seed_list = seeds;
    return(0);
}

