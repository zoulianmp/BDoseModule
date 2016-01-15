
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
read_sources(int *num_source_list, SOURCE_SPEC **source_list)
{
    int		num, loop, ret, fdes;
    struct stat status;
    char	filename[100];
    SOURCE_SPEC	*sources;

    sprintf(filename, "%s/ls_dat", get_phys_dat_dir());
    if ((fdes = open(filename, O_RDONLY, 0400)) < 0) {
	fprintf(stderr, "Error opening %s\n", filename);
	return(1);
    }

    if (fstat(fdes, &status) == -1) {
	fprintf(stderr, "source_get_specs: stat fail");
	return(1);
    }

    num = status.st_size/sizeof(SOURCE_SPEC);
    if (num < 1) {
	fprintf(stderr, "No sources on file\n");
	return(1);
    }

    sources = (SOURCE_SPEC *)malloc(num*sizeof(SOURCE_SPEC));
    if (sources == NULL) {
 	fprintf(stderr, "source_get_specs: malloc failed\n");
 	return(1);
     }

    for (loop = 0; loop < num; loop++) {
	ret = read(fdes, (char *) &sources[loop], sizeof(SOURCE_SPEC));
	if (ret != sizeof(SOURCE_SPEC)) {
	    fprintf(stderr, "error reading sources\n");
	    return(1);
	}
    }
    close(fdes);

    *num_source_list = num;
    *source_list = sources;
    return(0);
}

