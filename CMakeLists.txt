cmake_minimum_required (VERSION 2.6.1)

project (HoughPlanes)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

## Use OpenMP if it is available
FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
  OPTION(WITH_OPENMP "Whether to use parallel processing capabilities of OPENMP. ON/OFF" ON)
ENDIF(OPENMP_FOUND)

IF(OPENMP_FOUND AND WITH_OPENMP)
  MESSAGE(STATUS "With OpenMP ")
  # This is the directory of CountProcessors.cmake
  SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdparty/CMakeModules" ${CMAKE_MODULE_PATH})

  INCLUDE(CountProcessors)

  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=${PROCESSOR_COUNT} -DOPENMP_NUM_THREADS=${PROCESSOR_COUNT} ${OpenMP_CXX_FLAGS} -DOPENMP")

INCLUDE(CountProcessors)
  SET(NUMBER_OF_CPUS "${PROCESSOR_COUNT}" CACHE STRING "The number of processors to use (default: ${PROCESSOR_COUNT})" )
ELSE(OPENMP_FOUND AND WITH_OPENMP)
  MESSAGE(STATUS "Without OpenMP")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=1 -DOPENMP_NUM_THREADS=1")
ENDIF(OPENMP_FOUND AND WITH_OPENMP)

OPTION(BIT_64 "Architecture is 64 Bit (default 32 Bit). ON/OFF" OFF)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS}")

# Set output directories for libraries and executables
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/ann_1.1.1_modified/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/ann_1.1.1_modified/src)

# Set link directories
link_directories(${CMAKE_SOURCE_DIR}/obj)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Compile dependencies
add_subdirectory(3rdparty)
add_subdirectory(src/slam6d)

# Compile the actual HoughPlanes code
add_subdirectory(src)
