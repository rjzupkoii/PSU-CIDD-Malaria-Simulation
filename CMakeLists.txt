cmake_minimum_required(VERSION 2.8)

option(USING_ZAPCC "Use ZAPCC compiler." OFF) # Makes boolean 'test' available.
if (USING_ZAPCC)
    SET(CMAKE_C_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc)
    SET(CMAKE_CXX_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc++)
endif ()
# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

project(POMS)

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

IF (CMAKE_VERSION VERSION_LESS 3.1)
    MESSAGE("It is not greater than 3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    endif ()
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
else ()
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
endif (CMAKE_VERSION VERSION_LESS 3.1)

#${COMMON_INCLUDES}
#include dependent libs

find_package(GSL REQUIRED)
if (GSL_FOUND)
    #MESSAGE(${GSL_LIBRARY})
    include_directories(${GSL_INCLUDE_DIRS})
endif ()

find_package(Boost REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_subdirectory(${EXT_PROJECTS_DIR}/yaml-cpp)
set(YAML_CPP_INCLUDE_DIRS ${EXT_PROJECTS_DIR}/yaml-cpp/include)
set(YAML_CPP_LIBS_DIR ${CMAKE_BINARY_DIR}/ext/yaml-cpp)

include_directories(${YAML_CPP_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
#includes Gtest

#set(YAML_CPP_INCLUDE_DIRS ${EXT_PROJECTS_DIR}/yaml-cpp/include)
#set(YAML_CPP_LIBS_DIR ${CMAKE_BINARY_DIR}/ext/yaml-cpp)

option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
    add_subdirectory(${EXT_PROJECTS_DIR}/googletest)
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
