include_directories(${CATCH_INCLUDE_DIR} ${FAKEIT_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src)

set(TEST_SRC_FILES
    main.cpp
    sample_catch_test.cpp
    sample_yaml_cpp_test.cpp
    SimpleFakeItTest.cpp
    Spatial/CoordinateTest.cpp
    Spatial/LocationTest.cpp)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/Spatial/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SRC_FILES})
#
#if (YAMLCPP_FOUND)
#  add_dependencies(${PROJECT_TEST_NAME}  yaml-cpp)
#else ()
#  add_dependencies(${PROJECT_TEST_NAME}  yaml-cpp-ext)
#endif ()

add_dependencies(${PROJECT_TEST_NAME} catch FakeIt spdlog)

target_link_libraries(${PROJECT_TEST_NAME}
    ${Boost_LIBRARIES}
    ${YAML_CPP_LIBS_DIR}/libyaml-cpp.a
    )

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)


set(SPDLOG_TEST_FILES spdlogTest.cpp)
add_executable(spdlog_test ${SPDLOG_TEST_FILES})
add_dependencies(spdlog_test spdlog)

set(FMT_TEST_FILES fmtTest.cpp)
add_executable(fmtTest ${FMT_TEST_FILES})
target_link_libraries(fmtTest
    ${FMT_LIBS_DIR}/libfmt.a
    )
add_dependencies(fmtTest fmt)