include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4 ${CATCH_INCLUDE_DIR})

set(TEST_SRC_FILES
    main.cpp
    sample_catch_test.cpp
    sample_yaml_cpp_test.cpp
    #SimpleFakeItTest.cpp
    Spatial/CoordinateTest.cpp
    Spatial/LocationTest.cpp
    Core/RandomTest.cpp
    Core/TimeHelpersTest.cpp
    Core/StringHelpersTest.cpp
    Core/Config/ConfigTest.cpp
    )

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} )
add_dependencies(${PROJECT_TEST_NAME} MaSimCore date args catch)

target_link_libraries(${PROJECT_TEST_NAME}
    MaSimCore
    yaml-cpp
    fmt::fmt fmt::fmt-header-only
    GSL::gsl GSL::gslcblas
    )

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)


add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/misc/input.yml $<TARGET_FILE_DIR:${PROJECT_TEST_NAME}>)

#
#install(TARGETS ${PROJECT_TEST_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
#install(FILES ${PROJECT_SOURCE_DIR}/misc/input.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)

