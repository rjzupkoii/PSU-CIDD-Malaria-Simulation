include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

file(GLOB SRC_FILES *.cpp  Spatial/*.cpp  Reporters/*.cpp Strategies/*.cpp 
MDC/*.cpp  Helpers/*.cpp 	Core/*.cpp	 Core/Config/*.cpp 	Therapies/*.cpp
Parasites/*.cpp Events/*.cpp Malaria/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

add_executable(MaSim ${SRC_FILES} 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h 
	${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)
add_dependencies(MaSim date args)

target_link_libraries(MaSim
    yaml-cpp
    GSL::gsl GSL::gslcblas	
	fmt::fmt fmt::fmt-header-only
	#unofficial::date::date
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_dev.yml  DESTINATION ${PROJECT_SOURCE_DIR}/bin)