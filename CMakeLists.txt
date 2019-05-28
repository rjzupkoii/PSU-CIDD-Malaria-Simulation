cmake_minimum_required(VERSION 3.12)
project(POMS)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

option(USING_ZAPCC "Use ZAPCC compiler." OFF)
if (USING_ZAPCC)
  SET(CMAKE_C_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc)
  SET(CMAKE_CXX_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc++)
endif ()

# for Windows only
if (WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "-DNOMINMAX ${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "-DNOMINMAX ${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

IF(APPLE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer")
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-omit-frame-pointer")
  include_directories(/usr/local/include/)
ENDIF()


#if(UNIX)
#    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#endif()

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#include dependent libs
find_package(GSL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenMP REQUIRED)

add_subdirectory(${EXT_PROJECTS_DIR}/date)
add_subdirectory(${EXT_PROJECTS_DIR}/args)

include_directories(${YAML_CPP_INCLUDE_DIR} ${DATE_INCLUDE_DIR} ${ARGS_INCLUDE_DIR} ${EXT_PROJECTS_DIR}/CLI11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
#  find_package(Catch2 CONFIG REQUIRED)
  add_subdirectory(${EXT_PROJECTS_DIR}/catch)

  set(PROJECT_TEST_NAME masim_test)
  add_subdirectory(test)
endif ()
#enable_testing()
#add_test(test1 ${CMAKE_BINARY_DIR}/bin/${PROJECT_TEST_NAME} --gtest_color=yes)


#add_custom_target(run
#    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/hello_2
#    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/hello_2
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#)
