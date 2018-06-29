cmake_minimum_required(VERSION 2.8.8)
project(yaml_cpp_builder C CXX)
include(ExternalProject)

ExternalProject_Add(yaml-cpp-ext
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    CMAKE_ARGS -DYAML_CPP_BUILD_TESTS=OFF
    -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
    UPDATE_DISCONNECTED 1
    PREFIX ${CMAKE_BINARY_DIR}/ext
    #    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    # Disable install step
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(yaml-cpp-ext source_dir)
set(YAMLCPP_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(yaml-cpp-ext binary_dir)
set(YAMLCPP_LIBRARY_DIR ${binary_dir} PARENT_SCOPE)

if (YAMLCPP_STATIC_LIBRARY)
  message(STATUS "Using static YamlCpp library")
  set(YAMLCPP_STATIC libyaml-cpp.a)
  set(YAMLCPP_STATIC_DEBUG libyaml-cpp-dbg.a)
endif ()