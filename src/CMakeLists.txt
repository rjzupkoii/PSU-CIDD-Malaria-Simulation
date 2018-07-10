file(GLOB SRC_FILES *.cpp *.h Spatial/*.cpp Spatial/*.h Reporters/*.cpp Reporters/*.h Strategies/*.cpp Strategies/*.h
    MDC/*.cpp MDC/*.h Helpers/*.cpp Helpers/*.h
	Core/*.h Core/*.cpp
	Core/Config/*.h Core/Config/*.cpp)

include_directories(${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

add_executable(MaSim ${SRC_FILES} 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)
add_dependencies(MaSim date args)

target_link_libraries(MaSim
    yaml-cpp
    GSL::gsl GSL::gslcblas	
	fmt::fmt fmt::fmt-header-only
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)

