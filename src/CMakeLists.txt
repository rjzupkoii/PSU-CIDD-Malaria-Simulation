# Enable if building on the Windows Subsystem for Linux (WSL), via the command line use -DBUILD_WSL:BOOL=true
set(BUILD_WSL false CACHE BOOL "Enable for builds on WSL")

include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

file(GLOB SRC_FILES *.cpp 
        Core/*.cpp 
        Core/Config/*.cpp 
        Core/Index/*.cpp 
        Events/*.cpp 
        Events/Population/*.cpp 
        Helpers/*.cpp 
        Malaria/*.cpp
        MDC/*.cpp 
        Parasites/*.cpp 
        Population/*.cpp
        Population/Properties/*.cpp
        Reporters/*.cpp 
        Spatial/*.cpp 
        Strategies/*.cpp
        Therapies/*.cpp       
)

set(SRC_FILES ${SRC_FILES}
        ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h
        ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc
       )

add_library(MaSimCore STATIC ${SRC_FILES})
add_dependencies(MaSimCore date args)

if (BUILD_WSL)
target_link_libraries(MaSimCore PUBLIC
        yaml-cpp
        GSL::gsl GSL::gslcblas
        fmt::fmt
        )
else()
target_link_libraries(MaSimCore PUBLIC
        yaml-cpp
        GSL::gsl GSL::gslcblas
        fmt::fmt fmt::fmt-header-only
        )
endif()

add_executable(MaSim MaSim/main.cpp)
add_dependencies(MaSim MaSimCore)

target_link_libraries(MaSim PRIVATE MaSimCore)
target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_dev.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)


file(GLOB DXG_SRC_FILES DxGGenerator/*.cpp DxGGenerator/*.h)

add_executable(DxGGenerator ${DXG_SRC_FILES})
add_dependencies(DxGGenerator  MaSimCore)
target_link_libraries(DxGGenerator PRIVATE MaSimCore)
target_compile_features(DxGGenerator PRIVATE cxx_range_for)

#install(TARGETS DxGGenerator DESTINATION ${PROJECT_SOURCE_DIR}/bin)
#install(FILES ${PROJECT_SOURCE_DIR}/misc/input_DxG.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)
