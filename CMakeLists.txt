cmake_minimum_required(VERSION 3.2)
project(POMS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable if building on the cluster, via the command line use -DBUILD_CLUSTER:BOOL=true
set(BUILD_CLUSTER false CACHE BOOL "Enable for builds on the cluster")

option(USING_ZAPCC "Use ZAPCC compiler." OFF)
if (USING_ZAPCC)
  SET(CMAKE_C_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc)
  SET(CMAKE_CXX_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc++)
endif ()

if (WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNOMINMAX ${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "-DNOMINMAX ${CMAKE_CXX_FLAGS_DEBUG} /MTd")

  # Sets the flag to disable Linux specific error handling 
  add_definitions(-D__DISABLE_CRIT_ERR)  
endif ()

IF(APPLE)
  # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
  LINK_DIRECTORIES(/usr/local/lib)
  INCLUDE_DIRECTORIES(/usr/local/include)
ENDIF()

# Include dependent libs, note that this might be extended if we are on the cluster
find_package(GSL REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Include external projects
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)
add_subdirectory(${EXT_PROJECTS_DIR}/date)
add_subdirectory(${EXT_PROJECTS_DIR}/args)
include_directories(${YAML_CPP_INCLUDE_DIR} ${DATE_INCLUDE_DIR} ${ARGS_INCLUDE_DIR} ${EXT_PROJECTS_DIR}/CLI11)

# Add the source subdirectory
add_subdirectory(src)

# Setup test project
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
  add_subdirectory(${EXT_PROJECTS_DIR}/catch)
  set(PROJECT_TEST_NAME masim_test)
  add_subdirectory(test)
endif ()
