cmake_minimum_required(VERSION 2.8.8)
project(spdlog_builder CXX)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    spdlog
    PREFIX ${CMAKE_BINARY_DIR}/ext
    GIT_REPOSITORY https://github.com/gabime/spdlog
	CMAKE_ARGS -DSPDLOG_BUILD_TESTING=OFF 
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(spdlog source_dir)
set(SPDLOG_INCLUDE_DIR ${source_dir}/include CACHE INTERNAL "Path to include folder for spdlog")

