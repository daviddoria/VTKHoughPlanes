add_executable(HoughPlanes HoughPlanes.cpp hough.cc convexplane.cc accumulator.cc hsm3d.cc ConfigFileHough.cc parascan.cc quadtree.cc geom_math.cc vtkHoughPlanes.cxx)

if(VTK_LIBRARIES) # should be defined for later versions of VTK 5.x and above
  target_link_libraries(HoughPlanes ${VTK_LIBRARIES} scanlib newmat ANN)
else()
  target_link_libraries(HoughPlanes vtkHybrid scanlib newmat ANN)
endif()
