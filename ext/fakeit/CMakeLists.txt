cmake_minimum_required(VERSION 2.8.8)
project(fakeit_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)
# Official repository: https://github.com/eranpeer/FakeIt.git
# Temp repository that has latest bug fix https://github.com/myrgy/FakeIt.git


ExternalProject_Add(
    FakeIt
    PREFIX ${CMAKE_BINARY_DIR}/ext
    GIT_REPOSITORY https://github.com/myrgy/FakeIt.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(FakeIt source_dir)
set(FAKEIT_INCLUDE_DIR ${source_dir}/single_header/catch CACHE INTERNAL "Path to include folder for FakeIt")

