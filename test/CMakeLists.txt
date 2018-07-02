include_directories(${CATCH_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src)

set(TEST_SRC_FILES
    main.cpp
    sample_catch_test.cpp
    sample_yaml_cpp_test.cpp
    Spatial/CoordinateTest.cpp
    Spatial/LocationTest.cpp)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/Spatial/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} date)

target_link_libraries(${PROJECT_TEST_NAME}
    yaml-cpp
	fmt::fmt fmt::fmt-header-only	
    )

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)


set(FMT_TEST_FILES fmtTest.cpp)
add_executable(fmtTest ${FMT_TEST_FILES})
target_link_libraries(fmtTest
    fmt::fmt fmt::fmt-header-only
    )

install(TARGETS ${PROJECT_TEST_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS fmtTest DESTINATION ${PROJECT_SOURCE_DIR}/bin)
