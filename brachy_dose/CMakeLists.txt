
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)


set(exeSrc 
       brachy_dose.cxx 
	calc_norm_mat.cxx 
	calc_norm_vec.cxx 
	output.cxx 
	setup.cxx

 )


#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_executable(brachy_dose  ${exeSrc})

target_link_libraries(brachy_dose brachy plan planio 3d m pthread z)


