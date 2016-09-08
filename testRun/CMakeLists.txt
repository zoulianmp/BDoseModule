
#------------------------------------------------------------------------------
# Define CMake requirements
#
cmake_minimum_required(VERSION 2.6)


include_directories(AFTER ${CMAKE_SOURCE_DIR}/libbrachy)

add_definitions(-DPHYS_DAT_DIR)



message("(${CMAKE_SOURCE_DIR}/libbrachy)")
#------------------------------------------------------------------------------
# Add the source subdirectory - all the source code for the libraries is under
# this.
#

add_executable(testRun  main.cxx)

target_link_libraries(testRun  libbrachy)