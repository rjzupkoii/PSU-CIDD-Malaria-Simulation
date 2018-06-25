include_directories(${CATCH_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src)

file(GLOB TEST_SRC_FILES *.cpp Spatial/*.cpp)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/Spatial/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SRC_FILES})

add_dependencies(${PROJECT_TEST_NAME} catch yaml_cpp)

target_link_libraries(${PROJECT_TEST_NAME}
    ${Boost_LIBRARIES}
    ${YAML_CPP_LIBS_DIR}/libyaml-cpp.a
    )

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)


