
#include "defines.h"

static Fl_Window	*wait_panel;

void
please_wait(char *message)
{
    /*
    fl_cursor(FL_CURSOR_WAIT, FL_BLACK, FL_WHITE);
    */
    if (wait_panel == NULL) {
	wait_panel = new Fl_Window(Fl::w()/2 - 100, Fl::h()/2 - 5,
				   200, 20, message);
    }
    wait_panel->label(message);
    wait_panel->set_modal();
    wait_panel->end();
    wait_panel->show();
    Fl::flush();
    Fl::wait();
}

void
please_wait_end()
{
    wait_panel->hide();
    Fl::flush();
    Fl::wait();
    /*
    fl_cursor(FL_CURSOR_DEFAULT, FL_BLACK, FL_WHITE);
    */
}

void
output()
{   int			i, fd;
    FILE		*fp;
    plan_im_header	*header = &state.header;

    please_wait("Please Wait");
    please_wait_end();
    if (state.pat_name[0]) {
	fp = fopen("patient_name", "w");
	if (fp != NULL) {
	    fprintf(fp, "%s", state.pat_name);
	    fclose(fp);
	}
    }

    fd = open("point", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_points(fd, &state.point, TRUE);
	close(fd);
    }

    fd = open("implant", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_implant(fd, &state.implant);
	close(fd);
    }

    fd = open("object", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_objects(fd, &state.objects);
	close(fd);
    }

    fd = open("weight", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd > 0) {
	write_weights(fd, &state.weight);
	close(fd);
    }

    if (state.grid.x_count) {
	fd = open("grid", O_RDWR | O_CREAT | O_TRUNC, 0664);
	if (fd > 0) {
	    write_grid(fd, &state.grid, FALSE);
	    close(fd);
	}
    }

    if (state.write_image_flag) {
	printf("writing image header\n");
	header->pixel_to_patient_TM[3][0] = state.origin.x;
	header->pixel_to_patient_TM[3][1] = state.origin.y;
	header->pixel_to_patient_TM[3][2] = state.origin.z;
	header->pixel_to_patient_TM[0][0] = state.pat_scale[0];
	header->pixel_to_patient_TM[1][1] = state.pat_scale[1];
printf("origin: %.1f %.1f %.1f\n",
state.origin.x, state.origin.y, state.origin.z);

	printf("writing image %d x %d\n", header->x_dim, header->y_dim);
	fd = open("plan_im", O_RDWR | O_CREAT | O_TRUNC, 0664);
	if (fd > 0) {
	    write_image_header(fd, header);
	    if (state.write_image_flag & 2) {
		printf("writing image data\n");
		for (i = 0; i < header->slice_count; i++) {
		    header->per_scan[i].offset_ptrs = lseek(fd, 0, SEEK_CUR);
		    write_scan_xy(fd,
			state.data + i*header->x_dim*header->y_dim,
			header, i);
		}
		lseek(fd, 0, SEEK_SET);
		write_image_header(fd, header);
	    }
	    close(fd);
	}
    }

    fp = fopen("window", "w");
    if (fp != NULL) {
	fprintf(fp, "%f %f %f %f %f\n",
		greybar[AP]->value(),
		greybar[AP]->value2(),
		greybar[LAT]->value(),
		greybar[LAT]->value2(),
		zoom->value());
	fclose(fp);
    }
}


