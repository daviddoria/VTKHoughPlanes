cmake_minimum_required (VERSION 2.6.1)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdparty/CMakeModules" ${CMAKE_MODULE_PATH})
project (Slam6D)


#################################################
# Declare Options and modify build accordingly ##
#################################################


## Shapes
OPTION(WITH_SHAPE_DETECTION "Whether to build shapes and planes executable for detecting planes. ON/OFF" ON)
IF(WITH_SHAPE_DETECTION)
  MESSAGE(STATUS "With shape detection")
ELSE(WITH_SHAPE_DETECTION)
  MESSAGE(STATUS "Without shape detection")
ENDIF(WITH_SHAPE_DETECTION)

## Multiple Cores 
INCLUDE(CountProcessors)
SET(NUMBER_OF_CPUS "${PROCESSOR_COUNT}" CACHE STRING "The number of processors to use (default: ${PROCESSOR_COUNT})" )

# OPEN
FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
  OPTION(WITH_OPENMP "Whether to use parallel processing capabilities of OPENMP. ON/OFF" ON)
ENDIF(OPENMP_FOUND)

IF(OPENMP_FOUND AND WITH_OPENMP)
  MESSAGE(STATUS "With OpenMP ")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=${PROCESSOR_COUNT} -DOPENMP_NUM_THREADS=${PROCESSOR_COUNT} ${OpenMP_CXX_FLAGS} -DOPENMP")
ELSE(OPENMP_FOUND AND WITH_OPENMP)
  MESSAGE(STATUS "Without OpenMP")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=1 -DOPENMP_NUM_THREADS=1")
ENDIF(OPENMP_FOUND AND WITH_OPENMP)


IF(WIN32)
  SET(ADDITIONAL_CFLAGS "-O2" CACHE STRING"Additional flags given to the compiler ()" )
  include_directories(${CMAKE_SOURCE_DIR}/windows/)
  link_directories(${CMAKE_SOURCE_DIR}/windows)
  add_library(XGetopt STATIC ${CMAKE_SOURCE_DIR}/windows/XGetopt.cpp)
  SET(CMAKE_STATIC_LIBRARY_SUFFIX "32.lib")
ELSE(WIN32)
  SET(ADDITIONAL_CFLAGS "-O3 -msse3 -Wall -finline-functions -Wno-write-strings -Wno-char-subscripts" CACHE STRING"Additional flags given to the compiler (-O3 -Wall -finline-functions -Wno-write-strings)" )
ENDIF(WIN32)


SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS}")

# Hide CMake variables
SET (CMAKE_INSTALL_PREFIX "/usr/local" CACHE INTERNAL "" FORCE)
SET (CMAKE_BUILD_TYPE "" CACHE INTERNAL "" FORCE)


# Set output directories for libraries and executables
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )

# Set include and link dirs ... 
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/obj)
link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/ann_1.1.1_modified/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/ann_1.1.1_modified/src)

add_subdirectory(3rdparty)
add_subdirectory(src/shapes)
add_subdirectory(src/slam6d)

MESSAGE (STATUS "Build environment is set up!")

# hack to "circumvent" Debug and Release folders that are created under visual studio
# this is why the INSTALL target has to be used in visual studio
IF(MSVC)
  INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release/ CONFIGURATIONS Release DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")

  INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug/ CONFIGURATIONS rebug DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
ENDIF(MSVC)
