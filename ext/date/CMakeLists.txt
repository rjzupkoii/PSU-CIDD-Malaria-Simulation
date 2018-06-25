cmake_minimum_required(VERSION 2.8.8)
project(date_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    date
    CMAKE_ARGS -DENABLE_DATE_TESTING=OFF
    PREFIX ${CMAKE_BINARY_DIR}/ext
    GIT_REPOSITORY https://github.com/HowardHinnant/date
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(date source_dir)
set(DATE_INCLUDE_DIR ${source_dir}/include CACHE INTERNAL "Path to include folder for Date")