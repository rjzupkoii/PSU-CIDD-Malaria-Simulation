cmake_minimum_required(VERSION 3.1)
project(PCMS)

option(USING_ZAPCC "Use ZAPCC compiler." OFF)
if (USING_ZAPCC)
  SET(CMAKE_C_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc)
  SET(CMAKE_CXX_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc++)
endif ()


set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#${COMMON_INCLUDES}
#include dependent libs

find_package(GSL REQUIRED)
if (GSL_FOUND)
  MESSAGE(${GSL_LIBRARIES})
  include_directories(${GSL_INCLUDE_DIRS})
endif ()

set(YAMLCPP_STATIC_LIBRARY TRUE)
find_package(yaml-cpp CONFIG REQUIRED)

#if (YAMLCPP_FOUND)
#  MESSAGE("Found local yaml-cpp")
#  MESSAGE(${YAMLCPP_LIBRARY})
#else ()
#  MESSAGE(STATUS "Yaml-cpp not found, use github repository instead")
#  add_subdirectory(${EXT_PROJECTS_DIR}/yaml-cpp)
##  MESSAGE(${YAMLCPP_STATIC})
#endif ()

add_subdirectory(${EXT_PROJECTS_DIR}/date)
add_subdirectory(${EXT_PROJECTS_DIR}/spdlog)
add_subdirectory(${EXT_PROJECTS_DIR}/fmt)

include_directories(${YAMLCPP_INCLUDE_DIR} ${DATE_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR} ${FMT_INCLUDE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
  add_subdirectory(${EXT_PROJECTS_DIR}/catch)
  add_subdirectory(${EXT_PROJECTS_DIR}/fakeit)
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
