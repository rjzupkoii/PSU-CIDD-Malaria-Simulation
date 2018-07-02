file(GLOB SRC_FILES *.cpp *.h *.cc Spatial/*.cpp Spatial/*.h Reporters/*.cpp Reporters/*.h Strategies/*.cpp Strategies/*.h
    MDC/*.cpp MDC/*.h Helpers/*.cpp Helpers/*.h)

add_executable(MaSim ${SRC_FILES})
add_dependencies(MaSim date)

target_link_libraries(MaSim
    yaml-cpp
    GSL::gsl GSL::gslcblas	
	fmt::fmt
	gflags_static
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)

