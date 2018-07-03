cmake_minimum_required(VERSION 2.8.8)
project(args_builder C CXX)
include(ExternalProject)

ExternalProject_Add(args
    GIT_REPOSITORY https://github.com/taywee/args
    CMAKE_ARGS -DARGS_BUILD_EXAMPLE=OFF -DARGS_BUILD_UNITTESTS=OFF
    UPDATE_DISCONNECTED 1
    PREFIX ${CMAKE_BINARY_DIR}/ext
    #    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    # Disable install step
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(args source_dir)
set(ARGS_INCLUDE_DIR ${source_dir} PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(args binary_dir)
set(ARGS_LIBS_DIR ${binary_dir} PARENT_SCOPE)