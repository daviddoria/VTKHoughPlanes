add_library(hough hough.cc convexplane.cc accumulator.cc hsm3d.cc ConfigFileHough.cc parascan.cc quadtree.cc geom_math.cc vtkHoughPlanes.cxx)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) #without this line and the "dl" library below, there is an error "dlopen@@GLIBC is defined in DSO could not read symbols invalid operation"

if(VTK_LIBRARIES) # should be defined for later versions of VTK 5.x and above
  target_link_libraries(hough ${VTK_LIBRARIES} scanlib newmat ANN dl)
else()
  target_link_libraries(hough vtkHybrid scanlib newmat ANN dl)
endif()

# The following executable is the provided example code, as well as demonstrates how to link to the code in this repository.
add_executable(HoughPlanes HoughPlanes.cpp)
target_link_libraries(HoughPlanes hough)
