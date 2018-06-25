cmake_minimum_required(VERSION 2.8.8)
project(yaml_cpp_builder C CXX)
include(ExternalProject)

ExternalProject_Add(yaml_cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    CMAKE_ARGS -DYAML_CPP_BUILD_TESTS=OFF
    -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
    UPDATE_DISCONNECTED 1
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    # Disable install step
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(yaml_cpp source_dir)
set(YAML_CPP_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(yaml_cpp binary_dir)
set(YAML_CPP_LIBS_DIR ${binary_dir} PARENT_SCOPE)