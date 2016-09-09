
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})



include_directories(CMAKE_SOURCE_DIR)

set( LibSrc 
     libbrachy.h
	 read_seeds.cxx 
	 seed_pdose.cxx 
	 v_interp.cxx
	 bin_search.cxx
	 get_phys_dat_dir.cxx
	 vtkIr192SeedSource.cxx
)


#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_library(libbrachy  ${LibSrc})

#target_link_libraries(adjust_anastruct_density  plan planio 3d z m)

if(VTK_LIBRARIES)
  target_link_libraries(libbrachy ${VTK_LIBRARIES})
else()
  target_link_libraries(libbrachy vtkHybrid vtkWidgets)
endif()



set (SeedSrc

	make_seed_dat.cxx
	
	)

add_executable(make_seed_dat ${SeedSrc})

target_link_libraries(make_seed_dat  libbrachy)