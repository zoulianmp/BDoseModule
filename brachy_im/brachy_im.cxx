
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gen.h"
#include "plan_im.h"
#include "plan_file_io.h"
#include "plan_strings.h"
#include "lib3d.h"
#include "libplanio.h"
#include "libplan.h"

int
main(int argc, char **argv)
{   int		i, j, k, num_in = 0;
    int		index, outdex;
    int		slicecnt;
    int		c;
    int		fd;
    int		invert_flag = TRUE;
    char	*in_file[100], *out_file;
    PIXELTYPE	**in_data, *out_data;
    plan_im_header	*in_header, out_header, *h;

    memset(&out_header, 0, sizeof(out_header));
    out_header.min = 10000;
    out_header.max = -10000;
    while ((c = getopt(argc, argv, "i:o:I")) != EOF) {
      switch (c) {
	case 'i':
	    in_file[num_in++] = optarg;
	    break;
	case 'o':
	    out_file = optarg;
	    break;
	case 'I':
	    invert_flag = FALSE;
	    break;
      }
    }
    if (num_in == 0) {
	in_file[num_in++] = "ap_im";
	in_file[num_in++] = "lat_im";
    }
    if (out_file == NULL) {
	out_file = "plan_im";
    }
    in_header = (plan_im_header *)calloc(num_in, sizeof(plan_im_header));
    in_data = (PIXELTYPE **)calloc(num_in, sizeof(PIXELTYPE *));
    for (i = 0; i < num_in; i++) {
	fd = open(in_file[i], O_RDONLY, 0);
	if (fd < 0) {
	    fprintf(stderr, "Can't open %s for input\n", in_file[i]);
	    exit(1);
	}
	h = &in_header[i];
	if (read_image_header(fd, h)) {
	    fprintf(stderr, "Can't read header for %s\n", in_file[i]);
	    exit(1);
	}
	in_data[i] = (PIXELTYPE *)
	    calloc(h->x_dim*h->y_dim*h->slice_count, sizeof(PIXELTYPE));
	for (j = 0; j < h->slice_count; j++) {
	    read_scan_xy(fd, in_data[i] + j*h->x_dim*h->y_dim, h, j);
	}
	close(fd);
	if (out_header.x_dim < h->x_dim) out_header.x_dim = h->x_dim;
	if (out_header.y_dim < h->y_dim) out_header.y_dim = h->y_dim;
	if (out_header.min > h->min) out_header.min = h->min;
	if (out_header.max < h->max) out_header.max = h->max;
	out_header.slice_count += h->slice_count;
	out_header.resolution = out_header.x_dim;
    }
    slicecnt = out_header.x_dim*out_header.y_dim;
    cp_mat(in_header[0].pixel_to_patient_TM, out_header.pixel_to_patient_TM);
    out_header.pixel_size = out_header.pixel_to_patient_TM[0][0];
    out_header.x_size = out_header.pixel_size;
    out_header.pixel_to_patient_TM[1][1] =
	in_header[1].pixel_to_patient_TM[0][0];
    out_header.y_size = out_header.pixel_to_patient_TM[1][1];
    out_header.pixel_to_patient_TM[3][0] =
	out_header.x_dim/2;
    out_header.pixel_to_patient_TM[3][1] =
	-out_header.x_dim/2;
    out_header.pixel_to_patient_TM[3][2] =
	out_header.y_dim/2;
    out_data = (PIXELTYPE *)
	calloc(slicecnt*out_header.slice_count, sizeof(PIXELTYPE));
    outdex = 0;
    for (i = 0; i < num_in; i++) {
	h = &in_header[i];
	index = 0;
	for (j = 0; j < h->y_dim; j++) {
	    for (k = 0; k < h->x_dim; k++, outdex++, index++) {
		out_data[outdex] = in_data[i][index];
		if (invert_flag) {
		    out_data[outdex] = out_header.max - out_data[outdex];
		}
	    }
	    outdex += out_header.x_dim - h->x_dim;
	}
	outdex += (out_header.y_dim - h->y_dim)*out_header.x_dim;
    }
    fd = open(out_file, O_RDWR | O_TRUNC | O_CREAT, 0664);
    if (fd < 0) {
	fprintf(stderr, "Can't open %s for output\n", out_file);
	exit(1);
    }
    if (write_image_header(fd, &out_header)) {
	fprintf(stderr, "Can't write header for %s\n", out_file);
	exit(1);
    }
    for (i = 0; i < out_header.slice_count; i++) {
	out_header.per_scan[i].offset_ptrs = lseek(fd, 0, SEEK_CUR);
	if (write_scan_xy(fd, out_data + i*slicecnt, &out_header, i);
	    fprintf(stderr, "Can't write image data for %s\n", out_file);
	    exit(1);
	}
    }
    lseek(fd, 0, SEEK_SET);
    if (write_image_header(fd, &out_header)) {
	fprintf(stderr, "Can't write header for %s\n", out_file);
	exit(1);
    }
    close(fd);
    return(0);
}

