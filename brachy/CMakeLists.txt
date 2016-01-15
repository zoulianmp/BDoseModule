
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)


set(exeSrc 
       main.cxx 
	init.cxx 
	make_windows.cxx 
	mirror_cb.cxx 
	options_cb.cxx 
	output.cxx 
	read_image.cxx 
	rotate_cb.cxx 
	sample_image.cxx
	seed_source_select.cxx 
	set_grid_cb.cxx 
	set_grid_plane_cb.cxx 
	spread_sheet.cxx 
	window.cxx

 )


#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_executable(exe_brachy  ${exeSrc})

target_link_libraries(exe_brachy brachy ntab com planX plan planio ps 
	                      GL fltk X11 3d m pthread z)


