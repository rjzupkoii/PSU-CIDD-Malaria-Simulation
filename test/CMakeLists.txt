include_directories(${CATCH_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

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

file(GLOB SRC_FILES 
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/Reporters/*.cpp
	${PROJECT_SOURCE_DIR}/src/MDC/*.cpp
	${PROJECT_SOURCE_DIR}/src/Spatial/*.cpp
	${PROJECT_SOURCE_DIR}/src/Core/*.cpp
	${PROJECT_SOURCE_DIR}/src/Core/Config/*.cpp
	${PROJECT_SOURCE_DIR}/src/Helpers/*.cpp 	
	${PROJECT_SOURCE_DIR}/src/Therapies/*.cpp
	${PROJECT_SOURCE_DIR}/src/Strategies/*.cpp
	${PROJECT_SOURCE_DIR}/src/Parasites/*.cpp
	${PROJECT_SOURCE_DIR}/src/Events/*.cpp
  ${PROJECT_SOURCE_DIR}/src/Malaria/*.cpp)

set(SRC_FILES ${SRC_FILES} 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)

list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES} ${SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} date)

target_link_libraries(${PROJECT_TEST_NAME}
    yaml-cpp
	fmt::fmt fmt::fmt-header-only	
	GSL::gsl GSL::gslcblas		
	#unofficial::date::date
    )

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)


add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/misc/input.yml $<TARGET_FILE_DIR:${PROJECT_TEST_NAME}>)



install(TARGETS ${PROJECT_TEST_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input.yml  DESTINATION ${PROJECT_SOURCE_DIR}/bin)

