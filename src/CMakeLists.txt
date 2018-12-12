include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

file(GLOB SRC_FILES *.cpp Spatial/*.cpp Reporters/*.cpp Strategies/*.cpp
    MDC/*.cpp Helpers/*.cpp Core/*.cpp Core/Config/*.cpp Core/Index/*.cpp Therapies/*.cpp
    Parasites/*.cpp Events/*.cpp Events/Population/*.cpp Malaria/*.cpp)


add_executable(MaSim ${SRC_FILES}
    MaSim/main.cpp
    ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)
add_dependencies(MaSim date args)

target_link_libraries(MaSim
    yaml-cpp
    GSL::gsl GSL::gslcblas
    fmt::fmt fmt::fmt-header-only
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_dev.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)


file(GLOB DXG_SRC_FILES DxGGenerator/*.cpp DxGGenerator/*.h)

add_executable(DxGGenerator ${SRC_FILES} ${DXG_SRC_FILES}
    ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc)
add_dependencies(DxGGenerator date args)

target_link_libraries(DxGGenerator
    yaml-cpp
    GSL::gsl GSL::gslcblas
    fmt::fmt fmt::fmt-header-only
    )

target_compile_features(DxGGenerator PRIVATE cxx_range_for)

install(TARGETS DxGGenerator DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_DxG.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)





