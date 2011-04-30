FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

IF (WITH_SHAPE_DETECTION)
  include_directories(${CMAKE_SOURCE_DIR}/include/shapes/)
  
  add_executable(planes plane.cc hough.cc convexplane.cc accumulator.cc hsm3d.cc ConfigFileHough.cc parascan.cc quadtree.cc geom_math.cc )
  target_link_libraries(planes scanlib newmat dl vtkHybrid)

  add_executable(vtkHough vtk.cpp hough.cc convexplane.cc accumulator.cc hsm3d.cc ConfigFileHough.cc parascan.cc quadtree.cc geom_math.cc )
  target_link_libraries(vtkHough scanlib newmat dl vtkHybrid)

ENDIF(WITH_SHAPE_DETECTION)
