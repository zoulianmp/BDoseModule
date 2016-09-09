
cmake_minimum_required(VERSION 2.8)

PROJECT(ImageIterator)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

Set(Src
    vtkIr192SeedSorce.cxx
	ImageIterator.cxx
	)


add_executable(ImageIterator MACOSX_BUNDLE ${Src})

if(VTK_LIBRARIES)
  target_link_libraries(ImageIterator ${VTK_LIBRARIES})
else()
  target_link_libraries(ImageIterator vtkHybrid vtkWidgets)
endif()
