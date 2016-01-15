
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)


set( LibSrc 
       Sievert.cxx 
	read_sources.cxx 
	read_seeds.cxx 
	seed_pdose.cxx 
	source_pdose.cxx
)


#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_library(libbrachy  ${LibSrc})

#target_link_libraries(adjust_anastruct_density  plan planio 3d z m)


