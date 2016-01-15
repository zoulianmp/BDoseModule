// HISTORY
// 28-jun-2004 zsx/gst: use new usage() from usage.h

#include <stdio.h>
#include "plan_sys.h"
#include "plan_strings.h"
#include "plan_syslog.h"
#include "plan_file_io.h"
#include "gen.h"
#include "libbrachy.h"
#include "state.h"

char *usageStr = "\
brachy_dose [options]\n\
\n\
-g file:  grid description file\n\
-G file:  output summed matrix file\n\
-i file:  file from which to read implant_desc\n\
-M file:  output partial point dose files\n\
-o file:  object file\n\
-p file:  point file\n\
-P file:  output summed point dose file\n\
-q     :  query usage and list options\n\
-w file:  weights file\n";

char *purposeStr = "\
TBD\n";

#include "usage.h"

int
setup(int argc, char **argv)
{
    int       have_implant = FALSE,
    	      have_object = FALSE,
    	      have_weight = FALSE;

    int       c,
	      pipe_fdes,
	      stat,
	      accept_fdes;
    static
    int		sock, df_fdes;
    unsigned char     size;
    char      code,
    	      *grid_desc_file,
    	      *implant_desc_file,
    	      *object_desc_file,
    	      *point_desc_file,
    	      *weight_file;

    extern char *optarg;
    extern int optind;



    exitIfQueryFlag(argc, argv);

    state.have_point = FALSE;
    state.have_grid = FALSE;
    state.have_grid_sum = FALSE;
    state.have_point_sum = FALSE;

    if (read_sources(&num_source_list, &source_list)) {
	fprintf(stderr, "Can't read source list\n");
	return(1);
    }
    if (read_seeds(&num_seed_list, &seed_list)) {
	fprintf(stderr, "Can't read seed list\n");
	return(1);
    }

    state.exact = TRUE;

    while ((c = getopt(argc, argv, "i:o:g:p:w:G:P:M:")) != EOF) {
	switch(c) {
	    case 'i':
		implant_desc_file = optarg;
		have_implant = TRUE;
		break;

	    case 'o':
		object_desc_file = optarg;
		have_object = TRUE;
		break;

	    case 'p':
		point_desc_file = optarg;
		state.have_point = TRUE;
		break;

	    case 'g':
		grid_desc_file = optarg;
		state.have_grid = TRUE;
		break;

	    case 'w':
		weight_file = optarg;
		have_weight = TRUE;
		break;

	    case 'G':
		state.sum_file = optarg;
		state.have_grid_sum = TRUE;
		break;

	    case 'P':
		state.point_sum_file = optarg;
		state.have_point_sum = TRUE;
		break;

	    case 'M':
		state.partial_point_file = optarg;
		state.have_partial_point = TRUE;
		break;

	    default:
		usage(NULL);
		break;
	}
    }

    if (!have_implant ||
	!have_object ||
	(!(state.have_grid_sum || state.have_point_sum) && have_weight) ||
	((state.have_grid_sum || state.have_point_sum) && !have_weight))
	usage(NULL);


    if ((df_fdes = open(implant_desc_file, O_RDONLY, 0400)) < 0) {
	perror("brachy_dose: could not open descfile");
	fprintf(stderr, "file %s\n", implant_desc_file);
	exit(3);
    }
    if (stat = read_implant(df_fdes, &implant)) {
	fprintf(stderr, "brachy_dose: read error on descfile\n");
	exit(4);
    }
    close(df_fdes);

    if ((df_fdes = open(object_desc_file, O_RDONLY, 0400)) < 0) {
	perror("brachy_dose: could not open object descfile");
	fprintf(stderr, "file %s\n", grid_desc_file);
	exit(3);
    }
    if (stat = read_objects(df_fdes, &objects)) {
	fprintf(stderr, "brachy_dose: read error on object descfile\n");
	exit(4);
    }

    close(df_fdes);

/*
 * read grid description
 */
    if (state.have_grid) {
	if ((df_fdes = open(grid_desc_file, O_RDONLY, 0400)) < 0) {
	    perror("brachy_dose: could not open grid descfile");
	    fprintf(stderr, "file %s\n", grid_desc_file);
	    exit(3);
	}
	if (stat = read_grid(df_fdes, &grid, TRUE)) {
	    fprintf(stderr, "brachy_dose: read error on grid descfile\n");
	    exit(4);
	}
	close(df_fdes);
    }

/*
 * read point description
 */
    if (state.have_point) {
	if ((df_fdes = open(point_desc_file, O_RDONLY, 0400)) < 0) {
	    perror("brachy_dose: could not open point descfile");
	    fprintf(stderr, "file %s\n", point_desc_file);
	    exit(3);
	}
	if (stat = read_points(df_fdes, &point, TRUE)) {
	    fprintf(stderr, "brachy_dose: read error on point descfile\n");
	    exit(4);
	}
	close(df_fdes);
    }

/*
 * read weights
 */
    if (have_weight) {
	if ((df_fdes = open(weight_file, O_RDONLY, 0400)) < 0) {
	    perror("brachy_dose: could not open weight file");
	    fprintf(stderr, "file %s\n", weight_file);
	    exit(3);
	}
	if (stat = read_weights(df_fdes, &weight)) {
	    fprintf(stderr, "brachy_dose: read error on weight file\n");
	    exit(4);
	}
	close(df_fdes);
    }

    return(0);
}

