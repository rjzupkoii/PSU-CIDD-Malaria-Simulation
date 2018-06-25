cmake_minimum_required(VERSION 2.8.8)
project(fmt_builder C CXX)
include(ExternalProject)

ExternalProject_Add(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    CMAKE_ARGS -DFMT_TEST=OFF -DFMT_DOC=OFF -DFMT_INSTALL=OFF
    UPDATE_DISCONNECTED 1
    PREFIX ${CMAKE_BINARY_DIR}/ext
    #    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    # Disable install step
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(fmt source_dir)
set(FMT_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(fmt binary_dir)
set(FMT_LIBS_DIR ${binary_dir} PARENT_SCOPE)