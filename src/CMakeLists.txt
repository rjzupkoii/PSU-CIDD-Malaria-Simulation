include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

file(GLOB SRC_FILES *.cpp Spatial/*.cpp Reporters/*.cpp Strategies/*.cpp
    MDC/*.cpp Helpers/*.cpp Core/*.cpp Core/Config/*.cpp Core/Index/*.cpp Therapies/*.cpp
    Parasites/*.cpp Events/*.cpp Events/Population/*.cpp Malaria/*.cpp)

set(SRC_FILES ${SRC_FILES}
    ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h
    ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)

add_library(MaSimCore ${SRC_FILES})
add_dependencies(MaSimCore date args)


add_executable(MaSim MaSim/main.cpp)
add_dependencies(MaSim MaSimCore date args)

target_link_libraries(MaSim
    MaSimCore
    yaml-cpp
    GSL::gsl GSL::gslcblas
    fmt::fmt
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_dev.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)


file(GLOB DXG_SRC_FILES DxGGenerator/*.cpp DxGGenerator/*.h)

add_executable(DxGGenerator ${DXG_SRC_FILES})
add_dependencies(DxGGenerator MaSimCore date args)

target_link_libraries(DxGGenerator
    MaSimCore
    yaml-cpp
    GSL::gsl GSL::gslcblas
    fmt::fmt
    )

target_compile_features(DxGGenerator PRIVATE cxx_range_for)

install(TARGETS DxGGenerator DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_DxG.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)
