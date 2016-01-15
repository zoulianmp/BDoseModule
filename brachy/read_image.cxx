
#include "defines.h"

int
read_lumisys(int num, char *in_file, char *warp_file)
{   int		fdes;
    int		i, j, index, outdex;
    int		swap_bytes_flag;
    int		base;
    short	nscans, dimbuf[2];
    float	*warp_data;

    fdes = open(in_file, O_RDONLY, 0);
    if (fdes < 0) {
	fprintf(stderr, "Can't open %s for input\n", in_file);
	return(1);
    }
    lseek(fdes, NUM_IMAGES_OFFSET, SEEK_SET);
    read(fdes, &nscans, NUM_IMAGES_LEN);
    swap_bytes_flag = (nscans == 256);

    lseek(fdes, XY_DIM_OFFSET,  SEEK_SET);
    read(fdes, dimbuf, XY_DIM_LEN);
    if (swap_bytes_flag)
	my_swab((char*)dimbuf, (char*)dimbuf, XY_DIM_LEN);

    if (num > 0 && dimbuf[1] > state.ydim) dimbuf[1] = state.ydim;

    state.xdim = dimbuf[0];
    state.ydim = dimbuf[1];
    printf("xdim, ydim: %d %d\n", state.xdim, state.ydim);

    lseek(fdes, PIXEL_START_OFFSET, SEEK_SET);

    state.data = (short *)
	Realloc(state.data, state.xdim*state.ydim*state.zdim*sizeof(short));
    state.data += state.xdim*state.ydim*num;
    memset(state.data, 0, state.xdim*state.ydim*sizeof(short));
    for (j = 0; j < state.ydim; j++) {
	read(fdes, state.data + j*state.xdim, state.xdim*sizeof(short));
    }
    close(fdes);
    if (warp_file) {
	int	count, index;
	short	*data2;
	FILE	*fp;

	fp = fopen(warp_file, "r");
	if (fp == NULL) {
	    fprintf(stderr, "Can't open warp file: %s\n", warp_file);
	    exit(1);
	}
	fscanf(fp, "%d", &count);
	warp_data = (float *)calloc(count, sizeof(float));
	for (i = 0; i < count; i++) {
	    fscanf(fp, "%f %f\n", &index, &warp_data[i]);
	}
	fclose(fp);
	data2 = (short *)
	    calloc(state.xdim*state.ydim, sizeof(short));
	memcpy(data2, state.data, state.xdim*state.ydim*sizeof(short));
	outdex = 0;
	for (j = 0; j < state.ydim; j++) {
	    for (i = 0; i < state.xdim; i++, outdex++) {
		state.data[outdex] =
		    interp_data(data2, i, j, state.xdim, warp_data);
	    }
	}
	free(data2);
	free(warp_data);
    }

    for (j = 0; j < state.ydim; j++) {
	index = j*state.xdim;
	for (i = 0; i < state.xdim; i++, index++) {
	    if (state.min > state.data[index])
		state.min = state.data[index];
	    if (state.max < state.data[index])
		state.max = state.data[index];
	}
    }
    state.win_lo[0] = state.min;
    state.win_hi[0] = state.max;
    state.win_lo[1] = state.min;
    state.win_hi[1] = state.max;
    printf("min, max: %d %d\n", state.min, state.max);
    state.data -= state.xdim*state.ydim*num;

    state.header.x_dim = state.header.resolution = state.xdim;
    state.header.y_dim = state.ydim;
    state.header.slice_count = state.zdim;
    state.header.machine_id = film_scanner;
    state.header.min = state.min;
    state.header.max = state.max;
    state.write_image_flag |= 2;

    return(0);
}

int
read_image(char *in_file)
{   int		i, j;
    int		fdes;
    plan_im_header	*header = &state.header;

    fdes = open(in_file, O_RDONLY, 0);
    if (fdes < 0) {
	fprintf(stderr, "Can't open %s for input\n", in_file);
	return(1);
    }
    if (read_image_header(fdes, header)) {
	fprintf(stderr, "Can't read header for %s\n", in_file);
	close(fdes);
	return(1);
    }
    state.xdim = header->x_dim;
    state.ydim = header->y_dim;
    state.zdim = header->slice_count;
    if (header->pixel_to_patient_TM[1][1] < 0.0)
	header->pixel_to_patient_TM[1][1] *= -1.0;
    state.origin.x = header->pixel_to_patient_TM[3][0];
    state.origin.y = header->pixel_to_patient_TM[3][1];
    state.origin.z = header->pixel_to_patient_TM[3][2];
    if (header->per_scan[0].z_position == 0.0)
	header->per_scan[0].z_position = 100.0;
    if (header->per_scan[1].z_position == 0.0)
	header->per_scan[1].z_position = 100.0;
    state.pat_scale[0] = header->pixel_to_patient_TM[0][0];
    state.pat_scale[1] = header->pixel_to_patient_TM[1][1];
    state.min = header->min;
    state.max = header->max;
    state.win_lo[0] = state.min;
    state.win_hi[0] = state.max;
    state.win_lo[1] = state.min;
    state.win_hi[1] = state.max;
    printf("read image %d x %d\n", header->x_dim, header->y_dim);
    state.data = (short *)
	calloc(state.xdim*state.ydim*state.zdim, sizeof(short));
    if (read_scan_xy(fdes, state.data, header, 0)) {
	fprintf(stderr, "Can't read slice 0 for %s\n", in_file);
	close(fdes);
	return(1);
    }
    if (read_scan_xy(fdes, state.data + header->x_dim*header->y_dim,
		     header, 1)) {
	fprintf(stderr, "Can't read slice 1 for %s\n", in_file);
	close(fdes);
	return(1);
    }
    return(0);
}

