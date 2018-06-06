set(gmock_SOURCE_DIR ${EXT_PROJECTS_DIR}/googletest/googlemock)
set(gtest_SOURCE_DIR ${EXT_PROJECTS_DIR}/googletest/googletest)

#message(${gmock_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include)
include_directories(${gtest_SOURCE_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB TEST_SRC_FILES *.cpp Spatial/*.cpp)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/Spatial/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} gmock gtest gmock_main)

#add_dependencies(${PROJECT_TEST_NAME} googletest)
#
#  {add_dependencies(${PROJECT_TEST_NAME} yaml_cpp) 

target_link_libraries(${PROJECT_TEST_NAME}
        gmock
        gmock_main
        ${Boost_LIBRARIES}
        ${YAML_CPP_LIBS_DIR}/libyaml-cpp.a
)

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)
