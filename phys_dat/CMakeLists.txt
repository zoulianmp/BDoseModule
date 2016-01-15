
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)



#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_executable(make_ls_dat  make_ls_dat.cxx)

target_link_libraries(make_ls_dat brachy plan planio 3d z m)


add_executable(make_seed_dat make_seed_dat.cxx)
target_link_libraries(make_seed_dat brachy plan planio 3d z m)
