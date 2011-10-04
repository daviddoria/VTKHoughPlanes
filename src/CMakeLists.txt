add_executable(HoughPlanes HoughPlanes.cpp hough.cc convexplane.cc accumulator.cc hsm3d.cc ConfigFileHough.cc parascan.cc quadtree.cc geom_math.cc vtkHoughPlanes.cxx)
target_link_libraries(HoughPlanes scanlib newmat vtkHybrid ANN)
